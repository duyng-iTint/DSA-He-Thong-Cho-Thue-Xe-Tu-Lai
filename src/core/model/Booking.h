// Booking.h

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include <initializer_list>

struct Booking {
    std::string booking_id;      // Ma dinh danh don thue -> KHOA cua MC1
    std::string bien_so;         // Bien so xe -> KHOA thay the cua MC1
    std::string ten_khach;       // Ten khach thue
    std::string hang_xe;         // Hang xe, VD: Hyundai
    std::string dong_xe;         // Dong xe, VD: Accent
    std::string ngay_bat_dau;    // YYYY-MM-DD
    std::string ngay_ket_thuc;   // YYYY-MM-DD
    std::string trang_thai = "DANG_THUE"; // DANG_THUE / DA_TRA / DA_HUY
    double gia_tien = 0.0;
    int hang_thanh_vien = 0;
    long long thoi_diem_dat = 0; // Unix milliseconds

    // Chuyen record thanh vector<string> de ghi ra CSV.
    std::vector<std::string> toRow() const {
        std::ostringstream price;
        price << std::setprecision(15) << gia_tien;
        return {booking_id, bien_so, ten_khach, hang_xe, dong_xe,
                ngay_bat_dau, ngay_ket_thuc, trang_thai, price.str(),
                std::to_string(hang_thanh_vien), std::to_string(thoi_diem_dat)};
    }

    // Dong tieu de tuong ung voi toRow(), dung khi ghi CSV.
    static std::vector<std::string> header() {
        return {"booking_id", "bien_so", "ten_khach", "hang_xe", "dong_xe",
                "ngay_bat_dau", "ngay_ket_thuc", "trang_thai", "gia_tien",
                "hang_thanh_vien", "thoi_diem_dat"};
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
        try { b.gia_tien = row.size() > 8 ? std::stod(row[8]) : 0.0; } catch (...) { b.gia_tien = 0.0; }
        try { b.hang_thanh_vien = row.size() > 9 ? std::stoi(row[9]) : 0; } catch (...) { b.hang_thanh_vien = 0; }
        try { b.thoi_diem_dat = row.size() > 10 ? std::stoll(row[10]) : 0; } catch (...) { b.thoi_diem_dat = 0; }
        return b;
    }

    static Booking fromCsvRow(const std::vector<std::string>& row, const std::vector<std::string>& columns) {
        Booking b;
        auto normalize = [](const std::string& value) {
            std::string result;
            for (unsigned char ch : value) if (std::isalnum(ch)) result.push_back(static_cast<char>(std::tolower(ch)));
            return result;
        };
        auto get = [&](std::initializer_list<const char*> aliases, const std::string& fallback = "") {
            for (const char* alias : aliases) {
                const std::string key = normalize(alias);
                for (std::size_t i = 0; i < columns.size(); ++i)
                    if (normalize(columns[i]) == key) return i < row.size() ? row[i] : fallback;
            }
            return fallback;
        };
        b.booking_id = get({"booking_id", "BookingID"});
        b.bien_so = get({"bien_so", "CarPlate"});
        b.ten_khach = get({"ten_khach", "CustomerName"});
        b.hang_xe = get({"hang_xe", "CarBrand"});
        b.dong_xe = get({"dong_xe", "CarModel"});
        b.ngay_bat_dau = get({"ngay_bat_dau", "RentDate"});
        b.ngay_ket_thuc = get({"ngay_ket_thuc", "ReturnDate"});
        b.trang_thai = get({"trang_thai", "Status"}, "DANG_THUE");
        try { b.gia_tien = std::stod(get({"gia_tien", "Price"}, "0")); } catch (...) {}
        try { b.hang_thanh_vien = std::stoi(get({"hang_thanh_vien", "MemberRank"}, "0")); } catch (...) {}
        try { b.thoi_diem_dat = std::stoll(get({"thoi_diem_dat", "BookingTimestamp"}, "0")); } catch (...) {}
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
            << ", trang_thai=" << trang_thai
            << ", gia_tien=" << gia_tien
            << ", hang_thanh_vien=" << hang_thanh_vien
            << ", thoi_diem_dat=" << thoi_diem_dat << "}";
        return oss.str();
    }
};
