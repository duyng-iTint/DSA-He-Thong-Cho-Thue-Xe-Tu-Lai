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

### RF2 — Xử lý ưu tiên khi tranh xe

RF2 xử lý trường hợp nhiều khách hàng cùng đặt một xe và xảy ra tranh chấp.

Khi đó, hệ thống cần xác định yêu cầu thuê xe nào được ưu tiên xử lý trước dựa trên mức độ ưu tiên của khách hàng và thời gian đặt xe. Ngoài ra, hệ thống cần hỗ trợ lấy yêu cầu ưu tiên cao nhất, hủy yêu cầu theo `BookingID` và tiếp tục xử lý trên tập yêu cầu đang thay đổi.

**Component cá nhân được sử dụng:**

```text
MyMaxHeap
```

Kết hợp với:

```text
indexMap: BookingID → Heap Index
```

---

## 2. Quy tắc xác định độ ưu tiên

Priority của một `RentalRequest` được xác định theo khóa tổng hợp gồm ba tiêu chí:

```text
1. Membership Tier giảm dần
        ↓
2. Booking Timestamp tăng dần
        ↓
3. Booking ID tăng dần
```

Cụ thể:

- `membershipTier` cao hơn → ưu tiên cao hơn.
- Nếu cùng `membershipTier` → `bookingTimestamp` sớm hơn → ưu tiên cao hơn.
- Nếu cùng `membershipTier` và cùng `bookingTimestamp` → `bookingId` nhỏ hơn theo thứ tự tăng dần sẽ được ưu tiên.

Việc thêm `BookingID` làm tiêu chí thứ ba giúp xác định rõ thứ tự khi hai yêu cầu có cùng hạng thành viên và cùng thời điểm đặt.

---

## 3. Q1 — Requirement cần thao tác gì?

RF2 không cần sắp xếp toàn bộ hàng chờ. Requirement chỉ cần luôn lấy ra yêu cầu có priority cao nhất.

Các thao tác chính:

| Thao tác | Mục đích |
|---|---|
| `InsertRequest()` | Thêm yêu cầu mới vào hàng chờ |
| `Top()` | Xem yêu cầu có priority cao nhất |
| `ExtractMax()` | Lấy và xóa yêu cầu có priority cao nhất |
| `RemoveById()` | Hủy một yêu cầu theo `BookingID` |
| `BuildHeap()` | Xây dựng Heap từ tập dữ liệu ban đầu |
| `Empty()` | Kiểm tra hàng chờ có rỗng hay không |
| `Size()` | Lấy số lượng yêu cầu đang chờ |

Các hàm hỗ trợ:

| Hàm | Vai trò |
|---|---|
| `HigherPriority()` | So sánh priority giữa hai `RentalRequest` |
| `SiftUp()` | Khôi phục tính chất Max-Heap sau khi thêm phần tử |
| `SiftDown()` | Khôi phục tính chất Max-Heap sau khi xóa hoặc thay đổi vị trí |
| `RemoveAtIndex()` | Xóa request tại một vị trí trong Heap |
| `SwapAndSync()` | Hoán đổi hai phần tử và cập nhật `indexMap` |

---

## 4. Q2 — Key và Workload

### Key

RF2 sử dụng khóa tổng hợp gồm:

```text
Membership Tier
        ↓
Booking Timestamp
        ↓
Booking ID
```

Thứ tự ưu tiên:

```text
Tier cao hơn
    ↓
Timestamp sớm hơn
    ↓
BookingID nhỏ hơn
```

`BookingID` được sử dụng làm tiêu chí cuối để bảo đảm thứ tự được xác định rõ khi các tiêu chí trước có cùng giá trị.

### Workload

RF2 có các thao tác chính:

- Thêm yêu cầu khi có khách tranh xe.
- Lấy yêu cầu có priority cao nhất.
- Hủy yêu cầu đang chờ theo `BookingID`.
- Xem yêu cầu đứng đầu.
- Tiếp tục xử lý trên tập yêu cầu đang thay đổi.

Yêu cầu đến liên tục và có thể bị hủy giữa chừng. Mỗi lần xử lý thường chỉ cần lấy ra một yêu cầu có priority cao nhất thay vì sắp xếp toàn bộ hàng chờ.

Do đó, cấu trúc dữ liệu cần hỗ trợ tốt việc thêm, lấy phần tử lớn nhất và hủy một yêu cầu bất kỳ.

---

## 5. Q3 — Phân tích Worst-case

Nếu sử dụng danh sách chưa sắp xếp, mỗi lần tìm yêu cầu có priority cao nhất phải duyệt qua toàn bộ danh sách:

```text
O(N)
```

Điều này không phù hợp khi số lượng yêu cầu tăng và các thao tác lấy hoặc hủy xảy ra liên tục.

Với Binary Max-Heap:

