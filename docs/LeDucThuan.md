# PHẦN ĐÓNG GÓP CÁ NHÂN — THÀNH VIÊN 4

## Thông tin thành viên

| Nội dung              | Thông tin                                         |
| --------------------- | ------------------------------------------------- |
| Thành viên            | Thành viên 4                                      |
| Họ tên                | Lê Đức Thuần                                      |
| MSSV                  | 25110352                                          |
| Requirement phụ trách | RF1 — Gợi ý nhanh tên hãng / dòng xe theo tiền tố |
| Cấu trúc dữ liệu      | Trie                                              |
| Component             | `CarTrie`                                         |
| File cài đặt chính    | `src/core/structures/Trie.h`                      |
| File demo             | `src/presentation/mainRF1.cpp`                    |
| File kiểm thử         | `test/TrieTest.cpp`                               |

---

## D1 — BÀI ĐỌC-HIỂU ĐỀ BÀI

### 1. Requirement phụ trách

**RF1 — Gợi ý nhanh tên hãng / dòng xe theo tiền tố**

Hệ thống cần hỗ trợ người dùng tìm kiếm và gợi ý tên xe khi nhập một phần đầu của tên hãng hoặc dòng xe.

Ví dụ:

* Nhập `toy` → gợi ý `Toyota Innova`, `Toyota Vios`.
* Nhập `inno` → gợi ý `Toyota Innova`.
* Nhập `i10` → gợi ý `Hyundai Grand i10`, `Hyundai i10`.

Component được sử dụng để thực hiện chức năng này là **Trie (cây tiền tố)**.

---

### 2. Input

Input của RF1 gồm:

* Tên hãng xe (`brand`).
* Tên dòng xe (`model`) khi nạp dữ liệu.
* Chuỗi tiền tố (`prefix`) khi người dùng tìm kiếm.

Ví dụ dữ liệu được nạp:

```text
Toyota - Vios
Toyota - Innova
Hyundai - i10
Hyundai - Grand i10
Kia - Morning
```

Ví dụ truy vấn:

```text
toy
inno
i10
```

---

### 3. Output

Hệ thống trả về danh sách tên xe đầy đủ phù hợp với tiền tố tìm kiếm.

Ví dụ:

```text
Input: toy

Output:
Toyota Innova
Toyota Vios
```

Hoặc:

```text
Input: inno

Output:
Toyota Innova
```

Nếu không có kết quả:

```text
Input: Ferrari

Output:
Danh sách rỗng
```

---

### 4. Các yêu cầu chính

Component `CarTrie` phải:

1. Nạp tên hãng và dòng xe vào Trie.
2. Hỗ trợ tìm kiếm theo tiền tố.
3. Cho phép tìm trực tiếp theo tên dòng xe.
4. Không phân biệt chữ hoa và chữ thường.
5. Loại bỏ khoảng trắng thừa ở đầu và cuối chuỗi tìm kiếm.
6. Trả về đầy đủ các kết quả phù hợp.
7. Trả về danh sách rỗng nếu không có kết quả.
8. Kết quả được sắp xếp theo thứ tự từ điển A-Z.

---

### 5. Edge cases cần xử lý

Các trường hợp biên được kiểm tra gồm:

* Chuỗi tìm kiếm rỗng.
* Chuỗi chỉ chứa khoảng trắng.
* Prefix không tồn tại.
* Prefix viết hoa.
* Prefix viết thường.
* Prefix có khoảng trắng ở đầu/cuối.
* Prefix khớp nhiều dòng xe.
* Tìm kiếm bằng tên dòng xe độc lập.

---

### 6. Access pattern

Access pattern chính của RF1 là:

```text
Người dùng nhập prefix
        ↓
Chuẩn hóa prefix
        ↓
Duyệt Trie theo từng ký tự
        ↓
Tìm node tương ứng với prefix
        ↓
Duyệt cây con để lấy các kết quả
        ↓
Sắp xếp kết quả A-Z
        ↓
Trả về danh sách gợi ý
```

Thao tác quan trọng nhất là:

```text
getAllSuggestions(prefix)
```

