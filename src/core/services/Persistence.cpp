// Persistence.cpp
#include "Persistence.h"

#include <fstream>
#include <sstream>

namespace {

// Tach 1 dong CSV don gian theo dau phay (khong xu ly truong hop co
// dau phay/nhay kep trong du lieu - dung voi du lieu giả lap cua bai
// nay vi cac truong khong chua dau phay).
std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> result;
    std::string field;
    std::istringstream ss(line);
    while (std::getline(ss, field, ',')) {
        result.push_back(field);
    }
    return result;
}

std::string joinCsvRow(const std::vector<std::string>& row) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < row.size(); ++i) {
        if (i > 0) oss << ',';
        oss << row[i];
    }
    return oss.str();
}

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
    bool isHeader = true;
    int count = 0;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (isHeader) { // bo qua dong tieu de
            isHeader = false;
            continue;
        }
        std::vector<std::string> row = splitCsvLine(line);
        if (row.empty()) continue;

        Booking booking = Booking::fromRow(row);
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

    out << joinCsvRow(Booking::header()) << "\n";
    for (const auto& kv : tableById.allItems()) {
        const Booking* b = kv.second;
        out << joinCsvRow(b->toRow()) << "\n";
        ++count;
    }
    return count;
}
