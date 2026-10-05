# D5. Bộ test tự động

## 1. Mục tiêu

Bộ test tự động được xây dựng để kiểm tra tính đúng đắn của các thành phần chính trong hệ thống, gồm:

- MC1 – `MyHashTable` và Persistence.
- MC2 – Merge Sort, Binary Search và Range Query.
- RF1 – Trie.
- RF2 – `MyMaxHeap`.
- RF3 – `MyStack` và `UndoManager`.

Các test được viết độc lập, sử dụng các hàm kiểm tra đơn giản như `CHECK` hoặc `check`, không phụ thuộc framework kiểm thử bên ngoài.

---

## 1.1. MC1 – MyHashTable + Persistence

### File test

`test_thanhvien1.cpp`

### Nội dung kiểm thử

Bộ test kiểm tra các thao tác cơ bản của `MyHashTable`, các trường hợp biên và khả năng lưu/đọc dữ liệu CSV.

| STT | Test | Nội dung kiểm tra |
|---:|---|---|
| 1 | `test_insert_and_search_basic` | Thêm 2 phần tử vào Hash Table và kiểm tra Search trả về đúng giá trị |
| 2 | `test_search_key_not_found` | Tìm một key không tồn tại |
| 3 | `test_search_on_empty_table` | Tìm kiếm trên Hash Table rỗng và kiểm tra `size() = 0` |
| 4 | `test_update_existing_key` | Insert cùng một key lần thứ hai và kiểm tra giá trị được cập nhật, không tạo phần tử mới |
| 5 | `test_remove_existing_key` | Xóa key tồn tại và kiểm tra key không còn trong bảng |
| 6 | `test_remove_nonexistent_key` | Xóa một key không tồn tại |
| 7 | `test_collision_handling` | Kiểm tra 2 key khác nhau nhưng cùng bucket vẫn được tìm đúng |
| 8 | `test_resize_preserves_data` | Kiểm tra dữ liệu vẫn tồn tại sau khi Hash Table resize |
| 9 | `test_all_items_returns_everything` | Kiểm tra `allItems()` trả về đầy đủ các phần tử |
| 10 | `test_save_then_load_matches` | Ghi dữ liệu ra CSV rồi load lại và kiểm tra dữ liệu khớp |
| 11 | `test_load_from_nonexistent_file_returns_zero` | Load file không tồn tại và kiểm tra số bản ghi trả về bằng 0 |

### Kết quả kiểm thử

Chương trình sử dụng hai biến:

```cpp
int g_passed = 0;
int g_failed = 0;
```
Sau khi chạy toàn bộ test, chương trình in:

X passed, Y failed

và trả về mã:

return g_failed == 0 ? 0 : 1;

Do đó có thể xác định trực tiếp toàn bộ bộ test MC1 có vượt qua hay không.

## 1.2. MC2 – Merge Sort + Binary Search + Range Query
File test
```cpp
test_thanhvien2.cpp
```
Mục tiêu

Bộ test tự kiểm chứng các thuật toán của MC2 bằng cách so sánh kết quả thực tế với kết quả mong đợi hoặc với phương pháp lọc brute-force.

Các nội dung kiểm thử

| STT | Nội dung | Cách kiểm tra |
|---:|---|---|
| 1 | Merge Sort | Kiểm tra mảng sau khi sắp xếp có đúng thứ tự tăng dần |
| 2 | Binary Search – `lowerBound` | Kiểm tra chỉ số biên dưới trả về đúng |
| 3 | Binary Search – `upperBound` | Kiểm tra chỉ số biên trên trả về đúng |
| 4 | Range Query | So sánh kết quả truy vấn khoảng với kết quả lọc brute-force |
| 5 | Top xe hot | Kiểm tra danh sách xe được thuê nhiều được sắp xếp đúng thứ tự |

Test sử dụng:

```cpp
int totalChecks = 0;
int failedChecks = 0;
```
Mỗi phép kiểm tra được gọi thông qua:
```cpp

check(condition, "ten_test");
```

Nếu đúng:
```cpp
[OK] ten_test
```

Nếu sai:
```cpp
[THAT BAI] ten_test
```
Cuối chương trình báo:
```cpp
KET QUA: X/Y test PASS
```

và trả về 0 khi không có test thất bại.

## 1.3. RF1 – Trie
File test
```cpp
TrieTest.cpp
```
Nội dung kiểm thử

Bộ test của RF1 tập trung vào chức năng tìm kiếm theo tiền tố của Trie.

| STT | Nội dung                 |
| --- | ------------------------ |
| 1   | Chèn dữ liệu xe vào Trie |
| 2   | Tìm kiếm theo tiền tố    |
| 3   | Tiền tố có nhiều kết quả |
| 4   | Tiền tố không tồn tại    |

