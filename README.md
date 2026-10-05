# Hệ thống cho thuê xe tự lái

Đồ án C++17 áp dụng cấu trúc dữ liệu và thuật toán vào quản lý đơn thuê xe. Chương trình gồm CLI, web local, API JSON, lưu CSV, kiểm thử và benchmark. README này tóm tắt chức năng và cách chạy; [đặc tả hệ thống đầy đủ](docs/SPECIFICATION.md) mô tả yêu cầu, mô hình dữ liệu, thiết kế, API và giới hạn hiện tại.

## Mô tả

Hệ thống giải quyết các thao tác tra cứu, lọc, thống kê, gợi ý, xếp ưu tiên và hoàn tác trên dữ liệu đơn thuê. Bản đặc tả gốc đặt mục tiêu hoạt động trên lịch sử CSV lớn; các thao tác chính cần chạy trên cấu trúc dữ liệu trong RAM sau khi nạp dữ liệu.

## Thành viên

| STT | Họ và tên | MSSV | Vai trò |
|---|---|---|---|
| 1 | Nguyễn Hữu Thịnh | 25110349 | MC1 — Hash Table, Persistence |
| 2 | Nguyễn Minh Duy | 25110167 | MC2 — Merge Sort, Binary Search |
| 3 | Lê Đức Thuần | 25110352 | RF1 — Trie, autocomplete |
| 4 | Lý Nguyễn Mạnh Thuyên | 25110354 | RF2 — Max Heap, ưu tiên request |
| 5 | Lê Nguyễn Minh Thư | 25110356 | RF3 — Stack Undo, test, benchmark |

## Kiến trúc hệ thống

Hệ thống có ba tầng logic. CLI gọi trực tiếp C++ Core; web đi qua Node.js HTTP adapter và chương trình C++ bridge. Persistence lưu file CSV.

```
┌───────────────────────────────────────────┐
│              PRESENTATION                 │
│                                           │
│ CLI / Web UI                              │
│ Nhận input và hiển thị output             │
└─────────────────────┬─────────────────────┘
                      │
                      ▼
┌───────────────────────────────────────────┐
│                 DSA CORE                  │
│                                           │
│ Domain Logic                              │
│ MyHashTable                               │
│ MyMaxHeap                                 │
│ Trie                                      │
│ Stack                                     │
│ Merge Sort                                │
│ Binary Search                             │
│ Booking / Rental Services                 │
└─────────────────────┬─────────────────────┘
                      │
                      ▼
┌───────────────────────────────────────────┐
│               PERSISTENCE                 │
│                                           │
│ CSV Load / Save                           │
│ data/donthue_xe.csv                       │
└───────────────────────────────────────────┘
```

## Cấu trúc project thực tế

```
DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/
├── CMakeLists.txt
├── README.md
├── docs/                    # Đặc tả và báo cáo thành viên
├── data/donthue_xe.csv      # CSV mặc định
├── src/
│   ├── core/
│   │   ├── algorithms/      # Merge Sort, Binary Search, kiểm tra ngày
│   │   ├── model/           # Booking, RentalRecord, Rental
│   │   ├── services/        # Persistence, CSV, nghiệp vụ
│   │   └── structures/      # Hash Table, Heap, Trie, Stack
│   └── presentation/        # CLI MC1, MC2, RF1, RF3
├── web/                     # Frontend, Node API, C++ bridge, start script
├── test/                    # Automated và demo tests
└── benchmark/               # Benchmark MC1, MC2, RF2
```

## Đặc tả chức năng

| Yêu cầu | Mục tiêu nghiệp vụ | Cấu trúc chính | File triển khai |
|---|---|---|---|
| MC1 | Tra đơn theo Booking ID hoặc biển số nhanh | `MyHashTable` Separate Chaining | `src/core/structures/MyHashTable.h`, `Persistence.*` |
| MC2 | Lọc đơn theo khoảng ngày thuê và xem Top xe | Merge Sort + Sorted Array + Binary Search | `RentalService.*`, `MergeSort.h`, `BinarySearch.h` |
| RF1 | Gợi ý hãng/dòng xe theo tiền tố gõ | `CarTrie` | `Trie.h`, `mainRF1.cpp`, `web/core_bridge.cpp` |
| RF2 | Xếp request theo hạng thành viên/thời điểm đặt | `MyMaxHeap` + index map | `MyMaxHeap.h`, API `/api/priority*` |
| RF3 | Undo thao tác ADD/UPDATE/DELETE theo LIFO | Linked Stack + Undo Manager | `UndoStack.*`, `RentalSystem.*`, `MainRF3.cpp` |

### Quy tắc và phạm vi hiện thực

- Ngày truy vấn MC2 được lấy từ **ngày bắt đầu thuê** (`ngay_bat_dau`). Mã hiện tại chưa lọc theo điều kiện khoảng thuê của booking giao nhau với khoảng tìm kiếm.
- RF2 hiện xếp thứ tự các request được gửi vào queue. Phát hiện xe cuối cùng còn trống, kiểm tra lịch khả dụng và đồng bộ heap mỗi khi booking đổi trạng thái chưa được tích hợp.
- CLI RF3 dùng `UndoStack` trong bộ nhớ. Web Undo dùng snapshot dữ liệu trước thay đổi trong bộ nhớ Node.js. Cả hai lịch sử mất khi chương trình tương ứng dừng và không dùng chung lịch sử.
- Hàng đợi RF2 web cũng ở RAM, mất khi server restart. Đơn thuê được lưu bền vững trong CSV.
- Các mục tiêu và sai khác so với đặc tả đề tài được trình bày rõ hơn trong [docs/SPECIFICATION.md](docs/SPECIFICATION.md).