- Yêu cầu có priority cao nhất luôn nằm tại root.
- `Top()` lấy phần tử tại root trong `O(1)`.
- `InsertRequest()` sử dụng `SiftUp()`.
- `ExtractMax()` sử dụng `SiftDown()`.
- `RemoveById()` sử dụng `indexMap` để tìm nhanh vị trí của request trước khi điều chỉnh Heap.

### Complexity

| Operation | Complexity |
|---|---:|
| `Top()` | O(1) |
| `InsertRequest()` | O(log N) |
| `ExtractMax()` | O(log N) |
| `RemoveAtIndex()` | O(log N) |
| `RemoveById()` | O(1) average để tìm + O(log N) điều chỉnh |
| `BuildHeap()` | O(N) |
| `Empty()` | O(1) |
| `Size()` | O(1) |

Thiết kế này phù hợp với yêu cầu cần lấy và hủy yêu cầu trong thời gian `O(log N)` thay vì phải quét toàn bộ hàng chờ.

---

## 6. Q4 — Các yếu tố phi tiệm cận

### Bộ nhớ

Component sử dụng:

```cpp
vector<RentalRequest> heap;
unordered_map<string, int> indexMap;
```

Trong đó:

- `heap` lưu các `RentalRequest` theo cấu trúc Binary Max-Heap.
- `indexMap` lưu ánh xạ:

```text
BookingID → Heap Index
```

`indexMap` sử dụng thêm bộ nhớ nhưng giúp xác định vị trí của một yêu cầu theo `BookingID` nhanh hơn, phục vụ thao tác hủy yêu cầu.

### Đồng bộ dữ liệu

Khi hai request đổi vị trí trong Heap, vị trí tương ứng trong `indexMap` cũng phải được cập nhật.

Do đó component sử dụng:

```text
SwapAndSync()
```

Hàm này thực hiện:

1. Hoán đổi vị trí hai request trong Heap.
2. Cập nhật vị trí mới của hai `BookingID` trong `indexMap`.

Nếu không cập nhật `indexMap`, vị trí lưu trong bảng chỉ mục có thể không còn khớp với vị trí thực tế trong Heap.

### Khả năng cập nhật

RF2 thường xuyên phải xử lý:

```text
Insert
Remove
Extract Max
```

Max-Heap phù hợp với workload này vì:

- Thêm phần tử: `O(log N)`.
- Lấy phần tử ưu tiên cao nhất: `O(log N)`.
- Hủy một phần tử theo `BookingID`: tìm vị trí nhanh nhờ `indexMap`, sau đó điều chỉnh Heap.

---

## 7. Trade-off

### Ưu điểm

- `Top()` lấy yêu cầu có priority cao nhất trong `O(1)`.
- `InsertRequest()` không cần sắp xếp lại toàn bộ hàng chờ.
- `ExtractMax()` thực hiện trong `O(log N)`.
- Có thể hủy yêu cầu theo `BookingID`.
- `indexMap` giúp xác định vị trí request nhanh.
- Phù hợp với trường hợp yêu cầu đến và bị hủy liên tục.

### Nhược điểm

- Heap không giữ toàn bộ hàng chờ theo thứ tự ưu tiên hoàn chỉnh.
- Muốn xem toàn bộ danh sách theo thứ tự phải lấy lần lượt các phần tử, có thể tốn `O(N log N)`.
- Cần thêm bộ nhớ cho `indexMap`.
- `heap` và `indexMap` phải luôn được đồng bộ.
- Chỉ mục phụ thuộc vào `BookingID`, không hỗ trợ trực tiếp các kiểu truy vấn phức tạp khác.

---

## 8. Điều kiện làm lựa chọn không còn phù hợp

Max-Heap phù hợp khi hệ thống thường xuyên:

- Thêm yêu cầu.
- Lấy yêu cầu có priority cao nhất.
- Hủy yêu cầu theo `BookingID`.

Tuy nhiên, lựa chọn này không còn tối ưu nếu requirement thay đổi thành:

- Cần hiển thị toàn bộ hàng chờ theo thứ tự ưu tiên.
- Cần truy vấn theo khoảng hoặc nhiều tiêu chí phức tạp.
- Hàng chờ rất nhỏ, khi đó duyệt tìm phần tử lớn nhất có thể đơn giản hơn.
- Quy tắc priority thay đổi liên tục theo thời gian chờ và cần cập nhật khóa thường xuyên.

Trong trường hợp cần hiển thị toàn bộ danh sách theo thứ tự ưu tiên, có thể sao chép dữ liệu từ Heap rồi thực hiện Sort mà không làm thay đổi Heap gốc. Nếu cần tìm kiếm hoặc lọc theo nhiều tiêu chí, có thể kết hợp thêm cấu trúc dữ liệu phù hợp.

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

## 14 Lịch sử commit
[Link commit 01](https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/4850d66059c46acf664e0b01734069fc3c603296)

