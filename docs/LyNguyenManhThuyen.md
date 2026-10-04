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
---

# D4 — IMPLEMENTATION

## 1. Cấu trúc dữ liệu RentalRequest

RF2 sử dụng cấu trúc `RentalRequest` để lưu thông tin của một yêu cầu đặt xe.

```cpp
struct RentalRequest {
    string bookingId;
    string customerId;
    string carId;
    int membershipTier;
    long long bookingTimestamp;
};
```
Các thuộc tính trong cấu trúc được sử dụng:
| Trường |	Vai trò |
|---|---|
| `bookingId` |	Định danh duy nhất của request và dùng làm tiêu chí phá hòa khi membershipTier và bookingTimestamp giống nhau | 
| `customerId` |	Xác định khách hàng |
|`carId` |	Xác định xe đang tranh |
|`membershipTier` |	Tiêu chí ưu tiên thứ nhất, Tier nào cao hơn sẽ được ưu tiên cao hơn |
|`bookingTimestamp` |	Tiêu chí ưu tiên thứ hai, Timestamp nhỏ hơn được ưu tiên |
## 2. Cấu trúc MyMaxHeap

Component sử dụng:
```cpp
vector<RentalRequest> heap;
unordered_map<string, int> indexMap;
```
`heap` sẽ lưu các `RentalRequest` theo cấu trúc Max Heap.
Request có priority cao nhất luôn được duy trì tại vị trí:
`heap[0]`

`indexMap`
Lưu ánh xạ:
BookingID → vị trí trong Heap

Mục đích là hỗ trợ tìm vị trí của request khi thực hiện
`RemoveById()`.
## 3. HigherPriority()
``` cpp
bool HigherPriority(const RentalRequest& a, const RentalRequest& b)
```
Hàm dùng để xác định yêu cầu a có priority (ưu tiên) cao hơn yêu cầu b hay không.
Thứ tự so sánh:
```text
   Membership Tier cao hơn
              ↓
  Booking Timestamp nhỏ hơn
              ↓
          BookingID
```
```cpp
Code:
if (a.membershipTier != b.membershipTier) {
    return a.membershipTier > b.membershipTier;
}

if (a.bookingTimestamp != b.bookingTimestamp) {
    return a.bookingTimestamp < b.bookingTimestamp;
}

return a.bookingId < b.bookingId;
```
## 4. Hàm InsertRequest()
InsertRequest() dùng để thêm một request mới vào Heap.
Quy trình:
```text
  Thêm request vào cuối Heap
              ↓
Cập nhật vị trí trong indexMap
              ↓
          SiftUp()
              ↓
Khôi phục tính chất Max Heap
```
Vì `InsertRequest()` thêm phần tử vào cuối vector là `O(1)` trung bình, nhưng sau đó phải gọi `SiftUp()` để đưa request (yêu cầu) về đúng vị trí trong Max Heap. Và trong `Binary Max Heap`, mỗi lần `SiftUp()` chỉ đi lên một tầng. Với `n` thì chiều cao của heap chỉ có `log n`. Vậy trường hợp xấu nhất là nó phải đi từ dưới cuối cùng lên đầu đỉnh request cao nhất nên nó phải đi `log n` bước.

Độ phức tạp thuật toán là: `O(log n)`.
## 5. Hàm SiftUp()
`SiftUp()` được sử dụng sau khi thêm request.

Request mới được so sánh với node cha. Nếu request mới có priority cao
hơn node cha thì hai phần tử được đổi vị trí.
Quá trình tiếp tục cho đến khi tính chất Max Heap được khôi phục.

Độ phức tạp thuật toán là: O(log n).
## 6. Hàm SiftDown()
SiftDown() được sử dụng khi cần khôi phục Max Heap sau thao tác
xóa.

Hàm so sánh node hiện tại với các node con và đưa request có priority cao hơn lên vị trí phù hợp.

Độ phức tạp thuật toán là: O(log n).
## 7. Hàm Top()
`Top()` trả về request có priority cao nhất nhưng không xóa request.
Do request có priority cao nhất luôn nằm tại đỉnh:
`heap[0]`.