vì đây là thao tác được thực hiện liên tục trong quá trình autocomplete.


---

## D3 — THIẾT KẾ & BIỆN MINH LỰA CHỌN

| Nội dung                                        | Phân tích                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| ----------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Q1. Có cần thứ tự không?**                    | **Có.** Kết quả autocomplete cần có thứ tự ổn định để giao diện hiển thị nhất quán. Trong implementation, các node con của Trie được lưu bằng `unordered_map`, nên thứ tự duyệt cây ban đầu không được đảm bảo. Vì vậy sau khi thu thập kết quả, hàm `getAllSuggestions()` sử dụng `sort(results.begin(), results.end())` để sắp xếp danh sách theo thứ tự từ điển A-Z. Điều này được kiểm tra trực tiếp trong `TrieTest.cpp`: `Toyota Innova` phải đứng trước `Toyota Vios`. |
| **Q2. Loại khóa và khối lượng thao tác**        | Khóa là **chuỗi ngắn**, gồm tên hãng và tên dòng xe. Danh mục xe tương đối nhỏ và ít thay đổi. Truy vấn được thực hiện lặp lại nhiều lần khi người dùng gõ từng ký tự, ví dụ `t` → `to` → `toy`. Input có thể viết hoa hoặc viết thường nên cần chuẩn hóa trước khi tìm kiếm.                                                                                                                                                                                                 |
| **Q3. Có chấp nhận trường hợp xấu nhất không?** | **Có.** Chi phí đi từ root đến node đại diện cho prefix là `O(L)`, với `L` là độ dài prefix. Sau đó cần duyệt cây con để thu thập các gợi ý, nên tổng chi phí còn phụ thuộc vào số lượng node/kết quả cần duyệt. Vì autocomplete phải trả về danh sách kết quả nên chi phí duyệt kết quả là cần thiết.                                                                                                                                                                        |
| **Q4. Yếu tố phi tiệm cận**                     | Mỗi node sử dụng `unordered_map<char, TrieNode*>` để lưu các node con nên có overhead bộ nhớ lớn hơn cách lưu một mảng cố định. Tuy nhiên danh mục xe không quá lớn nên mức sử dụng bộ nhớ này có thể chấp nhận được. Implementation cũng nạp cả cụm tên đầy đủ và các từ khóa dòng xe để người dùng có thể nhập `vios` và nhận `Toyota Vios`.                                                                                                                                |
| **Cấu trúc được chọn**                          | **Trie (cây tiền tố)**. Dữ liệu được tổ chức theo từng ký tự của chuỗi. `CarTrie` sử dụng `normalize()` để chuẩn hóa chữ thường và loại bỏ khoảng trắng đầu/cuối. Mỗi node có `matchedFullCarNames` để lưu tên xe đầy đủ kết thúc tại node đó.                                                                                                                                                                                                                                |
| **Đánh đổi đã chấp nhận**                       | Trie sử dụng nhiều node và con trỏ nên tốn bộ nhớ hơn một danh sách/mảng đơn giản. Việc lưu thêm các khóa dòng xe làm tăng số lượng đường đi trong Trie. `unordered_map` giúp chỉ tạo các nhánh ký tự thực sự xuất hiện nhưng có overhead quản lý bảng băm. Implementation hiện tại cũng chưa xử lý riêng trường hợp tiếng Việt có dấu.                                                                                                                                       |
| **Điều kiện làm lựa chọn không còn đúng**       | Nếu danh mục chỉ có vài chục tên và số lần autocomplete rất ít, duyệt tuyến tính và so prefix có thể đã đủ đơn giản. Nếu yêu cầu sắp gợi ý theo độ phổ biến thì Trie hiện tại chưa có thông tin về số lượt sử dụng và cần kết hợp thêm dữ liệu đếm/ranking. Nếu cần tìm chuỗi ở giữa tên hoặc hỗ trợ sai chính tả thì Trie theo prefix hiện tại không đáp ứng trực tiếp.                                                                                                      |
| **Người viết**                                  | **Lê Đức Thuần**                                                                                                                                                                                                                                                                                                                                                                                                                                                     |

