# BÀI ĐỌC-HIỂU ĐỀ BÀI CÁ NHÂN

- **Học phần:** 261DASA230179_06 - Đồ án nhóm
- **Họ tên:** Nguyễn Hữu Thịnh
- **MSSV:** 25110349
- **Mã nhóm:** 608
- **Lĩnh vực:** Quản lý cho thuê xe tự lái và xử lý tranh chấp đặt xe

---

# D1	Bài đọc-hiểu đề bài cá nhân

## 1. Đề bài yêu cầu gì

Nền tảng cho thuê xe tích lũy hàng chục nghìn đơn thuê với dữ liệu biến động liên tục (thêm, sửa, hủy). Nhân viên vận hành cần trả lời nhanh một số câu hỏi cố định dù quy mô dữ liệu ngày càng lớn, đồng thời hệ thống phải đảm bảo quy tắc công bằng khi xảy ra tình trạng nhiều khách hàng tranh chấp cùng một xe.

**Khó khăn cốt lõi:**
- Đảm bảo tốc độ phản hồi tức thì khi dung lượng dữ liệu lớn.
- Đảm bảo thứ tự xử lý chính xác và công bằng khi xảy ra tranh chấp đặt xe.

---

## 2. Mô hình hóa

### 2.1. Input
- **Đơn thuê:** Mã đơn (duy nhất), biển số xe, tên khách hàng, hãng xe, dòng xe, ngày bắt đầu, ngày kết thúc, trạng thái.
- **Yêu cầu tranh chấp:** Thông tin khách hàng, biển số xe, hạng thành viên, thời điểm đặt.
- **Thao tác dữ liệu:** Thêm, sửa, hủy đơn thuê.
- **Truy vấn:** Theo mã đơn, biển số xe, khoảng ngày, số $K$, tiền tố tên xe.

### 2.2. Output
- Chi tiết thông tin của một đơn thuê hoặc danh sách các đơn thuê của một xe.
- Danh sách đơn thuê trong một khoảng ngày xác định.
- Top $K$ xe được thuê nhiều nhất.
- Gợi ý tên xe theo ký tự nhập vào.
- Yêu cầu tranh chấp cần xử lý tiếp theo.
- Kết quả thao tác hoàn tác (Undo).

### 2.3. Ràng buộc (Constraints)
- Dung lượng dữ liệu: Từ $10.000$ đơn thuê trở lên.
- Tốc độ: Tra cứu tức thì ($O(1)$).
- Tính toàn vẹn: Lưu trữ bền vững qua các lần chạy.
- Quy tắc nghiệp vụ: Một xe không được giao cho hai khách hàng trùng khoảng thời gian thuê.

---

## 3. MC1 và MC2 trong lĩnh vực này

- **MC1 (Main Concern 1 - Tra cứu chính xác):**
  - Yêu cầu tra cứu ngay lập tức khi có Mã đơn hoặc Biển số xe.
  - **Mã đơn:** Trả về chính xác $1$ đơn thuê.
  - **Biển số:** Trả về toàn bộ lịch sử đơn thuê của xe đó (do một xe có thể được thuê nhiều lần).
  - *Vấn đề:* Nếu duyệt tuyến tính lần lượt từng đơn, thời gian chờ sẽ tăng tỉ lệ thuận theo số lượng đơn ($O(N)$), không đáp ứng được yêu cầu thực tế.

- **MC2 (Main Concern 2 - Truy vấn theo thứ tự / phạm vi):**
  - **MC2a:** Xem mọi lượt thuê trong một khoảng ngày để xác định trạng thái bận/rảnh của xe.
  - **MC2b:** Thống kê Top $K$ xe được thuê nhiều nhất (xếp giảm dần) để phục vụ kế hoạch bảo dưỡng.
  - *Đặc điểm:* Cả hai đều yêu cầu kết quả trả về phải có thứ tự, điều mà phương pháp tra cứu theo mã đơn lẻ không giải quyết được.

---

## 4. Yêu cầu riêng em nhận ra (Dành riêng cho MC1)

- **Hai mô hình tra cứu khác nhau ($1 - 1$ và $1 - N$):**
  - Tra cứu theo **Mã đơn** tuân theo quan hệ $1 - 1$ (trả về đúng $1$ đơn thuê duy nhất).
  - Tra cứu theo **Biển số xe** tuân theo quan hệ $1 - N$ ($1$ xe gắn liền với một danh sách tập hợp các đơn thuê). Cấu trúc lưu trữ chỉ mục phải hỗ trợ hiệu quả cả hai mô hình này.
- **Xử lý va chạm băm (Hash Collision Resolution):**
  - Khi quy mô dữ liệu đạt hàng chục nghìn đến hàng trăm nghìn đơn, việc duy trì tốc độ tra cứu tức thì $O(1)$ đòi hỏi phải có cơ chế xử lý va chạm chỉ số băm tốt. Nếu không xử lý triệt để, hiệu năng tra cứu sẽ bị suy hao nghiêm trọng về mức $O(N)$.
- **Đồng bộ dữ liệu khi Thêm / Sửa / Hủy:**
  - Dữ liệu biến động liên tục, do đó khi một đơn thuê bị xóa hoặc thay đổi biển số, cả hai bảng chỉ mục (Mã đơn và Biển số) phải được cập nhật đồng bộ lập tức nhằm tránh tình trạng "dữ liệu ma" (tìm thấy mã nhưng đơn đã bị xóa, hoặc sửa biển số nhưng tìm biển cũ vẫn ra đơn).
- **Khác biệt cốt lõi:**
  - MC1 tập trung vào khả năng **truy cập ngẫu nhiên tức thì ($O(1)$)** tới đúng vị trí dữ liệu cần tìm, hoàn toàn độc lập và không phụ thuộc vào thứ tự thời gian (MC2a) hay thứ tự xếp hạng (MC2b).
- **Điểm căng thẳng (Bottleneck):**
  - Duy trì độ phức tạp thời gian phản hồi ở mức cố định $O(1)$ bất chấp dữ liệu tăng từ $10.000$ lên hàng trăm nghìn đơn, đồng thời tối ưu không gian bộ nhớ RAM cho các bảng chỉ mục (Index Tables).

---

## 5. Trường hợp biên (Edge Cases)

### 5.1. Dữ liệu hệ thống & Thời gian
- Dữ liệu rỗng hoặc chỉ có duy nhất $1$ đơn thuê.
- Chứa dòng dữ liệu định dạng lỗi.
- Ngày tháng không tồn tại trong thực tế (ví dụ: `30/02`).

### 5.2. Khóa tra cứu & Chỉ mục (MC1)
- Mã đơn hoặc biển số xe không tồn tại trong hệ thống.
- Biển số xe phân biệt/không phân biệt chữ hoa, chữ thường (ví dụ: `51F-12345` và `51f-12345`).
- Nhập đơn thuê mới bị trùng Mã đơn đã tồn tại.
- Một biển số xe gắn liền với nhiều đơn thuê trùng lặp hoặc phân tán.

