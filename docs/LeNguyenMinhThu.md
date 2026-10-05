# ĐỒ ÁN DSA — PHẦN ĐÓNG GÓP CÁ NHÂN

## Thông tin thành viên

| Thông tin | Nội dung |
|---|---|
| Họ tên | Lê Nguyễn Minh Thư |
| MSSV | 25110356 |
| Lĩnh vực | Hệ thống quản lý cho thuê xe tự lái & xử lý tranh chấp đặt xe |
| Requirement phụ trách | RF3 |
| Component | MyStack + UndoManager |
| Công việc bổ sung | Automated Test + Benchmark  |
| Branch | `feature/rf3 - undo - stack` |

-- -

# 1. D1 — BÀI ĐỌC - HIỂU ĐỀ BÀI

## 1.1.Bài toán trong một câu

Hệ thống quản lý cho thuê xe cần lưu các đơn thuê xe và cho phép người dùng thêm, sửa, xóa dữ liệu; khi có thao tác sai, hệ thống phải cho phép hoàn tác thao tác gần nhất.

## 1.2.Input / Output / Constraints

### Input

* Booking_ID
* Biển số / Car_ID
* Customer_ID
* Tên khách hàng
* Ngày bắt đầu thuê
* Ngày kết thúc thuê
* Trạng thái đơn thuê
* Các thao tác thêm đơn
* Các thao tác sửa đơn
* Các thao tác xóa đơn
* Yêu cầu Undo

### Output

* Danh sách các đơn thuê hiện tại
* Thông tin chi tiết của một đơn thuê
* Trạng thái dữ liệu sau khi thêm / sửa / xóa
* Trạng thái dữ liệu trước thao tác gần nhất sau khi Undo
* Số lượng thao tác có thể Undo
* Thao tác gần nhất đang nằm trên đỉnh Stack

### Ràng buộc

* `Booking_ID` là duy nhất.
* Dữ liệu được xử lý chủ yếu trong bộ nhớ.
* Ngày bắt đầu và ngày kết thúc phải là ngày hợp lệ trong lịch thực tế.
* Ngày kết thúc phải lớn hơn hoặc bằng ngày bắt đầu.
* Undo phải hoàn tác thao tác gần nhất trước tiên.
* Khi Stack Undo rỗng, hệ thống không được xảy ra lỗi.
* Mỗi thao tác ADD / UPDATE / DELETE phải lưu đủ thông tin để có thể khôi phục trạng thái cũ.

-- -

# 2. Phân tích các yêu cầu

| Requirement | Nội dung | Access pattern chính |
| --- | --- | --- |
| MC1 | Lấy một bản ghi từ mã | Tra cứu theo ID |
| MC2 | Truy vấn theo thời gian / thống kê | Range / Statistics |
| RF1 | Gợi ý hãng xe theo prefix | Prefix search |
| RF2 | Xử lý tranh chấp đặt xe | Priority |
| RF3 | Hoàn tác thao tác gần nhất | LIFO |

-- -

# 3. RF3 — HOÀN TÁC THAO TÁC GẦN NHẤT

## 3.1.Mô tả yêu cầu

RF3 cho phép hệ thống hoàn tác thao tác quản lý đơn thuê gần nhất.

Các thao tác có thể được hoàn tác gồm :

```text
ADD
UPDATE
DELETE
```

Khi người dùng thực hiện nhiều thao tác liên tiếp, thao tác được thực hiện sau cùng phải được hoàn tác trước.

Ví dụ :

```text
ADD B001
↓
UPDATE B001
↓
DELETE B001
```

Khi thực hiện Undo :

```text
Undo
↓
khôi phục UPDATE gần nhất trước
↓
Undo
↓
khôi phục ADD
```

Do đó RF3 sử dụng nguyên tắc :

```text
LIFO
Last In - First Out
```

-- -

## 3.2.Cấu trúc dữ liệu sử dụng

Component cá nhân :

```text
MyStack
↓
UndoManager
↓
renTalSystem
```

Trong đó :