Nên thao tác `Top()` có độ phức tạp là O(1).
Nếu Heap rỗng, hàm phát sinh `runtime_error`.
## 8. Hàm ExtractMax()
`ExtractMax()` lấy và xóa request có priority cao nhất.
Quy trình:
```text
Lấy request tại đỉnh
        ↓
 RemoveAtIndex(0)
        ↓
 Khôi phục Heap
        ↓
 Trả về request
```
Độ phức tạp là: O(log n).
## 9. Hàm RemoveById()
RemoveById() dùng để xóa request dựa trên BookingID.

Quy trình:
```text
     BookingID
         ↓
 Tìm trong indexMap
         ↓
Lấy vị trí của request
         ↓
   RemoveAtIndex()
         ↓
   Khôi phục Heap
```
Nếu BookingID không tồn tại thì hàm trả về `false`.

Nếu tìm thấy request thì hàm xóa request và trả về `true`.

Độ phức tạp thuật toán: O(log n) vì:
- Khi tìm kiếm trên index là: `O(1)`.
- Khi khôi phục phại Heap là: `O(log n)`.
## 10. Hàm RemoveAtIndex()
`RemoveAtIndex()` thực hiện xóa một request tại vị trí bất kỳ trong
Heap.
Khi xóa một phần tử không phải phần tử cuối, phần tử cuối của Heap
được đưa vào vị trí bị xóa.
Sau đó Heap được điều chỉnh bằng SiftUp() và SiftDown().

Độ phức tạp thuật toán: O(log n).
## 11. Hàm SwapAndSync()
`SwapAndSync()` thực hiện việc đổi vị trí hai request trong Heap và
đồng thời cập nhật vị trí tương ứng trong `indexMap`.

Điều này đảm bảo:
```text
 Heap
  ↕
indexMap
```
Sẽ luôn được đồng bộ sau khi các request thay đổi vị trí.
## 12. Hàm BuildHeap()
`BuildHeap()` xây dựng Max Heap từ một danh sách `RentalRequest` ban đầu.
Quy trình:
```text
Danh sách request ban đầu
           ↓
     Đưa vào Heap
           ↓
   Xây dựng indexMap
           ↓
  SiftDown từ dưới lên
           ↓
        Max Heap
```
Độ phức tạp thuật toán: O(n).
## 13. Hàm Empty() và Size()
`Empty()` kiểm tra Heap có rỗng hay không.

Độ phức tạp thuật toán là: O(1).

`Size()` trả về số lượng request hiện đang có trong Heap.

Độ phức tạp thuật toán là: O(1).
# D5 — TESTING VÀ DEBUGGING

## 1. Mục tiêu kiểm thử

Mục tiêu của phần kiểm thử là xác nhận `MyMaxHeap` thực hiện đúng các quy tắc ưu tiên của RF2 và duy trì đúng tính chất Max Heap sau các thao tác thêm, lấy và xóa request.

Các nội dung cần kiểm tra:

- Thứ tự ưu tiên theo `membershipTier`.
- Thứ tự ưu tiên theo `bookingTimestamp`.
- Trường hợp cùng Tier và cùng Timestamp.
- Thêm request bằng `InsertRequest()`.
- Lấy request ưu tiên cao nhất bằng `Top()` và `ExtractMax()`.
- Xóa request theo `BookingID` bằng `RemoveById()`.
- Xử lý Heap rỗng.
- Đảm bảo `indexMap` được đồng bộ với Heap.

---

## 2. Kiểm thử quy tắc Priority (Ưu tiên)

### Test Case 1 — Khác Membership Tier

| BookingID | Tier | Timestamp |
|---|---:|---:|
| B001 | 1 | 100 |
| B002 | 3 | 200 |
| B003 | 2 | 150 |

Kết quả mong đợi:

