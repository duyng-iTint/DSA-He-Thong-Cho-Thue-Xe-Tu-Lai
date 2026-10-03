// project/tools/DataGenerator.cpp
#include "DataGenerator.h"
#include "../model/Booking.h"
#include "CsvCodec.h"

#include <fstream>
#include <random>
#include <vector>
#include <utility>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <string>

namespace {

    const std::vector<std::pair<std::string, std::string>> HANG_DONG_XE = {
        {"Toyota", "Vios"}, {"Toyota", "Innova"}, {"Hyundai", "Accent"},
        {"Hyundai", "i10"}, {"Hyundai", "Grand i10"}, {"Kia", "Morning"},
        {"Kia", "Seltos"}, {"Honda", "City"}, {"Mazda", "CX-5"},
        {"Mitsubishi", "Xpander"}, {"Ford", "Everest"}, {"VinFast", "VF5"},
    };

    const std::vector<std::string> TRANG_THAI_CHOICES = {
        "DANG_THUE", "DA_TRA", "DA_HUY"
    };

    // % giảm giá cho hạng thành viên 0..3
    const int GIAM_GIA_THEO_HANG[4] = { 0, 3, 5, 10 };

    std::string padLeft(int value, int width) {
        std::ostringstream oss;
        oss << std::setw(width) << std::setfill('0') << value;
        return oss.str();
    }

    // Số ngày kể từ 1970-01-01
    long long daysFromCivil(int y, int m, int d) {
        y -= (m <= 2);
        long long era = (y >= 0 ? y : y - 399) / 400;
        unsigned yoe = static_cast<unsigned>(y - era * 400);
        unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
        unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + static_cast<long long>(doe) - 719468;
    }

    // Giá thuê 1 ngày theo dòng xe (VND)
    long long giaMotNgay(const std::string& dongXe) {
        if (dongXe == "i10" || dongXe == "Grand i10" || dongXe == "Morning" || dongXe == "VF5") return 600000;
        if (dongXe == "Vios" || dongXe == "City" || dongXe == "Accent") return 800000;
        if (dongXe == "Seltos" || dongXe == "Xpander" || dongXe == "CX-5") return 1100000;
        if (dongXe == "Innova") return 1200000;
        if (dongXe == "Everest") return 1800000;
        return 1000000;
    }

} // namespace

void generateCsv(const std::string& csvPath, int nRecords, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> distProvince(29, 51);
    std::uniform_int_distribution<int> distLetter(0, 25);
    std::uniform_int_distribution<int> distNum3(100, 999);
    std::uniform_int_distribution<int> distNum2(10, 99);
    std::uniform_int_distribution<int> distKhach(1, nRecords / 2 + 1);
    std::uniform_int_distribution<std::size_t> distXe(0, HANG_DONG_XE.size() - 1);
    std::uniform_int_distribution<int> distThang(1, 12);
    std::uniform_int_distribution<int> distNgay(1, 28);
    std::uniform_int_distribution<int> distSoNgayThue(1, 10);
    std::uniform_int_distribution<int> distDatTruoc(1, 30);        // đặt trước 1..30 ngày
    std::uniform_int_distribution<int> distGiayTrongNgay(0, 86399);
    std::uniform_int_distribution<std::size_t> distTrangThai(0, TRANG_THAI_CHOICES.size() - 1);

    std::ofstream out(csvPath, std::ios::trunc);
    auto header = Booking::header();
    for (std::size_t i = 0; i < header.size(); ++i) {
        if (i > 0) out << ',';
        out << header[i];
    }
    out << "\n";

    for (int i = 1; i <= nRecords; ++i) {
        std::string bookingId = "RENT_HCM_" + padLeft(i, 6);
        std::string bienSo = std::to_string(distProvince(rng))
            + static_cast<char>('A' + distLetter(rng))
            + "-" + std::to_string(distNum3(rng))
            + "." + std::to_string(distNum2(rng));
        int khachId = distKhach(rng);
        std::string tenKhach = "Khach_" + padLeft(khachId, 6);
        const auto& xe = HANG_DONG_XE[distXe(rng)];

        int thangBd = distThang(rng);
        int ngayBd = distNgay(rng);
        std::string ngayBatDau = "2026-" + padLeft(thangBd, 2) + "-" + padLeft(ngayBd, 2);
        int soNgayThue = distSoNgayThue(rng);
        int ngayKt = std::min(ngayBd + soNgayThue, 28);
        std::string ngayKetThuc = "2026-" + padLeft(thangBd, 2) + "-" + padLeft(ngayKt, 2);

        // Số ngày thực tế (thuê trong ngày vẫn tính 1 ngày)
        int soNgay = std::max(1, ngayKt - ngayBd);

        // Hạng thành viên cố định theo khách: cùng khách -> cùng hạng
        int hang = khachId % 4;

        // Giá = số ngày * giá/ngày * (100 - giảm%) / 100, làm tròn nghìn đồng
        long long gia = static_cast<long long>(soNgay) * giaMotNgay(xe.second)
            * (100 - GIAM_GIA_THEO_HANG[hang]) / 100;
        gia = (gia / 1000) * 1000;

        // Thời điểm đặt: trước ngày bắt đầu 1..30 ngày, giờ ngẫu nhiên (giờ VN = UTC+7)
        long long batDauGiay = daysFromCivil(2026, thangBd, ngayBd) * 86400LL - 7 * 3600LL;
        long long datGiay = batDauGiay
            - static_cast<long long>(distDatTruoc(rng)) * 86400LL
            + distGiayTrongNgay(rng);

        Booking booking;
        booking.booking_id = bookingId; booking.bien_so = bienSo; booking.ten_khach = tenKhach;
        booking.hang_xe = xe.first; booking.dong_xe = xe.second;
        booking.ngay_bat_dau = ngayBatDau; booking.ngay_ket_thuc = ngayKetThuc;
        booking.trang_thai = TRANG_THAI_CHOICES[distTrangThai(rng)];
        booking.gia_tien = static_cast<double>(gia);
        booking.hang_thanh_vien = hang;
        booking.thoi_diem_dat = datGiay * 1000LL;   // epoch milliseconds
        out << CsvCodec::encodeRow(booking.toRow()) << "\n";
    }
}