*`MyStack` là Stack tự cài đặt bằng Linked List.
* `UndoManager` quản lý lịch sử các thao tác có thể Undo.
* `renTalSystem` thực hiện thêm, sửa, xóa và gọi UndoManager khi cần.

### Action

Mỗi thao tác Undo được lưu dưới dạng :

```text
Action
├── type
├── oldData
├── newData
└── position
```

Ý nghĩa :

*`type`: xác định ADD / UPDATE / DELETE.
* `oldData`: dữ liệu trước khi thay đổi.
* `newData`: dữ liệu sau khi thay đổi.
* `position`: vị trí cũ của phần tử trong `vector`, dùng khi khôi phục DELETE.

-- -

## 3.3.Quy tắc Undo

### Trường hợp ADD

Nếu thao tác gần nhất là :

```text
ADD B001
```

Khi Undo :

```text
Xóa B001 khỏi danh sách
```

Có thể biểu diễn :

```text
ADD
↓
Undo
↓
Remove B001
```

-- -

### Trường hợp UPDATE

Nếu dữ liệu ban đầu :

```text
B001
Khach A
Xe 01
01 / 10 / 2026
05 / 10 / 2026
```

Sau UPDATE :

```text
B001
Khach B
Xe 02
02 / 10 / 2026
06 / 10 / 2026
```

Khi Undo :

```text
newData
↓
loại bỏ thay đổi
↓
restore oldData
```

Do đó UPDATE cần lưu cả :

```text
oldData
newData
```

-- -

### Trường hợp DELETE

Ví dụ :

```text
B001
```

bị xóa khỏi danh sách.

Khi Undo :

```text
DELETE
↓
lấy oldData
↓
chèn lại vào danh sách
```

Ngoài `oldData`, hệ thống lưu thêm :

```text
position
```

để có thể khôi phục vị trí cũ của đơn thuê.

-- -

## 3.4.Edge Cases

* Stack Undo rỗng.
* Chưa có thao tác nào nhưng người dùng chọn Undo.
* Chỉ có một thao tác.
* Có nhiều thao tác liên tiếp.
* Undo nhiều lần liên tiếp.
* ADD sau đó Undo.
* UPDATE sau đó Undo.
* DELETE sau đó Undo.
* UPDATE một Booking_ID không tồn tại.
* DELETE một Booking_ID không tồn tại.
* Thêm Booking_ID bị trùng.
* Ngày bắt đầu không hợp lệ.
* Ngày kết thúc không hợp lệ.
* Ngày kết thúc nhỏ hơn ngày bắt đầu.
* Người dùng Undo sau khi vừa thực hiện thao tác mới.

-- -

## 3.5.Giả định và vấn đề cần làm rõ với nhóm

### Giả định

* `Booking_ID` là duy nhất.
* Undo chỉ hoàn tác các thao tác đã được lưu vào Undo Stack.
* Thao tác Undo không được tự lưu lại vào Undo Stack.
* UPDATE phải lưu cả trạng thái trước và sau khi thay đổi.
* DELETE phải lưu dữ liệu cũ và vị trí cũ.
* Các thao tác Undo được xử lý theo LIFO.

### Các vấn đề cần thống nhất với nhóm

* Phạm vi Undo là một bước hay nhiều bước.
* Khi hệ thống tải lại dữ liệu từ file thì lịch sử Undo có được khôi phục hay không.
* Sau khi Undo rồi thực hiện thao tác mới thì lịch sử cũ được xử lý như thế nào.
* Có cần Redo hay chỉ yêu cầu Undo.

-- -

# D3 — PHẦN BIỆN MINH THIẾT KẾ CÁ NHÂN

## 1. Requirement phụ trách

### RF3 — Hoàn tác thao tác gần nhất

RF3 yêu cầu hệ thống cho phép hoàn tác thao tác gần nhất của người dùng, bao gồm :

```text
ADD
UPDATE
DELETE
```

Do thao tác mới nhất phải được hoàn tác trước, access pattern của RF3 là :

```text
Last In
↓
First Out
↓
LIFO
```

Vì vậy, cấu trúc dữ liệu phù hợp là :

