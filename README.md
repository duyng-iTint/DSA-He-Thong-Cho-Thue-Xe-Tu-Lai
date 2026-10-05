# DSA-He-Thong-Cho-Thue-Xe-Tu-Lai

# Car Rental System

Hệ thống cho thuê xe tự lái.

## Mô tả

Đây là đồ án nhóm xây dựng hệ thống quản lý và cho thuê xe tự lái.

## Thành viên

| STT | Họ và tên | MSSV | Vai trò |
|---|---|---|---|
| 1 | Nguyễn Hữu Thịnh | 25110349 | Team Leader |
| 2 | Nguyễn Minh Duy | 25110167 | Developer |
| 3 | Lê Đức Thuần | 25110352 | Developer |
| 4 | Lý Nguyễn Mạnh Thuyên | 25110354 | Developer |
| 5 | Lê Nguyễn Minh Thư | 25110356 | Developer |

## Kiến trúc hệ thống

Hệ thống được tổ chức thành 3 tầng:

```
┌───────────────────────────────────────────┐
│              PRESENTATION                 │
│                                           │
│ CLI / Demo / Visualization                │
│ Nhận input và hiển thị output             │
└─────────────────────┬─────────────────────┘
                      │
                      ▼
┌───────────────────────────────────────────┐
│                 DSA CORE                  │
│                                           │
│ Domain Logic                              │
│ MyHashTable                               │
│ MyMaxHeap                                 │
│ Trie                                      │
│ Stack                                     │
│ Merge Sort                                │
│ Binary Search                             │
│ Booking / Car Services                    │
└─────────────────────┬─────────────────────┘
                      │
                      ▼
┌───────────────────────────────────────────┐
│               PERSISTENCE                 │
│                                           │
│ CSV Load / Save                           │
│ data/donthue_xe.csv                       │
└───────────────────────────────────────────┘
```

## Cấu trúc project

```
car-rental-dsa/ 
│ 
├── README.md 
├── CMakeLists.txt 
├── .gitignore 
│ 
├── docs/
│   
├── src/ 
│   ├── presentation/ 
│   ├── core/ 
│   │   ├── models/ 
│   │   ├── structures/ 
│   │   ├── algorithms/ 
│   │   └── services/ 
│   │   
│   └── persistence/ 
│ 
├── web/
│
├── tests/ 
│ 
├── benchmark/ 
│   └── results/ 
│ 
├── data/ 
│   └── sample/ 
│ 
└── assets/ 
    ├── architecture/ 
    ├── benchmark/ 
    └── screenshots/
```

## Chạy bản web tích hợp

Trên Windows, chạy `./web/start.ps1`. Script build các chương trình C++ bằng CMake rồi chạy web tại `http://localhost:3000`.

Web dùng API Node.js làm presentation adapter và gọi `RentalWebCore` để kết nối `Persistence`, `MyHashTable`, `RentalSystem`, `RentalService`, Trie và `MyMaxHeap`. Dữ liệu đơn thuê được lưu trong `data/donthue_xe.csv`.

Schema CSV chuẩn: `booking_id,bien_so,ten_khach,hang_xe,dong_xe,ngay_bat_dau,ngay_ket_thuc,trang_thai,gia_tien,hang_thanh_vien,thoi_diem_dat`. Persistence hỗ trợ đọc hai định dạng CSV cũ của MC1 và MC2 rồi ghi lại theo schema chuẩn.

Các target CMake riêng:

- `CarRentalApp`: CLI quản lý và Undo RF3, test, benchmark MC1.
- `RentalMC2Demo`: CLI truy vấn ngày, top xe và benchmark MC2.
- `RentalMC1Demo`, `BenchmarkMC1`: demo và benchmark MC1.
- `RentalWebCore`: cầu nối DSA Core cho giao diện web.

Chi tiết endpoint và giới hạn dữ liệu RF2/Undo trong phiên xem [web/README.md](web/README.md).
