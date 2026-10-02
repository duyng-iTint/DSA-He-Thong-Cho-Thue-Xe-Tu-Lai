# ĐỒ ÁN DSA — PHẦN ĐÓNG GÓP CÁ NHÂN

## Thông tin thành viên

| Thông tin | Nội dung |
|---|---|
| Họ tên | Lý Nguyễn Mạnh Thuyên |
| MSSV | 25110354 |
| Lĩnh vực | Hệ thống quản lý cho thuê xe tự lái & xử lý tranh chấp đặt xe |
| Requirement phụ trách | RF2 |
| Component | MyMaxHeap |
| Branch | `feature/rf2-maxheap` |

---

# 1. D1 — BÀI ĐỌC-HIỂU ĐỀ BÀI

## 1.1. Bài toán trong một câu

Hệ thống này sẽ lưu đội xe và các đơn thuê xe tích lũy qua nhiều năm, và phải trả ra output nhanh cho một số yêu cầu vận hành cố định khi dữ liệu liên tục được thêm, sửa, hủy.

## 1.2. Input / Output / Constains

### Input

- Booking_ID
- Biển số / Car_ID
- Customer_ID
- Membership tier
- Booking timestamp
- Ngày nhận/trả
- Trạng thái
- Các thao tác thêm/sửa/hủy
- Các truy vấn của nhân viên

### Output

- Chi tiết một đơn xe hoặc một xe
- Danh sách lượt thuê trong một khoảng ngày
- Danh sách xe được thuê nhiều nhất
- Danh sách hãng xe được tìm kiếm nhiều nhất
- Gợi ý tên hãng xe từ vài ký tự đầu
- Xử lý xung đột khi nhiều khách tranh cùng một xe
- Trạng thái dữ liệu trước thao tác gần nhất sau khi hoàn tác

### Ràng buộc

- Quy mô dữ liệu ≥ 10.000 đơn và tăng dần
- Dữ liệu thay đổi liên tục
- `Booking_ID` là duy nhất
- Dữ liệu được xử lý trong bộ nhớ
- File chỉ dùng để lưu/nạp dữ liệu
- Quy tắc xử lý tranh chấp phải xác định và công bằng

---

# 2. Phân tích các yêu cầu

| Requirement | Nội dung | Access pattern chính |
|---|---|---|
| MC1 | Lấy một bản ghi từ mã | Tra cứu theo ID |
| MC2 | Truy vấn theo thời gian / xe được thuê nhiều nhất | Thời gian / thống kê |
| RF1 | Gợi ý hãng xe theo prefix | Prefix search |
| RF2 | Xử lý tranh chấp đặt xe | Priority |
| RF3 | Hoàn tác thao tác gần nhất | LIFO |

---

# 3. RF2 — XỬ LÝ TRANH CHẤP ĐẶT XE

## 3.1. Mô tả yêu cầu

Khi có nhiều yêu cầu cùng tranh một xe, hệ thống cần liên tục xác định yêu cầu có mức độ ưu tiên cao nhất để xử lý trước.

## 3.2. Quy tắc ưu tiên

Thứ tự ưu tiên:

1. `membershipTier` cao hơn → ưu tiên cao hơn.
2. Nếu cùng `membershipTier` → `bookingTimestamp` nhỏ hơn → ưu tiên cao hơn.
3. Nếu hai giá trị trên vẫn giống nhau → ưu tiên tài khoản nào được tạo trước `bookingID`.

Ví dụ:

| Booking | Tier | Timestamp |
|---|---:|---|
| B001 | 1 | 10:05 |
| B002 | 3 | 10:10 |
| B003 | 2 | 10:01 |
| B004 | 3 | 10:10 |
| B005 | 3 | 10:20 |

Thứ tự xử lý:

```text
B002
↓
B004
↓
B005
↓
B003
↓
B001
```
Trong đó B002 và B004 có cùng Tier và Timestamp, nên xét bookingID để xác định thứ tự.
## 3.3. Edge Cases

- Heap rỗng.
- Chỉ có một request.
- Các request khác Membership Tier.
- Cùng Tier nhưng khác Timestamp.
- Cùng Tier và cùng Timestamp.
- Request bị hủy khi đang chờ.
- BookingID không tồn tại khi RemoveById.

## 3.4. Conflict phát hiện được

MC2 và RF2 sử dụng cùng miền dữ liệu booking nhưng có access pattern khác nhau.

MC2 cần xử lý truy vấn theo thời gian và thống kê.

RF2 cần liên tục lấy request có priority cao nhất.

Do đó không nên ép một cấu trúc dữ liệu duy nhất phục vụ cả hai workload.

Giải pháp được đề xuất là composition:

MC2 → cấu trúc phục vụ truy vấn của MC2

RF2 → MyMaxHeap

# 3.5. Giả định và vấn đề cần làm rõ với nhóm
Giả định:
- "Xe đang bận" được xác định theo giao nhau của khoảng thời gian.
- Booking_ID là duy nhất.
- Booking_ID được tạo theo thứ tự booking nếu dùng làm tiêu chí
  phá hòa trong RF2.

Các vấn đề cần thống nhất với nhóm:
- Hoàn tác chỉ một bước hay nhiều bước.
- Cách xử lý khi request bị hủy trong lúc đang chờ.
- Cách xử lý khi hạng thành viên thay đổi.
- Cách xử lý khi xe được trả lại trong lúc đang có tranh chấp.
- Quy tắc cuối cùng khi nhiều request có cùng Tier và Timestamp.
---
# D3 — PHẦN BIỆN MINH THIẾT KẾ CÁ NHÂN
## 1. Requirement phụ trách

