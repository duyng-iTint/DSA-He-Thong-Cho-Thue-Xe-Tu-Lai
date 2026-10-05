# ĐỒ ÁN DSA — PHẦN ĐÓNG GÓP CÁ NHÂN

## Thông tin thành viên

| Thông tin | Nội dung |
|---|---|
| Họ tên | Nguyễn Minh Duy |
| MSSV | 25110167 |
| Lĩnh vực | Hệ thống quản lý cho thuê xe tự lái & xử lý tranh chấp đặt xe |
| Requirement phụ trách | MC2 |
| Component | Sorted Array + Merge Sort + Binary Search |
| Branch | `feature/m2-mc2` |

---

# 1. D1 — BÀI ĐỌC-HIỂU ĐỀ BÀI

## 1.1. Bài toán trong một câu

Hệ thống cho thuê xe tự lái phải quản lý lượng lớn các đơn thuê xe tích lũy theo thời gian và hỗ trợ nhân viên nhanh chóng truy vấn các lượt thuê trong một khoảng thời gian cũng như xác định các xe được thuê nhiều nhất.

Theo yêu cầu chính thức của đồ án, MC2 là yêu cầu truy xuất theo thứ tự / khoảng / mức ưu tiên. Với hệ thống cho thuê xe, access pattern phù hợp là truy vấn các đơn thuê theo khoảng ngày và thống kê Top-K xe được thuê nhiều nhất.

## 1.2. Input / Output / Constraints

### Input

- `Booking_ID`
- Biển số xe / `Car_ID`
- Hãng xe
- Dòng xe
- Ngày thuê `rentDate`
- Ngày trả `returnDate`
- Thông tin khách hàng
- Giá thuê
- Các bản ghi đơn thuê được nạp từ file dữ liệu
- Khoảng ngày cần truy vấn `[fromDate, toDate]`
- Giá trị `topK` khi cần lấy Top-K xe được thuê nhiều nhất

### Output

- Danh sách các đơn thuê có `rentDate` nằm trong khoảng `[fromDate, toDate]`
- Vị trí bắt đầu và kết thúc của khoảng truy vấn
- Danh sách Top-K xe được thuê nhiều nhất
- Số lượt thuê của từng xe
- Kết quả benchmark giữa Binary Search và Linear Scan

### Ràng buộc

- Dữ liệu có thể đạt quy mô lớn, tối thiểu khoảng 10.000 bản ghi theo yêu cầu đồ án.
- Dữ liệu cần được sắp xếp trước khi thực hiện Range Query bằng Binary Search.
- Không sử dụng `std::sort`, `std::lower_bound` hoặc `std::upper_bound` cho phần xử lý DSA chính.
- Merge Sort và Binary Search được tự cài đặt.
- Tầng Persistence chỉ có nhiệm vụ nạp dữ liệu; truy vấn MC2 được thực hiện trên dữ liệu trong bộ nhớ.
- Range Query cần trả về đúng toàn bộ các bản ghi thuộc khoảng ngày được yêu cầu.
- Top-K phải được sắp xếp giảm dần theo số lượt thuê.

---

# 2. PHÂN TÍCH CÁC YÊU CẦU

| Requirement | Nội dung | Access pattern chính |
|---|---|---|
| MC1 | Tra cứu một bản ghi theo mã định danh | Find-by-key |
| MC2 | Lọc đơn thuê theo khoảng thời gian / Top xe được thuê nhiều nhất | Range query / Ordered data / Extremes |
| RF1 | Gợi ý hãng / dòng xe theo tiền tố | Prefix search |
| RF2 | Xử lý tranh chấp đặt xe | Priority |
| RF3 | Hoàn tác thao tác gần nhất | LIFO |

Trong phần cá nhân, em tập trung vào MC2.

## 2.1. Hai thao tác chính của MC2

### MC2(a) — Lọc đơn thuê theo khoảng thời gian

Nhân viên nhập hai mốc ngày:

```text
fromDate <= rentDate <= toDate
```

Hệ thống phải trả về toàn bộ đơn thuê thuộc khoảng này.

Nếu dùng Linear Scan, hệ thống phải duyệt lần lượt toàn bộ `N` bản ghi để tìm các phần tử phù hợp, có chi phí `O(N)`.

Giải pháp của em là:

```text
Dữ liệu RentalRecord
        ↓
Merge Sort theo rentDate
        ↓
Mảng đã sắp xếp
        ↓
Binary Search
   ↓           ↓
lowerBound   upperBound
        ↓
Các phần tử trong [fromDate, toDate]
```

Sau khi tìm được hai biên, chỉ cần duyệt qua `k` phần tử thực sự thuộc khoảng.

Độ phức tạp:

```text
Tìm biên: O(log N)
Lấy kết quả: O(k)

Tổng: O(log N + k)
```

### MC2(b) — Top xe được thuê nhiều nhất

Hệ thống cần thống kê số lượt thuê của từng xe, sau đó sắp xếp giảm dần theo `rentCount` để lấy Top-K.

Quy trình:

```text
Danh sách RentalRecord
        ↓
Merge Sort theo carPlate
        ↓
Gom nhóm các record cùng carPlate
        ↓
Tạo CarStat
        ↓
Merge Sort giảm dần theo rentCount
        ↓
Lấy Top-K
```

Với `N` bản ghi và `M` xe khác nhau:

```text
Sort theo carPlate: O(N log N)
Gom nhóm: O(N)
Sort CarStat: O(M log M)
```

---

# 3. D3 — PHẦN BIỆN MINH THIẾT KẾ CÁ NHÂN

## 3.1. Requirement phụ trách

### MC2 — Truy vấn xe theo khoảng thời gian thuê / Top xe được thuê nhiều nhất

MC2 có hai workload chính:

1. Truy vấn toàn bộ lượt thuê trong một khoảng ngày.
2. Thống kê và xếp hạng các xe được thuê nhiều nhất.

Hai workload đều cần dữ liệu có thứ tự nên không phù hợp nếu chỉ sử dụng Hash Table.