[Link commit 02](https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/2a714ee303afb0fe6b4bf53e2715e4e6ea7464fd)

[Link commit 03](https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/44adac712f2e3d6ce6e2948a272be5dd40630889)

[Link commit 04](https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/dd36b37e911702c9b158ad856b44786ff91b2371)

[Link commit 05]()
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
# D7 — BÀI PHẢN TƯ CÁ NHÂN và NHẬT KÍ SỬ DỤNG AI
# Bài phản tư cá nhân
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

# Nhật kí trao đổi AI
# Nhật ký AI – Phát triển RF2 Max Heap

## 1. Thông tin cuộc trò chuyện

* **Dự án:** Hệ thống quản lý cho thuê xe tự lái và xử lý tranh chấp đặt xe
* **Yêu cầu:** RF2 – Ưu tiên xử lý các yêu cầu khi nhiều khách hàng tranh chấp chiếc xe cuối cùng
* **Thành viên phụ trách:** M3
* **Branch Git:** `feature/m3-maxheap`
* **Mục tiêu:** Xây dựng Max Heap tự cài đặt để ưu tiên yêu cầu thuê xe theo hạng thành viên, thời gian đặt và mã booking.

---

## 2. Nội dung đã trao đổi

### 2.1. Thiết kế `RentalRequest`

Mỗi yêu cầu thuê xe được biểu diễn bằng cấu trúc `RentalRequest`, gồm:

* `bookingId`: Mã đặt xe.
* `customerId`: Mã khách hàng.
* `carId`: Mã xe.
* `membershipTier`: Hạng thành viên.
* `bookingTimestamp`: Thời điểm đặt xe.

Cấu trúc này được Heap sử dụng để so sánh và xác định thứ tự ưu tiên của các yêu cầu.

### 2.2. Quy tắc ưu tiên

Hàm `HigherPriority()` so sánh hai yêu cầu theo thứ tự:

1. Hạng thành viên cao hơn được ưu tiên: VIP = 3, Gold = 2, Standard = 1.
2. Nếu cùng hạng, yêu cầu có `bookingTimestamp` nhỏ hơn (đặt sớm hơn) được ưu tiên.
3. Nếu cả hạng và thời gian đều giống nhau, dùng `bookingId` làm tiêu chí phụ để xác định thứ tự ổn định.

Ví dụ: Hai yêu cầu VIP lúc 10:02 và 10:05 sẽ được xử lý trước yêu cầu Gold lúc 10:01 vì hạng thành viên được xét trước thời gian.

### 2.3. Vai trò của `SiftUp()` và `SiftDown()`

* **`SiftUp(int i)`:** Dùng sau khi thêm phần tử mới. Nếu phần tử mới có ưu tiên cao hơn node cha, hai node đổi chỗ và tiếp tục kiểm tra lên trên.
* **`SiftDown(int i)`:** Dùng sau khi xóa phần tử, thường là phần tử ở gốc Heap. Phần tử được đưa lên thay thế có thể cần đi xuống để khôi phục tính chất Max Heap.

Điều kiện vòng lặp trong `SiftUp()` phải là `i > 0`, vì node tại index `0` là gốc và không có node cha.

### 2.4. Các hàm chính của `MyMaxHeap`

| Hàm                | Chức năng                                      |
| ------------------ | ---------------------------------------------- |
| `HigherPriority()` | So sánh độ ưu tiên giữa hai yêu cầu            |
| `SiftUp()`         | Điều chỉnh Heap theo hướng lên                 |
| `SiftDown()`       | Điều chỉnh Heap theo hướng xuống               |
| `SwapAndSync()`    | Đổi vị trí hai phần tử và cập nhật bảng ánh xạ |
| `RemoveAtIndex()`  | Xóa phần tử tại một index và khôi phục Heap    |
| `InsertRequest()`  | Thêm yêu cầu mới, từ chối `bookingId` trùng    |
| `ExtractMax()`     | Lấy và xóa yêu cầu có ưu tiên cao nhất         |
| `RemoveById()`     | Xóa yêu cầu theo mã booking                    |
| `Top()`            | Xem yêu cầu ưu tiên cao nhất mà không xóa      |
| `Empty()`          | Kiểm tra Heap có rỗng không                    |
| `Size()`           | Trả về số lượng yêu cầu hiện có                |
| `BuildHeap()`      | Khởi tạo Heap từ một danh sách có sẵn          |

### 2.5. Kết hợp Max Heap với Hash Table

Heap lưu các yêu cầu theo thứ tự ưu tiên. Bảng ánh xạ `indexMap` lưu quan hệ:

```text
bookingId → index trong Heap
```

Bảng này hỗ trợ tìm vị trí của một booking để xóa bằng `RemoveById()`. Mỗi khi các phần tử đổi chỗ, `SwapAndSync()` cần cập nhật vị trí tương ứng trong bảng ánh xạ.

