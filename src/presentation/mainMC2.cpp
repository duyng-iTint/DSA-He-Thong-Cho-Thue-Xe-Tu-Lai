// main.cpp
// Chuong trinh Console (CLI) demo module MC2:
//   - Nap du lieu don thue xe tu file CSV
//   - Tra cuu xe theo khoang thoi gian thue (Range Query, Binary Search)
//   - Xem Top-N xe duoc thue nhieu nhat (Merge Sort)
//   - Benchmark Binary Search vs Linear Scan
//
// Bien dich & chay: xem huong dan trong README.md o thu muc goc.
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <string>

#include "../core/model/RentalRecord.h"
#include "../core/services/RentalService.h"
#include "../core/services/CSVUtils.h"

#ifdef _WIN32
#include <windows.h>
#undef max
#undef min
#endif

namespace {

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void printLine(char c, int n) {
    std::cout << std::string(n, c) << "\n";
}

void printHeaderTable() {
    printLine('-', 118);
    std::cout << std::left
              << std::setw(16) << "Booking ID"
              << std::setw(14) << "Bien so"
              << std::setw(12) << "Hang xe"
              << std::setw(14) << "Dong xe"
              << std::setw(13) << "Ngay thue"
              << std::setw(13) << "Ngay tra"
              << std::setw(15) << "Gia (VND)"
              << std::setw(18) << "Khach hang"
              << "\n";
    printLine('-', 118);
}

void printRecord(const RentalRecord& r) {
    std::cout << std::left
              << std::setw(16) << r.bookingId
              << std::setw(14) << r.carPlate
              << std::setw(12) << r.carBrand
              << std::setw(14) << r.carModel
              << std::setw(13) << r.rentDate
              << std::setw(13) << r.returnDate
              << std::setw(15) << std::fixed << std::setprecision(0) << r.price
              << std::setw(18) << r.customerName
              << "\n";
}

void printRecords(const std::vector<RentalRecord>& records, int maxRows = 50) {
    if (records.empty()) {
        std::cout << "(Khong co ket qua nao phu hop.)\n";
        return;
    }
    printHeaderTable();
    int shown = 0;
    for (const auto& r : records) {
        printRecord(r);
        ++shown;
        if (shown >= maxRows) {
            std::cout << "... (con " << (records.size() - shown) << " dong nua, da an bot de de doc)\n";
            break;
        }
    }
    printLine('-', 118);
    std::cout << "Tong so ket qua: " << records.size() << " don thue.\n";
}

void printCarStats(const std::vector<CarStat>& stats) {
    printLine('-', 70);
    std::cout << std::left
              << std::setw(6)  << "Hang"
              << std::setw(14) << "Bien so"
              << std::setw(14) << "Hang xe"
              << std::setw(16) << "Dong xe"
              << std::setw(10) << "Luot thue"
              << "\n";
    printLine('-', 70);
    int rank = 1;
    for (const auto& s : stats) {
        std::cout << std::left
                  << std::setw(6)  << rank++
                  << std::setw(14) << s.carPlate
                  << std::setw(14) << s.carBrand
                  << std::setw(16) << s.carModel
                  << std::setw(10) << s.rentCount
                  << "\n";
    }
    printLine('-', 70);
}

std::string promptString(const std::string& label) {
    std::cout << label;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int promptInt(const std::string& label) {
    std::cout << label;
    int v = 0;
    while (!(std::cin >> v)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Vui long nhap so nguyen hop le: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}

void showMenu() {
    std::cout << "\n=====================================================\n";
    std::cout << "  MODULE MC2 - TRA CUU KHOANG THOI GIAN / TOP XE HOT\n";
    std::cout << "  (Sorted Array + Binary Search + Merge Sort)\n";
    std::cout << "=====================================================\n";
    std::cout << " 1. Nap du lieu tu file CSV\n";
    std::cout << " 2. Tra cuu don thue theo khoang thoi gian (Range Query)\n";
    std::cout << " 3. Xem Top-N xe duoc thue nhieu nhat\n";
    std::cout << " 4. Benchmark: Binary Search vs Linear Scan\n";
    std::cout << " 5. Xem toan bo du lieu da nap (rut gon)\n";
    std::cout << " 0. Thoat\n";
    std::cout << "=====================================================\n";
}

} // namespace

int main() {
    setupConsole();

    std::vector<RentalRecord> records;      // du lieu goc (thu tu bat ky, nhu doc tu CSV)
    std::vector<RentalRecord> sortedByDate; // ban da Merge Sort theo rentDate - dung cho Range Query
    bool dataLoaded = false;

    std::cout << "HE THONG QUAN LY CHO THUE XE TU LAI - MODULE MC2\n";
    std::cout << "Phu trach: Thanh vien 2 (Merge Sort + Binary Search)\n";

    while (true) {
        showMenu();
        int choice = promptInt("Chon chuc nang: ");

        if (choice == 0) {
            std::cout << "Tam biet!\n";
            break;
        } else if (choice == 1) {
            std::string path = promptString("Nhap duong dan file CSV (Enter = data/donthue_xe.csv): ");
            if (path.empty()) path = "data/donthue_xe.csv";

            if (loadRentalCSV(path, records)) {
                sortedByDate = records;
                RentalService::sortByRentDate(sortedByDate); // Merge Sort O(N log N)
                dataLoaded = true;
                std::cout << "Da nap va sap xep " << records.size() << " don thue tu: " << path << "\n";
            } else {
                std::cout << "Nap du lieu that bai. Kiem tra lai duong dan file.\n";
            }
        } else if (choice == 2) {
            if (!dataLoaded) {
                std::cout << "Ban chua nap du lieu (chon muc 1 truoc).\n";
                continue;
            }
            std::string from = promptString("Tu ngay (YYYY-MM-DD): ");
            std::string to   = promptString("Den ngay (YYYY-MM-DD): ");
            auto result = RentalService::queryByDateRange(sortedByDate, from, to);
            printRecords(result);
        } else if (choice == 3) {
            if (!dataLoaded) {
                std::cout << "Ban chua nap du lieu (chon muc 1 truoc).\n";
                continue;
            }
            int k = promptInt("Nhap so luong Top xe muon xem (vd 5, 10): ");
            auto stats = RentalService::buildCarStats(records);
            auto top = RentalService::topRentedCars(stats, k);
            printCarStats(top);
        } else if (choice == 4) {
            if (!dataLoaded) {
                std::cout << "Ban chua nap du lieu (chon muc 1 truoc).\n";
                continue;
            }
            std::string from = promptString("Tu ngay (YYYY-MM-DD) de benchmark: ");
            std::string to   = promptString("Den ngay (YYYY-MM-DD) de benchmark: ");
            RentalService::benchmarkRangeQuery(sortedByDate, from, to);
        } else if (choice == 5) {
            if (!dataLoaded) {
                std::cout << "Ban chua nap du lieu (chon muc 1 truoc).\n";
                continue;
            }
            printRecords(sortedByDate, 30);
        } else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }

    return 0;
}