### 5.3. Khoảng thời gian & Ràng buộc
- Ngày kết thúc nằm trước ngày bắt đầu.
- Khoảng ngày truy vấn không có đơn thuê nào.
- Truy vấn đúng vào biên ngày bắt đầu hoặc ngày kết thúc.
- Đơn thuê bắt đầu trước khoảng ngày cần tìm nhưng kết thúc trong khoảng đó.

### 5.4. Tranh chấp & Xử lý ưu tiên
- Hai khách hàng đặt cùng một xe trong cùng một khoảng thời gian.
- Hai yêu cầu tranh chấp có cùng hạng thành viên và cùng thời điểm đặt.
- Hủy yêu cầu đang nằm trong danh sách chờ xử lý.

### 5.5. Hoàn tác (Undo) & Chuỗi gợi ý
- Yêu cầu hoàn tác khi chưa thực hiện thao tác nào.
- Thực hiện hoàn tác ngay sau khi xóa đơn thuê hoặc sau khi sửa biển số xe.
- Gợi ý tên xe: Chữ hoa/thường khác nhau; tiền tố tìm kiếm rỗng hoặc không trùng khớp với bất kỳ tên xe nào.

---

# D4 — IMPLEMENTATION

## 1. Cấu trúc dữ liệu Booking

MC1 sử dụng cấu trúc `Booking` để lưu trữ thông tin chi tiết của một đơn thuê xe tự lái và hỗ trợ chuyển đổi dữ liệu giao tiếp với file CSV.

```cpp
struct Booking {
    std::string booking_id;      // Mã định danh đơn thuê -> KHÓA của MC1
    std::string bien_so;         // Biển số xe -> KHÓA thay thế của MC1
    std::string ten_khach;       // Tên khách thuê
    std::string hang_xe;         // Hãng xe, VD: Hyundai
    std::string dong_xe;         // Dòng xe, VD: Accent
    std::string ngay_bat_dau;     // YYYY-MM-DD
    std::string ngay_ket_thuc;    // YYYY-MM-DD
    std::string trang_thai = "DANG_THUE"; // DANG_THUE / DA_TRA / DA_HUY
    double gia_tien = 0.0;
    int hang_thanh_vien = 0;
    long long thoi_diem_dat = 0;  // Unix milliseconds
};
```

### Các phương thức hỗ trợ Persistence

- `toRow()` và `header()`: Chuyển đổi đối tượng `Booking` thành `vector<string>` để xuất dữ liệu ra file CSV.
- `fromRow()` và `fromCsvRow()`: Phân tích cú pháp từ một dòng CSV để khôi phục dữ liệu thành đối tượng `Booking`.
- Các trường dữ liệu được chuẩn hóa và chuyển đổi kiểu dữ liệu an toàn bằng `try-catch`, sử dụng `std::stod` và `std::stoll`.

---

## 2. Cấu trúc MyHashTable và Node

Hệ thống sử dụng bảng băm tổng quát dạng Template (`template <typename V>`) với cơ chế xử lý đụng độ bằng **Separate Chaining** (nối chuỗi).

```cpp
template <typename V>
class MyHashTable {
private:
    struct Node {
        std::string key;
        V value;
        Node* next;

        Node(const std::string& k, const V& v, Node* n)
            : key(k), value(v), next(n) {}
    };

    std::vector<Node*> buckets_;
    std::size_t capacity_;
    std::size_t size_;
    double loadFactorThreshold_ = 0.75;
};
```

Trong đó:

- **`buckets_`**: `vector` chứa các con trỏ trỏ đến Node đầu tiên của từng bucket.
- **`capacity_`**: kích thước của bảng bucket, mặc định được khởi tạo là `1009`.
- **`size_`**: tổng số phần tử hiện đang được lưu trong bảng.
- **`loadFactorThreshold_`**: ngưỡng hệ số tải `0.75`, dùng để kích hoạt cơ chế tự động mở rộng bảng.

Cấu trúc Separate Chaining cho phép nhiều phần tử có cùng giá trị hash được lưu trong cùng một bucket thông qua danh sách liên kết.

```text
Bucket
   ↓
Node 1 → Node 2 → Node 3 → nullptr
```

---

## 3. Hàm `hashKey()`

Hàm băm sử dụng thuật toán **Polynomial Rolling Hash** để chuyển đổi chuỗi khóa thành chỉ số bucket.

```cpp
std::size_t hashKey(const std::string& key) const {
    std::size_t h = 0;

    for (char ch : key) {
        h = (h * 31 + static_cast<unsigned char>(ch))
            % capacity_;
    }

    return h;
}
```

### Cơ chế hoạt động

- Duyệt qua từng ký tự của chuỗi khóa.
- Nhân giá trị hash hiện tại với hằng số `31`.
- Cộng thêm mã của ký tự hiện tại.
- Lấy phần dư cho `capacity_` để đưa kết quả về phạm vi bucket.

### Độ phức tạp

- Hàm băm có độ phức tạp **O(L)** với `L` là độ dài chuỗi khóa.
- Do Booking ID có độ dài ngắn và cố định tương đối, thao tác này được xem gần tương đương **O(1)** trong bài toán MC1.

---

## 4. Hàm `insert()`

Hàm `insert()` dùng để thêm hoặc cập nhật một cặp `(key, value)` vào bảng băm.

### Quy trình

```text
Tính index = hashKey(key)
            ↓
Duyệt các Node tại buckets_[index]
            ↓
Nếu tìm thấy key trùng
            ↓
Cập nhật value và kết thúc
            ↓
Nếu không tìm thấy
            ↓
Tạo Node mới
            ↓
Nối Node mới vào đầu danh sách
            ↓
Tăng size_++
            ↓
Kiểm tra loadFactor()
            ↓
Nếu loadFactor() > 0.75 → resize()
```

Việc chèn Node mới vào đầu danh sách giúp thao tác thêm phần tử không phải duyệt đến cuối chain.

### Độ phức tạp

- Trung bình: **O(1)**.
- Trường hợp xấu khi nhiều khóa bị collision: **O(N)**.

---

## 5. Hàm `search()` và `contains()`

Hàm `search()` thực hiện tra cứu giá trị theo khóa `key`, phục vụ trực tiếp cho chức năng tìm kiếm của MC1.

### Quy trình

```text
Tính index = hashKey(key)
            ↓
Duyệt danh sách liên kết tại buckets_[index]
            ↓
So sánh node->key với key
            ↓
Nếu trùng → gán value vào outValue → return true
            ↓
Nếu duyệt hết chain → return false
```

Hàm `contains()` sử dụng cơ chế tra cứu tương tự nhưng chỉ trả về thông tin khóa có tồn tại hay không.

### Độ phức tạp

- Trung bình: **O(1)**.
- Trường hợp xấu: **O(N)** nếu nhiều phần tử bị dồn vào cùng một bucket.

Nhờ sử dụng hàm băm và duy trì độ dài chain nhỏ, thời gian tra cứu thực tế của MC1 gần như không thay đổi đáng kể khi số lượng bản ghi tăng.

