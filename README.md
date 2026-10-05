# Hệ thống cho thuê xe tự lái

Đồ án C++17 áp dụng cấu trúc dữ liệu và thuật toán để quản lý đơn thuê xe. Repo gồm C++ Core, các CLI/demo, web local dùng Node.js và C++ bridge, kiểm thử và benchmark. Tài liệu này là hướng dẫn tổng thể build/run; [đặc tả chi tiết](docs/SPECIFICATION.md) mô tả yêu cầu và giới hạn nghiệp vụ.

## Yêu cầu

- CMake 3.16 trở lên.
- Compiler C++17 (Visual Studio/MSVC, MinGW GCC hoặc Clang).
- Node.js chỉ cần khi chạy web; web không cần `npm install`.

## Chức năng và trạng thái hiện tại

| Mã | Chức năng | Thành phần chính | Điểm vào để chạy |
|---|---|---|---|
| MC1 | Tra cứu booking ID/biển số, lưu CSV | `MyHashTable`, Persistence | `RentalMC1Demo`, `BenchmarkMC1`, `TestMC1Persistence` |
| MC2 | Sắp xếp/lọc theo ngày, Top-N | Merge Sort, Binary Search, `RentalService` | `RentalMC2Demo`, `BenchmarkMC2`, `TestMC2Correctness` |
| RF1 | Gợi ý hãng/dòng xe theo tiền tố | `CarTrie` | `RentalRF1Demo`, `TestRF1Trie` |
| RF2 | Xếp request theo hạng thành viên và thời điểm | `MyMaxHeap` | `DemoRF2Heap`, `BenchmarkRF2`, API web `/api/priority*` |
| RF3 | CRUD và Undo theo LIFO | `UndoStack`, `RentalSystem` | `CarRentalApp` (menu) |
| Web | Quản lý đơn, tìm kiếm, thống kê, gợi ý, hàng đợi | Node API + C++ bridge + CSV | `web/start.ps1` |

Web tập trung vào nghiệp vụ cho thuê. Benchmark chạy thành chương trình C++ riêng để MC1, MC2 và RF2 đều có thể đo độc lập; giao diện web không có benchmark MC2.

Một số giới hạn của bản hiện tại: MC2 lọc theo ngày bắt đầu thuê; RF2 xếp các yêu cầu đã đưa vào hàng đợi nhưng chưa tự tính lịch xe còn trống; Undo web và hàng đợi RF2 chỉ tồn tại trong RAM của phiên server. Xem thêm [đặc tả](docs/SPECIFICATION.md).

## Mô hình dữ liệu CSV

File mặc định `data/donthue_xe.csv` dùng schema 11 cột:

```text
booking_id,bien_so,ten_khach,hang_xe,dong_xe,ngay_bat_dau,ngay_ket_thuc,trang_thai,gia_tien,hang_thanh_vien,thoi_diem_dat
```

Ngày theo `YYYY-MM-DD`; hạng thành viên 0–3; thời điểm đặt là Unix milliseconds. Core có thể đọc một số schema CSV cũ và chuẩn hóa về schema hiện tại khi ghi. Các model C++ hiện gồm `Booking`, `RentalRecord` và `renTal`, được chuyển đổi tại service/adapter.

## Kiến trúc và thư mục

CLI gọi C++ Core trực tiếp. Web dùng Node.js làm HTTP/API adapter, gọi executable `RentalWebCore`, rồi Core đọc/ghi CSV.

```text
src/core/algorithms/   thuật toán sắp xếp, tìm kiếm, xử lý ngày
src/core/model/        model nghiệp vụ
src/core/services/     CSV, persistence, rental services
src/core/structures/   hash table, heap, trie, undo stack
src/presentation/      các chương trình CLI/demo
test/                  test tự động và demo cấu trúc dữ liệu
benchmark/             benchmark MC1, MC2, RF2
web/                   giao diện, Node API, C++ bridge, script chạy web
data/                  CSV mặc định
docs/                  đặc tả và tài liệu đồ án
```

## Build toàn bộ

Chạy lệnh từ thư mục gốc repo:

```powershell
cmake -S . -B build/web-integration
cmake --build build/web-integration --config Release
```

Các executable sẽ nằm trong `build/web-integration` (hoặc thư mục con cấu hình như `Release`, tùy generator). Trên Linux/macOS bỏ `.exe` khỏi tên executable.

## Từng target: build và chạy

Mỗi chương trình độc lập có target `run_<TênTarget>` để build rồi chạy trực tiếp. Ví dụ:

```powershell
cmake --build build/web-integration --config Release --target run_RentalRF1Demo
```

