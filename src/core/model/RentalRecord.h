// RentalRecord.h
// Cấu trúc dữ liệu 1 đơn thuê xe (1 dòng trong file donthue_xe.csv)
// Dùng chung cho toàn bộ nhóm - do Thanh vien 1 dinh nghia phan Persistence,
// Thanh vien 2 (module nay) chi su dung lai de phuc vu MC2.
#pragma once
#include <string>

struct RentalRecord {
    std::string bookingId;     // Ma dinh danh don thue, vd: RENT_HCM_00842
    std::string carPlate;      // Bien so xe, vd: 51F-123.45
    std::string carBrand;      // Hang xe, vd: Toyota
    std::string carModel;      // Dong xe, vd: Vios
    std::string rentDate;      // Ngay bat dau thue, dinh dang YYYY-MM-DD (so sanh chuoi = so sanh ngay)
    std::string returnDate;    // Ngay tra xe, dinh dang YYYY-MM-DD
    double price;              // Gia tri don thue (VND)
    std::string customerName;  // Ten khach hang
    int memberRank;            // Hang thanh vien khach hang (0..3), dung cho RF2 (thanh vien 3)
};