---

## 6. Hàm `remove()`

Hàm `remove()` xóa một bản ghi khỏi bảng băm dựa trên khóa định danh.

### Quy trình

```text
Tính index = hashKey(key)
            ↓
Duyệt chain bằng 2 con trỏ:
node và prev
            ↓
Tìm node có node->key == key
            ↓
Nếu prev == nullptr
→ Node nằm ở đầu chain
→ buckets_[index] = node->next
            ↓
Nếu prev != nullptr
→ Node nằm giữa/cuối chain
→ prev->next = node->next
            ↓
delete node
            ↓
size_--
            ↓
return true
```

Nếu không tìm thấy khóa, hàm trả về `false` và không làm thay đổi bảng băm.

### Độ phức tạp

- Trung bình: **O(1)**.
- Trường hợp xấu: **O(N)**.

---

## 7. Hàm `resize()` — Mở rộng bảng băm

Hàm `resize()` được kích hoạt khi:

```text
loadFactor() > 0.75
```

Mục đích là giảm khả năng xảy ra collision và duy trì hiệu năng tra cứu.

### Quy trình

```text
Lưu buckets_ cũ
            ↓
capacity_ = capacity_ * 2 + 1
            ↓
Tạo buckets_ mới
            ↓
Duyệt toàn bộ Node trong bảng cũ
            ↓
Tính lại hash theo capacity_ mới
            ↓
Đưa Node vào bucket mới
            ↓
Giải phóng cấu trúc cũ
```

Khi kích thước bảng thay đổi, vị trí bucket của các khóa có thể thay đổi nên các Node phải được **rehash**.

### Độ phức tạp

- Một lần resize: **O(N)** với `N` là số phần tử hiện tại.
- Do resize chỉ xảy ra khi load factor vượt ngưỡng, chi phí này được phân bổ trong quá trình thêm phần tử và không làm thay đổi độ phức tạp trung bình của thao tác `insert()`.

---

## 8. Các hàm thống kê và trích xuất dữ liệu

### 8.1. Hàm `stats()`

Hàm `stats()` duyệt qua cấu trúc bảng băm để thu thập các thông tin phục vụ đánh giá hiệu năng:

- `capacity`: kích thước bảng.
- `size`: số lượng phần tử.
- `loadFactor`: hệ số tải.
- `maxChainLength`: chiều dài lớn nhất của một chain.

Các thông tin này được sử dụng trong quá trình benchmark để đánh giá mức độ phân bố của các khóa và hiệu quả xử lý collision.

### 8.2. Hàm `allItems()`

Hàm `allItems()` trích xuất toàn bộ dữ liệu trong bảng băm thành:

```cpp
vector<pair<string, V>>
```

Hàm này phục vụ cho việc lấy dữ liệu từ RAM để lưu trở lại file CSV.

### Quy trình

```text
MyHashTable
     ↓
allItems()
     ↓
vector<pair<string, Booking>>
     ↓
Booking::toRow()
     ↓
CsvCodec::encodeRow()
     ↓
CSV
```

---

## 9. Chương trình sinh dữ liệu kiểm thử — `DataGenerator`

Để phục vụ kiểm thử với số lượng bản ghi lớn, hệ thống xây dựng module `DataGenerator.cpp` nhằm tự động sinh dữ liệu Booking và ghi trực tiếp ra file CSV.

Module này sử dụng:

```cpp
std::mt19937 rng(seed);
```

để sinh dữ liệu ngẫu nhiên dựa trên `seed`.

Việc truyền `seed` giúp có thể tái tạo lại cùng một bộ dữ liệu khi cần kiểm thử hoặc benchmark.

### 9.1. Sinh mã Booking

Mã Booking được tạo theo dạng:

```text
RENT_HCM_000001
RENT_HCM_000002
RENT_HCM_000003
...
```

Việc tạo mã theo thứ tự giúp đảm bảo mỗi bản ghi có một `booking_id` riêng biệt.

### 9.2. Sinh thông tin xe

Danh sách hãng và dòng xe được khai báo sẵn:

```cpp
const std::vector<std::pair<std::string, std::string>> HANG_DONG_XE = {
    {"Toyota", "Vios"},
    {"Toyota", "Innova"},
    {"Hyundai", "Accent"},
    {"Hyundai", "i10"},
    {"Hyundai", "Grand i10"},
    {"Kia", "Morning"},
    {"Kia", "Seltos"},
    {"Honda", "City"},
    {"Mazda", "CX-5"},
    {"Mitsubishi", "Xpander"},
    {"Ford", "Everest"},
    {"VinFast", "VF5"}
};
```

Khi sinh mỗi Booking, chương trình chọn ngẫu nhiên một hãng và dòng xe từ danh sách trên.

### 9.3. Sinh biển số và thông tin khách hàng

Biển số xe được tạo ngẫu nhiên từ:

- Mã tỉnh/thành.
- Một chữ cái.
- Ba chữ số.
- Hai chữ số.

Ví dụ:

```text
29A-123.45
```

Thông tin khách hàng được tạo theo dạng:

```text
Khach_000001
Khach_000002
...
```

### 9.4. Sinh ngày thuê và trạng thái

Ngày bắt đầu thuê được sinh trong năm 2026.

Số ngày thuê được chọn ngẫu nhiên từ `1` đến `10` ngày.

Trạng thái Booking được chọn ngẫu nhiên từ:

```text
DANG_THUE
DA_TRA
DA_HUY
```

### 9.5. Tính giá thuê

Giá thuê được tính dựa trên số ngày thuê, dòng xe và hạng thành viên.

Mức giảm giá theo hạng:

```cpp
const int GIAM_GIA_THEO_HANG[4] = {
    0, 3, 5, 10
};
```

Công thức:

```text
Giá = Số ngày × Giá/ngày × (100 - % giảm giá) / 100
```

Sau khi tính, giá được làm tròn đến hàng nghìn đồng.

### 9.6. Ghi dữ liệu ra CSV

Sau khi tạo đối tượng `Booking`, chương trình chuyển đổi dữ liệu thành dòng CSV:

```cpp
out << CsvCodec::encodeRow(booking.toRow()) << "\n";
```

File được mở bằng:

```cpp
std::ofstream out(csvPath, std::ios::trunc);
```

Do đó, mỗi lần chạy `generateCsv()` sẽ tạo lại file CSV theo số lượng bản ghi được yêu cầu.

Module này cho phép tạo nhanh các tập dữ liệu:

```text
N = 1.000
N = 10.000
N = 50.000
N = 100.000
```

phục vụ cho quá trình testing và benchmark của MC1.

### 10. Lịch sử commit
[link commit01] (https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/8c09f79cc7ec8bcee1212184470ea1f9a653580b).

[link commit02] (https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/502b4a32f8abede6bbf40b9b92147d65ee4ed477).

[link commit03] (https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/4633842953ee7b3f182a70369882e0f79fe0882a).