```text
MyStack
↓
UndoManager
```

`MyStack` được tự cài đặt bằng Linked List để lưu lịch sử các thao tác Undo.

-- -

# 2. Q1 — Requirement cần thao tác gì ?

RF3 cần các thao tác chính :

| Thao tác | Mục đích                   |
| -------------- | -------------------------- |
| `push()` | Thêm Action mới vào Stack  |
| `pop()` | Lấy Action gần nhất        |
| `top()` | Xem Action gần nhất        |
| `empty()` | Kiểm tra Stack rỗng        |
| `getSize()` | Lấy số lượng Action        |
| `clear()` | Xóa lịch sử Undo           |
| `undoRental()` | Hoàn tác thao tác gần nhất |

Mỗi thao tác được lưu dưới dạng `Action`.

```text
Action
├── type
├── oldData
├── newData
└── position
```

Trong đó :

*`type`: xác định ADD / UPDATE / DELETE.
* `oldData`: dữ liệu trước khi thay đổi.
* `newData`: dữ liệu sau khi thay đổi.
* `position`: vị trí cũ, dùng để khôi phục DELETE.

-- -

# 3. Q2 — Key và Workload

## Key

RF3 không sử dụng key để xác định độ ưu tiên như RF2.

Thông tin quan trọng của Action là :

```text
ActionType
oldData
newData
position
```

`ActionType` quyết định cách thực hiện Undo.

## Workload

RF3 có workload :

*Thêm Action vào lịch sử.
* Lấy Action gần nhất.
* Xem Action gần nhất.
* Undo nhiều thao tác liên tiếp.
* Kiểm tra Stack có rỗng hay không.

Đặc điểm chính :

```text
Action mới nhất
↓
Undo trước
↓
LIFO
```

Do đó Stack phù hợp với workload của RF3.

-- -

# 4. Q3 — Phân tích Worst - case

`MyStack` được cài đặt bằng Linked List và thao tác tại đầu danh sách.

Complexity:

| Operation | Complexity |
| --- | --- |
| `push()` | O(1) |
| `pop()` | O(1) |
| `top()` | O(1) |
| `empty()` | O(1) |
| `getSize()` | O(1) |
| `clear()` | O(n) |

Tuy nhiên, cần phân biệt complexity của Stack với complexity của toàn bộ `undoRental()`.

Ví dụ :

```text
undoRental()
↓
pop()              O(1)
↓
tìm Booking_ID     O(n)
↓
khôi phục vector   O(n)
```

Vì vậy :

```text
MyStack.pop() → O(1)
```

nhưng:

```text
undoRental() → có thể O(n)
```

Đây là lý do không kết luận toàn bộ RF3 có Undo O(1).

-- -

# 5. Q4 — Các yếu tố phi tiệm cận

## Bộ nhớ

Mỗi Action được lưu thêm thông tin để có thể khôi phục trạng thái trước đó.

Đặc biệt :

```text
UPDATE → oldData + newData
DELETE → oldData + position
```

Điều này làm tăng bộ nhớ nhưng giúp Undo được đầy đủ và chính xác.

## Quản lý Linked List

Mỗi node của Stack chứa :

```cpp
Action action;
StackNode* next;
```

Node đầu tiên được sử dụng làm `TOP`.

```text
TOP
↓
Node
↓
Node
↓
Node
```

Do thao tác ở đầu danh sách nên không cần duyệt toàn bộ Stack khi `push()` hoặc `pop()`.

## Đồng bộ lịch sử

Khi thực hiện Undo, thao tác khôi phục không được lưu lại thành Action mới.

Vì vậy hệ thống sử dụng :

```text
saveHistory = true / false
```

* Thao tác của người dùng → `true`
* Thao tác do Undo thực hiện → `false`

Cách này tránh việc Undo tự tạo thêm lịch sử Undo.

-- -

# 6. Trade - off của lựa chọn MyStack

### Ưu điểm

