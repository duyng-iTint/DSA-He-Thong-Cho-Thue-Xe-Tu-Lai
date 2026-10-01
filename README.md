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
│ donthue_xe.csv                            │
└───────────────────────────────────────────┘

## Cấu trúc project
car-rental-dsa/ 
│ 
├── README.md 
├── CMakeLists.txt 
├── .gitignore 
│ 
├── docs/
│   
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
|
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