### Kết luận lựa chọn

Trie được chọn vì access pattern chính của RF1 là **tìm kiếm theo tiền tố**. Cấu trúc này cho phép xác định node tương ứng với prefix bằng cách duyệt theo từng ký tự, sau đó thu thập các kết quả nằm trong cây con.

Việc sử dụng `sort()` sau khi thu thập kết quả giúp đảm bảo output có thứ tự A-Z ổn định, mặc dù `children` được lưu bằng `unordered_map`.

---
## Biện minh cá nhân: Thành viên 4 - RF1
Lê Đức Thuần: RF1

| Nội dung                                        | Biện minh                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| ----------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Thành phần sở hữu**                           | **RF1 — Tính năng Autocomplete gợi ý tên hãng/dòng xe**, bao gồm component `CarTrie`, implementation trong `Trie.h` và bộ kiểm thử `TrieTest.cpp`. Thành phần này chịu trách nhiệm tìm kiếm xe theo tiền tố và trả về danh sách tên xe phù hợp.                                                                                                                                                                                                                                                          |
| **Yêu cầu mình phân tích**                      | RF1 cần hỗ trợ người dùng nhập một phần tên hãng hoặc dòng xe và nhận được các gợi ý tương ứng. Truy vấn được thực hiện nhiều lần khi người dùng gõ từng ký tự. Ngoài ra, hệ thống cần xử lý chữ hoa/chữ thường, khoảng trắng đầu/cuối, prefix không tồn tại và trường hợp một prefix khớp với nhiều xe. Kết quả cần có thứ tự từ điển A-Z để hiển thị ổn định.                                                                                                                                          |
| **Quyết định thiết kế mình bảo vệ**             | Mình lựa chọn **Trie (cây tiền tố)** vì access pattern chính của RF1 là tìm kiếm theo prefix. Trie cho phép đi từ node gốc theo từng ký tự của prefix với chi phí `O(L)`, trong đó `L` là độ dài prefix. Sau khi tìm được node tương ứng, hệ thống duyệt cây con để thu thập các tên xe phù hợp. Implementation sử dụng `normalize()` để chuẩn hóa input, đồng thời chèn cả tên đầy đủ và các khóa dòng xe để người dùng có thể tìm trực tiếp bằng tên model, ví dụ `inno` → `Toyota Innova`.            |
| **Đánh đổi và điều kiện làm nó không còn đúng** | Trie sử dụng nhiều node và con trỏ nên tốn bộ nhớ hơn cách lưu danh sách/mảng đơn giản. Việc chèn thêm các khóa phụ cho dòng xe cũng làm tăng số lượng node/đường đi trong Trie. `unordered_map` giúp quản lý các node con linh hoạt nhưng thứ tự duyệt không được đảm bảo, vì vậy kết quả phải được `sort()` lại theo A-Z. Lựa chọn Trie sẽ kém cần thiết nếu danh mục xe rất nhỏ và số lần tìm kiếm ít, khi đó duyệt tuyến tính có thể đơn giản hơn.                                                   |
| **Nếu yêu cầu thay đổi thì sao**                | Nếu yêu cầu chỉ tìm theo tên đầy đủ và danh mục rất nhỏ, có thể chuyển sang danh sách/mảng kết hợp tìm tuyến tính. Nếu cần sắp xếp gợi ý theo **độ phổ biến**, Trie hiện tại cần bổ sung thông tin về số lượt sử dụng hoặc cơ chế ranking. Nếu cần tìm chuỗi ở giữa tên xe thay vì chỉ tìm theo prefix, hoặc hỗ trợ tìm kiếm gần đúng/sai chính tả, cần cân nhắc cấu trúc hoặc thuật toán tìm kiếm khác. Nếu cần hỗ trợ tiếng Việt có dấu một cách đầy đủ, hàm chuẩn hóa hiện tại cũng cần được mở rộng. |

---

## D4 — IMPLEMENTATION

### 1. Component phụ trách

Component chính:

```text
CarTrie
```

File:

```text
src/core/structures/Trie.h
```

File kiểm thử:

```text
TrieTest.cpp
```