* Phù hợp trực tiếp với access pattern LIFO.
* `push()` và `pop()` O(1).
* Tự cài đặt bằng Linked List.
* Có thể Undo nhiều thao tác liên tiếp.
* Hỗ trợ ADD, UPDATE và DELETE.

### Nhược điểm

* Tốn thêm bộ nhớ để lưu lịch sử.
* UPDATE phải lưu cả dữ liệu cũ và mới.
* DELETE phải lưu thêm vị trí cũ.
* `undoRental()` có thể O(n) do phụ thuộc vào việc tìm kiếm và thao tác trên `vector`.
* Không phù hợp với yêu cầu Undo một thao tác bất kỳ ở giữa lịch sử.

-- -

# 7. Biện minh lựa chọn

So với việc sử dụng một danh sách thông thường, Stack phù hợp hơn vì RF3 luôn yêu cầu thao tác mới nhất được hoàn tác trước.

```text
ADD B001
↓
UPDATE B001
↓
DELETE B001
↓
TOP = DELETE B001
```

Khi Undo :

```text
DELETE B001
↓
POP
↓
UPDATE B001
```

Do đó, lựa chọn `MyStack` xuất phát trực tiếp từ** access pattern của RF3**, thay vì chọn cấu trúc dữ liệu chỉ dựa trên thói quen hoặc tính phổ biến.

Phần Automated Test và Benchmark được thực hiện để kiểm chứng tính đúng đắn và hiệu năng của component, nhưng được trình bày riêng trong phần công việc kiểm thử và đánh giá hiệu năng.
# D4 - IMPLEMENTATION
## 1. Lịch sử commit
Link commit:
1. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/c084f17c942221e8d74d1974ff3b1ff589750b01
2. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/7ee191a21fa38963199648006c360243622afece
3. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/b68c452ccac1da44e176fc48b7204276f7f554e8
4. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/043499513f30eb0aa237a8ae2c7c068b084d0c1e
5. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/0396428abec8bb530f85b5f76db99cea62fb4bd5
6. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/0396428abec8bb530f85b5f76db99cea62fb4bd5
7. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/33d8f224847c8dd42c96fee5f3d6b3b3fb9d31af
8. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/6d181ad251fb6e3c2bc4f47cc5c45614d4fd4d2d
9. https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/9a547bf5231fc33c27102569199b23d160a831bf
# D6 — ĐÁNH GIÁ KỸ THUẬT THÀNH VIÊN TRONG NHÓM

## 1. Thành phần được đánh giá

- **Cấu trúc dữ liệu:** Hash Table
- **Thành phần:** `MyHashTable`
- **File cài đặt:** `MyHashTable.h`
- **Mục đích:** Tra cứu đơn thuê/xe theo mã định danh cho yêu cầu MC1.

---

## 2. Nội dung đánh giá

Tiến hành xem xét phần cài đặt `MyHashTable` của thành viên 1, tập trung vào:

- Cách xây dựng Hash Table.
- Hàm băm và xử lý xung đột.
- Các thao tác `insert()`, `search()`, `remove()`.
- Cơ chế `resize()`.
- Kiểm thử và benchmark hiệu năng.

---

## 3. Đánh giá phần cài đặt

### 3.1. Cấu trúc dữ liệu

`MyHashTable` sử dụng **Separate Chaining**, mỗi bucket chứa một danh sách liên kết các `Node`.

| Thành phần | Vai trò |
|---|---|
| `key` | Khóa dùng để tra cứu |
| `value` | Dữ liệu tương ứng |
| `next` | Liên kết node tiếp theo |
| `buckets_` | Danh sách các bucket |

Thiết kế này phù hợp với MC1 vì yêu cầu tra cứu chính xác theo mã định danh.

### 3.2. Hàm băm và xử lý xung đột

Hàm `hashKey()` được tự cài đặt bằng **polynomial rolling hash**, không sử dụng `std::hash`.

Khi nhiều khóa có cùng vị trí, chương trình sử dụng Separate Chaining:

```text
Bucket
  ↓
[Node] → [Node] → [Node]
```
### 3.3. Các thao tác chính
insert(): thêm dữ liệu hoặc cập nhật nếu key đã tồn tại.

