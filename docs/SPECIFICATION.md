# Đặc tả hệ thống cho thuê xe tự lái

Tài liệu này mô tả hành vi của chương trình theo mã nguồn hiện có tại thời điểm cập nhật. Những mục được ghi là “giới hạn hiện tại” phản ánh phạm vi của đồ án, không phải yêu cầu đã triển khai.

## 1. Mục tiêu và phạm vi

Đề tài xuất phát từ nhu cầu tra cứu lượng lớn lịch sử thuê xe nhanh, tìm xe phổ biến, gợi ý tên xe, xử lý công bằng các yêu cầu cùng tranh một xe và khôi phục khi nhân viên thao tác nhầm. Chương trình quản lý các đơn thuê xe tự lái; trọng tâm đồ án là hiện thực và sử dụng cấu trúc dữ liệu/thuật toán cho các access pattern sau:

| Mã | Yêu cầu | Thao tác chính |
|---|---|---|
| MC1 | Tra cứu chính xác đơn thuê | Tìm theo `booking_id` hoặc `bien_so` |
| MC2 | Truy vấn và thống kê đơn thuê | Lọc theo ngày thuê; xếp hạng xe theo số lượt thuê |
| RF1 | Gợi ý tên xe | Tra cứu tiền tố hãng xe, dòng xe hoặc phần tên dòng |
| RF2 | Ưu tiên yêu cầu thuê | Lấy yêu cầu có độ ưu tiên cao nhất |
| RF3 | Hoàn tác thao tác dữ liệu | Undo ADD, UPDATE, DELETE theo thứ tự LIFO |

Phần trình bày gồm các ứng dụng console C++ và ứng dụng web local. Web dùng HTML/CSS/JavaScript cho giao diện, Node.js built-in HTTP server làm API adapter và chương trình `RentalWebCore` để gọi C++ Core. CSV là nơi lưu dữ liệu thuê bền vững.

### Yêu cầu thiết kế từ bản đặc tả đề tài

- Có tối thiểu hai cấu trúc dữ liệu trung tâm tự cài đặt: `MyHashTable` cho MC1 và `MyMaxHeap` cho RF2.
- Các thao tác tính toán chính chạy trong RAM sau khi nạp CSV; Persistence chỉ đảm nhiệm load/save, không thay Core thực hiện truy vấn.
- Giải quyết yêu cầu xung đột bằng composition: cấu trúc sắp xếp theo ngày phục vụ MC2 và heap theo khóa `(hạng thành viên, thời điểm đặt)` phục vụ RF2. Hai access pattern không thể thay thế lẫn nhau.
- Bộ dữ liệu/benchmark cần thể hiện tải tăng dần đến hàng chục nghìn hoặc nhiều hơn bản ghi. Benchmark MC1 trong code chạy ở các mốc 1.000, 10.000 và 100.000.

Các phần sau phân biệt mục tiêu thiết kế đó với hành vi tích hợp hiện tại để không trình bày ý tưởng chưa có trong code như chức năng đã hoàn tất.

## 2. Người dùng và đầu vào/đầu ra

### Đầu vào

- Thông tin đơn thuê theo schema tại mục 4.
- Khóa tra cứu là mã đơn hoặc biển số xe.
- Khoảng ngày bắt đầu/kết thúc truy vấn.
- Số lượng xe muốn xem trong kết quả Top-K.
- Từ khóa tiền tố tìm hãng/dòng xe.
- Yêu cầu thêm vào/lấy khỏi hàng đợi ưu tiên.
- Yêu cầu thêm, sửa, xóa hoặc Undo.

### Đầu ra

- Danh sách đơn thuê hoặc một đơn tìm thấy.
- Các đơn có ngày bắt đầu trong khoảng đã chọn.
- Thống kê số đơn và Top xe.
- Danh sách gợi ý xe theo Trie.
- Thứ tự yêu cầu ưu tiên và yêu cầu kế tiếp.
- Dữ liệu sau thao tác hoặc trạng thái được khôi phục sau Undo.
- Thời gian benchmark và xác nhận hai phương pháp truy vấn cho kết quả khớp.

## 3. Kiến trúc chương trình