Component cá nhân:

```text
Sorted Array
    +
Merge Sort
    +
Binary Search
```

---

## 3.2. Q1 — Requirement có cần thứ tự không?

MC2 rõ ràng cần thứ tự.

Đối với Range Query, dữ liệu phải được sắp xếp theo `rentDate` để Binary Search có thể xác định nhanh hai biên của khoảng.

Đối với Top xe hot, dữ liệu cần được tổ chức theo thứ tự:

```text
rentCount giảm dần
```

sau khi đã thống kê số lượt thuê của từng xe.

Vì vậy, câu trả lời Q1 là:

```text
Có, MC2 cần thứ tự.
```

---

## 3.3. Q2 — Loại key và workload

### Range Query

Key chính:

```text
rentDate
```

Workload:

- Bulk load dữ liệu.
- Sắp xếp theo ngày thuê.
- Tìm vị trí đầu tiên có `rentDate >= fromDate`.
- Tìm vị trí sau phần tử cuối có `rentDate <= toDate`.
- Duyệt các phần tử trong khoảng.

Hai hàm tự cài đặt:

```cpp
lowerBoundByDate()
upperBoundByDate()
```

### Top-K xe hot

Key thống kê:

```text
carPlate
```

Sau khi gom nhóm, key xếp hạng:

```text
rentCount
```

Workload:

- Gom các bản ghi cùng xe.
- Đếm số lượt thuê.
- Sắp xếp `CarStat` giảm dần theo `rentCount`.
- Lấy `topK` phần tử đầu tiên.

---

## 3.4. Q3 — Phân tích Worst-case

### Nếu sử dụng Linear Scan cho Range Query

Với `N` bản ghi, mỗi truy vấn phải kiểm tra từng record:

```text
O(N)
```

Khi dữ liệu tăng lên hàng chục nghìn hoặc hàng trăm nghìn bản ghi, chi phí này tăng tuyến tính.

### Với Binary Search

Dữ liệu đã được Merge Sort theo `rentDate`.

`lowerBoundByDate()` tìm phần tử đầu tiên thỏa:

```text
rentDate >= fromDate
```

`upperBoundByDate()` tìm vị trí ngay sau phần tử cuối cùng thỏa:

```text
rentDate <= toDate
```

Mỗi lần Binary Search giảm khoảng tìm kiếm xuống một nửa:

```text
N
↓
N/2
↓
N/4
↓
...
↓
1
```

Do đó:

```text
lowerBoundByDate() = O(log N)
upperBoundByDate() = O(log N)
```

Sau đó hệ thống chỉ duyệt `k` kết quả:

```text
Range Query = O(log N + k)
```

### Độ phức tạp các thao tác chính

| Operation | Complexity |
|---|---|
| Merge Sort | O(N log N) |
| `lowerBoundByDate()` | O(log N) |
| `upperBoundByDate()` | O(log N) |
| Range Query | O(log N + k) |
| Gom nhóm theo `carPlate` | O(N) sau khi sort |
| Top-K sau khi sort | O(M log M) |
| Lấy Top-K | O(K) |

---

## 3.5. Q4 — Các yếu tố phi tiệm cận

### Bộ nhớ

Range Query sử dụng:

```cpp
std::vector<RentalRecord>
```

Dữ liệu được giữ trong bộ nhớ để phục vụ truy vấn.

Khi tạo thống kê xe, chương trình tạo một bản sao:

```cpp
std::vector<RentalRecord> byPlate = records;
```

Mục đích là sắp xếp bản sao theo `carPlate` mà không làm thay đổi thứ tự dữ liệu đã được chuẩn bị cho Range Query.

### Không sử dụng thư viện sắp xếp / tìm kiếm có sẵn

Phần DSA chính không dùng:

```cpp
std::sort
std::lower_bound
std::upper_bound
```

Thay vào đó:

```text
MergeSort.h
BinarySearch.h
```

được tự cài đặt để đáp ứng yêu cầu của đồ án.

### Trade-off

Ưu điểm:

- Range Query nhanh sau khi dữ liệu đã được sắp xếp.
- Binary Search giảm chi phí tìm biên từ `O(N)` xuống `O(log N)`.
- Merge Sort có độ phức tạp `O(N log N)` và phù hợp cho bulk load.
- Không cần dùng Hash Table cho MC2.

Nhược điểm:

- Merge Sort cần chi phí xử lý khi bulk load.
- Chèn hoặc xóa giữa một Sorted Array có thể tốn `O(N)`.
- Top-K hiện tại sắp xếp toàn bộ danh sách `CarStat`, chưa tối ưu riêng cho trường hợp `K` rất nhỏ.
- Tạo bản sao dữ liệu khi xây dựng thống kê theo `carPlate` làm tăng chi phí bộ nhớ.

---

# 4. D4 — IMPLEMENTATION

## 4.1. Cấu trúc dữ liệu RentalRecord

MC2 sử dụng `RentalRecord` làm bản ghi đơn thuê.

Các trường quan trọng:

| Trường | Vai trò |
|---|---|
| `bookingId` | Định danh đơn thuê |
| `carPlate` | Biển số xe |
| `carBrand` | Hãng xe |
| `carModel` | Dòng xe |
| `rentDate` | Ngày thuê, key chính cho Range Query |
| `returnDate` | Ngày trả |
| `price` | Giá thuê |
| `customerName` | Tên khách hàng |
| `memberRank` | Hạng thành viên |

---

## 4.2. Merge Sort

File:

```text
MergeSort.h
```

Merge Sort được sử dụng cho nhiều mục đích trong MC2:

1. Sắp xếp `RentalRecord` theo `rentDate`.
2. Sắp xếp `RentalRecord` theo `carPlate`.
3. Sắp xếp `CarStat` giảm dần theo `rentCount`.

Ví dụ sắp xếp theo ngày thuê:

```cpp
mergeSort(records, [](const RentalRecord& a, const RentalRecord& b) {
    if (a.rentDate != b.rentDate) return a.rentDate < b.rentDate;
    return a.bookingId < b.bookingId;
});
```

`bookingId` được sử dụng để tạo thứ tự ổn định/deterministic khi hai bản ghi có cùng ngày thuê.

Độ phức tạp:

```text
O(N log N)
```

---

## 4.3. `sortByRentDate()`

Hàm:

```cpp
RentalService::sortByRentDate()
```

có nhiệm vụ chuẩn bị dữ liệu cho Range Query.

Quy trình:

```text
RentalRecord chưa sắp xếp
          ↓
Merge Sort
          ↓
rentDate tăng dần
          ↓
Sẵn sàng cho Binary Search
```

Sau bước này, các record có ngày thuê nhỏ hơn nằm trước các record có ngày thuê lớn hơn.

---

## 4.4. `lowerBoundByDate()`

File:

```text
BinarySearch.h
```

Hàm:

```cpp
lowerBoundByDate(
    const std::vector<RentalRecord>& arr,
    const std::string& target
)
```

trả về chỉ số đầu tiên `i` sao cho:

```text
arr[i].rentDate >= target
```

Pseudo-process:

```text
lo = 0
hi = size

while lo < hi:
    mid = lo + (hi - lo) / 2

    nếu arr[mid].rentDate < target:
        lo = mid + 1
    ngược lại:
        hi = mid

return lo
```

Độ phức tạp:

```text
O(log N)
```

---

## 4.5. `upperBoundByDate()`

Hàm:

```cpp
upperBoundByDate()
```

trả về chỉ số ngay sau phần tử cuối cùng có:

```text
rentDate <= target
```

Nhờ đó Range Query có thể sử dụng:

```text
startIdx = lowerBoundByDate(fromDate)
endIdx   = upperBoundByDate(toDate)
```

và lấy:

```text
[startIdx, endIdx)
```

Độ phức tạp:

```text
O(log N)
```

---

## 4.6. `queryByDateRange()`

Hàm:

```cpp
RentalService::queryByDateRange()
```

thực hiện truy vấn chính của MC2(a).

Quy trình:

```text
Sorted Array
     ↓
fromDate ──→ lowerBound
     ↓
startIdx

toDate ────→ upperBound
     ↓
endIdx

[startIdx, endIdx)
     ↓
Danh sách kết quả
```

Code logic chính:

```cpp
int startIdx = lowerBoundByDate(sortedByDate, fromDate);
int endIdx   = upperBoundByDate(sortedByDate, toDate);

result.reserve(endIdx > startIdx ? (endIdx - startIdx) : 0);

for (int i = startIdx; i < endIdx; ++i) {
    result.push_back(sortedByDate[i]);
}
```

Độ phức tạp:

```text
O(log N + k)
```

Trong đó `k` là số bản ghi thực sự nằm trong khoảng.

---

## 4.7. `buildCarStats()`

Hàm:

```cpp
RentalService::buildCarStats()
```

dùng để chuẩn bị dữ liệu cho MC2(b).

Bước 1:

```text
Copy records
     ↓
Merge Sort theo carPlate
```

Bước 2:

```text
P1 P1 P1 P2 P2 P3 P3 P3
↓
Gom nhóm từng carPlate
↓
Tạo CarStat
```

`CarStat` gồm:

```cpp
struct CarStat {
    std::string carPlate;
    std::string carBrand;
    std::string carModel;
    int rentCount;
};
```

Ví dụ:

```text
P1 → 3 lượt thuê
P2 → 2 lượt thuê
P3 → 3 lượt thuê
```

Sau bước này tạo ra:

```text
P1, rentCount = 3
P2, rentCount = 2
P3, rentCount = 3
```

Độ phức tạp:

```text
Merge Sort: O(N log N)
Gom nhóm:   O(N)
Tổng:       O(N log N)
```

---

## 4.8. `topRentedCars()`

Hàm:

```cpp
RentalService::topRentedCars()
```

nhận danh sách `CarStat` và `topK`.

Dữ liệu được Merge Sort giảm dần theo:

```text
rentCount
```

Sau đó giới hạn `topK` để trả về số lượng kết quả cần thiết.

Ví dụ:

```text
Car A → 20 lượt
Car B → 17 lượt
Car C → 15 lượt
Car D → 10 lượt

topK = 3

→ A, B, C
```

Độ phức tạp:

```text
O(M log M)
```

với `M` là số xe khác nhau.

---

## 4.9. Benchmark Binary Search và Linear Scan

Hàm:

```cpp
benchmarkRangeQuery()
```

được xây dựng để kiểm chứng thực nghiệm sự khác biệt giữa:

```text
Binary Search O(log N)
```

và:

```text
Linear Scan O(N)
```

Do thời gian của một truy vấn đơn có thể quá nhỏ để đo ổn định, benchmark lặp lại truy vấn:

```cpp
const int REPEAT = 20000;
```

Sau đó tính thời gian trung bình cho mỗi lần chạy.

Kết quả được in ra console gồm:

- Số lượng bản ghi `N`
- Số lần lặp
- Thời gian trung bình của Binary Search
- Thời gian trung bình của Linear Scan
- Tỷ lệ Binary Search nhanh hơn Linear Scan

Linear Scan chỉ được sử dụng cho benchmark đối chiếu, không phải thuật toán xử lý chính của MC2.

---

## 4.10. Lịch sử commit

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/c0ce337af69e2d6a441937f75e4e5f4b0d6b9fe0

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/6486deed94b7e65cab3893a896ca6b1f7065ecdb

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/49c15cb942905858e850cbe076a0195f694f9ba5

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/e981244545fe9a34cc8721ad9f07c1768c8bd864

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/1786f3c96e3023659b7

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/ca22c7706140ae5b096714aeac90e8e3f2d4d9c7

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/1786f3c96e3023659b71aed5d77b2d1397ca957d

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/fb3700b5976dda5c5d518f85cd9fd645f9fe2fd0

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/7231d473b6ba5573655708e621394788c848725e

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/3a4cabd25aaa9a059ab6c6733262b5bbabf47ee7

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/001f7c1740ec154fe42817190f4319a2acbee5c8