search(): tìm kiếm theo key.

remove(): xóa node khỏi bucket.

resize(): mở rộng bảng khi load factor > 0.75 và thực hiện rehash.

Độ phức tạp:

insert/search/remove: O(1) trung bình.

Trường hợp xấu nhất: O(N).
## 4. Đánh giá phần kiểm thử

Các trường hợp chính được kiểm tra:

| Test | Nội dung | Kết quả |
|---|---|---|
| Test 1 | Thêm phần tử | PASS |
| Test 2 | Tìm key tồn tại | PASS |
| Test 3 | Tìm key không tồn tại | PASS |
| Test 4 | Cập nhật key | PASS |
| Test 5 | Xóa phần tử | PASS |
| Test 6 | Kiểm tra xử lý xung đột và resize | PASS |

Ngoài ra, benchmark được sử dụng để so sánh thời gian tìm kiếm của MyHashTable với tìm kiếm tuyến tính khi số lượng dữ liệu tăng.

## 5. Những điểm làm tốt
| STT	| Nội dung |	Đánh giá |
|---|---| --- |
| 1 |	Tự cài đặt Hash Table |	Đáp ứng yêu cầu cấu trúc dữ liệu
| 2 |	Separate Chaining |	Có xử lý xung đột
| 3 |	Tự cài đặt hàm băm |	Không phụ thuộc std::hash
| 4 |	Có resize()	| Phù hợp khi dữ liệu tăng
| 5 |	Có kiểm thử và benchmark |	Có cơ sở đánh giá tính đúng đắn và hiệu năng
## 6. Vấn đề phát hiện được

Qua kiểm tra phần MyHashTable, chưa phát hiện lỗi nghiêm trọng ảnh hưởng đến chức năng chính của MC1.

Thiết kế Separate Chaining phù hợp với việc xử lý xung đột. Các thao tác insert(), search() và remove() đều được cài đặt trực tiếp trên cấu trúc dữ liệu tự xây dựng.

Cơ chế resize() khi load factor vượt quá 0.75 giúp hạn chế việc các phần tử tập trung quá nhiều vào một bucket khi dữ liệu tăng.

Một điểm cần lưu ý là search() có độ phức tạp O(1) trung bình nhưng O(N) trong trường hợp xấu nhất, do vẫn phải duyệt các node trong cùng một chain nếu xảy ra nhiều va chạm.

Nhìn chung, MyHashTable phù hợp với yêu cầu MC1 – tra cứu chính xác theo mã định danh và có cơ sở để đánh giá hiệu năng thông qua benchmark với tìm kiếm tuyến tính.
# D7 — NHẬT KÝ SỬ DỤNG AI& BÀI PHẢN TƯ CÁ NHÂN
## 1. Nhật ký sử dụng AI

| STT | Công cụ | Mục đích sử dụng | Phần công việc bị ảnh hưởng | Cách kiểm chứng |
|---:|---|---|---|---|
| 1 | AI Assistant | Giải thích yêu cầu RF3 và cách áp dụng Stack cho chức năng Undo | D1, D3 – Phân tích RF3 | Tự phân tích lại access pattern của RF3 và kiểm tra nguyên tắc LIFO |
| 2 | AI Assistant | Hỗ trợ thiết kế Action, StackNode, MyStack và UndoManager | D4 – Cài đặt RF3 | Tự đọc, trace và kiểm tra từng hàm `push()`, `pop()`, `top()`, `empty()`, `clear()` |
| 3 | AI Assistant | Hỗ trợ hoàn thiện chức năng Undo cho ADD / UPDATE / DELETE | D4 – RF3 | Tự kiểm tra trạng thái dữ liệu trước và sau thao tác; xây dựng test riêng cho từng trường hợp |
| 4 | AI Assistant | Hỗ trợ phát hiện và sửa lỗi trong code như tên biến, hàm trùng, logic Undo | D4 – Debug | Compile lại chương trình, chạy thử và kiểm tra kết quả thực tế |
| 5 | AI Assistant | Hỗ trợ xây dựng kiểm tra ngày bắt đầu và ngày kết thúc | D4 – Validation dữ liệu | Kiểm tra ngày không tồn tại, năm nhuận và trường hợp ngày kết thúc trước ngày bắt đầu |
| 6 | AI Assistant | Hỗ trợ xây dựng Automated Test cho Stack và Undo | D5 – Automated Test | Chạy các test LIFO, Stack rỗng, Undo ADD, UPDATE, DELETE và Undo nhiều bước |
| 7 | AI Assistant | Hỗ trợ xây dựng Benchmark và cách đo thời gian | D5 – Benchmark | Chạy benchmark trên dữ liệu được tạo trong chương trình và sử dụng kết quả chạy thực tế |
| 8 | AI Assistant | Hỗ trợ chuẩn bị câu hỏi và cách giải thích khi bảo vệ | D8 – Defense | Tự trace code và luyện trả lời mà không nhìn tài liệu |
## 1.1.AI - audit