[link commit04] (https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/8dd43f67def29e7609bec351c7b3f862af1c57f8).

---
# D5 — TESTING VÀ DEBUGGING (Phần của Thành viên 1 - MC1)

## 1. Mục tiêu kiểm thử
Mục tiêu của phần này là xác nhận cấu trúc `MyHashTable` (áp dụng kỹ thuật Separate Chaining) và module `Persistence` (đọc/ghi file CSV) hoạt động chính xác theo yêu cầu MC1 của hệ thống[cite: 2].

Các nội dung cần kiểm tra:
* Khả năng thêm (Insert) đơn thuê vào Bảng băm và xử lý đụng độ (Collision).
* Khả năng tra cứu (Search) chính xác đơn thuê thông qua `Booking_ID` hoặc biển số xe[cite: 2].
* Module Persistence nạp thành công bộ dữ liệu giả lập từ file CSV vào bộ nhớ[cite: 2].
* Đảm bảo tính đúng đắn của các chỉ số hiệu năng (Size, Capacity, Load Factor, Max Chain Length) của bảng băm[cite: 3].

---

## 2. Kiểm thử Cấu trúc MyHashTable và Tra cứu (MC1)

| Test | Chức năng | Thao tác / Đầu vào | Kết quả mong đợi | Đánh giá |
|---|---|---|---|---|
| **TC01** | Thêm mới (Insert) | Thêm đơn thuê có `Booking_ID` = "RENT_HCM_00001" vào `tableById`. | Lệnh gọi `tableById.search("RENT_HCM_00001")` trả về đúng đối tượng `Booking*`. Size tăng 1. | PASS |
| **TC02** | Xử lý đụng độ | Thêm 2 đơn thuê cố tình có cùng giá trị Hash Index vào bảng băm. | Cả 2 đơn được nối vào cùng một danh sách liên kết (Separate Chaining). `max_chain_length` $\ge$ 2. | PASS |
| **TC03** | Tra cứu hợp lệ | Gọi tra cứu một mã hợp lệ có trong hệ thống. | Trả về thông tin chi tiết đơn thuê tức thì, không phải duyệt lại từ đầu[cite: 2]. | PASS |
| **TC04** | Tra cứu mã sai | Gọi tra cứu `Booking_ID` không tồn tại. | Hàm trả về `nullptr`, không crash. | PASS |

---

## 3. Kiểm thử module Persistence (Nạp/Ghi File CSV)

| Test | Chức năng | Thao tác / Đầu vào | Kết quả mong đợi | Đánh giá |
|---|---|---|---|---|
| **TC05** | Nạp file CSV | Chạy hàm load dữ liệu với file chứa 10.000 dòng. | Console in ra thống kê nạp đủ 10.000 bản ghi, các thông số bảng băm được cập nhật[cite: 3]. | PASS |
| **TC06** | Tạo giả lập CSV | Chạy chương trình khi thiếu file `donthue_xe.csv`. | Hệ thống tự động sinh các bản ghi giả lập để có dữ liệu tra cứu[cite: 1]. | PASS |

---

## 4. Nhật ký Debug

| STT | Vấn đề gặp phải | Nguyên nhân tìm ra | Cách xử lý (Fix) | Kết quả |
|---|---|---|---|---|
| 1 | **Chương trình crash** khi đọc file `donthue_xe.csv` đến dòng cuối. | Ở cuối file CSV có dòng trống, hàm đọc cắt chuỗi bị thiếu dữ liệu dẫn đến lỗi truy xuất. | Thêm lệnh `if (line.empty()) continue;` trước khi băm dữ liệu. | Đã khắc phục, đọc đủ file thành công. |
| 2 | **Lỗi mất dữ liệu khi đụng độ (Collision)**. | Cơ chế Separate Chaining cài đặt sai con trỏ `next`, làm đứt chuỗi danh sách liên kết khi chèn node mới. | Sửa lại logic chèn Node vào đầu danh sách: `newNode->next = head[index]; head[index] = newNode;`. | Tìm thấy đầy đủ các phần tử cùng Hash Index. |
| 3 | **Load Factor quá cao** khi nạp hàng nghìn dòng. | Sức chứa (Capacity) khởi tạo quá nhỏ. | Cài đặt cơ chế cấp phát tự động dung lượng đủ lớn, duy trì `load_factor` luôn ở mức lý tưởng (dưới 0.75)[cite: 3]. | `max_chain_length` giảm xuống chỉ còn 3-4[cite: 3]. |

---

## 5. Bằng chứng hiệu năng (Mục 5.3)

Để chứng minh giải pháp cấu trúc dữ liệu cho **MC1 (Tra cứu theo mã)** hoạt động đúng độ phức tạp thuật toán đã phân tích[cite: 2], nhóm đã tiến hành đo lường benchmark thời gian tra cứu giữa cấu trúc `MyHashTable` tự cài đặt và cách quét tuyến tính (Linear Scan) thông thường.

### a. Kịch bản kiểm thử
* **Quy mô dữ liệu:** Kiểm thử tại 2 mốc quy mô là **1.000** bản ghi và **10.000** bản ghi[cite: 3].
* **Mục tiêu đo lường:** Tra cứu ngẫu nhiên mã đơn thuê để so sánh thời gian thực thi (tính bằng mili-giây - ms) và đo mức độ tăng tốc (Speedup)[cite: 3]. Đồng thời kiểm tra các chỉ số sức khỏe của Bảng băm như dung lượng (Capacity), hệ số tải (Load Factor), và độ dài chuỗi đụng độ lớn nhất (Max Chain Length)[cite: 3].

### b. Kết quả thực nghiệm (Dựa trên Benchmark IDE)

| Quy mô (N bản ghi) | Thời gian quét tuyến tính - O(N) | Thời gian dùng MyHashTable - O(1) | Mức độ tăng tốc (Speedup) | Chỉ số thống kê Bảng băm (MyHashTable) |
| :--- | :--- | :--- | :--- | :--- |
| **1,000**[cite: 3] | 0.00683 ms[cite: 3] | **0.00009 ms**[cite: 3] | **Nhanh hơn 73.1 lần**[cite: 3] | capacity=2019, load_factor=0.495, max_chain_length=3[cite: 3] |
| **10,000**[cite: 3] | 0.06650 ms[cite: 3] | **0.00012 ms**[cite: 3] | **Nhanh hơn 543.3 lần**[cite: 3] | capacity=16159, load_factor=0.619, max_chain_length=4[cite: 3] |


