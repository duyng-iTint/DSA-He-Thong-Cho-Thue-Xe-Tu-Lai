#include "../src/core/services/Persistence.h"
#include "../src/core/services/CSVUtils.h"
#include "../src/core/services/CsvCodec.h"
#include "../src/core/services/RentalService.h"
#include "../src/core/services/RentalSystem.h"
#include "../src/core/structures/MyMaxHeap.h"
#include "../src/core/structures/Trie.h"

#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

static std::string json(const std::string& value) {
    std::ostringstream out;
    out << '"';
    for (unsigned char ch : value) {
        switch (ch) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default: if (ch < 0x20) out << ' '; else out << static_cast<char>(ch);
        }
    }
    out << '"';
    return out.str();
}

static std::string bookingJson(const Booking& b) {
    return "{\"booking_id\":" + json(b.booking_id) + ",\"bien_so\":" + json(b.bien_so) +
        ",\"ten_khach\":" + json(b.ten_khach) + ",\"hang_xe\":" + json(b.hang_xe) +
        ",\"dong_xe\":" + json(b.dong_xe) + ",\"ngay_bat_dau\":" + json(b.ngay_bat_dau) +
        ",\"ngay_ket_thuc\":" + json(b.ngay_ket_thuc) + ",\"trang_thai\":" + json(b.trang_thai) +
        ",\"gia_tien\":" + std::to_string(b.gia_tien) + ",\"hang_thanh_vien\":" + std::to_string(b.hang_thanh_vien) +
        ",\"thoi_diem_dat\":" + std::to_string(b.thoi_diem_dat) + "}";
}

