// CSVUtils.cpp
#include "CSVUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace {
    // Tach 1 dong CSV thanh danh sach cac truong (khong xu ly dau nhay kep,
    // du dung cho du lieu don thue xe da chuan hoa khong chua dau phay trong truong).
    std::vector<std::string> splitCsvLine(const std::string& rawLine) {
        // Loai bo ky tu '\r' cuoi dong (file CSV sinh tren Windows co CRLF)
        std::string line = rawLine;
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }
        return fields;
    }
}

bool loadRentalCSV(const std::string& filePath, std::vector<RentalRecord>& outRecords) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[LOI] Khong the mo file: " << filePath << "\n";
        return false;
    }

    outRecords.clear();
    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Bo qua dong tieu de (header)
        if (isFirstLine) {
            isFirstLine = false;
            if (line.rfind("BookingID", 0) == 0) {
                continue;
            }
        }

        auto f = splitCsvLine(line);
        if (f.size() < 9) continue; // dong loi dinh dang -> bo qua

        RentalRecord r;
        r.bookingId     = f[0];
        r.carPlate      = f[1];
        r.carBrand      = f[2];
        r.carModel      = f[3];
        r.rentDate      = f[4];
        r.returnDate    = f[5];
        try {
            r.price = std::stod(f[6]);
        } catch (...) {
            r.price = 0.0;
        }
        r.customerName  = f[7];
        try {
            r.memberRank = std::stoi(f[8]);
        } catch (...) {
            r.memberRank = 0;
        }

        outRecords.push_back(r);
    }

    return true;
}