```text
┌───────────────────┐       ┌───────────────────────┐
│ CLI C++           │       │ Web browser           │
│ MC1 / MC2 / RF1 /  │       │ index.html + app.js   │
│ RF3               │       └───────────┬───────────┘
└─────────┬─────────┘                   │ HTTP/JSON
          │                              ▼
          │                    ┌───────────────────────┐
          │                    │ web/server.js         │
          │                    │ Node.js HTTP adapter  │
          │                    └───────────┬───────────┘
          │                       gọi process C++
          │                              ▼
          │                    ┌───────────────────────┐
          └───────────────────►│ RentalWebCore         │
                               │ JSON/CSV ↔ C++ Core    │
                               └───────────┬───────────┘
                                           ▼
                               ┌───────────────────────┐
                               │ C++ Core              │
                               │ models/services/DSA   │
                               └───────────┬───────────┘
                                           ▼
                               data/donthue_xe.csv
```

### Trách nhiệm các tầng

- **Presentation:** CLI nhận input/in kết quả; web browser hiển thị và gửi request.
- **HTTP adapter:** `web/server.js` định tuyến API, kiểm tra đầu vào cơ bản, gọi bridge và trả JSON. Không cần npm dependencies.
- **Core bridge:** `web/core_bridge.cpp` chuyển dữ liệu giữa Node.js và C++ Core qua đối số dòng lệnh/stdin/stdout.
- **Core:** model, persistence, business logic, DSA và thuật toán.
- **Persistence:** `Persistence.*` và `CSVUtils.*` đọc/ghi CSV; truy vấn MC1/MC2 được thực hiện trong cấu trúc/logic Core sau khi nạp dữ liệu.

## 4. Mô hình và quy tắc dữ liệu

### Schema CSV chuẩn

`data/donthue_xe.csv` là file mặc định. Header chuẩn, đúng thứ tự:

| Cột | Kiểu trong model | Ý nghĩa | Quy ước |
|---|---|---|---|
| `booking_id` | string | Mã đơn thuê | Dùng làm khóa đơn; hệ thống từ chối trùng khi tạo qua web/Core |
| `bien_so` | string | Biển số xe | Khóa tra cứu phụ MC1 |
| `ten_khach` | string | Tên khách thuê | Văn bản |
| `hang_xe` | string | Hãng xe | Ví dụ `Toyota` |
| `dong_xe` | string | Dòng xe | Ví dụ `Vios` |
| `ngay_bat_dau` | string | Ngày bắt đầu thuê | Web nhập `YYYY-MM-DD`; MC2 so sánh theo thứ tự chuỗi ISO |
| `ngay_ket_thuc` | string | Ngày kết thúc thuê | Ngày kết thúc không trước ngày bắt đầu khi thêm/sửa qua RentalSystem |
| `trang_thai` | string | Trạng thái đơn | Giá trị mặc định `DANG_THUE`; dữ liệu dùng các giá trị `DANG_THUE`, `DA_TRA`, `DA_HUY` |
| `gia_tien` | double | Giá thuê | Không âm ở kiểm tra API web; mặc định `0` |
| `hang_thanh_vien` | int | Hạng thành viên | RF2 nhận hạng từ `0` đến `3` |
| `thoi_diem_dat` | long long | Thời điểm tạo đơn | Unix milliseconds; dùng để phá hòa RF2 |

Persistence có hỗ trợ header CSV cũ của MC1/MC2; khi ghi lại, dữ liệu được chuẩn hóa về schema 11 cột. Trường không có trong file cũ được gán mặc định theo parser (giá/hạng/thời điểm thường là `0`).

### Model C++

- `Booking` là bản ghi chính phục vụ unified schema, serialization CSV và JSON bridge.
- `RentalRecord` là model dùng cho MC2, có chuyển đổi `fromBooking()`/`toBooking()`.
- `renTal` là model tối giản được RentalSystem/RF3 sử dụng (mã đơn, khách, biển số, ngày bắt đầu/kết thúc).

Các model trên có mục đích tương thích với từng module; chúng chưa được hợp nhất thành một struct duy nhất.

### Quy tắc ngày

MC2 cần ngày dạng ISO `YYYY-MM-DD` để so sánh chuỗi tương đương thứ tự thời gian. RentalSystem C++ xác thực ngày tồn tại trong lịch và ngày đầu không sau ngày cuối. API web kiểm tra mẫu ISO và thứ tự ngày trước khi gọi Core; dữ liệu nhập từ CSV không được xác thực lại toàn bộ theo quy tắc tạo/sửa đơn.