Đã kiểm tra file Hash Table tự cài đặt của nhóm. File sử dụng class:

```cpp
MyHashTable<V>
```

và cung cấp các hàm:

```cpp
insert()
search()
contains()
remove()
clear()
```

Do đó, nếu tích hợp Hash Table tự cài đặt vào `MyMaxHeap`, khai báo có thể chuyển từ:

```cpp
unordered_map<string, int> indexMap;
```

sang:

```cpp
MyHashTable<int> indexMap;
```

Các thao tác cần thay đổi tương ứng:

| `unordered_map`    | `MyHashTable`             |
| ------------------ | ------------------------- |
| `find()`           | `search()` / `contains()` |
| `map[key] = value` | `insert(key, value)`      |
| `erase()`          | `remove()`                |
| `clear()`          | `clear()`                 |

Ví dụ trong `RemoveById()`:

```cpp
bool RemoveById(const string& bookingId) {
    int index;

    if (!indexMap.search(bookingId, index)) {
        return false;
    }

    RemoveAtIndex(index);
    return true;
}
```

Việc sử dụng Hash Table tự cài đặt giúp `MyMaxHeap` phù hợp hơn với yêu cầu của đồ án về cấu trúc dữ liệu tự cài đặt.

### 2.6. Sử dụng `Empty()` và `Size()`

```cpp
bool Empty() const {
    return heap.empty();
}

int Size() const {
    return static_cast<int>(heap.size());
}
```

* `Empty()` trả về `true` khi Heap không có phần tử.
* `Size()` trả về số lượng request đang có.
* `const` cho biết các hàm này không thay đổi trạng thái của đối tượng.

### 2.7. Xử lý `BookingID` trùng

`InsertRequest()` được thiết kế để không cho phép hai request có cùng `bookingId`.

Với `unordered_map`:

```cpp
if (indexMap.find(req.bookingId) != indexMap.end()) {
    return false;
}
```

Nếu chuyển sang `MyHashTable` thì dùng:

```cpp
if (indexMap.contains(req.bookingId)) {
    return false;
}
```

Nếu dữ liệu đầu vào đã đảm bảo `bookingId` duy nhất thì trường hợp trùng thường không xảy ra. Tuy nhiên, việc kiểm tra vẫn giúp cấu trúc dữ liệu an toàn hơn.

### 2.8. Kiểm tra `BuildHeap()`

`BuildHeap()` được sử dụng để tạo Heap từ một danh sách request có sẵn.

Phiên bản hiện tại:

```cpp
bool BuildHeap(const vector<RentalRequest>& initial) {
    heap.clear();
    indexMap.clear();

    for (const RentalRequest& req : initial) {
        if (indexMap.find(req.bookingId) != indexMap.end()) {
            return false;
        }

        heap.push_back(req);

        int index = static_cast<int>(heap.size()) - 1;
        indexMap[req.bookingId] = index;
    }

    for (int i = static_cast<int>(heap.size()) / 2 - 1; i >= 0; i--) {
        SiftDown(i);
    }

    return true;
}
```

Đã thảo luận về trường hợp dữ liệu đầu vào có `bookingId` trùng. Nếu phát hiện trùng trong quá trình thêm trực tiếp vào Heap rồi mới `return false`, Heap có thể đã bị thay đổi một phần.

Tuy nhiên, dữ liệu CSV của hệ thống được giả định có `bookingId` hợp lệ và duy nhất. Vì vậy, không cần làm `BuildHeap()` phức tạp hơn nếu nhóm đã đảm bảo tính hợp lệ của dữ liệu đầu vào.

### 2.9. Kiểm tra `SiftUp()`

Phát hiện điều kiện vòng lặp ban đầu:

```cpp
while (i >= 0)
```

không phù hợp.

Khi `i = 0`, node đang xét là root và không có parent. Vì vậy, điều kiện được sửa thành:

```cpp
while (i > 0) {
    int parent = (i - 1) / 2;

    if (!HigherPriority(heap[i], heap[parent])) {
        break;
    }

    SwapAndSync(i, parent);
    i = parent;
}
```

### 2.10. Kiểm tra `SiftDown()`

`SiftDown()` được dùng để tìm node có độ ưu tiên cao nhất giữa:

* Node hiện tại.
* Node con trái.
* Node con phải.

Cách triển khai:

```cpp
void SiftDown(int i) {
    int n = static_cast<int>(heap.size());

    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int best = i;

        if (left < n && HigherPriority(heap[left], heap[best])) {
            best = left;
        }

        if (right < n && HigherPriority(heap[right], heap[best])) {
            best = right;
        }

        if (best == i) {
            break;
        }

        SwapAndSync(i, best);
        i = best;
    }
}
```

Sau khi kiểm tra, hàm này không cần thay đổi.