https://github.com/duyng-iTint/DSA-He-Thong-Cho-Thue-Xe-Tu-Lai/commit/82cdf08e4915e84a0e4d8ea00dfa35ba5aaeafee




---

# 5. D5 — TESTING VÀ DEBUGGING

## 5.1. Mục tiêu kiểm thử

Mục tiêu là xác nhận:

- Merge Sort sắp xếp đúng.
- Binary Search tìm đúng hai biên.
- Range Query trả về đúng dữ liệu.
- Range Query khớp với phương pháp brute-force.
- Xử lý đúng khoảng không có dữ liệu.
- Xử lý đúng khoảng bao phủ toàn bộ dữ liệu.
- Top xe hot được sắp xếp giảm dần theo số lượt thuê.

File kiểm thử:

```text
test_correctness.cpp
```

---

## 5.2. Test Merge Sort

Dữ liệu kiểm thử:

```text
5, 3, 8, 1, 9, 2, 7, 4, 6, 0
```

Sau Merge Sort, chương trình kiểm tra toàn bộ mảng có tăng dần hay không.

Kết quả mong đợi:

```text
0, 1, 2, 3, 4, 5, 6, 7, 8, 9
```

---

## 5.3. Test Sort theo ngày thuê

Test tạo nhiều `RentalRecord` có các ngày khác nhau:

```text
2024-03-10
2024-01-05
2024-02-20
2024-03-01
2024-05-15
2024-06-01
2024-06-10
```

Sau:

```cpp
RentalService::sortByRentDate(records);
```

chương trình kiểm tra `rentDate` có tăng dần hay không.

---

## 5.4. Test Range Query bằng brute-force

Khoảng kiểm thử:

```text
from = 2024-02-01
to   = 2024-05-31
```

Chương trình tạo hai kết quả:

```text
1. Range Query bằng Binary Search
2. Kết quả brute-force bằng cách duyệt toàn bộ dữ liệu
```

Sau đó so sánh:

- Số lượng record.
- `bookingId` của từng record.

Điều kiện PASS:

```text
Hai tập kết quả giống nhau 100%.
```

---

## 5.5. Test Edge Case — Khoảng không có dữ liệu

Truy vấn:

```text
2030-01-01 → 2030-12-31
```

Không có record phù hợp.

Kết quả mong đợi:

```text
result.empty() == true
```

---

## 5.6. Test Edge Case — Khoảng bao phủ toàn bộ dữ liệu

Truy vấn:

```text
2000-01-01 → 2099-12-31
```

Khoảng này bao phủ toàn bộ dữ liệu test.

Kết quả mong đợi:

```text
result.size() == records.size()
```

---

## 5.7. Test Top xe hot

Sau khi gọi:

```cpp
auto stats = RentalService::buildCarStats(records);
auto top = RentalService::topRentedCars(stats, 3);
```

chương trình kiểm tra:

1. Danh sách được sắp xếp giảm dần theo `rentCount`.
2. Xe có số lượt thuê cao nhất nằm ở vị trí đầu.
3. `topK = 3` không trả quá 3 kết quả.

---

## 5.8. Tổng hợp test

Các nhóm kiểm thử chính:

| Test | Nội dung | Kết quả mong đợi |
|---|---|---|
| TC01 | Merge Sort số nguyên | PASS |
| TC02 | Sort RentalRecord theo ngày | PASS |
| TC03 | Range Query so với brute-force | PASS |
| TC04 | Range Query không có dữ liệu | PASS |
| TC05 | Range Query bao phủ toàn bộ dữ liệu | PASS |
| TC06 | Top xe hot giảm dần | PASS |
| TC07 | Xe có nhiều lượt thuê nhất đứng đầu | PASS |

---

## 5.9. Nhật ký Debug

| STT | Vấn đề | Nguyên nhân | Cách xử lý | Kết quả |
|---|---|---|---|---|
| 1 | Range Query có thể trả sai biên nếu xác định khoảng không chính xác | Chưa tách rõ phần tử đầu tiên `>= fromDate` và vị trí sau phần tử cuối `<= toDate` | Tự cài đặt riêng `lowerBoundByDate()` và `upperBoundByDate()` | Đã xử lý |
| 2 | Kết quả Range Query cần được kiểm chứng với cách làm đơn giản | Chỉ kiểm tra thứ tự chưa đủ để chứng minh tập kết quả đúng | Thêm brute-force scan để đối chiếu từng `bookingId` | Đã xử lý |
| 3 | Top xe hot cần gom đúng các record cùng biển số | Dữ liệu ban đầu chưa đảm bảo các record cùng `carPlate` nằm cạnh nhau | Copy dữ liệu và Merge Sort theo `carPlate` trước khi gom nhóm | Đã xử lý |
| 4 | Benchmark một truy vấn đơn có thể không ổn định | Thời gian chạy quá nhỏ | Lặp lại 20.000 lần và tính thời gian trung bình | Đã xử lý |

---

# 6. D6 — ĐÁNH GIÁ KỸ THUẬT THÀNH VIÊN TRONG NHÓM

## 6.1. Thành phần được đánh giá

Trong phần D6, em đánh giá phần việc của **thành viên 5 — RF3: Hoàn tác thao tác gần nhất**.

Thành phần được đánh giá:

- Requirement: `RF3`
- Chức năng: Hoàn tác thao tác gần nhất trên đơn thuê
- Cấu trúc dữ liệu: Stack
- Component: `Mystack` + `UnoManager`
- File cài đặt: `UndoStack.h`, `UndoStack.cpp`
- Thành phần tích hợp: `RentalSystem`
- Mục đích: lưu lịch sử các thao tác `ADD`, `UPDATE`, `DELETE_ACTION` để có thể hoàn tác thao tác gần nhất.

---

