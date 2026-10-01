// project/tools/DataGenerator.cpp
#include "DataGenerator.h"
#include "Booking.h"

#include <fstream>
#include <random>
#include <vector>
#include <utility>
#include <iomanip>
#include <sstream>
#include <algorithm>

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

std::string padLeft(int value, int width) {
    std::ostringstream oss;
    oss << std::setw(width) << std::setfill('0') << value;
    return oss.str();
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
        std::string tenKhach = "Khach_" + padLeft(distKhach(rng), 6);
        const auto& xe = HANG_DONG_XE[distXe(rng)];

        int thangBd = distThang(rng);
        int ngayBd = distNgay(rng);
        std::string ngayBatDau = "2026-" + padLeft(thangBd, 2) + "-" + padLeft(ngayBd, 2);
        int soNgayThue = distSoNgayThue(rng);
        int ngayKt = std::min(ngayBd + soNgayThue, 28);
        std::string ngayKetThuc = "2026-" + padLeft(thangBd, 2) + "-" + padLeft(ngayKt, 2);
        std::string trangThai = TRANG_THAI_CHOICES[distTrangThai(rng)];

        out << bookingId << ',' << bienSo << ',' << tenKhach << ','
            << xe.first << ',' << xe.second << ','
            << ngayBatDau << ',' << ngayKetThuc << ',' << trangThai << "\n";
    }
}