## 5. Đặc tả chức năng và thiết kế DSA

### 5.1 MC1 — Tra cứu đơn

**Cấu trúc:** `MyHashTable<V>` dùng Separate Chaining, hàm băm tự cài đặt và resize khi load factor vượt ngưỡng.

**Chỉ mục:** Persistence nạp các `Booking` vào bộ nhớ, sau đó tạo bảng theo `booking_id` và bảng theo `bien_so`. Bảng băm là cấu trúc tra cứu; file CSV chỉ là nguồn lưu trữ, không được dùng để tìm từng bản ghi trực tiếp.

**Độ phức tạp dự kiến:** O(1) trung bình cho insert/search/remove; O(n) trường hợp xấu nhất khi xung đột tập trung.

### 5.2 MC2 — Truy vấn ngày và Top xe

**Lọc theo ngày:** `RentalService::sortByRentDate()` sắp xếp theo `rentDate` tăng dần bằng Merge Sort (nếu cùng ngày, so tiếp `bookingId`). `queryByDateRange()` dùng lower bound và upper bound để lấy khoảng đóng `[fromDate, toDate]`. Trường được truy vấn là ngày bắt đầu thuê (`ngay_bat_dau`/`rentDate`).

**Độ phức tạp:** sắp xếp O(n log n); tìm biên O(log n); xuất k kết quả O(k), tổng O(log n + k) sau khi đã sắp xếp.

**Top xe:** `buildCarStats()` sắp xếp theo biển số rồi quét nhóm để đếm số đơn trên mỗi biển số. `topRentedCars()` dùng Merge Sort giảm dần theo lượt thuê và trả tối đa K kết quả. `topK <= 0` trả danh sách rỗng; `topK` lớn hơn số xe trả toàn bộ.

**CLI:** `RentalMC2Demo` nạp CSV, cho phép truy vấn ngày, Top-N, xem dữ liệu và benchmark tương tác.

### 5.3 RF1 — Gợi ý xe bằng Trie

`CarTrie::insertCar(brand, model)` lập chỉ mục tên đầy đủ hãng + dòng, đồng thời lập chỉ mục các hậu tố từ khóa dòng xe để có thể gõ riêng tên dòng. `getAllSuggestions(prefix)` chuẩn hóa chữ thường/khoảng trắng đầu-cuối và trả tên xe khớp tiền tố.

Trong web, bridge dựng Trie từ hãng/dòng xe của các bản ghi đã nạp và trả tối đa 8 kết quả cho API suggest. CLI RF1 (`mainRF1.cpp`) dùng danh mục xe mẫu trong bộ nhớ.

### 5.4 RF2 — Hàng đợi ưu tiên

`MyMaxHeap` lưu request trong max heap và dùng `MyHashTable<int>` ánh xạ `bookingId` sang index để xóa/cập nhật vị trí.

Thứ tự ưu tiên:

1. Hạng thành viên cao hơn đứng trước.
2. Nếu cùng hạng, thời điểm đặt sớm hơn đứng trước.
3. Nếu cùng hạng và timestamp, `bookingId` tăng dần đứng trước để kết quả xác định.

Insert/ExtractMax/RemoveById có độ phức tạp O(log n) (ngoài chi phí trung bình của bảng ánh xạ); BuildHeap O(n). Hàng đợi RF2 của web giữ trong RAM tiến trình, không ghi vào CSV.

**Giới hạn so với kịch bản đặc tả:** module hiện xếp thứ tự các request được gửi vào queue. Nó chưa tự phát hiện hai đơn tranh cùng một xe cuối cùng, chưa kiểm tra lịch xe trống, và chưa tự cập nhật heap khi mọi booking CRUD thay đổi. Đây là demo cấu trúc ưu tiên, chưa phải bộ điều phối tồn kho/khả dụng xe đầy đủ.

### 5.5 RF3 — CRUD và Undo

`renTalSystem` cung cấp add/update/delete/search/undo. `Mystack` là Stack liên kết đơn; `UnoManager` quản lý các action.