## 6.2. Phân tích yêu cầu RF3

RF3 có access pattern chính là **LIFO (Last In, First Out)**.

Ví dụ:

```text
Thao tác 1: ADD
        ↓
Thao tác 2: UPDATE
        ↓
Thao tác 3: DELETE

Undo lần 1
    ↓
Hoàn tác DELETE

Undo lần 2
    ↓
Hoàn tác UPDATE

Undo lần 3
    ↓
Hoàn tác ADD
```

Do thao tác mới nhất phải được hoàn tác trước, Stack là cấu trúc dữ liệu phù hợp với yêu cầu.

---

## 6.3. Đánh giá cấu trúc `Mystack`

`Mystack` được cài đặt bằng linked list thông qua `StackNode`.

Cấu trúc:

```cpp
struct StackNode {
    Action action;
    StackNode* next;
};
```

Stack duy trì:

```cpp
StackNode* topNode;
int size;
```

Các thao tác chính:

| Hàm | Chức năng | Complexity |
|---|---|---:|
| `push()` | Thêm thao tác mới lên đỉnh Stack | O(1) |
| `pop()` | Lấy và xóa thao tác mới nhất | O(1) |
| `top()` | Xem thao tác mới nhất | O(1) |
| `empty()` | Kiểm tra Stack rỗng | O(1) |
| `getSize()` | Lấy số thao tác đang lưu | O(1) |
| `clear()` | Xóa toàn bộ lịch sử | O(n) |

Việc thêm và xóa ở đầu linked list giúp RF3 đạt đúng workload LIFO mà không cần dịch chuyển các phần tử như khi dùng mảng.

---

## 6.4. Đánh giá `UnoManager`

`UnoManager` đóng vai trò lớp quản lý lịch sử Undo:

```cpp
class UnoManager {
private:
    Mystack undoStack;
};
```

Các API:

```cpp
saveAction()
undo()
canUndo()
top()
historySize()
clearHistory()
```

Lớp này giúp tách phần quản lý Stack khỏi logic nghiệp vụ của `RentalSystem`.

Luồng xử lý:

```text
RentalSystem
     ↓
Thực hiện ADD / UPDATE / DELETE
     ↓
Tạo Action
     ↓
UnoManager::saveAction()
     ↓
Mystack::push()
```

Khi Undo:

```text
RentalSystem::undoRental()
        ↓
UnoManager::undo()
        ↓
Mystack::pop()
        ↓
Action gần nhất
        ↓
Khôi phục dữ liệu
```

Thiết kế này có tính module hóa tốt vì `RentalSystem` không cần trực tiếp quản lý node của Stack.

---

## 6.5. Đánh giá cách lưu `Action`

RF3 sử dụng:

```cpp
enum class ActionType {
    ADD,
    UPDATE,
    DELETE_ACTION
};
```

Mỗi `Action` lưu thông tin cần thiết để hoàn tác:

```cpp
struct Action {
    ActionType type;
    renTal oldData;
    renTal newData;
    int position = -1;
};
```

Cách lưu này phù hợp với ba thao tác:

### ADD

Khi thêm đơn:

```text
ADD
↓
Lưu newData
↓
Undo
↓
Xóa đơn vừa thêm
```

### UPDATE

Khi cập nhật:

```text
UPDATE
↓
Lưu oldData + newData
↓
Undo
↓
Khôi phục oldData
```

### DELETE

Khi xóa:

```text
DELETE_ACTION
↓
Lưu oldData + position
↓
Undo
↓
Chèn lại oldData vào vị trí cũ
```

Việc lưu `position` đối với DELETE là cần thiết vì sau khi xóa phần tử khỏi `vector`, Undo phải biết vị trí để khôi phục lại cấu trúc dữ liệu gần trạng thái trước thao tác.

---

## 6.6. Đánh giá `RentalSystem::undoRental()`

`undoRental()` lấy Action mới nhất:

```cpp
Action action;

if (!undoManager.undo(action))
    return false;
```

Sau đó xử lý theo loại thao tác.

### Undo ADD

```text
Tìm bookingID của newData
        ↓
Xóa record đó
```

### Undo UPDATE

```text
Tìm bookingID
        ↓
Khôi phục oldData
```

### Undo DELETE

```text
Lấy position
        ↓
Chèn oldData trở lại vị trí
```

Cách xử lý này phù hợp với semantics của Undo.

---

## 6.7. Edge Cases được đánh giá

Các trường hợp cần kiểm thử:

| Test | Trường hợp | Kết quả mong đợi |
|---|---|---|
| RF3-TC01 | Undo khi Stack rỗng | Trả về `false` |
| RF3-TC02 | ADD rồi Undo | Đơn vừa thêm bị xóa |
| RF3-TC03 | UPDATE rồi Undo | Dữ liệu cũ được khôi phục |
| RF3-TC04 | DELETE rồi Undo | Đơn bị xóa được khôi phục |
| RF3-TC05 | Nhiều thao tác liên tiếp | Undo theo đúng thứ tự LIFO |
| RF3-TC06 | Kiểm tra `historySize()` | Số lượng lịch sử chính xác |
| RF3-TC07 | `top()` khi Stack rỗng | Không làm thay đổi dữ liệu |
| RF3-TC08 | `clearHistory()` | Stack trở về trạng thái rỗng |

---

## 6.8. Những điểm làm tốt

| STT | Nội dung | Đánh giá |
|---:|---|---|
| 1 | Chọn Stack cho workload LIFO | Phù hợp |
| 2 | `push/pop/top` đều thao tác tại đầu danh sách | Hiệu quả |
| 3 | Có `ActionType` phân biệt 3 loại thao tác | Rõ ràng |
| 4 | UPDATE lưu `oldData` để phục hồi | Đúng yêu cầu |
| 5 | DELETE lưu `position` | Hỗ trợ khôi phục vị trí |
| 6 | Có lớp `UnoManager` trung gian | Tách trách nhiệm tốt |
| 7 | Có `historySize()` và `canUndo()` | Hỗ trợ giao diện người dùng |
| 8 | Có `clear()` để giải phóng toàn bộ Stack | Hạn chế memory leak |