```text
B002 → B003 → B001
```
-> Membership Tier cao hơn được ưu tiên trước.
### Test Case 2 — Cùng Tier, khác Timestamp
| BookingID | Tier | Timestamp |
|---|---:|---:|
| B001 | 3 | 300 |
| B002 | 3 | 100 |
| B003 | 3 | 200 |
Kết quả mong đợi:
```text
B002 → B003 → B001
```
-> Khi cùng Tier, request có bookingTimestamp nhỏ hơn được ưu
tiên trước.
### Test Case 3 — Cùng Tier và cùng Timestamp
| BookingID | Tier | Timestamp |
|---|---:|---:|
| B001 | 3 | 300 |
| B002 | 3 | 300 |
| B003 | 3 | 300 |
Kết quả mong đợi:
```text
B001 → B002 → B003
```
Khi Tier và Timestamp giống nhau, bookingId được sử dụng để
phá hòa theo quy tắc đã thiết kế.
## 3. Kiểm thử `InsertRequest()`

| Test | Thao tác | Kết quả mong đợi | Kết quả thực tế | Đánh giá |
|---|---|---|---|---|
| TC01 | Thêm `B001` (Tier 1) | `Size() = 1` và `Top() = B001` | `Size() = 1`, `Top() = B001` | PASS |
| TC02 | Thêm `B002` (Tier 3) sau `B001` | `Top() = B002` | `Top() = B002` | PASS |

---

## 4 Kiểm thử `ExtractMax()`

| Test | Dữ liệu | Thao tác | Kết quả mong đợi | Kết quả thực tế | Đánh giá |
|---|---|---|---|---|---|
| TC03 | `B001` (Tier 1), `B002` (Tier 3) | `ExtractMax()` | Lấy ra `B002` | Lấy ra `B002` | PASS |
| TC04 | Sau khi lấy `B002` | `Top()` | `Top() = B001` | `Top() = B001` | PASS |

---

## 5 Kiểm thử `RemoveById()`

| Test | Dữ liệu / Thao tác | Kết quả mong đợi | Kết quả thực tế | Đánh giá |
|---|---|---|---|---|
| TC05 | `RemoveById("B002")` — B002 tồn tại | Trả về `true`, `Size` giảm 1 | ... | ... |
| TC06 | `RemoveById("B999")` — B999 không tồn tại | Trả về `false`, Heap không thay đổi | ... | ... |

---

## 6 Kiểm thử Heap rỗng

| Test | Thao tác | Kết quả mong đợi | Kết quả thực tế | Đánh giá |
|---|---|---|---|---|
| TC07 | `Empty()` khi Heap rỗng | Trả về `true` | ... | ... |
| TC08 | `Size()` khi Heap rỗng | Trả về `0` | ... | ... |
| TC09 | `Top()` khi Heap rỗng | Phát sinh `runtime_error` | ... | ... |
| TC10 | `ExtractMax()` khi Heap rỗng | Phát sinh `runtime_error` | ... | ... |

---

## 7 Kiểm thử `indexMap`

| Test | Thao tác | Kết quả mong đợi | Kết quả thực tế | Đánh giá |
|---|---|---|---|---|
| TC11 | Thêm `B001`, `B002`, `B003` | `indexMap` lưu đúng vị trí của từng request | ... | ... |
| TC12 | Thêm request có priority cao để xảy ra `SiftUp()` | Vị trí trong `indexMap` được cập nhật theo vị trí mới | ... | ... |
| TC13 | `RemoveById("B002")` | `B002` bị xóa khỏi Heap và `indexMap` | ... | ... |

