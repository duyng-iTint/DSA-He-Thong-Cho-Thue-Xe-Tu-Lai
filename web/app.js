const $ = selector => document.querySelector(selector);
const fmtDate = value => value ? new Date(`${value}T00:00:00`).toLocaleDateString('vi-VN') : '—';
const statusInfo = { DANG_THUE: ['Đang thuê', 'active'], DA_TRA: ['Đã trả', 'returned'], DA_HUY: ['Đã hủy', 'cancelled'] };
const rankInfo = ['Cơ bản', 'Bạc', 'Vàng', 'Kim cương'];
let editId = null;
let toastTimer;
let suggestTimer;

async function api(url, options = {}) {
  const response = await fetch(url, { ...options, headers: { 'Content-Type': 'application/json', ...(options.headers || {}) } });
  const body = await response.json();
  if (!response.ok) throw new Error(body.error || 'Có lỗi xảy ra.');
  return body;
}
function notify(message, error = false) {
  const toast = $('#toast'); toast.textContent = message; toast.classList.toggle('error', error); toast.classList.add('show');
  clearTimeout(toastTimer); toastTimer = setTimeout(() => toast.classList.remove('show'), 2800);
}
function renderRentals(rows, total) {
  const body = $('#rentals-body');
  $('#result-count').textContent = `Hiển thị ${rows.length} / ${total} đơn thuê`;
  $('#nav-count').textContent = total;
  if (!rows.length) { body.innerHTML = '<tr><td colspan="8" class="empty">Không tìm thấy đơn thuê phù hợp.</td></tr>'; return; }
  body.innerHTML = rows.slice(0, 150).map(r => {
    const [label, cls] = statusInfo[r.trang_thai] || ['Khác', 'returned'];
    const rank = Math.max(0, Math.min(3, Number(r.hang_thanh_vien) || 0));
    const price = Number(r.gia_tien) > 0 ? `${Number(r.gia_tien).toLocaleString('vi-VN')} ₫` : '—';
    return `<tr class="row"><td><span class="id-cell">${escapeHtml(r.booking_id)}</span></td><td><strong>${escapeHtml(r.hang_xe)} ${escapeHtml(r.dong_xe)}</strong><span class="subtext">${escapeHtml(r.bien_so)}</span></td><td><strong>${escapeHtml(r.ten_khach)}</strong></td><td>${fmtDate(r.ngay_bat_dau)} <span class="subtext">đến ${fmtDate(r.ngay_ket_thuc)}</span></td><td>${price}</td><td>${rankInfo[rank]}</td><td><span class="status ${cls}">${label}</span></td><td><div class="row-actions"><button data-edit="${escapeHtml(r.booking_id)}" title="Sửa">✎</button><button data-delete="${escapeHtml(r.booking_id)}" title="Xóa">×</button></div></td></tr>`;
  }).join('');
}
function escapeHtml(value) { return String(value ?? '').replace(/[&<>"']/g, char => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' })[char]); }
async function loadRentals() {
  const params = new URLSearchParams();
  if ($('#search').value.trim()) params.set('q', $('#search').value.trim());
  if ($('#date-from').value) params.set('from', $('#date-from').value);
  if ($('#date-to').value) params.set('to', $('#date-to').value);
  const data = await api(`/api/rentals?${params}`); renderRentals(data.rentals, data.total);
}
async function loadStats() {
  const stats = await api('/api/stats');
  $('#stat-total').textContent = stats.total.toLocaleString('vi-VN');
  $('#stat-active').textContent = stats.active.toLocaleString('vi-VN');
  $('#stat-returned').textContent = stats.returned.toLocaleString('vi-VN');
  $('#stat-cancelled').textContent = stats.cancelled.toLocaleString('vi-VN');
  const max = Math.max(1, ...stats.topCars.map(car => car.count));
  $('#top-cars').innerHTML = stats.topCars.length ? stats.topCars.map((car, i) => `<div class="car-row"><span class="car-rank">0${i + 1}</span><div><div class="car-name">${escapeHtml(car.car)}</div><div class="bar"><i style="width:${Math.round(car.count / max * 100)}%"></i></div></div><span class="car-count">${car.count} đơn</span></div>`).join('') : '<div class="empty">Chưa có dữ liệu xe.</div>';
}
async function refresh() { await Promise.all([loadRentals(), loadStats()]); }
function openCreate() {
  editId = null; $('#booking-form').reset(); $('#booking-form').elements.booking_id.disabled = false;
  $('#dialog-title').textContent = 'Tạo đơn thuê mới'; $('#save-button').textContent = 'Lưu đơn thuê'; $('#booking-dialog').showModal();
}
async function openEdit(id) {
  const { rentals } = await api('/api/rentals'); const rental = rentals.find(item => item.booking_id === id); if (!rental) return;
  editId = id; const form = $('#booking-form');
  for (const key of Object.keys(rental)) if (form.elements[key]) form.elements[key].value = rental[key];
  form.elements.booking_id.disabled = true; $('#dialog-title').textContent = 'Cập nhật đơn thuê'; $('#save-button').textContent = 'Lưu thay đổi'; $('#booking-dialog').showModal();
}
$('#today').textContent = new Intl.DateTimeFormat('vi-VN', { weekday: 'long', day: 'numeric', month: 'long', year: 'numeric' }).format(new Date());
$('#new-booking').addEventListener('click', openCreate);
$('#close-dialog').addEventListener('click', () => $('#booking-dialog').close());
$('#cancel-dialog').addEventListener('click', () => $('#booking-dialog').close());
$('#booking-form').addEventListener('submit', async event => {
  event.preventDefault(); const payload = Object.fromEntries(new FormData(event.currentTarget));
  try {
    await api(editId ? `/api/rentals/${encodeURIComponent(editId)}` : '/api/rentals', { method: editId ? 'PUT' : 'POST', body: JSON.stringify(payload) });
    $('#booking-dialog').close(); await refresh(); notify(editId ? 'Đã cập nhật đơn thuê.' : 'Đã tạo đơn thuê.');
  } catch (error) { notify(error.message, true); }
});
let searchTimer;
$('#search').addEventListener('input', () => { clearTimeout(searchTimer); searchTimer = setTimeout(() => loadRentals().catch(error => notify(error.message, true)), 180); });
$('#search').addEventListener('input', () => {
  clearTimeout(suggestTimer);
  suggestTimer = setTimeout(async () => {
    const prefix = $('#search').value.trim(); if (prefix.length < 2) { $('#search-suggestions').replaceChildren(); return; }
    try { const result = await api(`/api/suggest?prefix=${encodeURIComponent(prefix)}`); $('#search-suggestions').innerHTML = result.items.map(item => `<option value="${escapeHtml(item)}"></option>`).join(''); } catch { /* Core suggestions are optional while typing. */ }
  }, 220);
});
$('#exact-search').addEventListener('click', async () => {
  try {
    const result = await api(`/api/search?key=${encodeURIComponent($('#search').value.trim())}`);
    if (!result.rental) { renderRentals([], 0); notify('Không tìm thấy mã đơn hoặc biển số.', true); return; }
    renderRentals([result.rental], 1); notify(`Đã tra cứu bằng ${result.index}.`);
  } catch (error) { notify(error.message, true); }
});
$('#date-from').addEventListener('change', () => loadRentals().catch(error => notify(error.message, true)));
$('#date-to').addEventListener('change', () => loadRentals().catch(error => notify(error.message, true)));
$('#rentals-body').addEventListener('click', async event => {
  const edit = event.target.closest('[data-edit]'); const del = event.target.closest('[data-delete]');
  try {
    if (edit) await openEdit(edit.dataset.edit);
    if (del && confirm(`Xóa đơn ${del.dataset.delete}?`)) { await api(`/api/rentals/${encodeURIComponent(del.dataset.delete)}`, { method: 'DELETE' }); await refresh(); notify('Đã xóa đơn. Bạn có thể hoàn tác thao tác này.'); }
  } catch (error) { notify(error.message, true); }
});
$('#undo-button').addEventListener('click', async () => { try { const result = await api('/api/undo', { method: 'POST' }); await refresh(); notify(result.message); } catch (error) { notify(error.message, true); } });
async function refreshPriority() {
  const result = await api('/api/priority');
  $('#priority-result').innerHTML = result.items.length ? result.items.map((item, i) => `<div class="priority-item"><span class="car-rank">${i + 1}</span><b>${escapeHtml(item.booking_id)}</b><span>Hạng ${item.membership_tier}</span><small>${new Date(item.booking_timestamp).toLocaleTimeString('vi-VN')}</small></div>`).join('') : 'Hàng đợi ưu tiên đang trống.';
}
$('#priority-form').addEventListener('submit', async event => {
  event.preventDefault(); const form = event.currentTarget; const payload = Object.fromEntries(new FormData(form));
  try { await api('/api/priority', { method: 'POST', body: JSON.stringify(payload) }); form.reset(); await refreshPriority(); notify('Đã xếp yêu cầu bằng Max-Heap.'); }
  catch (error) { notify(error.message, true); }
});
$('#process-priority').addEventListener('click', async () => {
  try { const result = await api('/api/priority/next', { method: 'POST' }); await refreshPriority(); notify(`Đang xử lý đơn ${result.next.booking_id} · hạng ${result.next.membership_tier}.`); }
  catch (error) { notify(error.message, true); }
});
document.querySelectorAll('.nav-item').forEach(item => item.addEventListener('click', () => { document.querySelectorAll('.nav-item').forEach(nav => nav.classList.remove('active')); item.classList.add('active'); $('#crumb').textContent = item.textContent.trim(); }));
refresh().catch(error => { $('#rentals-body').innerHTML = `<tr><td colspan="8" class="empty">${escapeHtml(error.message)}</td></tr>`; notify('Không thể tải dữ liệu từ máy chủ.', true); });
refreshPriority().catch(error => notify(error.message, true));