---

## 6.9. Nhận xét và hướng cải thiện

Phần RF3 lựa chọn cấu trúc dữ liệu phù hợp với yêu cầu. Complexity của thao tác `push`, `pop`, `top` đều là `O(1)`.

Một điểm cần lưu ý là `findRental()` trong `RentalSystem` vẫn tìm kiếm tuyến tính trên `vector`:

```text
O(N)
```

Do đó, mặc dù bản thân Stack có thao tác Undo `O(1)`, toàn bộ `undoRental()` có thể tốn `O(N)` khi cần tìm lại `bookingID`.

Tuy nhiên, điều này thuộc phần quản lý dữ liệu của `RentalSystem`, không phải điểm yếu của cấu trúc Stack.

Ngoài ra, nếu hệ thống trong tương lai yêu cầu **Redo**, có thể bổ sung một Stack thứ hai:

```text
Undo Stack
    ↓
Redo Stack
```

Khi đó thao tác Undo sẽ chuyển Action sang Redo Stack và thao tác Redo sẽ lấy Action từ Redo Stack.

---

## 6.10. Đánh giá tổng thể

RF3 được thiết kế phù hợp với access pattern LIFO:

```text
Requirement RF3
      ↓
LIFO
      ↓
Stack
      ↓
Mystack
      ↓
UnoManager
      ↓
RentalSystem::undoRental()
```

Phần cài đặt có sự tách biệt giữa cấu trúc dữ liệu và logic nghiệp vụ, đồng thời xử lý được ba loại thao tác chính `ADD`, `UPDATE`, `DELETE`.

Đánh giá tổng thể: **Tốt và phù hợp với yêu cầu RF3**.

---

# 6.11. ĐÓNG GÓP BỔ SUNG — WEB TÍCH HỢP HỆ THỐNG

Ngoài Requirement MC2, em có đóng góp thêm một **web interface tích hợp các chức năng của hệ thống cho thuê xe tự lái**.

Phần web nằm trong thư mục:

```text
web/
├── index.html
├── styles.css
├── app.js
├── server.js
├── core_bridge.cpp
├── start.ps1
└── README.md
```

## 6.11.1. Mục tiêu

Mục tiêu của phần web là chuyển các chức năng DSA của hệ thống từ chương trình console thành một giao diện quản lý trực quan, đồng thời kết nối frontend với C++ Core hiện có.

Kiến trúc:

```text
Browser
   ↓
HTML / CSS / JavaScript
   ↓
Node.js HTTP Server
   ↓
REST-like API
   ↓
RentalWebCore.exe
   ↓
C++ Core
   ├── MyHashTable      → MC1
   ├── MergeSort        → MC2
   ├── BinarySearch     → MC2
   ├── Trie             → RF1
   ├── MyMaxHeap        → RF2
   └── UndoStack        → RF3
   ↓
donthue_xe.csv
```

## 6.11.2. Các chức năng web đã tích hợp

### Dashboard

Trang tổng quan hiển thị:

- Tổng số đơn thuê.
- Số đơn đang thuê.
- Số đơn đã hoàn tất.
- Số đơn đã hủy.
- Danh sách các dòng xe phổ biến.

### Quản lý đơn thuê

Web hỗ trợ:

- Tạo đơn thuê mới.
- Cập nhật đơn thuê.
- Xóa đơn thuê.
- Hiển thị danh sách đơn thuê.
- Tìm kiếm theo mã đơn, biển số, tên khách hàng, hãng xe hoặc dòng xe.

### MC1 — Exact Search

Nút **Tra mã / biển số** gọi Core bằng `MyHashTable` để tìm nhanh theo:

```text
Booking_ID
hoặc
Biển số xe
```

### MC2 — Range Query

Giao diện có bộ lọc:

```text
Từ [date]
Đến [date]
```

Server chuyển truy vấn vào C++ Core:

```text
RentalService::sortByRentDate()
        ↓
queryByDateRange()
        ↓
Merge Sort + Binary Search
```

API trả về dữ liệu từ Range Query thay vì chỉ lọc bằng JavaScript.

### MC2 — Top xe được thuê nhiều nhất

Dashboard gọi Core:

```text
buildCarStats()
        ↓
topRentedCars()
```

để lấy danh sách xe có số lượt thuê cao.

### RF1 — Prefix Search

Ô tìm kiếm có gợi ý tên hãng / dòng xe.

Server gọi:

```text
CarTrie::getAllSuggestions()
```

để trả về các gợi ý phù hợp với prefix người dùng nhập.

### RF2 — Priority Queue

Web có khu vực:

```text
Hàng đợi yêu cầu ưu tiên
```

Người dùng có thể thêm mã đơn vào hàng đợi và xử lý request đầu tiên.

Core sử dụng:

```text
MyMaxHeap
```

để xác định request có priority cao nhất.

### RF3 — Undo

Giao diện có nút:

```text
↶ Hoàn tác
```

cho phép khôi phục thao tác gần nhất.

API:

```text
POST /api/undo
```

được tích hợp với cơ chế lịch sử Undo của hệ thống.

### Benchmark MC2

Web có nút:

```text
Chạy benchmark
```

để gọi:

```text
RentalService::benchmarkRangeQuery()
```

và hiển thị kết quả so sánh:

```text
Binary Search
vs
Linear Scan
```

## 6.11.3. Ý nghĩa của đóng góp web

Phần web giúp kết nối các component DSA của cả nhóm thành một hệ thống có giao diện thống nhất thay vì chỉ chạy từng module riêng lẻ.

Đặc biệt, phần `core_bridge.cpp` đóng vai trò cầu nối giữa:

```text
Web API
    ↕
C++ DSA Core
```

Nhờ đó các thuật toán và cấu trúc dữ liệu của đồ án vẫn được sử dụng ở tầng C++, còn web đảm nhiệm việc trình bày và tương tác với người dùng.