### 2.11. Kiểm tra `RemoveAtIndex()`

`RemoveAtIndex()` thực hiện xóa phần tử tại một index bất kỳ.

Quy trình:

1. Xóa mapping của phần tử cần xóa.
2. Đưa phần tử cuối Heap vào vị trí đó.
3. Cập nhật `indexMap`.
4. Xóa phần tử cuối.
5. Gọi `SiftUp()` và `SiftDown()` để khôi phục tính chất Heap.

Code sử dụng:

```cpp
void RemoveAtIndex(int idx) {
    int lastIdx = static_cast<int>(heap.size()) - 1;

    indexMap.erase(heap[idx].bookingId);

    if (idx != lastIdx) {
        heap[idx] = heap.back();
        indexMap[heap[idx].bookingId] = idx;
    }

    heap.pop_back();

    if (idx < static_cast<int>(heap.size())) {
        SiftUp(idx);
        SiftDown(idx);
    }
}
```

Nếu sử dụng `MyHashTable`, các thao tác tương ứng là:

```cpp
indexMap.remove(heap[idx].bookingId);
```

và:

```cpp
indexMap.insert(heap[idx].bookingId, idx);
```

### 2.12. Kiểm tra `RemoveById()`

`RemoveById()` không trực tiếp xóa phần tử trong vector. Hàm trước tiên tìm index thông qua `indexMap`, sau đó gọi `RemoveAtIndex()`.

Với `unordered_map`:

```cpp
auto it = indexMap.find(bookingId);

if (it == indexMap.end()) {
    return false;
}

RemoveAtIndex(it->second);
return true;
```

Với `MyHashTable`:

```cpp
int index;

if (!indexMap.search(bookingId, index)) {
    return false;
}

RemoveAtIndex(index);
return true;
```

Cách này giúp việc tìm request theo `bookingId` có độ phức tạp trung bình `O(1)` khi dùng Hash Table phù hợp, sau đó việc điều chỉnh Heap có độ phức tạp `O(log n)`.

---

## 3. Chương trình `main.cpp` kiểm thử

Đã xây dựng một chương trình `main.cpp` riêng để kiểm thử các chức năng của `MyMaxHeap`.

### 3.1. Kiểm tra Insert

Các request mẫu:

```cpp
RentalRequest a = {
    "B001", "C001", "CAR01", 1, 100
};

RentalRequest b = {
    "B002", "C002", "CAR01", 3, 200
};

RentalRequest c = {
    "B003", "C003", "CAR01", 3, 100
};
```

Theo quy tắc ưu tiên:

1. `B003` → Tier 3, Timestamp 100.
2. `B002` → Tier 3, Timestamp 200.
3. `B001` → Tier 1, Timestamp 100.

Do đó:

```cpp
heap.Top().bookingId
```

phải trả về:

```text
B003
```

### 3.2. Kiểm tra `ExtractMax()`

Sau khi gọi:

```cpp
RentalRequest x = heap.ExtractMax();
```

request có độ ưu tiên cao nhất được lấy ra khỏi Heap.

Sau đó, kiểm tra `Top()` để xác nhận phần tử kế tiếp đã trở thành root.

### 3.3. Kiểm tra `RemoveById()`

Kiểm tra:

```cpp
heap.RemoveById("B001");
```

và:

```cpp
heap.RemoveById("B999");
```

Trong đó, `B001` tồn tại còn `B999` không tồn tại.

Kết quả mong đợi:

```text
Remove B001: Thanh cong
Remove B999: Khong tim thay
```

### 3.4. Kiểm tra Heap rỗng

Tạo một Heap mới:

```cpp
MyMaxHeap emptyHeap;
```

Sau đó kiểm tra:

```cpp
emptyHeap.Empty();
emptyHeap.Size();
emptyHeap.Top();
emptyHeap.ExtractMax();
```

`Top()` và `ExtractMax()` được thiết kế để ném `runtime_error` nếu Heap rỗng.

### 3.5. Kiểm tra cùng tier và cùng timestamp

Tạo hai request:

```cpp
RentalRequest d = {
    "B004", "C004", "CAR01", 3, 300
};

RentalRequest e = {
    "B002", "C005", "CAR01", 3, 300
};
```

Hai request có cùng:

```text
membershipTier = 3
bookingTimestamp = 300
```

nên `bookingId` được dùng làm tiêu chí phụ.

Theo điều kiện:

```cpp
return a.bookingId < b.bookingId;
```

`B002` sẽ có độ ưu tiên cao hơn `B004`.

---

## 4. Xử lý lỗi và kiểm tra code

### 4.1. Lỗi `SiftUp()`

Đã xác định lỗi ở điều kiện:

```cpp
while (i >= 0)
```

và sửa thành:

```cpp
while (i > 0)
```

Đây là điều kiện phù hợp vì node tại index `0` là root và không có node cha.