---
## 8 Nhật ký Debug
| STT | Vấn đề | Nguyên nhân | Cách xử lý | Kết quả |
|---|---|---|---|---|
| 1 | Khi request a và b cùng Tier và cùng Time| Do ban đầu `HigherPriority()` chỉ xét trường hợp khác Tier và Time nên khi cả hai đều giống nhau về Tier và Time không có điều kiện nào để phân biệt được hai request | Bổ sung `bookingId` điều kiện phân biệt Id nào nhỏ hơn sẽ được ưu tiên | Đã khắc phục |
| 2 | `indexMap` có thể lưu sai vị trí khi mà các request nó được đổi chỗ trong Heap | Khi chương trình thực hiện `SiftUp()` và ` SiftDown()` để đổi vị trí của hai request nhưng vị trí trong `indexMap` không được cập nhật | Nên đã bổ sung thêm hàm `SwapAndSyncc()` để vừa thay đổi vị trí trong Heap mà còn vừa cập nhật vị trí của `indexMap` | Đã khắc phục |
# D6 — ĐÁNH GIÁ KỸ THUẬT THÀNH VIÊN TRONG NHÓM

## 1. Thành phần được đánh giá

- Cấu trúc dữ liệu: Trie
- Thành phần: `CarTrie`
- File cài đặt: `Trie.h`
- File kiểm thử: `TrieTest.cpp`
- Mục đích: Hỗ trợ tìm kiếm và gợi ý tên xe theo tiền tố.

---

## 2. Nội dung đánh giá

 Tiến hành xem xét phần cách cài đặt `CarTrie` từ nhiệm vụ của thành viên 4 trong nhóm, nó tập trung vào:

- Cách xây dựng cây Trie.
- Cách thêm tên hãng và dòng xe.
- Cách tìm kiếm theo tiền tố.
- Cách xử lý khi không tìm thấy kết quả.
- Cách kiểm thử các chức năng.
- Cách kiểm tra tốc độ Trie so với tìm kiếm tuyến tính.

---

## 3. Đánh giá phần cài đặt

### 3.1. Cấu trúc dữ liệu

`CarTrie` sử dụng `TrieNode` để lưu các ký tự của tên xe.

| Thành phần | Vai trò |
|---|---|
| `children` | Lưu các node con theo từng ký tự |
| `isEndOfWord` | Xác định node có kết thúc một từ khóa |
| `matchedFullCarNames` | Lưu tên xe đầy đủ tương ứng với từ khóa |

Việc sử dụng Trie phù hợp với yêu cầu tìm kiếm theo tiền tố vì các tên xe được tổ chức theo từng ký tự.

### 3.2. Thêm dữ liệu

Hàm `insertCar()` tạo tên đầy đủ từ hãng xe và dòng xe.

Ví dụ:

```text
Toyota Innova
```
Ngoài tên đầy đủ, chương trình còn thêm từ khóa của dòng xe để có thể tìm kiếm linh hoạt hơn.
```text
     Toyota Innova
           ↓
toyota innova và innova
```
### 3.3. Tìm kiếm
Hàm `getAllSuggestions()` duyệt `Trie` theo từng ký tự của tiền tố cần tìm.

Nếu tìm thấy tiền tố, chương trình tiếp tục duyệt các node phía dưới để lấy các tên xe phù hợp.

Cách thực hiện rất phù hợp với mục đích tìm kiếm tên xe theo tiền tố.
## 4. Đánh giá phần kiểm thử
File kiểm thử đã kiểm tra một số trường hợp chính:

| Test |	Nội dung kiểm tra |	Kết quả |
|---|---|---|
|Test 1 |	Tìm xe theo tiền tố toy |	PASS |
| Test 2 |  Tìm dòng xe inno |	PASS |
|Test 3 |	Tìm tiền tố i10 có nhiều kết quả |	PASS |
| Test 4 |	Tìm tiền tố không tồn tại Ferrari |	PASS |