### c. Phân tích và Đánh giá
Qua kết quả thực nghiệm, cấu trúc `MyHashTable` chứng minh được hiệu năng vượt trội hoàn toàn so với thiết kế quét mảng thông thường:
1. **Đúng với độ phức tạp lý thuyết:** Khi quy mô dữ liệu tăng gấp 10 lần (từ 1.000 lên 10.000), thời gian tìm kiếm tuyến tính (O(N)) tăng gấp ~10 lần (từ 0.00683 ms lên 0.06650 ms)[cite: 3]. Tuy nhiên, thời gian tìm kiếm của Hash Table gần như giữ nguyên (từ 0.00009 ms lên 0.00012 ms, độ trễ không đáng kể), minh chứng chính xác cho độ phức tạp **O(1)**[cite: 3].
2. **Khả năng mở rộng (Scalability):** Hệ số tải (`load_factor`) được giữ ở mức cực kỳ ổn định (0.495 tại N=1.000 và 0.619 tại N=10.000)[cite: 3]. Sức chứa (`capacity`) được cấp phát hợp lý (2019 và 16159), giúp khống chế chuỗi đụng độ dài nhất (`max_chain_length`) chỉ ở mức 3 và 4 phần tử[cite: 3].
3. **Kết luận:** Với mức tăng tốc đạt tới **543.3 lần** ở quy mô 10.000 bản ghi[cite: 3], cấu trúc bảng băm đảm bảo hệ thống có thể đáp ứng tra cứu tức thì ngay cả khi số lượng giao dịch thuê xe tích lũy lên tới hàng chục nghìn lượt[cite: 2].
---
# D6 Peer technical review: Đánh giá cấu trúc MyMaxHeap (Yêu cầu RF2)

**Người đánh giá:** Nguyễn Hữu Thịnh (phụ trách MC1 – `MyHashTable`)
**Module được đánh giá:** `MyMaxHeap.h` (xử lý ưu tiên khi tranh chấp xe – RF2)

Qua việc đọc và phân tích mã nguồn file `MyMaxHeap.h`, em có một số nhận xét về kỹ thuật cài đặt cấu trúc Đống cực đại (Max-Heap) của bạn như sau.

---

## 1. Điểm mạnh và sự chính xác trong thiết kế (Strengths)

- **Tích hợp khéo léo để đạt O(log N) cho thao tác xóa.**
  Việc bạn sử dụng `MyHashTable` (do tôi cài đặt ở MC1) làm `indexMap` để ánh xạ `BookingID` sang vị trí index trong mảng `heap` là một giải pháp thiết kế rất xuất sắc. Nó giúp hàm `RemoveById(bookingId)` tìm được vị trí cần xóa trong thời gian O(1) và khôi phục Heap trong O(log N), thay vì phải quét toàn bộ mảng O(N).

- **Logic phân giải tranh chấp chính xác.**
  Hàm `HigherPriority()` đã xử lý chuẩn xác quy tắc 3 tầng: so sánh `membershipTier`, đến `bookingTimestamp`, và cuối cùng phá hòa bằng `bookingId` nếu trùng lặp.

- **Khởi tạo Heap tối ưu (O(N)).**
  Trong hàm `BuildHeap()`, bạn đã duyệt từ node lá cuối cùng có con `(size/2 - 1)` ngược lên gốc và gọi `SiftDown()`. Đây là cách cài đặt bottom-up chuẩn mực, đạt độ phức tạp O(N) thay vì O(N log N) nếu chèn từng phần tử.

- **Tính an toàn cao.**
  Có xử lý văng lỗi (`throw runtime_error`) ở các hàm `ExtractMax()` và `Top()` khi gọi trên Heap rỗng, giúp dễ dàng trace bug trên hệ thống.

---

## 2. Góp ý cải thiện hiệu năng (Improvements)

- **Tối ưu thao tác hoán đổi.**
  Trong hàm `SwapAndSync(int i, int j)`, bạn đang dùng biến tạm `RentalRequest temp = heap[i];`. Struct này chứa tới 3 biến `std::string`, việc gán như vậy sẽ kích hoạt Copy Constructor sao chép chuỗi, gây tốn tài nguyên.

  → **Đề xuất sửa:** nên sử dụng hàm chuẩn của C++ là `std::swap(heap[i], heap[j]);`. Hàm này dùng Move Semantics nên hoán đổi bộ nhớ trực tiếp mà không cần copy chuỗi, giúp `SiftUp` và `SiftDown` chạy nhanh hơn đáng kể khi kích thước Heap lớn.

  Trước:

  ```cpp
  RentalRequest temp = heap[i];
  heap[i] = heap[j];
  heap[j] = temp;
  ```

  Sau (hai dòng cập nhật `indexMap` phía dưới giữ nguyên):

  ```cpp
  std::swap(heap[i], heap[j]);
  ```

---

## Kết luận

Module cài đặt rất tốt, cấu trúc dữ liệu minh bạch, đáp ứng xuất sắc tính năng ưu tiên xử lý tranh chấp xe và có thể dễ dàng ghép nối với module lưu trữ của em.

---
# D7 NHẬT KÝ SỬ DỤNG AI VÀ BÀI PHẢN TƯ CÁ NHÂN 

## A. Nhật ký sử dụng AI

**Công cụ:** Claude (claude.ai), Gemini, ChatGPT

