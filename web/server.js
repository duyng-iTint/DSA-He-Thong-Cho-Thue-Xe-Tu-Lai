const http = require('node:http');
const fs = require('node:fs');
const path = require('node:path');
const { execFile, spawn } = require('node:child_process');
const { promisify } = require('node:util');
const execFileAsync = promisify(execFile);

const ROOT = path.resolve(__dirname, '..');
const DATA_FILE = path.resolve(process.env.RENTAL_DATA_FILE || path.join(ROOT, 'data', 'donthue_xe.csv'));
const PORT = Number(process.env.PORT || 3000);
const HEADERS = ['booking_id', 'bien_so', 'ten_khach', 'hang_xe', 'dong_xe', 'ngay_bat_dau', 'ngay_ket_thuc', 'trang_thai', 'gia_tien', 'hang_thanh_vien', 'thoi_diem_dat'];
let undoStack = [];
let priorityQueue = [];

function coreExecutable() {
  for (const candidate of [process.env.RENTAL_WEB_CORE, path.join(ROOT, 'build', 'RentalWebCore.exe'), path.join(ROOT, 'build', 'Debug', 'RentalWebCore.exe'), path.join(ROOT, 'build', 'Release', 'RentalWebCore.exe'), path.join(ROOT, 'build', 'web-integration', 'RentalWebCore.exe'), path.join(ROOT, 'build', 'RentalWebCore')].filter(Boolean)) {
    if (fs.existsSync(candidate)) return candidate;
  }
  throw new Error('Chưa build C++ Core. Chạy: cmake -S . -B build; cmake --build build --target RentalWebCore');
}
async function runCore(command, args = []) {
  const { stdout } = await execFileAsync(coreExecutable(), [DATA_FILE, command, ...args.map(String)], { cwd: ROOT, maxBuffer: 16 * 1024 * 1024, windowsHide: true });
  return stdout.trim();
}
async function runCoreJson(command, args = []) { return JSON.parse(await runCore(command, args)); }
function runCoreInput(command, input) {
  return new Promise((resolve, reject) => {
    const child = spawn(coreExecutable(), [DATA_FILE, command], { cwd: ROOT, windowsHide: true });
    let stdout = '', stderr = '';
    child.stdout.setEncoding('utf8').on('data', chunk => { stdout += chunk; });
    child.stderr.setEncoding('utf8').on('data', chunk => { stderr += chunk; });
    child.on('error', reject);
    child.on('close', code => code === 0 ? resolve(JSON.parse(stdout.trim())) : reject(new Error(stderr || `RentalWebCore exited ${code}`)));
    child.stdin.end(input);
  });
}

