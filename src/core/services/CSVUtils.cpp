// CSVUtils.cpp
#include "CSVUtils.h"
#include <fstream>
#include <iostream>
#include "CsvCodec.h"

bool loadRentalCSV(const std::string& filePath, std::vector<RentalRecord>& outRecords) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[LOI] Khong the mo file: " << filePath << "\n";
        return false;
    }

    outRecords.clear();
    std::string line;
    std::vector<std::string> header;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (header.empty()) { header = CsvCodec::parseRow(line); continue; }
        const auto fields = CsvCodec::parseRow(line);
        Booking booking = Booking::fromCsvRow(fields, header);
        if (booking.booking_id.empty()) continue;
        outRecords.push_back(RentalRecord::fromBooking(booking));
    }

    return true;
}
