// RentalRecord.h
// Model MC2 duoc chuyen doi hai chieu voi Booking va unified CSV schema.
#pragma once
#include <string>
#include "Booking.h"

struct RentalRecord {
    std::string bookingId;     // Ma dinh danh don thue, vd: RENT_HCM_00842
    std::string carPlate;      // Bien so xe, vd: 51F-123.45
    std::string carBrand;      // Hang xe, vd: Toyota
    std::string carModel;      // Dong xe, vd: Vios
    std::string rentDate;      // Ngay bat dau thue, dinh dang YYYY-MM-DD (so sanh chuoi = so sanh ngay)
    std::string returnDate;    // Ngay tra xe, dinh dang YYYY-MM-DD
    double price = 0.0;        // Gia tri don thue (VND)
    std::string customerName;  // Ten khach hang
    int memberRank = 0;        // Hang thanh vien khach hang (0..3), dung cho RF2
    std::string status = "DANG_THUE";
    long long bookingTimestamp = 0; // Unix milliseconds

    static RentalRecord fromBooking(const Booking& b) {
        RentalRecord r{};
        r.bookingId = b.booking_id; r.carPlate = b.bien_so;
        r.carBrand = b.hang_xe; r.carModel = b.dong_xe;
        r.rentDate = b.ngay_bat_dau; r.returnDate = b.ngay_ket_thuc;
        r.price = b.gia_tien; r.customerName = b.ten_khach;
        r.memberRank = b.hang_thanh_vien; r.status = b.trang_thai;
        r.bookingTimestamp = b.thoi_diem_dat;
        return r;
    }

    Booking toBooking() const {
        Booking b;
        b.booking_id = bookingId; b.bien_so = carPlate;
        b.hang_xe = carBrand; b.dong_xe = carModel;
        b.ngay_bat_dau = rentDate; b.ngay_ket_thuc = returnDate;
        b.gia_tien = price; b.ten_khach = customerName;
        b.hang_thanh_vien = memberRank; b.trang_thai = status;
        b.thoi_diem_dat = bookingTimestamp;
        return b;
    }
};
