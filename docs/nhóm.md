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