int main(int argc, char** argv) {
    if (argc < 3) { std::cerr << "usage: RentalWebCore <csv> <command> [args]"; return 2; }
    const std::string file = argv[1], command = argv[2];

    if (command == "replace") {
        const std::string temp = file + ".web-restore.tmp";
        { std::ofstream out(temp, std::ios::trunc); out << std::cin.rdbuf(); }
        PersistenceContext restoreContext;
        MyHashTable<Booking*> restoreById, restoreByPlate;
        const int loaded = loadIntoHashTable(temp, restoreContext, restoreById, restoreByPlate);
        saveFromHashTable(file, restoreById);
        std::remove(temp.c_str());
        std::cout << "{\"ok\":true,\"count\":" << loaded << '}'; return 0;
    }

    if (command == "migrate") {
        std::vector<RentalRecord> records;
        if (!loadRentalCSV(file, records)) return 1;
        std::ofstream out(file, std::ios::trunc);
        out << CsvCodec::encodeRow(Booking::header()) << '\n';
        for (const auto& record : records) out << CsvCodec::encodeRow(record.toBooking().toRow()) << '\n';
        std::cout << "{\"ok\":true,\"count\":" << records.size() << '}'; return 0;
    }

    if (command == "priority") {
        std::vector<RentalRequest> requests;
        for (int i = 3; i + 2 < argc; i += 3) {
            RentalRequest item{};
            item.bookingId = argv[i]; item.membershipTier = std::atoi(argv[i + 1]);
            item.bookingTimestamp = std::atoll(argv[i + 2]); requests.push_back(item);
        }
        MyMaxHeap heap; heap.BuildHeap(requests);
        std::cout << "{\"size\":" << heap.Size() << ",\"items\":[";
        bool first = true;
        while (!heap.Empty()) {
            auto item = heap.ExtractMax();
            if (!first) std::cout << ','; first = false;
            std::cout << "{\"booking_id\":" << json(item.bookingId) << ",\"membership_tier\":" << item.membershipTier << ",\"booking_timestamp\":" << item.bookingTimestamp << '}';
        }
        std::cout << "]}"; return 0;
    }

    PersistenceContext context;
    MyHashTable<Booking*> byId, byPlate;
    loadIntoHashTable(file, context, byId, byPlate);
    if (command == "mutate" && argc >= 5) {
        renTalSystem system;
        for (const auto& entry : byId.allItems()) {
            const Booking& b = *entry.second;
            renTal existing{b.booking_id, b.ten_khach, b.bien_so, b.ngay_bat_dau, b.ngay_ket_thuc};
            if (!system.addRental(existing, false)) {
                std::cout << "{\"ok\":false,\"error\":\"Dữ liệu hiện có không hợp lệ với RentalSystem.\"}"; return 0;
            }
        }
        const std::string action = argv[3];
        bool ok = false;
        if (action == "delete") {
            ok = system.deleteRental(argv[4], false);
            if (ok) byId.remove(argv[4]);
        }
        else if ((action == "add" || action == "update") && argc >= 15) {
            renTal item{argv[4], argv[5], argv[6], argv[7], argv[8]};
            ok = action == "add" ? system.addRental(item, false) : system.updateRental(argv[4], item, false);
            if (ok && action == "add") {
                Booking booking{};
                booking.booking_id = argv[4]; booking.ten_khach = argv[5]; booking.bien_so = argv[6];
                booking.ngay_bat_dau = argv[7]; booking.ngay_ket_thuc = argv[8];
                booking.hang_xe = argv[9]; booking.dong_xe = argv[10]; booking.trang_thai = argv[11];
                booking.gia_tien = std::atof(argv[12]); booking.hang_thanh_vien = std::atoi(argv[13]); booking.thoi_diem_dat = std::atoll(argv[14]);
                Booking* stored = context.addBooking(booking);
                byId.insert(booking.booking_id, stored);
            } else if (ok) {
                Booking* stored = nullptr;
                byId.search(argv[4], stored);
                stored->ten_khach = argv[5]; stored->bien_so = argv[6];
                stored->ngay_bat_dau = argv[7]; stored->ngay_ket_thuc = argv[8];
                stored->hang_xe = argv[9]; stored->dong_xe = argv[10]; stored->trang_thai = argv[11];
                stored->gia_tien = std::atof(argv[12]); stored->hang_thanh_vien = std::atoi(argv[13]); stored->thoi_diem_dat = std::atoll(argv[14]);
            }
        }
        if (ok) {
            byPlate.clear();
            for (const auto& entry : byId.allItems()) byPlate.insert(entry.second->bien_so, entry.second);
            saveFromHashTable(file, byId);
        }
        std::cout << "{\"ok\":" << (ok ? "true" : "false") << '}'; return 0;
    }
    if (command == "search" && argc >= 4) {
        Booking* found = nullptr;
        if (byId.search(argv[3], found) || byPlate.search(argv[3], found)) std::cout << bookingJson(*found);
        else std::cout << "null";
        return 0;
    }

    std::vector<Booking*> bookings;
    for (const auto& entry : byId.allItems()) bookings.push_back(entry.second);
    std::sort(bookings.begin(), bookings.end(), [](const Booking* a, const Booking* b) {
        if (a->thoi_diem_dat != b->thoi_diem_dat) return a->thoi_diem_dat > b->thoi_diem_dat;
        return a->booking_id > b->booking_id;
    });
    std::unordered_map<std::string, Booking*> byBookingId;
    std::vector<RentalRecord> records;
    CarTrie suggestions;
    for (Booking* b : bookings) {
        byBookingId[b->booking_id] = b;
        suggestions.insertCar(b->hang_xe, b->dong_xe);
        records.push_back(RentalRecord::fromBooking(*b));
    }

    if (command == "list") {
        std::cout << '[';
        for (std::size_t i = 0; i < bookings.size(); ++i) { if (i) std::cout << ','; std::cout << bookingJson(*bookings[i]); }
        std::cout << ']'; return 0;
    }

    if (command == "range" && argc >= 5) {
        RentalService::sortByRentDate(records);
        auto matches = RentalService::queryByDateRange(records, argv[3], argv[4]);
        std::cout << "[";
        for (std::size_t i = 0; i < matches.size(); ++i) { if (i) std::cout << ','; std::cout << bookingJson(*byBookingId[matches[i].bookingId]); }
        std::cout << ']'; return 0;
    }
    if (command == "top") {
        int limit = argc >= 4 ? std::max(0, std::atoi(argv[3])) : 5;
        auto stats = RentalService::buildCarStats(records);
        auto top = RentalService::topRentedCars(stats, limit);
        std::cout << "[";
        for (std::size_t i = 0; i < top.size(); ++i) {
            if (i) std::cout << ',';
            std::cout << "{\"bien_so\":" << json(top[i].carPlate) << ",\"hang_xe\":" << json(top[i].carBrand) << ",\"dong_xe\":" << json(top[i].carModel) << ",\"so_luot_thue\":" << top[i].rentCount << '}';
        }
        std::cout << ']'; return 0;
    }
    if (command == "suggest" && argc >= 4) {
        auto result = suggestions.getAllSuggestions(argv[3]);
        std::sort(result.begin(), result.end());
        if (result.size() > 8) result.resize(8);
        std::cout << '[';
        for (std::size_t i = 0; i < result.size(); ++i) { if (i) std::cout << ','; std::cout << json(result[i]); }
        std::cout << ']'; return 0;
    }
    if (command == "stats") {
        const auto tableStats = byId.stats();
        int active = 0, returned = 0, cancelled = 0;
        for (Booking* b : bookings) { if (b->trang_thai == "DANG_THUE") ++active; else if (b->trang_thai == "DA_TRA") ++returned; else if (b->trang_thai == "DA_HUY") ++cancelled; }
        std::cout << "{\"total\":" << bookings.size() << ",\"active\":" << active << ",\"returned\":" << returned << ",\"cancelled\":" << cancelled << ",\"hash_capacity\":" << tableStats.capacity << ",\"hash_load_factor\":" << tableStats.loadFactor << '}'; return 0;
    }
    std::cerr << "unknown command"; return 2;
}