Trong quá trình sử dụng AI, một số đoạn code và cách tổ chức ban đầu được đề xuất để hỗ trợ hoàn thiện RF3.Em không sử dụng kết quả một cách nguyên trạng mà kiểm tra lại bằng cách compile, chạy thử, trace từng hàm và xây dựng Automated Test.

Một số điểm em phát hiện và điều chỉnh :

Bổ sung oldData cho UPDATE để có thể khôi phục dữ liệu cũ.
Bổ sung position cho DELETE để có thể khôi phục bản ghi về vị trí cũ.
Sửa lỗi khai báo trùng hàm clear().
Sửa tên biến sai như odlData.
Tách chức năng nhập dữ liệu, xử lý Rental, Undo, Test và Benchmark thành các hàm / class riêng.
Bổ sung kiểm tra ngày bắt đầu và ngày kết thúc.
Kiểm tra lại testUndoUpdate() để test trực tiếp trạng thái dữ liệu sau Undo thay vì chỉ kiểm tra biến tạm.

Qua quá trình kiểm chứng, em hiểu rõ hơn rằng AI chỉ là công cụ hỗ trợ; code thuộc tầng DSA Core cần được tự kiểm tra, hiểu và có khả năng giải thích khi bảo vệ.

# 2. Bài phản tư cá nhân(Reflection)

Trong quá trình thực hiện đồ án, phần em phụ trách là RF3 – hoàn tác thao tác tạo, sửa và xóa đơn thuê bằng Stack.Khó khăn lớn nhất ban đầu của em là hiểu cách chuyển yêu cầu Undo trong bài toán thực tế thành một mô hình dữ liệu có thể cài đặt được.

Sau khi phân tích access pattern, em nhận ra RF3 yêu cầu thao tác gần nhất phải được hoàn tác trước nên phù hợp với nguyên tắc LIFO.Từ đó em xây dựng Action, StackNode, MyStack và UndoManager.Em cũng phải hiểu rõ rằng chỉ sử dụng Stack là chưa đủ; hệ thống còn phải lưu thông tin cần thiết để khôi phục dữ liệu.Vì vậy UPDATE cần oldData và newData, còn DELETE cần oldData và position.

Một khó khăn khác là kiểm tra tính đúng đắn của Undo.Ban đầu em tập trung vào việc làm cho chương trình chạy, nhưng sau khi kiểm thử em nhận ra cần kiểm tra trạng thái thực tế của dữ liệu sau mỗi lần Undo.Vì vậy em xây dựng các test riêng cho ADD, UPDATE, DELETE và kiểm tra thứ tự LIFO.

Phần ngày bắt đầu và ngày kết thúc cũng giúp em nhận ra rằng validation dữ liệu là một phần quan trọng của tính đúng đắn.Em bổ sung kiểm tra định dạng DD / MM / YYYY, ngày tồn tại thực tế, năm nhuận và điều kiện ngày kết thúc không được trước ngày bắt đầu.