Đây là phần đóng góp bổ sung ngoài Requirement MC2 của em.

---

# 7. D7 — BÀI PHẢN TƯ CÁ NHÂN VÀ NHẬT KÍ SỬ DỤNG AI

# Bài phản tư cá nhân

## 7.1. Những gì em đã học được

Qua quá trình thực hiện MC2 và tích hợp thêm giao diện web, em hiểu rõ hơn rằng việc lựa chọn cấu trúc dữ liệu phải xuất phát từ workload của bài toán, đồng thời cần thiết kế API và tầng tích hợp để các component DSA có thể được sử dụng trong một hệ thống hoàn chỉnh.

Ban đầu có thể nghĩ rằng việc lọc các đơn thuê theo ngày chỉ cần duyệt toàn bộ danh sách. Tuy nhiên, khi số lượng bản ghi tăng lên hàng chục nghìn hoặc lớn hơn, Linear Scan sẽ phải kiểm tra rất nhiều phần tử cho mỗi truy vấn.

Sau khi phân tích access pattern, em nhận thấy dữ liệu cần có thứ tự theo `rentDate`, từ đó có thể sử dụng Binary Search để tìm nhanh hai biên của khoảng.

Em cũng hiểu rõ hơn mối quan hệ giữa:

```text
Merge Sort
    ↓
Sorted Array
    ↓
Binary Search
    ↓
Range Query
```

Ngoài Range Query, phần Top xe hot giúp em hiểu cách sử dụng Merge Sort nhiều lần cho các mục đích khác nhau: trước tiên sắp xếp theo `carPlate` để gom nhóm và đếm số lượt thuê, sau đó sắp xếp `CarStat` theo `rentCount`.

## 7.2. Khó khăn gặp phải

Khó khăn đầu tiên là xác định chính xác hai biên của Range Query. Nếu chỉ tìm một vị trí bắt đầu mà không xác định đúng vị trí kết thúc, kết quả có thể thiếu hoặc thừa record.

Khó khăn tiếp theo là xử lý trường hợp nhiều bản ghi có cùng `rentDate`. Khi đó việc xác định `lowerBound` và `upperBound` phải dựa đúng vào điều kiện:

```text
lowerBound: rentDate >= target
upperBound: rentDate > target
```

Một khó khăn khác là xây dựng Top xe hot mà không sử dụng Hash Table, vì Hash Table thuộc phần MC1 của thành viên khác. Em phải sử dụng Merge Sort theo `carPlate` rồi quét tuyến tính để gom nhóm.

Cuối cùng là việc benchmark. Thời gian của một lần Binary Search quá nhỏ nên có thể bị ảnh hưởng bởi sai số đo. Vì vậy cần lặp nhiều lần rồi tính thời gian trung bình.

## 7.3. Cách em giải quyết

Đối với Range Query, em xây dựng hai hàm riêng:

```cpp
lowerBoundByDate()
upperBoundByDate()
```

Sau đó sử dụng:

```cpp
[startIdx, endIdx)
```

để lấy đúng các record trong khoảng.

Để kiểm chứng tính đúng đắn, em so sánh kết quả Binary Search với phương pháp brute-force trên cùng dữ liệu.

Đối với Top xe hot, em tạo bản sao dữ liệu và Merge Sort theo `carPlate`, sau đó gom các record có cùng biển số thành `CarStat`.

Đối với benchmark, em chạy cùng một truy vấn nhiều lần:

```cpp
const int REPEAT = 20000;
```

và so sánh thời gian trung bình của Binary Search với Linear Scan.

## 7.4. Điều em nhận ra sau khi thực hiện

Qua MC2 và phần web tích hợp, em nhận ra rằng một cấu trúc dữ liệu tốt không phải chỉ là cấu trúc có độ phức tạp lý thuyết thấp, mà phải phù hợp với cách hệ thống thực sự sử dụng dữ liệu và phải có cách kết nối rõ ràng với tầng ứng dụng.

Sorted Array phù hợp với MC2 vì dữ liệu lịch sử có thể được bulk load và sắp xếp trước. Khi đó Binary Search có thể tận dụng thứ tự này để tìm nhanh vị trí bắt đầu và kết thúc của khoảng.

Tuy nhiên, em cũng nhận ra trade-off: Sorted Array không phù hợp với workload có quá nhiều thao tác chèn/xóa giữa mảng vì những thao tác đó có thể tốn `O(N)`.

Việc review phần RF2 cũng giúp em hiểu rõ hơn tại sao MC2 và RF2 cần hai cấu trúc khác nhau. MC2 cần truy vấn theo thứ tự và khoảng, trong khi RF2 cần lấy phần tử có priority cao nhất. Vì vậy composition là cách phù hợp hơn thay vì cố sử dụng một cấu trúc duy nhất.

## 7.5. Tự đánh giá và hướng cải thiện

Qua phần MC2, em đã hiểu tốt hơn về Merge Sort, Binary Search, Range Query và cách phân tích complexity theo workload.

Điểm em cần cải thiện là thiết kế test case ngay từ đầu thay vì chỉ bổ sung test sau khi hoàn thành phần code.

Nếu thực hiện lại, em sẽ:

1. Xác định đầy đủ các edge case trước khi code.
2. Thiết kế API của MC2 thống nhất sớm với các thành viên khác.
3. Chuẩn bị benchmark ở nhiều kích thước dữ liệu ngay từ đầu.
4. Bổ sung nhiều trường hợp có ngày thuê trùng nhau.
5. Xem xét phương án tối ưu Top-K nếu hệ thống yêu cầu `K` nhỏ nhưng số lượng xe rất lớn.

---

## Nhật kí sử dụng AI

Trong quá trình thực hiện đồ án, em sử dụng AI như một công cụ hỗ trợ học tập, phân tích thuật toán, kiểm tra thiết kế, debug và hoàn thiện tài liệu. AI được sử dụng để hỗ trợ quá trình suy nghĩ và kiểm chứng; phần code và kết quả cuối cùng được em kiểm tra lại bằng chương trình thực tế.