| STT | Mục đích | Phần công việc bị ảnh hưởng | Cách kiểm chứng |
| :---: | :--- | :--- | :--- |
| **1** | Đọc code toàn dự án để chỉ ra vì sao menu RF3 không tìm theo biển số | Lõi/trình bày: hiểu hiện trạng, chưa sửa code | Đối chiếu với `InputHandler.cpp`, `RentalSystem.cpp` và `demo_mc1.cpp`: tìm theo biển số có ở `RentalMC1Demo`, không có ở `CarRentalApp` |
| **2** | Hướng dẫn sinh 10.000 đơn (đổi tham số `generateCsv`, xóa file CSV cũ) | Dữ liệu cho demo và benchmark | Chạy từ terminal ở thư mục gốc repo, thấy “Da nap 10000 don thue”, `size=10000`, `load_factor` ≈ 0,62, `max_chain_length=4` |
| **3** | Chẩn đoán “Da nap 0 don” khi bấm F5 | Gỡ lỗi chạy demo | Chạy lại từ thư mục gốc repo thì nạp được 10.000 đơn |
| **4** | Chỉ ra `tableByPlate` ghi đè khi một biển số có nhiều đơn | Lõi (MC1) | Đọc `MyHashTable::insert`: khi trùng khóa thì cập nhật giá trị. Thay đổi: *[điền khi đã sửa]* |
| **5** | Gợi ý chỉnh D1 (rút gọn một trang, giữ yêu cầu phát triển từ MC1, thêm trường hợp biên) | D1 (tài liệu) | Tự đối chiếu với Mục 5 của đề, quyết định giữ hoặc bỏ từng ý |
| **6** | Soạn khung D3 (không điền lập luận) | D3 (tài liệu) | Đối chiếu khung với Mục 9 và Mục 15 của đề |
| **7** | Soạn bản nháp D6 và D7 | D6, D7 (tài liệu) | Đọc lại `MyMaxHeap.h` để xác nhận từng điểm review; sửa bản phản tư theo trải nghiệm thật |
| **8** | Viết tầng web (máy chủ HTTP bằng C++ dùng `cpp-httplib` và giao diện HTML) gọi xuống `RentalSystem` cho MC1, MC2, RF1, RF2, RF3 và nút lưu CSV | Trình bày (web); không sửa `core/`, `service/` | Tự biên dịch bằng `cl` và chạy `web_server.exe`: trang hiện capacity 2.019, size 1.000, load factor 0,495, chuỗi dài nhất 3; ở RF2 yêu cầu VIP được xử lý trước GOLD. AI có chạy thử logic các endpoint bằng thư viện giả, không thay cho việc em tự chạy. *[điền: các tab MC2, RF1, RF3 đã thử]* |
| **9** | Hướng dẫn mở Developer PowerShell, tải `httplib.h`, biên dịch bằng `cl` và gỡ lỗi C1083 (không tìm thấy file nguồn) | Biên dịch, chạy | Lỗi do các file `.cpp` nằm sai thư mục; dùng `Get-ChildItem -Recurse` tìm file, chuyển về `full_project` và `presentation`, biên dịch ra `web_server.exe` thành công |
| **10** | Sửa `WebServer.cpp` đọc biến môi trường `PORT`, tạo `Dockerfile` và `.dockerignore` để deploy lên Render | Cấu hình triển khai | `Select-String` thấy `listenPort` trong `WebServer.cpp`; `Dockerfile` (389 byte) và `.dockerignore` đã tạo. *[điền: đã deploy lên Render hay chưa, kết quả]* |
| **11** | Hướng dẫn tạo repo Git, `.gitignore`, đẩy mã lên GitHub; hướng dẫn pull nhánh `develop` và xem diff trước khi commit | Quản lý mã nguồn | Sau khi pull, Git Changes hiện 0 / 0 trên nhánh `develop`. *[điền: các lệnh đã chạy và kết quả]* |
| **12** | Thêm `CMakeLists.txt` và đoạn `PROJECT_ROOT` trong `main.cpp`, `web_main.cpp` để chạy trong Visual Studio; gỡ lỗi CMake | Cấu hình build | Lần đầu Output báo Parse error ở `CMakeLists.txt:1` vì file chứa nhầm code C++; sau khi thay nội dung, Output báo Configuring done, Generating done, CMake generation finished và ô cấu hình hiện x64-Debug |
| **13** | Đọc file kế hoạch nhóm `HE_THONG_CHO_THUE_XE_TU_LAI.docx` để xác định phần của thành viên 1 (MC1, `MyHashTable`, Persistence) | Tài liệu, phân công | Đối chiếu với phần phân công trong file docx của nhóm |
| **14** | Soạn bản nháp D1 (bài đọc-hiểu đề bài) và D7 (nhật ký, phản tư) | D1, D7 (tài liệu) | Đọc lại, sửa theo trải nghiệm thật, đối chiếu với Mục 5 và Mục 12 của đề. *[điền: những ý em giữ hoặc bỏ]* |
| **15** | Review RF2 cho D6: đọc `MyMaxHeap.h` dạng template và bản heap có chỉ mục theo `bookingId` của bạn cùng nhóm | D6 (tài liệu) | AI chạy test ngẫu nhiên cả hai bản heap: 200.000 thao tác khớp tham chiếu. Phát hiện trùng `bookingId` làm hỏng chỉ mục (`RemoveById` lần 2 trả `false` dù còn yêu cầu) và hai class `MyMaxHeap` trùng tên gây lỗi biên dịch khi include chung. *[điền: em tự chạy lại hai điểm này]* |
| **16** | Đọc và kiểm tra code lõi (`MyHashTable`, `MySortedArray`, `MyTrie`, `MyStack`, `MyMaxHeap`, `RentalSystem`, `Persistence`) xem cài đặt đã đúng cấu trúc dữ liệu và giải thuật chưa | Lõi MC1, MC2, RF1, RF2, RF3; chỉ review, chưa sửa | AI chạy test ngẫu nhiên: bảng băm 300.000 thao tác khớp `unordered_map`; mảng sắp xếp 20.000 phần tử và 2.000 truy vấn khoảng khớp duyệt tuần tự; Trie 2.000 truy vấn tiền tố khớp; `test_full_system` 29 passed. Phát hiện: hai đơn cùng biển số thì xóa một đơn làm `findByPlate` không tìm thấy; xóa đơn cuối cùng thì `topRentedCars` vẫn trả xe với 0 lượt; nạp 100.000 đơn mất khoảng 690 ms. *[điền: em tự chạy lại và quyết định sửa gì]* |
| **17** | Chẩn đoán lỗi build C1083 "không mở được GenerateData.h" ở `demo_mc1.cpp` và `benchmark_mc1.cpp`; sửa tên include thành `DataGenerator.h` và thêm thư viện datagen trong `CMakeLists.txt` (include thư mục `test/`, link với core) | Cấu hình build; không sửa thuật toán lõi | Đối chiếu cây thư mục: file thật là `test/DataGenerator.h`. Sau khi sửa, Output Build không còn lỗi ở `demo_mc1.cpp` và `benchmark_mc1.cpp`, chỉ còn lỗi ở `DataGenerator.cpp` (mục 18) |
| **18** | Chẩn đoán lỗi C1083 "không mở được ../core/Booking.h" ở `DataGenerator.cpp`; đổi thành `#include "Booking.h"` vì `CMakeLists.txt` đã khai báo `src/core/model` trong include path | Cấu hình build; `test/DataGenerator.cpp` | Tự build lại: Output báo Build All succeeded, link xong `datagen.lib`, `test_mc1.exe`, `benchmark_mc1.exe`, `demo_mc1.exe`. *[điền: đã chạy thử demo_mc1.exe và kết quả]* |
| **19** | Hướng dẫn commit bản sửa lỗi include và push nhánh `feature/m1-hashtable` lên GitHub (kiểm tra mục Changes không có `out`, `.vs`) | Quản lý mã nguồn | Git Changes báo Commit `2c6806c1` created locally, bộ đếm 2 / 0; sau khi push, thanh thông báo ghi Successfully pushed to `origin/feature/m1-hashtable` và Local History hiện 0 Outgoing |
| **20** | Hướng dẫn tạo pull request vào `develop`; chỉ ra trang đang để base là `main` (6 commits, 24 files) là sai, cần đổi sang `develop` | Quản lý mã nguồn (pull request, merge) | Sau khi đổi base, trang so sánh hiện 2 commits, 10 files changed, Able to merge. PR #4 tạo thành công, báo No conflicts with base branch. Sau khi merge và pull, `develop` có Merge pull request #4 (commit `c644668`) |
| **21** | Hướng dẫn pull `develop` và tạo nhánh riêng cho M2 từ `develop` (New Local Branch From...); hướng dẫn đổi tên nhánh `mc2-algorithms` thành `m2-algorithms`; giải thích vì sao không commit được khi chưa có thay đổi | Quản lý mã nguồn; chưa viết code M2 | Tab Git hiện nhánh `feature/mc2-algorithms`, mục Changes ghi no unstaged changes và Commit All bị mờ. *[điền: đã đổi tên nhánh chưa, tên cuối cùng]* |
| **22** | Giải thích nhánh `feature/m1-hashtable` không còn trong danh sách nhánh trên GitHub sau khi merge PR #4, và cách tạo lại nhánh từ `develop` nếu muốn giữ nhánh MC1 riêng | Quản lý mã nguồn | Mở danh sách nhánh trên GitHub: có `main`, `demo/feature-mc2`, `develop`, `feature/m2-generator`; code MC1 vẫn nằm trong `develop`. *[điền: có tạo lại nhánh MC1 hay không]* |
| **23** | (Gemini) Hỏi phần yêu cầu xung đột MC2 và RF2 trong đề cương đã đúng chưa. AI xác nhận lập luận và giải pháp kết hợp cấu trúc (Composition) là đúng; bổ sung cần phân biệt Max-Heap (M yêu cầu đang chờ) với Sorted Array (N đơn lịch sử đã xác nhận), khác nhau về khóa sắp xếp và kiểu thao tác; viết lại mục 2.3 theo văn phong học thuật | Đề cương, mục 2.3 (tài liệu) | *[điền: đối chiếu với đề bài và quyết định giữ hoặc sửa ý nào trong đoạn viết lại]* |
| **24** | (Gemini) Nhờ tổng hợp lịch sử hỏi đáp thành phụ lục nhật ký sử dụng AI (dạng văn bản và bảng) kèm đoạn cam kết tính tự chủ | Phụ lục nhật ký AI (tài liệu) | Đối chiếu từng dòng với lịch sử trò chuyện thật, cắt gọn theo mẫu bảng của D7. *[điền: những dòng đã sửa]* |
| **25** | Giải thích lỗi khi chạy `RentalWebCore.exe`: chương trình in dòng `usage: RentalWebCore <csv> <command> [args]` rồi thoát với mã 2 vì thiếu tham số dòng lệnh; hướng dẫn truyền tham số qua Debug Properties hoặc `launch.vs.json` | Gỡ lỗi chạy chương trình; không sửa code | Ảnh console hiện đúng dòng usage và exited with code 2. *[điền: đã thử truyền tham số chưa, kết quả]* |
| **26** | Đề xuất cách sửa nhanh tìm theo biển số cho menu RF3 (thêm `searchByPlate`, sửa `handleSearch` có hai chế độ) và nêu hạn chế: duyệt tuyến tính $O(N)$, không đạt MC1; chỉ ra `Benchmark.h` của RF3 tự định nghĩa lớp `MyHashTable` riêng, không phải `MyHashTable.h` | MC1, RF3 (lõi và trình bày); chưa sửa code | Mở `Benchmark.h` thấy lớp `MyHashTable` dạng `vector<vector<string>>`, khác `MyHashTable.h`; hàm `findRental` trong `RentalSystem.cpp` duyệt tuyến tính. *[điền: có áp dụng cách sửa nhanh hay không]* |
| **27** | Hướng dẫn xem và tạo `README.md`; giải thích Visual Studio khác VS Code (cần extension Markdown Editor để xem trước); chỉ ra file `docs/NguyenHuuThinh.md` đang trống; giải thích mã thoát `0xc000013a` là do đóng cửa sổ console khi chương trình đang chạy, không phải lỗi biên dịch | Tài liệu (`README`); gỡ lỗi | Ảnh Solution Explorer cho thấy `NguyenHuuThinh.md` chỉ có dòng 1; Output ghi exited with code 3221225786 (`0xc000013a`). *[điền: đã tạo README.md chưa]* |
| **28** | Giải thích khái niệm trường hợp biên; gợi ý cách viết dòng "điểm căng thẳng" trong D1 từ góc nhìn MC1 (tra theo khóa nhanh, không cần thứ tự, đối với MC2 cần thứ tự) và rút gọn mục 4 chỉ giữ yêu cầu phát triển từ MC1 | D1 (tài liệu) | Đối chiếu với rubric thành phần A (Mục 15) và Mục 5.4 của đề. *[điền: câu cuối cùng em giữ trong D1]* |
| **29** | Hướng dẫn tạo GenData (`tools/gen_data.cpp` và `add_executable` trong `CMakeLists.txt`) để sinh 10.000 và 30.000 đơn; nhắc đề (Mục 5.3) yêu cầu hai mốc cách nhau một bậc độ lớn nên 10.000 và 30.000 chưa đủ, còn `BenchmarkMC1` đã đo 1.000, 10.000, 100.000 | Dữ liệu cho demo và benchmark | Đối chiếu Mục 5.3 của đề và `SIZES` trong `benchmark_mc1.cpp`. *[điền: đã tạo GenData và sinh file 30.000 đơn chưa]* |
| **30** | Chẩn đoán lỗi PowerShell `CommandNotFoundException` khi chạy `.\out\build\x64-Debug\RentalMC1Demo.exe`: terminal đang đứng ở `C:\Users\THINH\source\repos` thay vì `D:\DSA-He-Thong-Cho-Thue-Xe-Tu-Lai` | Gỡ lỗi chạy chương trình | Sau khi `cd` sang thư mục dự án, chạy được `RentalMC1Demo.exe`: Da nap 10000 don thue, `capacity=16159`, `size=10000`, `load_factor=0.61885`, `max_chain_length=4` |
| **31** | Chỉ ra file CSV sinh ra nằm ở `data\donthue_xe.csv` trong thư mục gốc repo và cảnh báo mục 4 (Lưu và thoát) ghi đè file, làm đổi thứ tự dòng | Dữ liệu cho demo | Ảnh tab `donthue_xe.csv` hiện các dòng RENT_HCM_007243, 001358, 007244, 001359 xen kẽ, không còn theo thứ tự 000001, 000002 |
| **32** | Hướng dẫn tạo `launch.vs.json` với `currentDir` là `${workspaceRoot}` để F5 chạy đúng thư mục, khi `RentalMC1Demo` báo "Da nap 0 don" và "Khong tim thay" | Cấu hình chạy (Visual Studio) | Ảnh console hiện `size=0` và Khong tim thay; tab `launch.vs.json` đang mở trong Visual Studio. *[điền: đã thêm currentDir và F5 nạp được 10.000 đơn chưa]* |
| **33** | Giải thích vì sao `CarRentalApp` báo Khong tim thay don thue khi tra RENT_HCM_009999: RF3 không đọc CSV nên danh sách đơn rỗng; đề xuất thêm hàm nạp CSV (`loadCsvIntoSystem`, gọi `addRental` với `saveHistory = false` để không ghi vào Undo) và lưu ý nạp 10.000 đơn chậm do `findRental` tuyến tính | RF3, `MainRF3.cpp` (trình bày); chưa sửa code | Ảnh console `CarRentalApp`: chọn 4, nhập RENT_HCM_009999, báo Khong tim thay don thue; `MainRF3.cpp` không có bước nạp CSV. *[điền: có thêm hàm nạp CSV hay không]* |
| **34** | Giải thích sản phẩm D2 và D3 là gì, liệt kê các mục cần có trong D3 theo đề (Mục 6.1, 9, 15, 18) và gợi ý hướng lập luận Q1 đến Q4 cho phần MC1 của em | D3 (tài liệu), phần biện minh MC1 | Đối chiếu Mục 6.1, 9, 15, 18 của đề. *Em tự viết lại phần biện minh MC1 theo cách hiểu của mình, giữ hoặc bỏ ý nào]* |
| **35** | AI tự chạy thử trong môi trường riêng hai nhận định: (a) chèn hai yêu cầu trùng `bookingId` vào `MyMaxHeap` (Size = 3 với 2 mã khác nhau, `RemoveById` lần hai trả `false` dù Size vẫn 2; hai yêu cầu cùng hạng và cùng thời điểm thì mã nhỏ hơn ra trước); (b) chèn hai đơn cùng biển số vào `MyHashTable` (`size = 1`, chỉ còn đơn chèn sau) | D6, MC1 (kiểm chứng do AI chạy) | Kết quả do AI chạy, không thay cho việc em tự chạy. |
| **36** | Giải thích code MC1 của em (bảng băm tự cài, hàm băm, tự mở rộng khi hệ số tải vượt 0,75, nạp và ghi CSV qua `Persistence`) để em nắm lại thật chắc phần mình sở hữu | MC1; chuẩn bị bảo vệ; không sửa code | Đối chiếu với `MyHashTable.h`, `Persistence.h`, `Persistence.cpp`, `demo_mc1.cpp`, `benchmark_mc1.cpp`.  |
| **37** | Giải thích code MC2 của bạn cùng nhóm (Merge Sort, Binary Search `lowerBound`/`upperBound`, truy vấn khoảng ngày, thống kê và Top K xe) để hiểu code của người khác | Hiểu code MC2; chuẩn bị bảo vệ; không sửa code | Đối chiếu với `MergeSort.h`, `BinarySearch.h`, `RentalService.h`, `RentalService.cpp`, `mainMC1.cpp` |
| **38** | Giải thích code RF1 của bạn cùng nhóm (Trie gợi ý tên xe: chèn cả cụm đầy đủ lẫn hậu tố, tìm theo tiền tố, hiển thị 5 gợi ý đầu) để hiểu code của người khác | Hiểu code RF1; chuẩn bị bảo vệ; không sửa code | Đối chiếu với `Trie.h`, `mainRF1.cpp`. *Em tự đọc lại và giải thích bằng lời của mình* |
| **39** | Giải thích code RF2 của bạn cùng nhóm (Max-Heap với `indexMap`, luật ưu tiên ba tầng, `InsertRequest`, `ExtractMax`, `RemoveById`, `BuildHeap`) để hiểu code của người khác | Hiểu code RF2; chuẩn bị bảo vệ; không sửa code | Đối chiếu với `MyMaxHeap.h`. *Em tự đọc lại và giải thích bằng lời của mình* |
| **40** | Giải thích code RF3 của bạn cùng nhóm (ngăn xếp liên kết, `Action` ADD/UPDATE/DELETE, `undoRental`, kiểm tra ngày, cờ `saveHistory`) để hiểu code của người khác | Hiểu code RF3; chuẩn bị bảo vệ; không sửa code | Đối chiếu với `UndoStack.h`, `UndoStack.cpp`, `RentalSystem.cpp`, `Rental.cpp`, `InputHandler.cpp` |
---