- Undo ADD: xóa bản ghi vừa thêm.
- Undo UPDATE: phục hồi bản ghi cũ.
- Undo DELETE: chèn lại bản ghi tại vị trí trước đó.
- Thao tác mới nhất được undo trước (LIFO). Undo khi stack rỗng trả thất bại, không gây lỗi.

Push/pop/top của Stack O(1). Toàn bộ Undo có thể O(n) do tìm/xóa/chèn trong vector RentalSystem. CLI RF3 có menu tự động test.

**Hai luồng Undo hiện tại:** CLI RF3 gọi `renTalSystem` và `UndoStack` trong cùng tiến trình. Web lưu snapshot trước thao tác trong một mảng JavaScript của server rồi khôi phục CSV qua bridge. Snapshot web cũng chỉ sống trong RAM và mất khi server dừng; nó không phải cùng một lịch sử với Stack của CLI.

### 5.6 Đối chiếu mục tiêu đề tài với phần tích hợp

| Mục tiêu trong đặc tả | Hành vi có trong code hiện tại |
|---|---|
| MC2 lọc các lượt thuê nằm trong khoảng thời gian thuê | Query theo ngày bắt đầu thuê trong khoảng; chưa lọc mọi đơn có khoảng thuê giao nhau với khoảng ngày tìm kiếm |
| MC2 và RF2 cùng phục vụ hai access pattern khác nhau | Có Merge Sort/Binary Search cho MC2 và heap RF2; web giữ priority queue riêng trong RAM, chưa đồng bộ heap cùng mọi thay đổi booking |
| RF2 xử lý tranh chấp xe cuối cùng còn trống | API cho nhập/lấy request ưu tiên; chưa có phát hiện tranh chấp hay kiểm tra xe cuối cùng/khả dụng |
| RF3 Undo thao tác gần nhất | CLI có Stack Undo ADD/UPDATE/DELETE; web có snapshot Undo; hai lịch sử độc lập và không bền vững sau restart |
| Lưu booking qua nhiều phiên | Web/Core đọc/ghi CSV thống nhất; không dùng database SQL |

Các hàng có ghi “chưa” là phần mở rộng cần thiết nếu muốn hệ thống đáp ứng đầy đủ kịch bản vận hành thực tế trong bản đặc tả.

## 6. Ứng dụng web và API

Web mặc định bind `127.0.0.1:3000`. API trả JSON. Các route hiện có:

| Method | Route | Tham số / body | Hành vi |
|---|---|---|---|
| GET | `/api/rentals` | `q`, `from`, `to` tùy chọn | Danh sách; tìm chuỗi theo mã/biển số/khách/hãng/dòng và lọc ngày bắt đầu |
| POST | `/api/rentals` | Booking JSON | Tạo đơn; kiểm tra trường bắt buộc, ngày, giá và hạng |
| PUT | `/api/rentals/{booking_id}` | Các trường cần cập nhật | Sửa đơn theo mã |
| DELETE | `/api/rentals/{booking_id}` | — | Xóa đơn theo mã |
| POST | `/api/undo` | — | Khôi phục snapshot dữ liệu gần nhất của phiên web |
| GET | `/api/search?key=...` | `key` là mã đơn hoặc biển số | Tra cứu bằng MyHashTable |
| GET | `/api/stats` | — | Tổng đơn, số đơn theo trạng thái, Top hãng/dòng và Top biển số |
| GET | `/api/top-cars?limit=5` | `limit` từ 1 đến 20 | Top xe theo biển số/lượt thuê |
| GET | `/api/suggest?prefix=...` | prefix tối thiểu 2 ký tự | Gợi ý Trie |
| GET | `/api/priority` | — | Trả heap hiện tại dưới dạng danh sách ưu tiên |
| POST | `/api/priority` | `booking_id`, `membership_tier`?, `booking_timestamp`? | Thêm yêu cầu vào heap; mặc định lấy hạng/thời điểm từ đơn hoặc thời gian hiện tại |
| POST | `/api/priority/next` | — | Lấy và loại yêu cầu ưu tiên nhất khỏi hàng đợi |

Ví dụ tạo đơn:

```json
{
  "booking_id": "RENT_API_001",
  "bien_so": "51A-111.22",
  "ten_khach": "Nguyen Van A",
  "hang_xe": "Toyota",
  "dong_xe": "Vios",
  "ngay_bat_dau": "2026-10-01",
  "ngay_ket_thuc": "2026-10-02",
  "trang_thai": "DANG_THUE",
  "gia_tien": 1250000,
  "hang_thanh_vien": 2
}
```