Chương trình sử dụng assert() để đối chiếu kết quả.
```cpp
assert(resInnova.size() == 1);
assert(resInnova[0] == "Toyota Innova");
```
Nếu kết quả không đúng điều kiện, chương trình sẽ báo lỗi kiểm thử.
## 5. Những điểm làm tốt
| STT | Nội dung | Đánh giá |
|---|---|---|
| 1 | Sử dụng Trie cho tìm kiếm theo tiền tố | Phù hợp với yêu cầu |
| 2 | Có chuẩn hóa chữ thường | Giúp việc tìm kiếm linh hoạt hơn |
| 3 | Có kiểm tra tiền tố tồn tại | Kiểm tra chức năng chính |
| 4 | Có kiểm tra tiền tố không tồn tại | Có xử lý trường hợp đặc biệt |
| 5 | Có kiểm tra trường hợp nhiều kết quả | Phù hợp với chức năng gợi ý |
| 6 | Có so sánh Trie với tìm kiếm tuyến tính | Có cơ sở đánh giá tốc độ |
| 7 | Kiểm tra với nhiều kích thước dữ liệu | Giúp đánh giá khi dữ liệu tăng |
## 7. Vấn đề phát hiện được

Qua kiểm tra phần `CarTrie` và các trường hợp kiểm thử hiện có,
chưa phát hiện lỗi nghiêm trọng ảnh hưởng đến chức năng chính.

Các chức năng tìm kiếm theo tiền tố, tìm kiếm dòng xe, xử lý nhiều
kết quả và trường hợp không tìm thấy đều được kiểm thử và cho kết quả
đúng.

Phần kiểm tra tốc độ cũng được thực hiện với nhiều kích thước dữ liệu
để đánh giá khả năng hoạt động khi số lượng xe tăng.
# D7 — ĐÁNH GIÁ HIỆU NĂNG và NHẬT KÍ SỬ DỤNG AI
# Đánh giá hiệu năng
### 1. Những gì em đã học được

Qua quá trình thực hiện RF2 — Xử lý xung đột đặt xe, em hiểu rõ hơn cách
lựa chọn và áp dụng cấu trúc dữ liệu dựa trên yêu cầu thực tế của bài
toán.

Trước khi thực hiện, em biết Max Heap có thể sử dụng để lấy phần tử có
độ ưu tiên cao nhất. Tuy nhiên, khi áp dụng vào RF2, em hiểu rằng cần
xác định rõ quy tắc priority trước khi xây dựng cấu trúc dữ liệu.

Trong phần của mình, request được xác định độ ưu tiên theo thứ tự:

```text
Membership Tier cao hơn
        ↓
Booking Timestamp sớm hơn
        ↓
BookingID
```
Em cũng hiểu rõ hơn về việc kết hợp nhiều cấu trúc dữ liệu. Ngoài
vector dùng để lưu Heap, em sử dụng indexMap để ánh xạ
BookingID đến vị trí của request trong Heap. Điều này giúp
RemoveById() có thể xác định vị trí request nhanh hơn thay vì phải
duyệt toàn bộ Heap.

### 2. Khó khăn gặp phải
Khó khăn đầu tiên là xử lý trường hợp hai request có cùng
membershipTier và cùng bookingTimestamp. Nếu chỉ xét hai thuộc tính
này thì chương trình không xác định được request nào có priority cao
hơn.

Khó khăn tiếp theo là duy trì sự đồng bộ giữa Heap và indexMap. Khi
request thay đổi vị trí trong quá trình SiftUp() hoặc SiftDown(),
vị trí được lưu trong indexMap cũng phải được cập nhật. Nếu không,
RemoveById() có thể tìm đến vị trí không chính xác.

Ngoài ra, em cũng gặp khó khăn trong việc kiểm thử các trường hợp đặc
biệt như Heap rỗng, BookingID không tồn tại và trường hợp nhiều
request có cùng priority.
### 3. Cách em giải quyết
Đối với trường hợp cùng Tier và cùng Timestamp, em bổ sung BookingID
làm tiêu chí phá hòa. Nhờ đó, các request luôn có một thứ tự ưu tiên
xác định.