## Mô hình dữ liệu

Dữ liệu dùng chung là `data/donthue_xe.csv` với 11 cột:

```text
booking_id,bien_so,ten_khach,hang_xe,dong_xe,ngay_bat_dau,ngay_ket_thuc,trang_thai,gia_tien,hang_thanh_vien,thoi_diem_dat
```

Ngày lưu theo `YYYY-MM-DD`; `hang_thanh_vien` có miền 0–3; `thoi_diem_dat` là Unix milliseconds; trạng thái thông dụng là `DANG_THUE`, `DA_TRA`, `DA_HUY`. Core còn đọc được một số schema CSV cũ và chuẩn hóa về schema này khi ghi.

Trong C++ hiện có ba model liên quan: `Booking` cho schema/persistence, `RentalRecord` cho MC2 và `renTal` cho chương trình RF3. Các model được chuyển đổi tại service/adapter, chưa hợp nhất thành một kiểu dữ liệu.

## API web

Các route API chính:

| Method | Route | Chức năng |
|---|---|---|
| GET | `/api/rentals?q=&from=&to=` | Danh sách, tìm chuỗi, lọc ngày bắt đầu |
| POST | `/api/rentals` | Tạo đơn |
| PUT / DELETE | `/api/rentals/{booking_id}` | Sửa / xóa đơn |
| POST | `/api/undo` | Hoàn tác snapshot gần nhất của phiên web |
| GET | `/api/search?key=` | Tìm theo mã đơn hoặc biển số |
| GET | `/api/stats`, `/api/top-cars?limit=` | Thống kê và Top xe |
| GET | `/api/suggest?prefix=` | Gợi ý Trie |
| GET | `/api/priority`; POST `/api/priority` | Xem / thêm request vào hàng đợi RF2 |
| POST | `/api/priority/next` | Lấy request ưu tiên nhất |
| GET | `/api/benchmark?from=&to=` | Chạy benchmark Range Query có trong Core |

API bind tại `127.0.0.1:3000` mặc định. Request tạo đơn nhận JSON theo các trường schema ở trên. Mô tả query, response, mã lỗi và payload mẫu được ghi trong [đặc tả](docs/SPECIFICATION.md#6-ứng-dụng-web-và-api).

## Chạy bản web tích hợp

Trên Windows, chạy `./web/start.ps1`. Script build các chương trình C++ bằng CMake rồi chạy web tại `http://localhost:3000`.

Web dùng API Node.js làm presentation adapter và gọi `RentalWebCore` để kết nối `Persistence`, `MyHashTable`, `RentalService`, Trie và `MyMaxHeap`. Schema CSV ở mục [Mô hình dữ liệu](#mô-hình-dữ-liệu); Persistence còn đọc được một số schema cũ rồi chuẩn hóa khi ghi.

Chi tiết endpoint và giới hạn dữ liệu RF2/Undo trong phiên xem [web/README.md](web/README.md).

## Build và chạy chương trình C++

Yêu cầu CMake 3.16 trở lên, compiler hỗ trợ C++17 và Node.js nếu chạy web. Build từ thư mục gốc:

```powershell
cmake -S . -B build/web-integration
cmake --build build/web-integration --config Release
```

Các target chính:

| Target | Mục đích |
|---|---|
| `CarRentalApp` | CLI RF3: CRUD, tra cứu, Undo, test RF3 và benchmark MC1 |
| `RentalMC1Demo` | CLI MC1: tra cứu theo mã/biển số và thêm đơn |
| `RentalMC2Demo` | CLI MC2: lọc theo ngày, Top-N, benchmark tương tác |
| `RentalWebCore` | Bridge giữa Node.js API và C++ Core |
| `BenchmarkMC1` | So sánh MyHashTable với Linear Scan tại 1k/10k/100k đơn |
| `BenchmarkMC2` | So sánh Binary Search Range Query với Linear Scan |

`src/presentation/mainRF1.cpp` là demo Trie RF1 độc lập nhưng hiện chưa được khai báo thành target CMake. Có thể biên dịch trực tiếp bằng MinGW/GCC sau khi tạo thư mục `build`:

```powershell
g++ -std=c++17 src/presentation/mainRF1.cpp -o build/RF1Demo.exe
./build/RF1Demo.exe
```

Ví dụ chạy MC2 CLI:

```powershell
cmake --build build/web-integration --config Release --target RentalMC2Demo
./build/web-integration/RentalMC2Demo.exe
```

## Kiểm thử tự động

Hai target được đăng ký trong CTest:

```powershell
cmake --build build/web-integration --config Release --target TestMC2Correctness TestMC1Persistence
ctest --test-dir build/web-integration -C Release --output-on-failure
```

`TestMC2Correctness` bao gồm kiểm tra sắp xếp, khoảng ngày, biên ngày, thống kê Top-K và dữ liệu rỗng. `TestMC1Persistence` kiểm tra hash table và CSV. RF3 automated test chạy từ menu `CarRentalApp` mục 9; RF2 có chương trình demo/test trong `test/test_thanhvien3.cpp`.

## Benchmark MC2

Benchmark MC2 độc lập xác nhận kết quả giữa hai phương pháp trước khi đo thời gian:

```powershell
cmake --build build/web-integration --config Release --target BenchmarkMC2
./build/web-integration/BenchmarkMC2.exe
```

Tham số tùy chọn theo thứ tự: `csv`, ngày bắt đầu, ngày kết thúc, số lần lặp. Benchmark là phép đo thực tế, thời gian phụ thuộc máy, compiler, build mode và dữ liệu.