Lỗi API được trả dưới dạng JSON `{ "error": "..." }`; status thường gặp: 400 dữ liệu/ thao tác không hợp lệ, 404 không tìm thấy đơn hoặc route, 409 trùng mã/đã vào hàng đợi. Cấu trúc response chi tiết xem trong `web/server.js` và [web/README.md](../web/README.md).

### Ví dụ gọi API bằng PowerShell

```powershell
$base = 'http://127.0.0.1:3000'
Invoke-RestMethod "$base/api/stats"
Invoke-RestMethod "$base/api/search?key=RENT_HCM_000001"
Invoke-RestMethod "$base/api/rentals?from=2026-01-01&to=2026-12-31"
```

API web dùng cùng file CSV mặc định như Core. Muốn thử an toàn, đặt `RENTAL_DATA_FILE` trỏ tới một bản CSV sao chép trước khi khởi động server.

## 7. Giao diện dòng lệnh

### `CarRentalApp` — RF3

| Mục | Hành vi |
|---:|---|
| 1–3 | Thêm / cập nhật / xóa đơn |
| 4–5 | Tìm kiếm / hiển thị toàn bộ |
| 6–8 | Undo / xem số action / xem action trên đỉnh Stack |
| 9–10 | Automated test / benchmark MC1 |
| 0 | Thoát |

Ứng dụng này là demo RF3 trên `renTalSystem` trong bộ nhớ; web mới là luồng CRUD bền vững qua CSV/Persistence.

### `RentalMC1Demo` — MC1

Nạp dữ liệu vào hai hash table theo mã đơn và biển số, cho phép tra cứu, thêm đơn và lưu khi thoát.

### `RentalMC2Demo` — MC2

Nạp CSV, sau đó dùng menu để lọc khoảng ngày, xem Top-N, benchmark Binary Search/Linear Scan hoặc xem dữ liệu rút gọn.

### `mainRF1.cpp` — RF1

Demo Trie console độc lập, dùng danh mục hãng/dòng xe mẫu. Phần RF1 trên web gọi cùng `CarTrie` qua core bridge.

## 8. Build, test, benchmark và chạy web

### Build

```powershell
cmake -S . -B build/web-integration
cmake --build build/web-integration --config Release
```

Build một target:

```powershell
cmake --build build/web-integration --config Release --target RentalWebCore
cmake --build build/web-integration --config Release --target RentalMC2Demo
cmake --build build/web-integration --config Release --target BenchmarkMC2
```

### Automated tests

```powershell
cmake --build build/web-integration --config Release --target TestMC2Correctness TestMC1Persistence
ctest --test-dir build/web-integration -C Release --output-on-failure
```

Các bài test chưa đăng ký với CTest, gồm RF2 demo trong `test/test_thanhvien3.cpp` và Automated test RF3 gọi từ menu `CarRentalApp` mục 9.

Các executable CLI, test và benchmark có target `run_<target>` để build rồi chạy trực tiếp. Danh sách đầy đủ gồm `run_CarRentalApp`, `run_RentalMC1Demo`, `run_RentalMC2Demo`, `run_RentalRF1Demo`, `run_DemoRF2Heap`, `run_BenchmarkMC1`, `run_BenchmarkMC2`, `run_BenchmarkRF2`, `run_TestMC1Persistence`, `run_TestMC2Correctness` và `run_TestRF1Trie`. Xem README gốc để biết chức năng từng target. File `.cpp` không có `main()` là module thư viện và được liên kết vào các chương trình này.

### Benchmark MC2

```powershell
./build/web-integration/BenchmarkMC2.exe [csv] [from] [to] [repeats]
```

Mặc định: `data/donthue_xe.csv`, khoảng `2026-01-01` đến `2026-12-31`, 1.000 lần lặp. Chương trình xác nhận Binary Search và Linear Scan trả cùng Booking ID trước khi in thời gian.

### Chạy web

```powershell
./web/start.ps1
```

Mở `http://localhost:3000`; nhấn `Ctrl+C` trong terminal để dừng. Hướng dẫn Windows/Linux/macOS và API xem tại [README web](../web/README.md).

