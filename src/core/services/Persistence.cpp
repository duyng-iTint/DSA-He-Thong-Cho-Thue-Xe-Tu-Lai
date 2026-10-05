// Persistence.cpp
#include "Persistence.h"

#include <fstream>
#include "CsvCodec.h"

namespace {

} // namespace

int loadIntoHashTable(const std::string& csvPath,
                       PersistenceContext& ctx,
                       MyHashTable<Booking*>& tableById,
                       MyHashTable<Booking*>& tableByPlate) {
    std::ifstream in(csvPath);
    if (!in.is_open()) {
        return 0; // file chua ton tai (lan chay dau tien) - khong loi
    }

    std::string line;
    std::vector<std::string> header;
    int count = 0;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (header.empty()) { header = CsvCodec::parseRow(line); continue; }
        std::vector<std::string> row = CsvCodec::parseRow(line);
        if (row.empty()) continue;

        Booking booking = Booking::fromCsvRow(row, header);
        if (booking.booking_id.empty()) continue;
        Booking* ptr = ctx.addBooking(booking); // storage so huu object that

        tableById.insert(ptr->booking_id, ptr);
        tableByPlate.insert(ptr->bien_so, ptr);
        ++count;
    }
    return count;
}

int saveFromHashTable(const std::string& csvPath,
                       const MyHashTable<Booking*>& tableById) {
    std::ofstream out(csvPath, std::ios::trunc);
    int count = 0;

    out << CsvCodec::encodeRow(Booking::header()) << "\n";
    for (const auto& kv : tableById.allItems()) {
        const Booking* b = kv.second;
        out << CsvCodec::encodeRow(b->toRow()) << "\n";
        ++count;
    }
    return count;
}