Đối với vấn đề đồng bộ indexMap, em sử dụng hàm SwapAndSync(). Hàm này vừa thực hiện đổi vị trí hai request trong Heap vừa cập nhật lại vị trí tương ứng trong indexMap.
Sau đó, em xây dựng chương trình kiểm thử riêng để kiểm tra các thao
tác chính của MyMaxHeap, bao gồm:
- InsertRequest()
- Top()
- ExtractMax()
- RemoveById()
- Empty()
- Size()
- Trường hợp cùng Tier và cùng Timestamp
- Trường hợp Heap rỗng
Việc chạy các trường hợp này giúp em kiểm tra lại kết quả thực tế thay
vì chỉ dựa vào việc đọc code.
### 4. Điều em nhận ra sau khi thực hiện
Qua phần RF2, em nhận ra rằng việc lựa chọn cấu trúc dữ liệu phải dựa
trên workload của bài toán chứ không chỉ dựa trên việc cấu trúc dữ
liệu đó có sẵn.

Max Heap phù hợp với RF2 vì hệ thống thường xuyên cần xác định request
có priority cao nhất. Tuy nhiên, khi triển khai thực tế thì chỉ dùng
Heap là chưa đủ cho thao tác xóa theo BookingID, nên cần kết hợp thêm
indexMap.

Em cũng nhận ra rằng các trường hợp đặc biệt cần được xác định ngay
trong quá trình thiết kế, đặc biệt là các trường hợp phá hòa và dữ liệu không tồn tại.
### 5. Tự đánh giá và hướng cải thiện
Qua phần việc này, em đã hiểu rõ hơn về Max Heap, độ phức tạp của các
thao tác và cách kết hợp Heap với cấu trúc dữ liệu phụ trợ.

Tuy nhiên, em vẫn cần cải thiện khả năng thiết kế và kiểm thử ngay từ
đầu thay vì chỉ xử lý các vấn đề khi chúng xuất hiện trong quá trình
cài đặt.

Nếu thực hiện lại, em sẽ xác định đầy đủ các trường hợp kiểm thử và
quy tắc priority trước khi bắt đầu code. Em cũng sẽ thống nhất cách
lưu và sử dụng dữ liệu với các thành viên khác sớm hơn để việc tích hợp RF2 vào hệ thống chung thuận lợi hơn.
## Nhật kí sử dụng AI
Trong quá trình thực hiện phần RF2, em sử dụng AI như một công cụ hỗ
trợ học tập, kiểm tra và hoàn thiện báo cáo. AI chủ yếu được sử dụng để
giải thích thuật toán, phân tích độ phức tạp, hỗ trợ tìm lỗi và gợi ý
cách kiểm thử.

| STT | Nội dung sử dụng AI | Mục đích | Kết quả |
|---:|---|---|---|
| 1 | Tìm hiểu cách hoạt động của Max Heap | Hiểu cách duy trì phần tử có độ ưu tiên cao nhất | Hiểu được vai trò của `SiftUp()` và `SiftDown()` |
| 2 | Phân tích độ phức tạp của các hàm | Kiểm tra lại phần phân tích Big-O | Hoàn thiện bảng độ phức tạp của `MyMaxHeap` |
| 3 | Xử lý trường hợp cùng Tier và cùng Timestamp | Tìm cách xác định thứ tự khi hai request có cùng độ ưu tiên | Sử dụng `BookingID` làm tiêu chí phá hòa |
| 4 | Kiểm tra vấn đề `indexMap` | Hiểu cách duy trì vị trí request khi Heap thay đổi | Sử dụng `SwapAndSync()` để cập nhật vị trí |
| 5 | Hỗ trợ xây dựng chương trình kiểm thử | Kiểm tra các thao tác của `MyMaxHeap` | Tạo file test và chạy các trường hợp chính |
| 6 | Hỗ trợ hoàn thiện nội dung báo cáo | Trình bày quá trình thực hiện theo từng D | Hoàn thiện báo cáo cá nhân |

### Link trao đổi AI

[Link nhật ký trao đổi AI01](https://chatgpt.com/share/6ac24c67-e584-83ec-a506-459957a65bce)

[Link nhật ký trao đổi AI02](https://chatgpt.com/share/6ac24ca5-f30c-83ec-b074-1202fa31aaef)

[Link nhật ký trao đổi AI03](https://chatgpt.com/share/6ac24f60-5540-83ec-a7e4-95c620243996)
