// Booking.h
// ---------
// Định nghĩa struct "bản ghi đơn thuê xe" (Booking record).
//
// Đây KHÔNG phải là một cấu trúc dữ liệu DSA (không tính vào yêu cầu
// "2 cấu trúc tự cài đặt từ đầu" của Muc 6.2) - no chi la mot struct
// don gian de luu thong tin 1 dong du lieu.
//
// Cac truong bam theo mo ta bai toan trong file
// HE_THONG_CHO_THUE_XE_TU_LAI.docx: ma don (Booking_ID), bien so xe,
// ten khach, ngay bat dau/ket thuc, hang/dong xe, trang thai.

#pragma once

#include <string>
#include <vector>
#include <sstream>

struct Booking {
    std::string booking_id;      // Ma dinh danh don thue -> KHOA cua MC1
    std::string bien_so;         // Bien so xe -> KHOA thay the cua MC1
    std::string ten_khach;       // Ten khach thue
    std::string hang_xe;         // Hang xe, VD: Hyundai
    std::string dong_xe;         // Dong xe, VD: Accent
    std::string ngay_bat_dau;    // YYYY-MM-DD
    std::string ngay_ket_thuc;   // YYYY-MM-DD
    std::string trang_thai = "DANG_THUE"; // DANG_THUE / DA_TRA / DA_HUY

    // Chuyen record thanh vector<string> de ghi ra CSV.
    std::vector<std::string> toRow() const {
        return {booking_id, bien_so, ten_khach, hang_xe, dong_xe,
                ngay_bat_dau, ngay_ket_thuc, trang_thai};
    }

    // Dong tieu de tuong ung voi toRow(), dung khi ghi CSV.
    static std::vector<std::string> header() {
        return {"booking_id", "bien_so", "ten_khach", "hang_xe", "dong_xe",
                "ngay_bat_dau", "ngay_ket_thuc", "trang_thai"};
    }

    // Dung lai 1 Booking tu 1 dong CSV (vector<string>).
    static Booking fromRow(const std::vector<std::string>& row) {
        Booking b;
        b.booking_id    = row.size() > 0 ? row[0] : "";
        b.bien_so       = row.size() > 1 ? row[1] : "";
        b.ten_khach     = row.size() > 2 ? row[2] : "";
        b.hang_xe       = row.size() > 3 ? row[3] : "";
        b.dong_xe       = row.size() > 4 ? row[4] : "";
        b.ngay_bat_dau  = row.size() > 5 ? row[5] : "";
        b.ngay_ket_thuc = row.size() > 6 ? row[6] : "";
        b.trang_thai    = row.size() > 7 ? row[7] : "DANG_THUE";
        return b;
    }

    // Chuoi mo ta ngan gon, dung de in ra console khi tra cuu.
    std::string toString() const {
        std::ostringstream oss;
        oss << "Booking{id=" << booking_id
            << ", bien_so=" << bien_so
            << ", ten_khach=" << ten_khach
            << ", hang_xe=" << hang_xe
            << ", dong_xe=" << dong_xe
            << ", ngay_bat_dau=" << ngay_bat_dau
            << ", ngay_ket_thuc=" << ngay_ket_thuc
            << ", trang_thai=" << trang_thai << "}";
        return oss.str();
    }
};