### RF2 — Xử lý xung đột đặt xe

RF2 xử lý trường hợp có nhiều yêu cầu cùng tranh một xe.

Khi xảy ra xung đột, hệ thống cần xác định yêu cầu có mức độ ưu tiên
cao nhất để xử lý trước.

Component cá nhân được sử dụng:

```text
MyMaxHeap
```
## 2. Quy tắc xác định độ ưu tiên
Priority của một RentalRequest được xác định theo thứ tự:
```text
1. Membership Tier
        ↓
2. Booking Timestamp
        ↓
3. Booking ID
```
Quy tắc:
- `membershipTier` cao hơn → ưu tiên cao hơn.
- Nếu cùng `membershipTier` → `bookingTimestamp` sớm hơn → ưu tiên cao hơn.
- Nếu cùng `membershipTier` và cùng `bookingTimestamp`
  → xét `bookingId`.
##  3. Q1 — Requirement cần thao tác gì?
RF2 cần quản lý một tập các request đang tranh cùng một xe.
Các thao tác chính:

| Thao tác | Mục đích |
|---|---|
| `InsertRequest()` | Thêm request mới vào hàng đợi |
| `Top()` | Xem request có priority cao nhất |
| `ExtractMax()` | Lấy và xóa request priority cao nhất |
| `RemoveById()` | Hủy request theo BookingID |
| `BuildHeap()` | Xây dựng Heap từ dữ liệu ban đầu |
| `Empty()` | Kiểm tra hàng đợi có rỗng hay không |
| `Size()` | Lấy số lượng request đang chờ |


Các hàm hỗ trợ:

| Hàm |	Vai trò |
| --- | --- |
| `HigherPriority()` |	So sánh priority |
| `SiftUp()` |	Khôi phục Heap sau khi thêm |
| `SiftDown()` |	Khôi phục Heap sau khi xóa/thay đổi |
| `RemoveAtIndex()` |	Xóa request tại một vị trí |
| `SwapAndSync()` |	Đổi vị trí và cập nhật indexMap |
## 4. Q2 — Key và Workload
Key
Các thuộc tính dùng để xác định priority:
membershipTier
bookingTimestamp
bookingId

Thứ tự:
``` text
Tier cao hơn
    ↓
Timestamp sớm hơn
    ↓
BookingID
```
### Workload
RF2 có các thao tác:
- Thêm request khi có khách tranh xe.
- Lấy request có priority cao nhất.
- Xóa request khi khách hủy.
- Kiểm tra request đang đứng đầu.
- Quản lý số lượng request đang chờ.
Dữ liệu có thể thay đổi liên tục nên cần cấu trúc dữ liệu có khả năng thêm và xóa request mà không phải sắp xếp lại toàn bộ danh sách.
## 5. Q3 — Phân tích Worst-case
Nếu sử dụng danh sách chưa sắp xếp, mỗi lần tìm request có priority (ưu tiên) cao nhất có thể phải duyệt toàn bộ danh sách:
O(n)

Với Max Heap:
- Request priority cao nhất nằm tại root.
- `Top()` lấy root trực tiếp.
- `InsertRequest()` sử dụng `SiftUp()`.
- `ExtractMax()` sử dụng `SiftDown()`.
- `RemoveById()` sử dụng indexMap để xác định vị trí trước khi điều chỉnh Heap.

Complexity:

| Operation | Complexity |
|---|---|
| `Top()` |	O(1) |
| `InsertRequest()` |	O(log n) |
| `ExtractMax()` |	O(log n) |
| `RemoveAtIndex()` |	O(log n) |
| ` RemoveById()`  |	O(1) average để tìm + O(log n) điều chỉnh |
| ` BuildHeap()`  |	O(n) |
| ` Empty()`  |	O(1) |
| ` Size()`  |	O(1) |
---
## 6. Q4 — Các yếu tố phi tiệm cận
### Bộ nhớ
Component sử dụng:
``` text
vector<RentalRequest> heap;
unordered_map<string, int> indexMap;
```
` heap ` lưu các request.

`indexMap` lưu ánh xạ: 
`BookingID` → `Heap Index`

`indexMap` sử dụng thêm bộ nhớ nhưng giúp tìm vị trí request theo
BookingID nhanh hơn.

### Đồng bộ dữ liệu
Khi hai request đổi vị trí trong Heap, vị trí trong indexMap cũng
phải được cập nhật.

Do đó component sử dụng: 
`SwapAndSync() `

Hàm này:
1. Đổi vị trí hai request trong Heap.
2. Cập nhật vị trí mới của hai BookingID trong indexMap.
Nếu không cập nhật indexMap, thông tin vị trí request có thể bị sai.

### Khả năng cập nhật
RF2 cần hỗ trợ:
``` text
Insert
Remove
Extract Max
```
Max Heap phù hợp với workload này vì các thao tác điều chỉnh Heap
là O(log n).
## 7. Trade-off
Ưu điểm
- Top() lấy request ưu tiên cao nhất trong O(1).
- InsertRequest() không cần sắp xếp lại toàn bộ dữ liệu.
- ExtractMax() thực hiện trong O(log n).
- Có thể xóa request theo BookingID.
- indexMap giúp xác định vị trí request nhanh.
Nhược điểm
- Cần thêm bộ nhớ cho indexMap.
- heap và indexMap phải luôn được đồng bộ.
- Logic xóa request phức tạp hơn so với việc chỉ sử dụng một mảng.