|STT|	Nội dung sử dụng AI|	Mục đích|	Kết quả áp dụng|
|---:|---|---|---|
|1	|Phân tích Requirement MC2	|Xác định access pattern của truy vấn theo khoảng thời gian và thống kê xe được thuê nhiều nhất	|Xác định MC2 phù hợp với Sorted Array, Merge Sort và Binary Search|
|2	|Tìm hiểu và kiểm tra cách hoạt động của Binary Search	|Hiểu cách xác định hai biên của Range Query	|Áp dụng lowerBoundByDate() và upperBoundByDate()|
|3	|Phân tích độ phức tạp của Range Query	|Kiểm tra và giải thích Big-O của thuật toán	|Xác định Range Query có độ phức tạp O(log N + k)|
|4	|Phân tích Merge Sort	|So sánh việc sắp xếp dữ liệu trước khi truy vấn với việc duyệt tuần tự	|Áp dụng Merge Sort để chuẩn bị dữ liệu cho MC2|
|5	|Kiểm tra các trường hợp biên của Range Query	|Phát hiện các trường hợp có thể làm sai kết quả	|Bổ sung test khoảng không có dữ liệu, khoảng bao phủ toàn bộ dữ liệu và dữ liệu có cùng ngày|
|6	|Kiểm tra kết quả Range Query bằng phương pháp đối chiếu	|Xác minh kết quả Binary Search có chính xác hay không	|So sánh kết quả Range Query với phương pháp brute-force/Linear Scan|
|7	|Phân tích phương pháp thống kê Top xe được thuê nhiều nhất	|Xác định cách gom nhóm và xếp hạng xe theo số lượt thuê	|Áp dụng sắp xếp theo carPlate, tạo CarStat và xếp hạng theo rentCount|
|8	|Hỗ trợ phân tích Benchmark	|Xác định cách so sánh Binary Search với Linear Scan công bằng hơn	|Sử dụng nhiều lần lặp và tính thời gian trung bình|
|9	|Hỗ trợ kiểm tra và debug code MC2	|Tìm nguyên nhân khi kết quả hoặc cách tổ chức thuật toán chưa phù hợp	|Điều chỉnh logic và bổ sung các trường hợp kiểm thử|
|10	|Phân tích kiến trúc tích hợp Web với C++ Core	|Xác định cách kết nối giao diện Web với các component DSA của nhóm	|Xây dựng luồng Web → Node.js → C++ Core → dữ liệu hệ thống|
|11	|Hỗ trợ kiểm tra thiết kế giao diện và chức năng Web	|Đảm bảo các chức năng MC1, MC2, RF1, RF2, RF3 có thể được truy cập từ giao diện	|Hoàn thiện Web tích hợp các chức năng của hệ thống|
|12	|Hỗ trợ hoàn thiện báo cáo cá nhân	|Kiểm tra cách trình bày D1–D7 và diễn giải phần đóng góp cá nhân	|Hoàn thiện báo cáo cá nhân theo cấu trúc yêu cầu|



Vai trò của AI trong quá trình thực hiện

AI chủ yếu đóng vai trò:

```
Phân tích vấn đề
      ↓
Gợi ý hướng tiếp cận
      ↓
Kiểm tra thuật toán / Complexity
      ↓
Hỗ trợ tìm lỗi
      ↓
Đề xuất Test Case
      ↓
Em tự triển khai và kiểm chứng
```

AI không được xem là nguồn thay thế cho quá trình tự cài đặt và kiểm thử. Các kết quả được sử dụng trong báo cáo đều được đối chiếu với source code và kết quả chạy thực tế của chương trình.

### Link trao đổi AI
---



---

# 8. TỔNG KẾT ĐÓNG GÓP CÁ NHÂN

Phần đóng góp chính của em là Requirement **MC2 — Truy vấn xe theo khoảng thời gian thuê / Top xe được thuê nhiều nhất**.

Ngoài các thành phần MC2, em còn thực hiện thêm phần web tích hợp hệ thống.

Các thành phần đã thực hiện:

```text
MC2
│
├── Merge Sort
│   ├── Sort theo rentDate
│   ├── Sort theo carPlate
│   └── Sort CarStat theo rentCount
│
├── Binary Search
│   ├── lowerBoundByDate()
│   └── upperBoundByDate()
│
├── Range Query
│   └── O(log N + k)
│
├── Top Rented Cars
│   ├── buildCarStats()
│   └── topRentedCars()
│
└── Benchmark
    └── Binary Search vs Linear Scan
```

MC2 đáp ứng access pattern cần dữ liệu có thứ tự của đồ án. Sau khi bulk load và sắp xếp dữ liệu, hệ thống có thể sử dụng Binary Search để xác định nhanh phạm vi truy vấn thay vì quét toàn bộ dữ liệu.

Đóng góp của em đồng thời cung cấp cơ sở để giải quyết conflict với RF2: MC2 sử dụng cấu trúc tối ưu cho range/ordered query, trong khi RF2 sử dụng cấu trúc tối ưu cho priority. Hai cấu trúc được duy trì theo từng workload thay vì ép một cấu trúc duy nhất phục vụ tất cả yêu cầu.


## Đóng góp Web

Phần web bổ sung giúp người dùng tương tác với toàn bộ hệ thống qua giao diện trực quan, đồng thời kết nối các component DSA của nhóm thông qua C++ Core.

Các chức năng đã tích hợp gồm:

```text
Quản lý đơn thuê
├── Thêm
├── Sửa
├── Xóa
├── Tìm kiếm
└── Hoàn tác

DSA Features
├── MC1 → MyHashTable
├── MC2 → Merge Sort + Binary Search
├── RF1 → Trie
├── RF2 → Max Heap
└── RF3 → Undo Stack

MC2
├── Range Query
├── Top xe được thuê nhiều nhất
└── Benchmark Binary Search vs Linear Scan
```