## 9. Cấu trúc file và trách nhiệm

| Đường dẫn | Trách nhiệm |
|---|---|
| `src/core/model/Booking.h` | Bản ghi chuẩn và chuyển đổi dòng CSV |
| `src/core/model/RentalRecord.h` | Model và chuyển đổi dùng cho MC2 |
| `src/core/model/Rental.h` | Model đơn tối giản dùng cho RF3 |
| `src/core/structures/MyHashTable.h` | Hash table Separate Chaining cho MC1 và index phụ |
| `src/core/structures/MyMaxHeap.h` | Max heap RF2, heap index map |
| `src/core/structures/Trie.h` | `CarTrie` cho prefix search RF1 |
| `src/core/structures/UndoStack.h`, `UndoStack.cpp` | Stack và UndoManager RF3 |
| `src/core/algorithms/MergeSort.h` | Merge Sort template cho MC2 |
| `src/core/algorithms/BinarySearch.h` | Lower/upper bound theo ngày |
| `src/core/algorithms/Rental.cpp` | Kiểm tra ngày, năm nhuận và thứ tự ngày |
| `src/core/services/Persistence.*` | Đọc/ghi CSV và nạp chỉ mục booking |
| `src/core/services/CSVUtils.*`, `CsvCodec.h` | Đọc CSV MC2 và encode/decode CSV |
| `src/core/services/DataGenerator.*` | Sinh CSV dữ liệu giả lập phục vụ demo |
| `src/core/services/RentalService.*` | Range Query, thống kê xe, Top-K, benchmark cũ |
| `src/core/services/RentalSystem.*` | CRUD, tìm kiếm và Undo RF3 trên model `renTal` |
| `src/presentation/demo_mc1.cpp` | CLI MC1 |
| `src/presentation/mainMC2.cpp` | CLI MC2 |
| `src/presentation/mainRF1.cpp` | Demo Trie RF1 |
| `src/presentation/MainRF3.cpp`, `InputHandler.*` | CLI RF3 |
| `web/server.js`, `web/app.js`, `web/index.html`, `web/styles.css` | HTTP API và giao diện browser |
| `web/core_bridge.cpp` | Gọi các Core service/DSA từ web |
| `test/test_thanhvien1.cpp` | Test hash table và persistence MC1 |
| `test/test_thanhvien2.cpp` | Test correctness MC2 |
| `test/test_thanhvien3.cpp` | Chương trình demo/test heap RF2 |
| `test/Tests.cpp` | Automated tests RF3 |
| `benchmark/benchmark_mc1.cpp` | Benchmark MC1 |
| `benchmark/benchmark-MC2.cpp` | Benchmark độc lập Range Query MC2 |
| `benchmark/benchmark_rf2.cpp` | Benchmark heap RF2 |

## 10. Giới hạn hiện tại

- Ứng dụng được thiết kế cho demo local; server chỉ bind loopback, không có đăng nhập/phân quyền.
- Hàng đợi RF2 web và snapshot Undo web chỉ nằm trong RAM, mất khi Node server khởi động lại. Dữ liệu đơn thuê chính vẫn lưu CSV.
- CLI RF3 thao tác trên `renTalSystem` trong bộ nhớ, không tự đồng bộ CRUD với CSV.
- `Booking`, `RentalRecord` và `renTal` là ba model phục vụ các luồng khác nhau; dữ liệu được chuyển đổi ở các adapter/service.
- `mainRF1.cpp` là demo RF1 độc lập nhưng chưa được đăng ký thành target CMake; web RF1 được gọi qua `RentalWebCore`.
- Benchmark phụ thuộc máy, compiler, build mode, khoảng ngày và số lượng bản ghi; không nên xem một lần chạy là cam kết.
- Định dạng ngày dùng trong MC2 phải là `YYYY-MM-DD` để phép so sánh chuỗi cho thứ tự đúng.

## 11. Tài liệu liên quan

- [README dự án](../README.md)
- [Hướng dẫn web và API](../web/README.md)
- [Tài liệu RF2](LyNguyenManhThuyen.md)
- [Tài liệu RF3](LeNguyenMinhThu.md)
- [Đánh giá MC1](NguyenHuuThinh.md)
- [Tổng hợp kiểm thử/hiệu năng nhóm](nhóm.md)