---

### 2. Cấu trúc `TrieNode`

Mỗi node trong Trie gồm:

```cpp
unordered_map<char, TrieNode*> children;
bool isEndOfWord;
vector<string> matchedFullCarNames;
```

Ý nghĩa:

| Thành phần            | Chức năng                                                           |
| --------------------- | ------------------------------------------------------------------- |
| `children`            | Lưu các node con tương ứng với ký tự tiếp theo                      |
| `isEndOfWord`         | Xác định node hiện tại có phải điểm kết thúc của một khóa hay không |
| `matchedFullCarNames` | Lưu tên xe đầy đủ tương ứng với khóa kết thúc tại node              |

---

### 3. `normalize()`

Hàm `normalize()` thực hiện hai nhiệm vụ:

* Loại bỏ khoảng trắng ở đầu và cuối chuỗi.
* Chuyển các ký tự về chữ thường.

Ví dụ:

```text
"   TOY   "
```

được chuẩn hóa thành:

```text
"toy"
```

Nhờ đó:

```text
toy
TOY
tOy
```

được xử lý giống nhau.

---

### 4. `splitWords()`

Hàm `splitWords()` tách tên xe thành các từ riêng biệt.

Ví dụ:

```text
Toyota Innova
```

được tách thành:

```text
Toyota
Innova
```

Hàm này được sử dụng khi xây dựng các khóa phụ cho phép tìm kiếm trực tiếp theo dòng xe.

---

### 5. `insertKey()`

`insertKey()` thực hiện việc đưa một khóa vào Trie.

Với mỗi ký tự trong khóa:

```text
key
 ↓
ký tự 1
 ↓
ký tự 2
 ↓
...
 ↓
ký tự cuối
```

Nếu node tương ứng chưa tồn tại thì tạo node mới.

Khi đến cuối khóa:

```text
isEndOfWord = true
```

và tên xe đầy đủ được lưu trong:

```text
matchedFullCarNames
```

---

### 6. `insertCar()`

Khi thêm:

```text
Toyota - Innova
```

component tạo tên đầy đủ:

```text
Toyota Innova
```

Sau đó nạp:

```text
toyota innova
```

vào Trie.

Đồng thời, component tạo khóa phụ từ phần dòng xe:

```text
innova
```

và liên kết khóa này với:

```text
Toyota Innova
```

Nhờ đó:

```text
getAllSuggestions("inno")
```

có thể trả về:

```text
Toyota Innova
```

---

### 7. `collectAllWords()`

Sau khi tìm được node tương ứng với prefix, `collectAllWords()` duyệt các node trong cây con để thu thập các tên xe phù hợp.

Để tránh một tên xe xuất hiện nhiều lần trong kết quả, hàm kiểm tra:

```cpp
find(results.begin(), results.end(), carName)
```

trước khi thêm kết quả.

---

### 8. `getAllSuggestions()`

Đây là thao tác chính của RF1.

Quy trình:

```text
prefix
   ↓
normalize(prefix)
   ↓
kiểm tra prefix rỗng
   ↓
duyệt Trie theo từng ký tự
   ↓
không tìm thấy → return empty
   ↓
tìm thấy node prefix
   ↓
collectAllWords()
   ↓
sort(results.begin(), results.end())
   ↓
return results
```

Việc gọi `sort()` đảm bảo kết quả cuối cùng được sắp xếp theo thứ tự từ điển A-Z.

---

### 9. Destructor

`TrieNode` có destructor đệ quy:

```cpp
~TrieNode()
```

Destructor duyệt qua các node con và `delete` chúng.

`CarTrie` cũng giải phóng:

```cpp
delete root;
```

nhằm tránh rò rỉ bộ nhớ do các node Trie được cấp phát động.

---

### 10. Tích hợp và kiểm thử

`TrieTest.cpp` tạo một đối tượng:

```cpp
CarTrie trie;
```

sau đó nạp dữ liệu mẫu và thực hiện 8 test case.

Các test kiểm tra:

* tìm theo hãng;
* tìm theo dòng xe;
* nhiều kết quả;
* thứ tự A-Z;
* không phân biệt hoa/thường;
* khoảng trắng;
* chuỗi rỗng;
* prefix không tồn tại.

Kết quả cuối cùng:

```text
=== TAT CA 8/8 UNIT TEST CASES DA PASS 100%! ===
```

---

## D5 — TESTING & DEBUG LOG

### 1. Mục tiêu

D5 tập trung kiểm tra tính đúng đắn và độ an toàn của component `CarTrie` thông qua unit test.

Các nội dung kiểm tra:

* Prefix search.
* Tìm theo hãng.
* Tìm theo dòng xe.
* Thứ tự kết quả.
* Không phân biệt hoa/thường.
* Trim khoảng trắng.
* Chuỗi rỗng.
* Prefix không tồn tại.

**RF1 không sử dụng benchmark làm tiêu chí kiểm thử bắt buộc.**

---

## 2. Unit Test

| Test   | Nội dung                        | Kết quả |
| ------ | ------------------------------- | ------- |
| Test 1 | Tìm theo tiền tố hãng `toy`     | PASS    |
| Test 2 | Tìm theo dòng xe `inno`         | PASS    |
| Test 3 | Prefix `i10` khớp nhiều dòng xe | PASS    |
| Test 4 | Kiểm tra thứ tự từ điển A-Z     | PASS    |
| Test 5 | Không phân biệt hoa/thường      | PASS    |
| Test 6 | Cắt khoảng trắng đầu/cuối       | PASS    |
| Test 7 | Chuỗi rỗng/toàn khoảng trắng    | PASS    |
| Test 8 | Prefix không tồn tại            | PASS    |

### Kết quả

```text
8/8 UNIT TEST CASES PASS
```

---

# 3. Nhật ký Debug

### Debug 01 — Chuẩn hóa chữ hoa/chữ thường

**Vấn đề cần kiểm tra:**
Autocomplete phải cho cùng kết quả khi người dùng nhập:

```text
toy
TOY
tOy
```

**Nguyên nhân tiềm ẩn:**
Nếu key khi insert và prefix khi search không được chuẩn hóa giống nhau thì Trie có thể không tìm thấy node tương ứng.

**Cách xử lý:**
Sử dụng `normalize()` cho cả dữ liệu khi insert và prefix khi search. Hàm chuyển chuỗi về lowercase.

**Verification:**

```cpp
assert(resUpper == resToy);
assert(resMixed == resToy);
```

**Kết quả:** PASS.

---

### Debug 02 — Khoảng trắng thừa

**Vấn đề cần kiểm tra:**
Người dùng có thể nhập:

```text
"   inno   "
```

thay vì:

```text
"inno"
```

**Cách xử lý:**
`normalize()` loại bỏ khoảng trắng ở đầu và cuối chuỗi.

**Verification:**

```cpp
vector<string> resSpace =
    trie.getAllSuggestions("   inno   ");

assert(resSpace == resInnova);
```

**Kết quả:** PASS.

---

### Debug 03 — Prefix không tồn tại

**Vấn đề cần kiểm tra:**
Một prefix không tồn tại không được gây crash hoặc truy cập node không hợp lệ.

Ví dụ:

```text
Ferrari
```

**Cách xử lý:**
Trong `getAllSuggestions()`, nếu một ký tự của prefix không tồn tại trong `children`, hàm trả về `results` rỗng.

**Verification:**

```cpp
vector<string> resNone =
    trie.getAllSuggestions("Ferrari");

assert(resNone.empty());
```

**Kết quả:** PASS.

---

### Debug 04 — Một prefix có nhiều kết quả

**Vấn đề cần kiểm tra:**
Một prefix có thể tương ứng với nhiều xe.

Ví dụ:

```text
i10
```

phải có khả năng trả về:

```text
Hyundai i10
Hyundai Grand i10
```

**Cách xử lý:**
Sau khi tìm được node prefix, `collectAllWords()` duyệt toàn bộ cây con để lấy các tên xe phù hợp.

**Verification:**

```cpp
vector<string> resI10 =
    trie.getAllSuggestions("i10");

assert(resI10.size() >= 2);
```