function csvValue(value) {
  const text = String(value ?? '');
  return /[",\r\n]/.test(text) ? `"${text.replaceAll('"', '""')}"` : text;
}

function snapshot(rentals) { undoStack.push(JSON.stringify(rentals)); }
function send(res, status, data) {
  res.writeHead(status, { 'Content-Type': 'application/json; charset=utf-8', 'Cache-Control': 'no-store' });
  res.end(JSON.stringify(data));
}
function readBody(req) {
  return new Promise((resolve, reject) => {
    let body = '';
    req.on('data', chunk => { body += chunk; if (body.length > 1_000_000) reject(new Error('Dữ liệu gửi lên quá lớn')); });
    req.on('end', () => { try { resolve(JSON.parse(body || '{}')); } catch { reject(new Error('JSON không hợp lệ')); } });
    req.on('error', reject);
  });
}
function validRental(input) {
  const required = ['booking_id', 'bien_so', 'ten_khach', 'hang_xe', 'dong_xe', 'ngay_bat_dau', 'ngay_ket_thuc'];
  if (required.some(key => !String(input[key] ?? '').trim())) return 'Vui lòng nhập đầy đủ thông tin đơn thuê.';
  const date = /^\d{4}-\d{2}-\d{2}$/;
  if (!date.test(input.ngay_bat_dau) || !date.test(input.ngay_ket_thuc) || input.ngay_bat_dau > input.ngay_ket_thuc) return 'Ngày thuê không hợp lệ.';
  if (input.gia_tien !== undefined && (!Number.isFinite(Number(input.gia_tien)) || Number(input.gia_tien) < 0)) return 'Giá thuê phải là số không âm.';
  if (input.hang_thanh_vien !== undefined && (!Number.isInteger(Number(input.hang_thanh_vien)) || Number(input.hang_thanh_vien) < 0 || Number(input.hang_thanh_vien) > 3)) return 'Hạng thành viên phải từ 0 đến 3.';
  return null;
}

const mime = { '.html': 'text/html; charset=utf-8', '.css': 'text/css; charset=utf-8', '.js': 'text/javascript; charset=utf-8', '.svg': 'image/svg+xml' };

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, `http://${req.headers.host || 'localhost'}`);
  try {
    if (url.pathname.startsWith('/api/')) {
      if (req.method === 'GET' && url.pathname === '/api/rentals') {
        const rentals = await runCoreJson('list');
        const q = (url.searchParams.get('q') || '').toLocaleLowerCase('vi');
        const from = url.searchParams.get('from') || '';
        const to = url.searchParams.get('to') || '';
        if (from || to) {
          const result = await runCoreJson('range', [from || '0000-01-01', to || '9999-12-31']);
          const filtered = result.filter(r => !q || [r.booking_id, r.bien_so, r.ten_khach, r.hang_xe, r.dong_xe].join(' ').toLocaleLowerCase('vi').includes(q));
          return send(res, 200, { rentals: filtered, total: rentals.length, queryEngine: 'MergeSort + BinarySearch' });
        }
        const filtered = rentals.filter(r => {
          const haystack = [r.booking_id, r.bien_so, r.ten_khach, r.hang_xe, r.dong_xe].join(' ').toLocaleLowerCase('vi');
          return (!q || haystack.includes(q)) && (!from || r.ngay_bat_dau >= from) && (!to || r.ngay_bat_dau <= to);
        });
        return send(res, 200, { rentals: filtered, total: rentals.length });
      }
      if (req.method === 'GET' && url.pathname === '/api/stats') {
        const [stats, topCars, rentals] = await Promise.all([runCoreJson('stats'), runCoreJson('top', [5]), runCoreJson('list')]);
        const models = new Map();
        rentals.forEach(r => { const name = `${r.hang_xe} ${r.dong_xe}`.trim(); models.set(name, (models.get(name) || 0) + 1); });
        const topModels = [...models].map(([car, count]) => ({ car, count })).sort((a, b) => b.count - a.count).slice(0, 5);
        return send(res, 200, { ...stats, topCars: topModels, topVehicles: topCars, dsa: { mc1: 'MyHashTable', mc2: 'MergeSort + BinarySearch' } });
      }
      if (req.method === 'GET' && url.pathname === '/api/search') {
        const key = (url.searchParams.get('key') || '').trim();
        if (!key) return send(res, 400, { error: 'Nhập mã đơn hoặc biển số xe.' });
        const result = await runCoreJson('search', [key]);
        return send(res, 200, { rental: result, index: 'MyHashTable' });
      }
      if (req.method === 'GET' && url.pathname === '/api/top-cars') {
        const limit = Math.max(1, Math.min(20, Number(url.searchParams.get('limit') || 5)));
        return send(res, 200, { items: await runCoreJson('top', [limit]), algorithm: 'RentalService::buildCarStats + topRentedCars (Merge Sort)' });
      }
      if (req.method === 'GET' && url.pathname === '/api/suggest') {
        const prefix = (url.searchParams.get('prefix') || '').trim();
        if (prefix.length < 2) return send(res, 200, { items: [] });
        return send(res, 200, { items: await runCoreJson('suggest', [prefix]), structure: 'Trie' });
      }
      if (req.method === 'GET' && url.pathname === '/api/benchmark') {
        const output = await runCore('benchmark', [url.searchParams.get('from') || '2026-01-01', url.searchParams.get('to') || '2026-12-31']);
        return send(res, 200, { output });
      }
      if (url.pathname === '/api/priority' && req.method === 'GET') {
        const args = priorityQueue.flatMap(item => [item.booking_id, item.membership_tier, item.booking_timestamp]);
        return send(res, 200, await runCoreJson('priority', args));
      }
      if (url.pathname === '/api/priority' && req.method === 'POST') {
        const body = await readBody(req);
        const rentals = await runCoreJson('list');
        if (!String(body.booking_id || '').trim()) return send(res, 400, { error: 'Nhập mã đơn cần ưu tiên.' });
        if (priorityQueue.some(item => item.booking_id === String(body.booking_id).trim())) return send(res, 409, { error: 'Mã đơn này đã nằm trong hàng đợi.' });
        const booking = rentals.find(item => item.booking_id === String(body.booking_id).trim());
        const tier = body.membership_tier === undefined || body.membership_tier === '' ? Number(booking?.hang_thanh_vien || 0) : Number(body.membership_tier);
        if (!Number.isInteger(tier) || tier < 0 || tier > 3) return send(res, 400, { error: 'Hạng thành viên phải từ 0 đến 3.' });
        priorityQueue.push({ booking_id: String(body.booking_id).trim(), membership_tier: tier, booking_timestamp: Number(body.booking_timestamp) || Number(booking?.thoi_diem_dat) || Date.now() });
        const args = priorityQueue.flatMap(item => [item.booking_id, item.membership_tier, item.booking_timestamp]);
        return send(res, 201, await runCoreJson('priority', args));
      }
      if (url.pathname === '/api/priority/next' && req.method === 'POST') {
        if (!priorityQueue.length) return send(res, 400, { error: 'Hàng đợi chưa có yêu cầu.' });
        const args = priorityQueue.flatMap(item => [item.booking_id, item.membership_tier, item.booking_timestamp]);
        const ordered = await runCoreJson('priority', args);
        const next = ordered.items[0]; priorityQueue = priorityQueue.filter(item => item.booking_id !== next.booking_id);
        return send(res, 200, { next });
      }
      if (req.method === 'POST' && url.pathname === '/api/undo') {
        if (!undoStack.length) return send(res, 400, { error: 'Không có thao tác nào để hoàn tác.' });
        const previous = JSON.parse(undoStack[undoStack.length - 1]);
        const csv = [HEADERS.join(','), ...previous.map(row => HEADERS.map(key => csvValue(row[key])).join(','))].join('\n') + '\n';
        const restored = await runCoreInput('replace', csv);
        if (!restored.ok) return send(res, 500, { error: 'Không thể khôi phục dữ liệu qua Persistence Core.' });
        undoStack.pop();
        return send(res, 200, { message: 'Đã hoàn tác thao tác gần nhất.' });
      }
      if (req.method === 'POST' && url.pathname === '/api/rentals') {
        const body = await readBody(req);
        const rentals = await runCoreJson('list');
        const error = validRental(body);
        if (error) return send(res, 400, { error });
        if (rentals.some(r => r.booking_id === body.booking_id.trim())) return send(res, 409, { error: 'Mã đơn thuê đã tồn tại.' });
        const createdAt = Number(body.thoi_diem_dat) || Date.now();
        const validation = await runCoreJson('mutate', ['add', body.booking_id.trim(), body.ten_khach.trim(), body.bien_so.trim(), body.ngay_bat_dau, body.ngay_ket_thuc, body.hang_xe.trim(), body.dong_xe.trim(), body.trang_thai || 'DANG_THUE', Number(body.gia_tien) || 0, Number(body.hang_thanh_vien) || 0, createdAt]);
        if (!validation.ok) return send(res, 400, { error: validation.error || 'RentalSystem từ chối đơn thuê.' });
        snapshot(rentals);
        const rental = Object.fromEntries(HEADERS.map(key => [key, String(body[key] ?? '').trim()]));
        rental.trang_thai = rental.trang_thai || 'DANG_THUE';
        rental.gia_tien = Number(body.gia_tien) || 0; rental.hang_thanh_vien = Number(body.hang_thanh_vien) || 0; rental.thoi_diem_dat = createdAt;
        rentals.unshift(rental);
        return send(res, 201, { rental });
      }
      const match = url.pathname.match(/^\/api\/rentals\/([^/]+)$/);
      if (match && ['PUT', 'DELETE'].includes(req.method)) {
        const rentals = await runCoreJson('list');
        const id = decodeURIComponent(match[1]);
        const index = rentals.findIndex(r => r.booking_id === id);
        if (index < 0) return send(res, 404, { error: 'Không tìm thấy đơn thuê.' });
        if (req.method === 'DELETE') {
          const validation = await runCoreJson('mutate', ['delete', id]);
          if (!validation.ok) return send(res, 400, { error: 'RentalSystem từ chối xóa đơn.' });
        } else {
          const body = await readBody(req);
          const merged = { ...rentals[index], ...body };
          const validation = await runCoreJson('mutate', ['update', id, merged.ten_khach, merged.bien_so, merged.ngay_bat_dau, merged.ngay_ket_thuc, merged.hang_xe, merged.dong_xe, merged.trang_thai, Number(merged.gia_tien) || 0, Number(merged.hang_thanh_vien) || 0, Number(merged.thoi_diem_dat) || 0]);
          if (!validation.ok) return send(res, 400, { error: validation.error || 'RentalSystem từ chối cập nhật đơn.' });
          req.validatedBody = body;
        }
        snapshot(rentals);
        if (req.method === 'DELETE') rentals.splice(index, 1);
        else {
          const body = req.validatedBody;
          const error = validRental({ ...rentals[index], ...body, booking_id: id });
          if (error) { undoStack.pop(); return send(res, 400, { error }); }
          rentals[index] = { ...rentals[index], ...body, booking_id: id };
        }
        return send(res, 200, { rentals });
      }
      return send(res, 404, { error: 'API không tồn tại.' });
    }

    const requested = url.pathname === '/' ? 'index.html' : decodeURIComponent(url.pathname.slice(1));
    const file = path.resolve(__dirname, requested);
    if (!file.startsWith(`${__dirname}${path.sep}`)) return send(res, 403, { error: 'Không được phép truy cập.' });
    if (!fs.existsSync(file) || !fs.statSync(file).isFile()) return send(res, 404, { error: 'Không tìm thấy trang.' });
    res.writeHead(200, { 'Content-Type': mime[path.extname(file)] || 'application/octet-stream' });
    fs.createReadStream(file).pipe(res);
  } catch (error) {
    send(res, 500, { error: error.message || 'Lỗi máy chủ.' });
  }
});

server.listen(PORT, '127.0.0.1', () => console.log(`Web quản lý xe chạy tại http://localhost:${PORT}`));