Mục tiêu là kiểm tra Trie trả về đúng các kết quả phù hợp với tiền tố được yêu cầu.
## 5. RF2 – MyMaxHeap
File test
```cpp
test_thanhvien3.cpp
```
### Các trường hợp kiểm thử

| STT | Test                            | Nội dung kiểm tra                                         |
| --- | ------------------------------- | --------------------------------------------------------- |
| 1   | `TEST INSERT`                   | Thêm các `RentalRequest` vào Heap và kiểm tra kích thước  |
| 2   | `TEST TOP`                      | Kiểm tra request có độ ưu tiên cao nhất nằm ở Top         |
| 3   | `TEST EXTRACT MAX`              | Lấy phần tử ưu tiên cao nhất và kiểm tra Top mới          |
| 4   | `TEST REMOVE BY ID`             | Xóa request theo `bookingId`                              |
| 5   | `TEST REMOVE ID KHONG TON TAI`  | Xóa request không tồn tại                                 |
| 6   | `TEST EMPTY`                    | Kiểm tra trạng thái Heap rỗng                             |
| 7   | `TEST HEAP RONG`                | Gọi `Top()` và `ExtractMax()` trên Heap rỗng              |
| 8   | `TEST CUNG TIER CUNG TIMESTAMP` | Kiểm tra thứ tự khi các request có cùng Tier và Timestamp |

Dữ liệu kiểm thử

Các request mẫu được tạo với:
```cpp
B001 – Tier 1 – Timestamp 100
B002 – Tier 3 – Timestamp 200
B003 – Tier 3 – Timestamp 100
```

Sau đó kiểm tra thứ tự ưu tiên và các thao tác trên Heap.

Trường hợp Heap rỗng

Test kiểm tra riêng:
```cpp
emptyHeap.Top();
emptyHeap.ExtractMax();
```

và bắt runtime_error để bảo đảm thao tác trên Heap rỗng được xử lý thay vì làm chương trình lỗi ngoài kiểm soát.

## 6. RF3 – MyStack + UndoManager
File test
```cpp
Tests.cpp
```
Bộ test

RF3 có 7 test tự động:
| STT | Test           | Nội dung kiểm tra                                       |
| --- | -------------- | ------------------------------------------------------- |
| 1   | Stack LIFO     | Kiểm tra phần tử được đưa vào sau được lấy ra trước     |
| 2   | Pop Stack rỗng | Kiểm tra Stack rỗng không gây lỗi                       |
| 3   | Undo ADD       | Sau Undo, bản ghi vừa thêm được xóa khỏi hệ thống       |
| 4   | Undo UPDATE    | Sau Undo, dữ liệu được khôi phục về `oldData`           |
| 5   | Undo DELETE    | Sau Undo, bản ghi bị xóa được khôi phục                 |
| 6   | Undo theo LIFO | Nhiều thao tác được hoàn tác theo thứ tự mới nhất trước |
| 7   | Kiểm tra ngày  | Kiểm tra ngày bắt đầu và ngày kết thúc hợp lệ           |

Luồng kiểm thử Undo
```cpp
ADD / UPDATE / DELETE
        |
        v
     Tạo Action
        |
        v
    push vào Stack
        |
        v
   Người dùng chọn Undo
        |
        v
     pop Action
        |
        v
 Khôi phục dữ liệu
```

Kiểm tra Undo ADD
```cpp
Thêm booking
    ↓
push Action ADD
    ↓
Undo
    ↓
booking được xóa
```
Kết quả mong đợi:
```cpp
Bản ghi vừa ADD không còn trong hệ thống.
Kiểm tra Undo UPDATE
Dữ liệu cũ
    ↓
UPDATE
    ↓
lưu oldData + newData
    ↓
Undo
    ↓
khôi phục oldData
```

Kết quả mong đợi:
```cpp
Dữ liệu quay lại đúng trạng thái trước khi UPDATE.
```

Kiểm tra Undo DELETE
```cpp
Dữ liệu ban đầu
    ↓
DELETE
    ↓
lưu oldData + position
    ↓
Undo
    ↓
khôi phục bản ghi
```

Kết quả mong đợi:
```cpp
Booking bị xóa được khôi phục đúng dữ liệu và vị trí.
```

Kiểm tra Undo nhiều bước
Ví dụ:
```cpp
ADD
 ↓
UPDATE
 ↓
DELETE
```

Khi thực hiện Undo:
```cpp
Undo 1 → DELETE
Undo 2 → UPDATE
Undo 3 → ADD
```

Điều này kiểm tra trực tiếp nguyên tắc:
```cpp
LIFO
Last In – First Out
```

Kiểm tra Stack rỗng

Khi không còn thao tác để Undo:
```cpp
Stack = empty
      ↓
    Undo
      ↓
Không crash / không lỗi
```