**Kết quả:** PASS.

---

### Debug 05 — Thứ tự kết quả

**Vấn đề cần kiểm tra:**
`children` sử dụng `unordered_map`, vì vậy thứ tự duyệt node không được đảm bảo.

Nếu trả kết quả trực tiếp sau khi duyệt Trie, thứ tự có thể không ổn định.

**Cách xử lý:**
Sau khi thu thập kết quả, `getAllSuggestions()` thực hiện:

```cpp
sort(results.begin(), results.end());
```

**Verification:**

```cpp
assert(resToy[0] == "Toyota Innova");
assert(resToy[1] == "Toyota Vios");
```

**Kết quả:** PASS.

---

## 4. Kết luận D5

Sau khi chạy bộ unit test:

```text
8/8 TEST CASES PASSED
```

Component `CarTrie` đáp ứng các trường hợp chức năng chính của RF1:

* tìm theo tên hãng;
* tìm theo dòng xe;
* tìm nhiều kết quả;
* sắp xếp A-Z;
* không phân biệt hoa/thường;
* xử lý khoảng trắng;
* xử lý chuỗi rỗng;
* xử lý prefix không tồn tại.

Các kiểm tra trên tập trung vào **correctness**, không thực hiện benchmark hiệu năng.

---

## D6 — PEER TECHNICAL REVIEW

### Thành phần được review

**Thành viên được review:** Thành viên 2

**Requirement:** MC2 — Lọc xe theo khoảng thời gian thuê / Top xe hot.

**DSA phụ trách:**

* Merge Sort.
* Binary Search.

### Nhận xét

Thiết kế của MC2 sử dụng Merge Sort và Binary Search phù hợp với bài toán khi dữ liệu cần được sắp xếp trước để hỗ trợ các truy vấn theo khoảng thời gian. Merge Sort có thể tạo ra danh sách được sắp xếp theo khóa thời gian, sau đó Binary Search được sử dụng để xác định phạm vi dữ liệu cần lấy thay vì phải duyệt toàn bộ danh sách cho mỗi truy vấn.

Điểm đáng chú ý là cần phân biệt rõ hai nhiệm vụ: **Merge Sort chịu trách nhiệm tạo thứ tự**, còn **Binary Search chịu trách nhiệm tìm vị trí trong dữ liệu đã được sắp xếp**. Điều này giúp thiết kế phù hợp với access pattern của truy vấn khoảng thời gian.

Đối với chức năng **Top xe hot**, cần đảm bảo tiêu chí xác định “hot” được định nghĩa rõ ràng, chẳng hạn dựa trên số lượt thuê. Trường hợp hai xe có cùng số lượt thuê cũng nên có quy tắc tie-break rõ ràng để kết quả ổn định.

Một số edge case nên được kiểm tra gồm:

* Khoảng thời gian không có xe phù hợp.
* Chỉ có một xe trong khoảng thời gian.
* Nhiều xe có cùng số lượt thuê.
* `fromDate` và `toDate` trùng nhau.
* Khoảng thời gian đầu vào không hợp lệ.

### Đề xuất cải thiện

1. Ghi rõ precondition của Binary Search: dữ liệu phải được sắp xếp theo đúng khóa trước khi tìm kiếm.
2. Xác định rõ khoảng thời gian là đóng hay mở ở hai đầu, ví dụ `[fromDate, toDate]`.
3. Bổ sung test cho trường hợp không có dữ liệu trong khoảng thời gian.
4. Xác định tie-break cho Top xe hot để kết quả deterministic.
5. Đảm bảo các test của Merge Sort và Binary Search kiểm tra cả trường hợp biên, không chỉ trường hợp thông thường.

### Kết luận

Nhìn chung, việc lựa chọn Merge Sort kết hợp Binary Search là phù hợp với bài toán MC2 vì dữ liệu được sắp xếp và có nhu cầu thực hiện truy vấn theo khoảng thời gian. Các điểm cần chú ý chủ yếu nằm ở việc xác định rõ precondition, edge case và quy tắc sắp xếp khi có các giá trị bằng nhau.