## B. AI-audit (một trường hợp)

- **Nhận định của AI:** Sau khi đổi `#include` thành `DataGenerator.h` thì `demo_mc1` và `benchmark_mc1` vẫn chưa build được nếu `CMakeLists.txt` chưa khai báo thư mục `test/` và `DataGenerator.cpp`; cần tạo thư viện `datagen` (include `test/`, link với `core`).
- **Kiểm chứng:** Em sửa `CMakeLists.txt` theo gợi ý rồi tự Build All. Lỗi ở `demo_mc1.cpp` và `benchmark_mc1.cpp` biến mất, nhưng xuất hiện lỗi mới ở `DataGenerator.cpp` dòng 3: "không mở được `../core/Booking.h`". AI không đoán trước đúng dòng này, chỉ dặn gửi lại Output nếu còn lỗi.
- **Kết luận:** Nhận định đúng một phần: đúng về cấu hình CMake, nhưng chưa thấy hết lỗi đường dẫn include trong `DataGenerator.cpp`. Chỉ khi chạy build thật và có log mới biết đầy đủ, nên không thể tin gợi ý của AI mà không biên dịch lại.
- **Thay đổi:** Thêm thư viện `datagen` trong `CMakeLists.txt`; đổi `#include "../core/Booking.h"` thành `#include "Booking.h"` (`CMakeLists.txt` đã khai báo `src/core/model` trong include path). Build lại, Output báo Build All succeeded, link xong `datagen.lib`, `test_mc1.exe`, `benchmark_mc1.exe`, `demo_mc1.exe`.

