# D5 — BỘ TEST TỰ ĐỘNG + BẰNG CHỨNG HIỆU NĂNG

## 1. Automated Test

Phần RF3 được kiểm thử tự động thông qua hàm `runAutomatedTests()`.

Các trường hợp kiểm thử gồm:

| Test | Nội dung kiểm tra | Kết quả |
|---|---|---|
| Test 1 | Kiểm tra Stack hoạt động theo LIFO | PASS |
| Test 2 | Kiểm tra Pop khi Stack rỗng | PASS |
| Test 3 | Kiểm tra Undo thao tác ADD | PASS |
| Test 4 | Kiểm tra Undo thao tác UPDATE | PASS |
| Test 5 | Kiểm tra Undo thao tác DELETE | PASS |
| Test 6 | Kiểm tra Undo nhiều thao tác theo LIFO | PASS |
| Test 7 | Kiểm tra ngày hợp lệ/không hợp lệ | PASS |

Hàm `runAutomatedTests()` tự động chạy các test và tổng hợp kết quả.

Ví dụ kết quả:

```text
====================================
          AUTOMATED TEST
====================================
Test Stack LIFO     : PASS
Test Pop Stack rong : PASS
Test Undo ADD       : PASS
Test Undo UPDATE    : PASS
Test Undo DELETE    : PASS
Test Undo LIFO      : PASS
Test Valid Date     : PASS
------------------------------------
Ket qua: 7/7 test PASS
====================================

```
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