## 7. Tổng hợp phạm vi test
| Thành phần | Chức năng được kiểm thử                                        | Edge case                                        |
| ---------- | -------------------------------------------------------------- | ------------------------------------------------ |
| **MC1**    | Insert, Search, Update, Remove, Collision, Resize, Persistence | Key không tồn tại, bảng rỗng, file không tồn tại |
| **MC2**    | Merge Sort, Binary Search, Range Query, Top xe hot             | Khoảng truy vấn, kết quả đối chiếu brute-force   |
| **RF1**    | Trie và tìm kiếm tiền tố                                       | Tiền tố không tồn tại, nhiều kết quả             |
| **RF2**    | Insert, Top, ExtractMax, RemoveById                            | Heap rỗng, ID không tồn tại, cùng Tier/Timestamp |
| **RF3**    | Push, Pop, Undo ADD/UPDATE/DELETE, Undo nhiều bước             | Stack rỗng, ngày không hợp lệ                    |
## 8. Kết luận

Bộ test tự động được xây dựng để kiểm tra các chức năng chính và các trường hợp biên của MC1, MC2, RF1, RF2 và RF3.

Các test tập trung vào:

MC1: Hash Table và Persistence.
MC2: Merge Sort, Binary Search và Range Query.
RF1: Tìm kiếm theo tiền tố bằng Trie.
RF2: Các thao tác Insert, Top, ExtractMax và RemoveById.
RF3: Undo ADD, UPDATE, DELETE và nguyên tắc LIFO của Stack.

Các test được sử dụng để phát hiện lỗi và xác nhận tính đúng đắn trước khi thực hiện benchmark và đánh giá hiệu năng.
## 2- Bằng chứng hiệu năng

Nhóm thực hiện benchmark để kiểm chứng hiệu năng của các cấu trúc dữ liệu được lựa chọn cho MC1 và MC2. Thử nghiệm so sánh giải pháp sử dụng cấu trúc dữ liệu với phương pháp Linear Scan, trên các quy mô dữ liệu 1.000, 10.000 và 100.000 bản ghi.

### 1. MC1 – MyHashTable

MC1 sử dụng MyHashTable để tra cứu đơn thuê/xe theo mã định danh. Theo thiết kế, Hash Table có thời gian tra cứu trung bình O(1), trong khi Linear Scan có độ phức tạp O(N).

| Số bản ghi | MyHashTable | Linear Scan | Speedup |
|---:|---:|---:|---:|
| 1.000 | 0.00009 ms | 0.00683 ms | 73.1× |
| 10.000 | 0.00012 ms | 0.06650 ms | 543.3× |
| 100.000 | Chưa có kết quả | Chưa có kết quả | ...× |

Nhận xét: Khi số lượng bản ghi tăng, Linear Scan phải duyệt nhiều phần tử hơn nên thời gian tăng theo N. MyHashTable có thời gian tra cứu ổn định hơn, phù hợp với mục tiêu tra cứu nhanh theo Booking_ID.

### 2. MC2 – Sorted Array + Binary Search

MC2 sử dụng Sorted Array + Binary Search để truy vấn các bản ghi theo khoảng thời gian. Thiết kế sử dụng Binary Search để tìm biên khoảng với độ phức tạp O(log N), sau đó xử lý các phần tử thuộc kết quả.

Kết quả benchmark thực tế:
| Số bản ghi | Binary Search | Linear Scan | Speedup |
|---:|---:|---:|---:|
| 1.000 | 98.40 ns/query | 937.00 ns/query | 9.52× |
| 10.000 | 143.90 ns/query | 9,693.50 ns/query | 67.36× |
| 100.000 | 143.50 ns/query | 86,993.80 ns/query | 606.23× |

Nhận xét: Khi dữ liệu tăng từ 1.000 lên 100.000 bản ghi, thời gian Linear Scan tăng rất mạnh, trong khi Binary Search thay đổi ít. Ở 100.000 bản ghi, Binary Search nhanh hơn Linear Scan khoảng 606 lần. Kết quả thực nghiệm phù hợp với lựa chọn Sorted Array + Binary Search cho MC2.

### 3. Kết luận

Kết quả benchmark cho thấy các lựa chọn thiết kế của nhóm phù hợp với workload:

MC1 – MyHashTable: phù hợp với tra cứu theo mã định danh, hướng tới O(1) trung bình.
MC2 – Sorted Array + Binary Search: phù hợp với truy vấn theo khoảng thời gian, tìm biên trong O(log N).
Khi quy mô dữ liệu tăng, lợi thế của các cấu trúc được lựa chọn thể hiện rõ hơn so với Linear Scan.

Do đó, kết quả thực nghiệm ủng hộ quyết định sử dụng MyHashTable cho MC1 và Sorted Array + Binary Search cho MC2. Đây cũng phù hợp với thiết kế DSA được mô tả trong tài liệu của nhóm.
