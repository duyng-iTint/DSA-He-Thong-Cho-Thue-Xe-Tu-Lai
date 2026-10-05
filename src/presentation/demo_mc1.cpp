// demo_mc1.cpp
// -------------
// TANG PRESENTATION (Muc 7.1) - CLI console toi gian cho MC1.
// Tang nay CHI nhan input/hien thi output, KHONG chua logic cau truc
// du lieu (moi logic tra cuu nam o MyHashTable.h).
//
// Bien dich & chay (xem Makefile):
//     make demo_mc1
//     ./demo_mc1
//
// Chuc nang demo:
// 1. Nap du lieu tu data/donthue_xe.csv (neu chua co thi tu sinh 1.000 dong).
// 2. Cho nguoi dung tra cuu theo Booking_ID hoac bien so.
// 3. Cho phep them 1 don thue moi roi luu lai ra CSV.

#include <iostream>
#include <fstream>
#include <string>

#include "../core/model/Booking.h"
#include "../core/structures/MyHashTable.h"
#include "../core/services/Persistence.h"
#include "../core/services/DataGenerator.h"

namespace {
const std::string CSV_PATH = "data/donthue_xe.csv";

bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}
}

int main() {
    if (!fileExists(CSV_PATH)) {
        std::cout << "Chua co " << CSV_PATH << ", tu sinh 10.000 ban ghi gia lap...\n";
        generateCsv(CSV_PATH, 10000);
    }

    PersistenceContext ctx;
    MyHashTable<Booking*> tableById;
    MyHashTable<Booking*> tableByPlate;

    int n = loadIntoHashTable(CSV_PATH, ctx, tableById, tableByPlate);
    std::cout << "Da nap " << n << " don thue vao MyHashTable.\n";

    auto stats = tableById.stats();
    std::cout << "Thong ke bang bam (theo Booking_ID): capacity=" << stats.capacity
              << ", size=" << stats.size
              << ", load_factor=" << stats.loadFactor
              << ", max_chain_length=" << stats.maxChainLength << "\n";

    while (true) {
        std::cout << "\n--- MC1: Tra cuu don thue / xe ---\n";
        std::cout << "1. Tra theo Booking_ID\n";
        std::cout << "2. Tra theo bien so\n";
        std::cout << "3. Them don thue moi\n";
        std::cout << "4. Luu va thoat\n";
        std::cout << "Chon (1-4): ";

        std::string choice;
        if (!std::getline(std::cin, choice)) break;

        if (choice == "1") {
            std::cout << "Nhap Booking_ID (VD RENT_HCM_000123): ";
            std::string key;
            std::getline(std::cin, key);
            Booking* result = nullptr;
            if (tableById.search(key, result)) {
                std::cout << "-> " << result->toString() << "\n";
            } else {
                std::cout << "-> Khong tim thay.\n";
            }
        } else if (choice == "2") {
            std::cout << "Nhap bien so: ";
            std::string key;
            std::getline(std::cin, key);
            Booking* result = nullptr;
            if (tableByPlate.search(key, result)) {
                std::cout << "-> " << result->toString() << "\n";
            } else {
                std::cout << "-> Khong tim thay.\n";
            }
        } else if (choice == "3") {
            Booking b;
            std::cout << "Booking_ID moi: "; std::getline(std::cin, b.booking_id);
            std::cout << "Bien so: "; std::getline(std::cin, b.bien_so);
            std::cout << "Ten khach: "; std::getline(std::cin, b.ten_khach);
            std::cout << "Hang xe: "; std::getline(std::cin, b.hang_xe);
            std::cout << "Dong xe: "; std::getline(std::cin, b.dong_xe);
            std::cout << "Ngay bat dau (YYYY-MM-DD): "; std::getline(std::cin, b.ngay_bat_dau);
            std::cout << "Ngay ket thuc (YYYY-MM-DD): "; std::getline(std::cin, b.ngay_ket_thuc);
            b.trang_thai = "DANG_THUE";

            Booking* ptr = ctx.addBooking(b);
            tableById.insert(ptr->booking_id, ptr);
            tableByPlate.insert(ptr->bien_so, ptr);
            std::cout << "-> Da them don thue moi vao bo nho (chua ghi ra file).\n";
        } else if (choice == "4") {
            int saved = saveFromHashTable(CSV_PATH, tableById);
            std::cout << "-> Da ghi " << saved << " ban ghi ra " << CSV_PATH << ". Tam biet!\n";
            break;
        } else {
            std::cout << "-> Lua chon khong hop le.\n";
        }
    }

    return 0;
}