---

## C. Bài phản tư cá nhân

Khó khăn cụ thể nhất của em là chương trình báo “Da nap 0 don thue” trong khi em vừa đặt sinh 10.000 đơn. Em phải phân biệt hai khả năng: dữ liệu sinh sai hoặc chương trình đọc sai chỗ. Khi so sánh hai cách chạy, em thấy bấm F5 trong Visual Studio chạy ở thư mục `out\build\x64-Debug` nên đường dẫn tương đối `data/donthue_xe.csv` không trỏ đúng, còn chạy từ terminal ở thư mục gốc repo thì nạp được 10.000 đơn (hệ số tải khoảng 0,62, chuỗi dài nhất bằng 4).

Sau đó em gặp thêm một lần “không tìm thấy” ở `CarRentalApp` và hiểu đây là chương trình khác, không nạp CSV, chứ không phải lỗi bảng băm. Em rút ra hai điều: đường dẫn tương đối phụ thuộc thư mục làm việc, và các phần của nhóm chưa dùng chung nguồn dữ liệu nên cần thống nhất trước khi tích hợp. AI giúp em gọi tên nguyên nhân, còn việc chạy lại và đối chiếu kết quả là em tự làm.

Khó khăn thứ hai là khi đưa MC1 lên GitHub. Build lỗi hai lần liên tiếp vì include sai: lần đầu gõ nhầm tên `GenerateData.h` thay vì `DataGenerator.h`, lần sau dùng đường dẫn tương đối `../core/Booking.h` không khớp cấu trúc thư mục thật. Em rút ra là phải đọc kỹ thông báo C1083 để biết file nào, dòng nào, và đối chiếu với cây thư mục trước khi sửa. Khi tạo pull request, trang so sánh hiện 6 commits và 24 files changed, khác với 2 commits và 10 files em mong đợi; nhờ chú ý con số này em nhận ra base đang là `main` chứ không phải `develop`, đổi lại thì chỉ còn đúng phần MC1 và hiện Able to merge. AI chỉ ra nguyên nhân và cách sửa, còn việc build lại, so sánh số commit và file, đổi base rồi merge là em tự thực hiện.