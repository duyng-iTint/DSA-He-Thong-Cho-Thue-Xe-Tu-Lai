# Web tích hợp hệ thống thuê xe

Web là presentation cho các module C++ hiện có. Không cần npm package.

## Chạy trên Windows

Từ thư mục gốc repo chạy:

```powershell
./web/start.ps1
```

Script cấu hình CMake, build toàn bộ chương trình C++ và mở Node.js API tại <http://localhost:3000>. Muốn build thủ công:

```powershell
cmake -S . -B build/web-integration
cmake --build build/web-integration --config Release
node web/server.js
```
Dừng web:
```
Stop-Process -Id (Get-NetTCPConnection -LocalPort 127.0.0.1:3000).OwningProcess -Force        
```

## Chức năng và module được gọi

- Tra đơn theo mã hoặc biển số: `MyHashTable` trong Persistence Core.
- Lọc ngày và top xe theo biển số: `RentalService`, Merge Sort và Binary Search.
- Thống kê số đơn theo hãng/dòng: dữ liệu CSV của hệ thống.
- Gợi ý hãng/dòng khi gõ: Trie RF1.
- Xếp yêu cầu tranh chấp theo hạng thành viên và thời điểm: `MyMaxHeap` RF2.
- Danh sách, tra cứu, thêm/sửa/xóa: Node chỉ chuyển HTTP; `RentalWebCore` gọi `RentalSystem`, `MyHashTable` và `Persistence` để xử lý.
- Undo: hoàn nguyên snapshot qua `Persistence` Core; lịch sử Undo nằm trong phiên máy chủ.
- Benchmark MC1, MC2 và RF2 chạy bằng executable CMake riêng, không chạy trong giao diện web. Xem bảng target và lệnh chạy trong README gốc.
- Tạo đơn, sửa, tìm, lọc và xếp ưu tiên có API JSON tại `/api`.

Schema CSV chuẩn có đúng thứ tự: `booking_id,bien_so,ten_khach,hang_xe,dong_xe,ngay_bat_dau,ngay_ket_thuc,trang_thai,gia_tien,hang_thanh_vien,thoi_diem_dat`. Các header MC1 8 cột cũ và MC2 9 cột cũ vẫn được đọc, sau đó chuẩn hóa khi ghi. Bản `data/donthue_xe.csv` hiện đã được chuyển schema; giá/hạng/thời điểm cũ không có dữ liệu được đặt về `0`. Có thể trỏ server vào bản dữ liệu khác bằng `RENTAL_DATA_FILE`.

## CLI C++

CMake có target chạy riêng cho CLI, test và benchmark: `CarRentalApp`, `RentalMC1Demo`, `RentalMC2Demo`, `RentalRF1Demo`, `DemoRF2Heap`, `BenchmarkMC1`, `BenchmarkMC2`, `BenchmarkRF2`, `TestMC1Persistence`, `TestMC2Correctness` và `TestRF1Trie`. Dùng `run_<target>` để build rồi chạy, ví dụ:

```powershell
cmake --build build/web-integration --config Release --target run_BenchmarkMC1
cmake --build build/web-integration --config Release --target run_BenchmarkMC2
cmake --build build/web-integration --config Release --target run_BenchmarkRF2
```

## Giới hạn dữ liệu nghiệp vụ

Các đơn cũ được bổ sung hạng, giá và thời điểm bằng `0` vì CSV trước đó chưa lưu những thông tin này. Đơn tạo mới ghi các trường đó; RF2 lấy hạng/thời điểm từ đơn khi xếp hàng (có thể chỉnh hạng tại form). Hàng đợi ưu tiên và lịch sử Undo nằm trong bộ nhớ máy chủ nên bị xóa khi khởi động lại; dữ liệu thuê chính vẫn được lưu trong CSV.