---

## D7 — AI USAGE LOG & REFLECTION

### 1. AI Usage Log

| STT | Nội dung sử dụng AI       | Mục đích                                                | Kết quả kiểm tra                                     |
| --- | ------------------------- | ------------------------------------------------------- | ---------------------------------------------------- |
| 1   | Phân tích requirement RF1 | Xác định input, output và access pattern                | Đối chiếu với yêu cầu đồ án                          |
| 2   | Phân tích lựa chọn Trie   | Giải thích vì sao Trie phù hợp với prefix search        | Đối chiếu với implementation `CarTrie`               |
| 3   | Phân tích độ phức tạp     | Xác định chi phí tìm prefix và duyệt kết quả            | Đối chiếu với `getAllSuggestions()`                  |
| 4   | Xây dựng test case        | Xác định các trường hợp cần kiểm tra                    | Kiểm tra lại bằng `TrieTest.cpp`                     |
| 5   | Phân tích edge case       | Kiểm tra chuỗi rỗng, khoảng trắng, prefix không tồn tại | Đối chiếu với `normalize()` và `getAllSuggestions()` |
| 6   | Hỗ trợ peer review        | Chuẩn bị nội dung review MC2 của thành viên 2           | Đối chiếu với requirement và thiết kế MC2            |

AI được sử dụng với vai trò hỗ trợ phân tích và kiểm tra. Implementation cuối cùng được đối chiếu với source code thực tế của project và kết quả unit test.

---

## 2. AI Audit

Một nội dung được kiểm tra kỹ là độ phức tạp của thao tác autocomplete.

Không chỉ kết luận rằng:

```text
Prefix Search = O(L)
```

mà cần phân biệt:

```text
Tìm node đại diện cho prefix:
O(L)
```

và:

```text
Thu thập các kết quả trong cây con:
phụ thuộc vào số node cần duyệt và số kết quả.
```

Ngoài ra, vì `children` được lưu bằng `unordered_map`, thứ tự duyệt Trie không được đảm bảo. Implementation giải quyết vấn đề này bằng cách sắp xếp:

```cpp
sort(results.begin(), results.end());
```

Do đó output cuối cùng có thứ tự A-Z ổn định.

---

## 3. Reflection cá nhân

Qua quá trình thực hiện RF1, em hiểu rõ hơn mối quan hệ giữa requirement, access pattern và việc lựa chọn cấu trúc dữ liệu.

RF1 có đặc điểm là người dùng liên tục nhập từng phần của tên xe để nhận được các gợi ý. Vì vậy, truy vấn chính là prefix search và Trie là cấu trúc phù hợp với dạng truy vấn này.

Trong quá trình hoàn thiện component, em nhận ra rằng chỉ tìm được node tương ứng với prefix là chưa đủ. Sau đó cần duyệt cây con để thu thập các kết quả phù hợp. Ngoài ra, do sử dụng `unordered_map` cho các node con, kết quả cần được sắp xếp lại nếu requirement yêu cầu thứ tự ổn định.

Việc xây dựng `TrieTest.cpp` cũng giúp em nhận ra tầm quan trọng của edge case. Ngoài trường hợp tìm kiếm thông thường, cần kiểm tra cả chữ hoa/chữ thường, khoảng trắng, chuỗi rỗng, prefix không tồn tại và prefix có nhiều kết quả.

Bài học chính của em là việc lựa chọn DSA không chỉ dựa vào độ phức tạp Big-O mà phải dựa trên **workload thực tế của requirement**.

Quy trình em áp dụng là:

```text
Requirement
     ↓
Access Pattern
     ↓
Lựa chọn Data Structure
     ↓
Implementation
     ↓
Testing
     ↓
Debug / Verification
```

Qua RF1, em hiểu rõ hơn cách biến một yêu cầu chức năng cụ thể thành một component DSA có thể kiểm thử và tích hợp vào hệ thống.

---

### Link trao đổi AI
Link trao đổi AI1: https://share.gemini.google/9NhWS0FxQQPt

Link trao đổi AI2: https://share.gemini.google/zUnKPP9FhLiQ