### 4.2. Kiểm tra lỗi `RemoveById()`

Đã kiểm tra điều kiện tìm kiếm:

```cpp
if (it == indexMap.end())
```

Nếu không tìm thấy booking thì phải trả về `false`.

Nếu tìm thấy thì sử dụng index tương ứng để gọi:

```cpp
RemoveAtIndex(it->second);
```

### 4.3. Kiểm tra dữ liệu trùng

Đã bổ sung kiểm tra `bookingId` trùng trong `InsertRequest()`.

Nếu request có `bookingId` đã tồn tại:

```cpp
return false;
```

Request sẽ không được thêm vào Heap.

### 4.4. Xem xét lỗi trạng thái của `BuildHeap()`

Đã thảo luận trường hợp `BuildHeap()` gặp dữ liệu trùng sau khi đã thêm một số phần tử vào Heap.

Nếu muốn đảm bảo trạng thái cũ không bị thay đổi khi dữ liệu đầu vào không hợp lệ, có thể kiểm tra toàn bộ dữ liệu trước rồi mới gán vào Heap.

Tuy nhiên, do dữ liệu CSV của hệ thống được thiết kế với `bookingId` duy nhất, phiên bản đơn giản hiện tại có thể được sử dụng nếu nhóm thống nhất rằng dữ liệu đầu vào đã hợp lệ.

---

## 5. Kết hợp với `MyHashTable` tự cài đặt

File Hash Table của nhóm sử dụng:

```cpp
template <typename V>
class MyHashTable
```

với kỹ thuật **Separate Chaining** và hàm băm polynomial rolling hash tự cài đặt.

Hash Table có các thao tác:

```cpp
insert()
search()
contains()
remove()
clear()
```

Do đó, khi tích hợp vào `MyMaxHeap`, có thể sử dụng:

```cpp
#include "MyHashTable.h"

MyHashTable<int> indexMap;
```

Thay cho:

```cpp
#include <unordered_map>

unordered_map<string, int> indexMap;
```

### Các thay đổi chính

**`SwapAndSync()`**

```cpp
indexMap.insert(heap[i].bookingId, i);
indexMap.insert(heap[j].bookingId, j);
```

**`RemoveAtIndex()`**

```cpp
indexMap.remove(heap[idx].bookingId);
```

và:

```cpp
indexMap.insert(heap[idx].bookingId, idx);
```

**`InsertRequest()`**

```cpp
if (indexMap.contains(req.bookingId)) {
    return false;
}
```

**`RemoveById()`**

```cpp
int index;

if (!indexMap.search(bookingId, index)) {
    return false;
}

RemoveAtIndex(index);
return true;
```

**`BuildHeap()`**

```cpp
if (indexMap.contains(req.bookingId)) {
    return false;
}
```

và:

```cpp
indexMap.insert(req.bookingId, index);
```

Logic của Max Heap không thay đổi; chỉ thay đổi cách quản lý bảng ánh xạ.

Lưu ý: Cần kiểm tra chữ ký hàm và cách xử lý khóa đã tồn tại của `MyHashTable` thực tế trước khi tích hợp. Các ví dụ trên giả định API hỗ trợ đúng cách gọi được mô tả.

---

## 6. Đánh giá phiên bản hiện tại

Sau khi kiểm tra các hàm chính, cấu trúc `MyMaxHeap` hiện tại gồm:

```text
HigherPriority()
        ↓
SiftUp() / SiftDown()
        ↓
SwapAndSync()
        ↓
RemoveAtIndex()
        ↓
InsertRequest()
ExtractMax()
RemoveById()
Top()
Empty()
Size()
BuildHeap()
```

Các hàm chính đã được thiết kế cho RF2.

`indexMap` đóng vai trò hỗ trợ cho thao tác `RemoveById()`, còn `heap` chịu trách nhiệm chính trong việc duy trì thứ tự ưu tiên.

---

## 7. Kiểm thử cần thực hiện

Các trường hợp cần kiểm thử:

* [ ] Thêm request bình thường.
* [ ] Thêm request có tier cao hơn.
* [ ] Hai request cùng tier nhưng khác timestamp.
* [ ] Hai request cùng tier và cùng timestamp.
* [ ] Thêm `bookingId` bị trùng.
* [ ] `ExtractMax()` trên Heap có dữ liệu.
* [ ] `RemoveById()` với ID tồn tại.
* [ ] `RemoveById()` với ID không tồn tại.
* [ ] `Top()` trên Heap có dữ liệu.
* [ ] `Top()` khi Heap rỗng.
* [ ] `ExtractMax()` khi Heap rỗng.
* [ ] `BuildHeap()` với danh sách request.
* [ ] Kiểm tra mapping `bookingId → index` sau khi Heap swap.
* [ ] Kiểm tra dữ liệu CSV thực tế.
* [ ] Kiểm tra lại sau khi chuyển sang `MyHashTable`.