Trong quá trình sử dụng AI, em nhận được hỗ trợ về cách tổ chức code, giải thích thuật toán và xây dựng test.Tuy nhiên em phải tự compile, trace, kiểm thử và sửa lại các phần chưa phù hợp với chương trình của nhóm.Điều này giúp em hiểu rằng việc có code chạy được không đồng nghĩa với việc em đã thực sự hiểu code.

Qua phần công việc này, em hiểu rõ hơn về Stack, linked list, LIFO, độ phức tạp của từng thao tác và cách kết nối cấu trúc dữ liệu với yêu cầu thực tế.Em cũng nhận ra rằng quyết định thiết kế cần xuất phát từ access pattern và workload thay vì chọn cấu trúc dữ liệu trước rồi mới tìm lý do.
/*
# D8 – DEMO NHÓM + BẢO VỆ CÁ NHÂN
# 1. Phần công việc phụ trách
RF3 – Hoàn tác thao tác tạo/sửa/xóa đơn thuê.
Component: MyStack + UndoManager.
Công việc bổ sung: Automated Test + Benchmark.
Branch: feature/m5-rf3.
# 2. Nội dung Demo
Luồng Demo chính
```text
ADD
↓
UPDATE
↓
DELETE
↓
UNDO
↓
```
Khôi phục trạng thái dữ liệu

Các trường hợp cần minh họa
Undo ADD:
- Sau khi Undo, bản ghi vừa thêm được xóa khỏi hệ thống.
* Undo UPDATE:
- Sau khi Undo, dữ liệu được khôi phục về trạng thái oldData trước khi cập nhật.
* Undo DELETE:
- Sau khi Undo, bản ghi được khôi phục từ oldData và position ban đầu.
Undo nhiều bước:
- Các thao tác được hoàn tác theo thứ tự LIFO – thao tác mới nhất được hoàn tác trước.
* Stack rỗng:
- Khi chọn Undo nhưng không còn thao tác nào để hoàn tác, hệ thống không bị lỗi.
# 3. Nội dung bảo vệ cá nhân
## 3.1. Giải thích và Trace Code
Luồng xử lý
Người dùng ADD / UPDATE / DELETE
```text
              |
              v
          Tạo Action
              |
              v
        Push vào MyStack
              |
              v
       Người dùng chọn Undo
              |
              v
       Pop Action gần nhất
              |
              v
      Khôi phục dữ liệu từ Action
```
Các thành phần chính
- Action: Lưu thông tin của một thao tác có thể Undo.
- StackNode: Node dùng để xây dựng Stack bằng danh sách liên kết.
- MyStack: Cấu trúc Stack tự cài đặt, quản lý các Action theo nguyên tắc LIFO.
- UndoManager: Quản lý lịch sử các thao tác cần hoàn tác.
- renTalSystem::undoRental(): Thực hiện Undo thao tác gần nhất và khôi phục dữ liệu.
## 3.2. Biện minh quyết định thiết kế

RF3 yêu cầu thao tác gần nhất phải được hoàn tác trước.

Thao tác mới nhất
       |
       v
    Undo trước
       |
       v
      LIFO
       |
       v
    MyStack

Do đó, lựa chọn MyStack xuất phát trực tiếp từ access pattern của RF3, thay vì lựa chọn cấu trúc dữ liệu chỉ dựa trên thói quen hoặc tính phổ biến.

Các thao tác chính của Stack như:

push()
pop()
top()

có độ phức tạp O(1).

Phần Automated Test và Benchmark được thực hiện để kiểm chứng tính đúng đắn và hiệu năng của component, nhưng được trình bày riêng trong phần kiểm thử và đánh giá hiệu năng.

## 3.3. Nếu Requirement thay đổi

Nếu requirement thay đổi thành:

“Cho phép Undo một thao tác bất kỳ trong lịch sử.”

thì Stack không còn phù hợp trực tiếp với access pattern mới, vì Stack được thiết kế để xử lý theo nguyên tắc LIFO.

Khi đó, cần xem xét lại cấu trúc dữ liệu hoặc cách tổ chức lịch sử thao tác để có thể truy cập và hoàn tác một thao tác bất kỳ.