| Target chạy | Chức năng | Lệnh build và chạy |
|---|---|---|
| `CarRentalApp` | CLI RF3: CRUD, tìm, Undo; menu 9 test RF3, menu 10 benchmark MC1 | `cmake --build build/web-integration --config Release --target run_CarRentalApp` |
| `RentalMC1Demo` | Demo tra cứu và thêm đơn bằng hash table | `cmake --build build/web-integration --config Release --target run_RentalMC1Demo` |
| `RentalMC2Demo` | Menu lọc ngày, Top-N và benchmark tương tác MC2 | `cmake --build build/web-integration --config Release --target run_RentalMC2Demo` |
| `RentalRF1Demo` | Demo autocomplete Trie | `cmake --build build/web-integration --config Release --target run_RentalRF1Demo` |
| `DemoRF2Heap` | Demo heap ưu tiên | `cmake --build build/web-integration --config Release --target run_DemoRF2Heap` |
| `BenchmarkMC1` | Hash lookup so với Linear Scan | `cmake --build build/web-integration --config Release --target run_BenchmarkMC1` |
| `BenchmarkMC2` | Binary Search range query so với Linear Scan | `cmake --build build/web-integration --config Release --target run_BenchmarkMC2` |
| `BenchmarkRF2` | Max-Heap lấy ưu tiên cao nhất so với tìm max tuyến tính | `cmake --build build/web-integration --config Release --target run_BenchmarkRF2` |
| `TestMC1Persistence` | Test MC1/hash table và Persistence/CSV | `cmake --build build/web-integration --config Release --target run_TestMC1Persistence` |
| `TestMC2Correctness` | Test thuật toán/chức năng MC2 | `cmake --build build/web-integration --config Release --target run_TestMC2Correctness` |
| `TestRF1Trie` | Test Trie RF1 | `cmake --build build/web-integration --config Release --target run_TestRF1Trie` |

Các target executable tương ứng cũng có thể chỉ build bằng cách bỏ tiền tố `run_`, ví dụ `--target BenchmarkRF2`. `RentalWebCore` là tiến trình giao tiếp nội bộ theo giao thức dòng lệnh; web server tự khởi chạy nó nên không có target `run_` dành cho người dùng.

### Chạy riêng từng file `.cpp` như thế nào?

CMake cung cấp target cho mọi file `.cpp` có `main()` ở presentation, test/demo và benchmark. Các file như `src/core/services/Persistence.cpp`, `RentalService.cpp`, `InputHandler.cpp`, `Tests.cpp`, `Benchmark.cpp` không có `main()`: chúng là module, không phải chương trình độc lập. CMake biên dịch toàn bộ `src/core/*.cpp` thành thư viện `core` rồi liên kết module cần thiết vào executable. Vì vậy chạy file module đơn lẻ sẽ không có điểm bắt đầu chương trình; hãy chạy target ứng dụng/test/benchmark ở bảng trên. `RentalWebCore` là bridge nội bộ do web gọi.

## Test

Chạy cả test đã đăng ký với CTest:

```powershell
ctest --test-dir build/web-integration -C Release --output-on-failure
```

CTest hiện gồm MC1 Persistence, MC2 correctness và RF1 Trie. Muốn chạy trực tiếp một bài test, dùng target `run_Test...` ở bảng trên. RF3 có test từ menu `CarRentalApp` mục 9; `DemoRF2Heap` là demo thao tác heap, không phải bộ test tự động.

## Benchmark

Ba benchmark có executable riêng và chạy bằng các lệnh `run_Benchmark...` ở bảng target:

| Benchmark | So sánh | Quy mô chính |
|---|---|---|
| MC1 | Tìm kiếm `MyHashTable` và Linear Scan | 1.000 / 10.000 / 100.000 bản ghi |
| MC2 | Binary Search trên dữ liệu đã sắp xếp và Linear Scan | 1.000 / 10.000 / 100.000 bản ghi |
| RF2 | `MyMaxHeap` lấy max và quét mảng tìm max | 1.000 / 10.000 / 50.000 request |

Thời gian phụ thuộc máy, compiler, cấu hình build và dữ liệu. Benchmark MC2 kiểm tra kết quả hai phương pháp trước khi báo thời gian. Benchmark MC1 cũng có mục chạy trong menu `CarRentalApp` mục 10; MC2 có mục tương tác trong `RentalMC2Demo`.

## Chạy web

Từ thư mục gốc, chạy:

```powershell
./web/start.ps1
```

Mở <http://localhost:3000>. Dừng server bằng `Ctrl+C` tại terminal chạy script. Web dùng cùng CSV mặc định `data/donthue_xe.csv`; để thử API an toàn trên bản sao, đặt biến `RENTAL_DATA_FILE` trỏ tới file CSV đã copy trước khi khởi động server.

API chính: `GET/POST /api/rentals`, `PUT/DELETE /api/rentals/{booking_id}`, `POST /api/undo`, `GET /api/search`, `/api/stats`, `/api/top-cars`, `/api/suggest`, `/api/priority` và `POST /api/priority/next`. Không có API benchmark trên web. Payload và response được mô tả trong [web/README.md](web/README.md) và [đặc tả API](docs/SPECIFICATION.md#6-ứng-dụng-web-và-api).

## Dữ liệu thành viên

| Thành viên | MSSV | Phần phụ trách |
|---|---:|---|
| Nguyễn Hữu Thịnh | 25110349 | MC1 — Hash Table, Persistence |
| Nguyễn Minh Duy | 25110167 | MC2 — Merge Sort, Binary Search |
| Lê Đức Thuần | 25110352 | RF1 — Trie, autocomplete |
| Lý Nguyễn Mạnh Thuyên | 25110354 | RF2 — Max Heap, ưu tiên request |
| Lê Nguyễn Minh Thư | 25110356 | RF3 — Stack Undo, test, benchmark |