---

## 8. Cập nhật GitHub

Trước khi commit, cần kiểm tra:

```bash
git status
git branch
```

Đảm bảo đang ở branch:

```text
feature/m3-maxheap
```

Sau khi hoàn thiện:

```bash
git add .
git commit -m "Update RF2 Max Heap"
git push
```

Không push trực tiếp lên branch chính nếu chưa được nhóm thống nhất.

---

## 9. Kết luận

Quá trình trao đổi tập trung vào việc hoàn thiện module `MyMaxHeap` cho RF2, bao gồm thiết kế cấu trúc `RentalRequest`, xây dựng quy tắc ưu tiên, triển khai `SiftUp()` và `SiftDown()`, hỗ trợ xóa request theo `bookingId`, xử lý dữ liệu trùng và kiểm thử các trường hợp Heap rỗng.

Trong quá trình kiểm tra, đã phát hiện và sửa điều kiện của `SiftUp()` từ `i >= 0` thành `i > 0`.

Ngoài ra, đã kiểm tra `MyHashTable` tự cài đặt của nhóm và xác định có thể tích hợp nó làm `indexMap` thay cho `std::unordered_map`. Khi tích hợp, cần thay đổi các thao tác `find`, `operator[]` và `erase` sang `search`, `contains`, `insert` và `remove` theo API thực tế của `MyHashTable`.

**Bước tiếp theo:** Hoàn thiện phiên bản tích hợp, biên dịch `main.cpp`, chạy các test case, kiểm tra với dữ liệu CSV và sau đó commit lên branch cá nhân trước khi tích hợp vào hệ thống chung.
## AI-AUDIT – PHÂN TÍCH XUNG ĐỘT GIỮA MC2 VÀ RF2

### 1. Thông tin chung

* **Dự án:** Hệ thống quản lý cho thuê xe tự lái và xử lý tranh chấp đặt xe
* **Yêu cầu liên quan:** MC2 và RF2
* **Thành viên thực hiện:** M3
* **Mục đích AI-Audit:** Kiểm tra lại lập luận về xung đột giữa hai yêu cầu và đánh giá tính phù hợp của giải pháp sử dụng cấu trúc dữ liệu.

---

### 2. Nội dung AI được sử dụng

AI được sử dụng để hỗ trợ:

* Phân tích mối quan hệ giữa MC2 và RF2.
* Xác định các điểm có khả năng xảy ra xung đột khi hai yêu cầu cùng thao tác trên dữ liệu đặt xe.
* Phân tích sự khác nhau giữa việc sắp xếp dữ liệu thông thường và việc duy trì thứ tự ưu tiên bằng Binary Max-Heap.
* Đề xuất hướng giải quyết để hai yêu cầu có thể cùng tồn tại trong hệ thống.
* Kiểm tra lại cách diễn đạt trong phần báo cáo để tránh kết luận rằng MC2 và RF2 xung đột trực tiếp nếu chưa có cơ sở.

---

### 3. Lập luận ban đầu

AI nhận định rằng MC2 và RF2 có thể phát sinh xung đột khi cùng sử dụng hoặc cập nhật dữ liệu liên quan đến các yêu cầu đặt xe.

RF2 sử dụng **Binary Max-Heap** để duy trì thứ tự ưu tiên của các yêu cầu tranh chấp cùng một chiếc xe. Khi có nhiều khách hàng cùng yêu cầu một xe, hệ thống cần nhanh chóng xác định yêu cầu có mức ưu tiên cao nhất.

Nếu MC2 thực hiện các thao tác cập nhật, sắp xếp hoặc thay đổi dữ liệu đặt xe theo một tiêu chí khác, các thay đổi đó có thể ảnh hưởng đến dữ liệu mà RF2 đang sử dụng.

Tuy nhiên, AI xác định rằng đây không nhất thiết là **xung đột trực tiếp về chức năng**. Điểm xung đột chủ yếu nằm ở việc hai yêu cầu có thể cùng tác động đến dữ liệu và có tiêu chí xử lý khác nhau.

---

### 4. Kiểm tra và phản biện kết quả AI

Sau khi kiểm tra, cần tránh khẳng định rằng MC2 và RF2 chắc chắn xung đột chỉ vì cả hai đều sử dụng dữ liệu đặt xe.

Có thể phân biệt hai trường hợp:

#### Trường hợp 1 – Không có xung đột trực tiếp

Nếu MC2 chỉ thực hiện chức năng riêng và không thay đổi các thuộc tính dùng để xác định độ ưu tiên của RF2 thì hai yêu cầu có thể hoạt động độc lập.

Trong trường hợp này, MC2 và RF2 chỉ **chia sẻ nguồn dữ liệu**, chứ không thực sự xung đột về thuật toán.

#### Trường hợp 2 – Có khả năng xung đột

Nếu MC2 thay đổi thông tin của một yêu cầu đặt xe có ảnh hưởng đến thứ tự ưu tiên, RF2 phải cập nhật lại vị trí của phần tử trong Max-Heap.

Ví dụ, nếu mức độ ưu tiên của một khách hàng thay đổi, vị trí của yêu cầu tương ứng trong heap có thể không còn chính xác. Nếu không cập nhật heap, RF2 có thể xử lý sai yêu cầu được ưu tiên.

Do đó, xung đột cần được mô tả chính xác là **xung đột về dữ liệu và duy trì trạng thái của cấu trúc dữ liệu**, thay vì nói hai yêu cầu hoàn toàn đối nghịch nhau.

---

### 5. Đánh giá giải pháp sử dụng Max-Heap

AI đánh giá việc sử dụng Binary Max-Heap là phù hợp với RF2 vì RF2 cần thường xuyên xác định yêu cầu có độ ưu tiên cao nhất.

Heap cho phép hệ thống duy trì một cấu trúc trong đó phần tử có độ ưu tiên cao nhất được đặt ở vị trí đầu.

Các thao tác chính của RF2 gồm:

* **Thêm yêu cầu:** đưa yêu cầu mới vào heap và điều chỉnh vị trí bằng `SiftUp`.
* **Lấy yêu cầu ưu tiên cao nhất:** lấy phần tử ở đầu heap.
* **Xóa yêu cầu:** loại bỏ yêu cầu và khôi phục tính chất heap bằng thao tác điều chỉnh phù hợp.
* **Cập nhật ưu tiên:** khi dữ liệu ảnh hưởng đến độ ưu tiên thay đổi, cần điều chỉnh lại vị trí của phần tử.

Ngoài ra, hệ thống có thể sử dụng `unordered_map` để ánh xạ `bookingId` với vị trí của yêu cầu trong heap. Điều này giúp tìm nhanh vị trí của một booking khi cần cập nhật hoặc hủy yêu cầu.

---

### 6. Hướng giải quyết xung đột

Để hạn chế xung đột giữa MC2 và RF2, hệ thống nên tách rõ trách nhiệm:

**MC2:** thực hiện chức năng nghiệp vụ thuộc phạm vi của MC2.

**RF2:** chịu trách nhiệm duy trì và xử lý thứ tự ưu tiên của các yêu cầu tranh chấp bằng Max-Heap.

Khi MC2 thay đổi dữ liệu có liên quan đến tiêu chí ưu tiên của RF2, hệ thống phải thông báo hoặc thực hiện thao tác cập nhật tương ứng trên heap.

Nếu cần sắp xếp toàn bộ danh sách để hiển thị hoặc tạo báo cáo, có thể tạo bản sao dữ liệu rồi sử dụng thuật toán sắp xếp trên bản sao đó. Không nên tùy tiện thay đổi trực tiếp thứ tự nội bộ của Max-Heap vì điều này có thể phá vỡ tính chất của heap.

---

### 7. Kết quả sau AI-Audit

Sau khi kiểm tra, kết luận được điều chỉnh như sau:

> MC2 và RF2 không nhất thiết xung đột trực tiếp về chức năng. Xung đột có thể phát sinh khi MC2 thay đổi dữ liệu mà RF2 sử dụng để xác định mức độ ưu tiên. Khi đó, RF2 phải cập nhật lại cấu trúc Binary Max-Heap để bảo đảm thứ tự ưu tiên luôn chính xác.

Việc sử dụng Max-Heap trong RF2 vẫn phù hợp vì yêu cầu chính của RF2 là nhanh chóng xác định và xử lý yêu cầu có mức ưu tiên cao nhất trong trường hợp nhiều khách hàng cùng tranh chấp một chiếc xe.

---

### 8. Vai trò của AI và người thực hiện

AI chỉ đóng vai trò **hỗ trợ phân tích và phản biện**.

AI được sử dụng để:

* Gợi ý các khả năng xung đột.
* Giải thích ưu và nhược điểm của các cách tổ chức dữ liệu.
* Đề xuất cách diễn đạt và hướng giải quyết.
* Hỗ trợ kiểm tra tính logic của lập luận.

Người thực hiện chịu trách nhiệm:

* Kiểm tra yêu cầu thực tế của MC2 và RF2.
* Kiểm tra code và cấu trúc dữ liệu được cài đặt.
* Xác định lập luận nào phù hợp với hệ thống.
* Quyết định giải pháp cuối cùng.
* Chịu trách nhiệm về kết quả được đưa vào đồ án.

#### Kết luận AI-Audit

AI hỗ trợ quá trình phân tích nhưng không được xem là nguồn quyết định cuối cùng. Kết quả cuối cùng phải được đối chiếu với yêu cầu của đồ án và mã nguồn thực tế trước khi sử dụng.
