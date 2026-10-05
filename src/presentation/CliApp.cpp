#include "CliApp.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../core/services/DataGenerator.h"

namespace {

    using Clock = std::chrono::steady_clock;

    const char* RED = "\033[31m";
    const char* GREEN = "\033[32m";
    const char* YELLOW = "\033[33m";
    const char* RESET = "\033[0m";

    // ---------------- nhap lieu ----------------
    std::string trimStr(const std::string& s) {
        std::size_t a = 0, b = s.size();
        while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) ++a;
        while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r' || s[b - 1] == '\n')) --b;
        return s.substr(a, b - a);
    }

    bool readLine(const std::string& label, std::string& out) {
        std::cout << label;
        if (!std::getline(std::cin, out)) return false;
        out = trimStr(out);
        return true;
    }

    std::string ask(const std::string& label) {
        std::string s;
        readLine(label, s);
        return s;
    }

    int parseInt(const std::string& s, int fallback) {
        try { return s.empty() ? fallback : std::stoi(s); }
        catch (...) { return fallback; }
    }

    double parseDouble(const std::string& s, double fallback) {
        try { return s.empty() ? fallback : std::stod(s); }
        catch (...) { return fallback; }
    }

    // ---------------- hien thi ----------------
    void printLine(char c, int n) { std::cout << std::string(n, c) << "\n"; }

    void printBooking(const Booking& b) {
        std::cout << "  Booking_ID  : " << b.booking_id << "\n"
            << "  Bien so     : " << b.bien_so << "\n"
            << "  Khach hang  : " << b.ten_khach << " (hang thanh vien " << b.hang_thanh_vien << ")\n"
            << "  Xe          : " << b.hang_xe << " " << b.dong_xe << "\n"
            << "  Thoi gian   : " << b.ngay_bat_dau << " -> " << b.ngay_ket_thuc << "\n"
            << "  Trang thai  : " << b.trang_thai << "\n"
            << "  Gia         : " << std::fixed << std::setprecision(0) << b.gia_tien << " VND\n";
        std::cout.unsetf(std::ios::floatfield);
    }

    void printRecords(const std::vector<RentalRecord>& records, std::size_t maxRows = 30) {
        if (records.empty()) {
            std::cout << "(Khong co ket qua nao phu hop.)\n";
            return;
        }
        printLine('-', 112);
        std::cout << std::left << std::setw(18) << "Booking ID" << std::setw(13) << "Bien so"
            << std::setw(12) << "Hang xe" << std::setw(12) << "Dong xe"
            << std::setw(12) << "Ngay thue" << std::setw(12) << "Ngay tra"
            << std::setw(13) << "Gia (VND)" << "Khach hang\n";
        printLine('-', 112);
        std::size_t shown = 0;
        for (const auto& r : records) {
            std::cout << std::left << std::setw(18) << r.bookingId << std::setw(13) << r.carPlate
                << std::setw(12) << r.carBrand << std::setw(12) << r.carModel
                << std::setw(12) << r.rentDate << std::setw(12) << r.returnDate
                << std::setw(13) << std::fixed << std::setprecision(0) << r.price
                << r.customerName << "\n";
            if (++shown >= maxRows) {
                std::cout << "... (con " << (records.size() - shown) << " dong nua, an bot cho de doc)\n";
                break;
            }
        }
        std::cout.unsetf(std::ios::floatfield);
        printLine('-', 112);
        std::cout << "Tong so ket qua: " << records.size() << " don thue.\n";
    }

    void printDisputeHeader() {
        printLine('-', 84);
        std::cout << std::left << std::setw(18) << "Booking_ID" << std::setw(14) << "Bien so"
            << std::setw(16) << "Khach" << std::setw(10) << "Hang TV" << "Thoi diem dat (ms)\n";
        printLine('-', 84);
    }

    void printDisputeRow(const Booking& b) {
        std::cout << std::left << std::setw(18) << b.booking_id << std::setw(14) << b.bien_so
            << std::setw(16) << b.ten_khach << std::setw(10) << b.hang_thanh_vien
            << b.thoi_diem_dat << "\n";
    }

    double elapsedUs(Clock::time_point a, Clock::time_point b) {
        return std::chrono::duration<double, std::micro>(b - a).count();
    }

    // MC1: bang bam vs duyet tuyen tinh tren chinh du lieu dang nap.
    void benchmarkMC1(const UnifiedSystem& sys) {
        const auto& recs = sys.records();
        if (recs.empty()) { std::cout << "Chua co du lieu de benchmark.\n"; return; }

        const std::size_t SAMPLES = 100;
        const std::size_t step = std::max<std::size_t>(1, recs.size() / SAMPLES);
        std::vector<std::string> ids;
        for (std::size_t i = 0; i < recs.size() && ids.size() < SAMPLES; i += step) ids.push_back(recs[i].bookingId);

        const int HASH_REPS = 200, LINEAR_REPS = 3;
        volatile long long sink = 0;

        auto t1 = Clock::now();
        for (int r = 0; r < HASH_REPS; ++r)
            for (const auto& id : ids)
                if (sys.findById(id)) sink = sink + 1;
        auto t2 = Clock::now();

        auto t3 = Clock::now();
        for (int r = 0; r < LINEAR_REPS; ++r)
            for (const auto& id : ids)
                for (const auto& rec : recs)
                    if (rec.bookingId == id) { sink = sink + 1; break; }
        auto t4 = Clock::now();

        const double hashUs = elapsedUs(t1, t2) / (static_cast<double>(HASH_REPS) * ids.size());
        const double linUs = elapsedUs(t3, t4) / (static_cast<double>(LINEAR_REPS) * ids.size());

        std::cout << "\n--- BENCHMARK MC1: tra cuu theo Booking_ID (N = " << recs.size() << ") ---\n";
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "  MyHashTable  O(1)  - trung binh/lan: " << hashUs << " micro-giay\n";
        std::cout << "  Linear Scan  O(N)  - trung binh/lan: " << linUs << " micro-giay\n";
        if (hashUs > 0.0) std::cout << "  => Bang bam nhanh hon khoang " << (linUs / hashUs) << " lan.\n";
        std::cout.unsetf(std::ios::floatfield);
    }

    bool readChoice(std::string& choice) {
        if (!std::getline(std::cin, choice)) return false;
        choice = trimStr(choice);
        return true;
    }

} // namespace

// =====================================================================
// Du lieu
// =====================================================================

void CliApp::ensureData() {
    namespace fs = std::filesystem;
    const std::string relative = "data/donthue_xe.csv";
    std::error_code ec;

    // Tim data/donthue_xe.csv o thu muc hien tai hoac cac thu muc cha (VS chay exe trong out/build/...).
    std::string prefix;
    csvPath.clear();
    for (int up = 0; up <= 5; ++up) {
        const std::string candidate = prefix + relative;
        if (fs::exists(candidate, ec)) { csvPath = candidate; break; }
        prefix += "../";
    }

    if (csvPath.empty()) {
        csvPath = relative;
        fs::create_directories(fs::path(csvPath).parent_path(), ec);
        std::cout << "Chua co file CSV, tu sinh 10.000 ban ghi gia lap...\n";
        generateCsv(csvPath, 10000);
    }

    const int n = sys.loadFromCsv(csvPath);
    std::cout << "Da nap " << n << " don thue tu: " << fs::absolute(csvPath, ec).string() << "\n";
    const auto st = sys.hashStats();
    std::cout << "Bang bam: capacity=" << st.capacity << ", load_factor=" << st.loadFactor
        << ", max_chain=" << st.maxChainLength << " | Trie: " << sys.catalogSize() << " dong xe\n";
}

void CliApp::saveData() {
    const int saved = sys.saveToCsv(csvPath);
    std::cout << "-> Da ghi " << saved << " don thue ra " << csvPath << "\n";
}

// =====================================================================
// Menu chinh
// =====================================================================

void CliApp::showMainMenu() {
    std::cout << "\n=======================================\n";
    std::cout << "       HE THONG CHO THUE XE TU LAI\n";
    std::cout << "=======================================\n";
    std::cout << "1. MC1 - Tra cuu don thue / xe\n";
    std::cout << "2. MC2 - Truy van theo thoi gian / Top xe\n";
    std::cout << "3. RF1 - Goi y ten hang / dong xe\n";
    std::cout << "4. RF2 - Xu ly tranh chap xe\n";
    std::cout << "5. RF3 - Tao / Sua / Xoa / Undo don thue\n";
    std::cout << "6. Luu du lieu ra CSV\n";
    std::cout << "0. Thoat\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Chon: ";
}

void CliApp::run() {
    ensureData();

    std::string choice;
    while (true) {
        showMainMenu();
        if (!readChoice(choice)) break;

        if (choice == "1") menuMC1();
        else if (choice == "2") menuMC2();
        else if (choice == "3") menuRF1();
        else if (choice == "4") menuRF2();
        else if (choice == "5") menuRF3();
        else if (choice == "6") saveData();
        else if (choice == "0") {
            if (sys.dirty()) {
                const std::string a = ask("Co thay doi chua luu. Luu ra CSV truoc khi thoat? (y/n): ");
                if (a == "y" || a == "Y") saveData();
            }
            std::cout << "Tam biet!\n";
            break;
        }
        else {
            std::cout << "-> Lua chon khong hop le.\n";
        }
    }
}

// =====================================================================
// MC1 - Bang bam
// =====================================================================

void CliApp::menuMC1() {
    std::string choice;
    while (true) {
        std::cout << "\n--- MC1: TRA CUU DON THUE / XE ---\n";
        std::cout << "(Bang bam, " << sys.size() << " don thue)\n\n";
        std::cout << "1. Tra theo Booking_ID\n";
        std::cout << "2. Tra theo bien so\n";
        std::cout << "3. Thong ke bang bam\n";
        std::cout << "4. Benchmark: Bang bam vs Linear Scan\n";
        std::cout << "0. Quay lai\n";
        std::cout << "Chon: ";
        if (!readChoice(choice) || choice == "0") return;

        if (choice == "1" || choice == "2") {
            const bool byId = (choice == "1");
            const std::string key = ask(byId ? "Nhap Booking_ID (VD RENT_HCM_000123): " : "Nhap bien so (VD 51F-123.45): ");
            const auto t1 = Clock::now();
            const Booking* b = byId ? sys.findById(key) : sys.findByPlate(key);
            const auto t2 = Clock::now();
            if (b) {
                std::cout << "-> Tim thay (" << std::fixed << std::setprecision(2) << elapsedUs(t1, t2) << " micro-giay):\n";
                std::cout.unsetf(std::ios::floatfield);
                printBooking(*b);
            }
            else {
                std::cout << "-> Khong tim thay.\n";
            }
        }
        else if (choice == "3") {
            const auto st = sys.hashStats();
            std::cout << "capacity=" << st.capacity << ", size=" << st.size
                << ", load_factor=" << st.loadFactor
                << ", max_chain_length=" << st.maxChainLength << "\n";
        }
        else if (choice == "4") {
            benchmarkMC1(sys);
        }
        else {
            std::cout << "-> Lua chon khong hop le.\n";
        }
    }
}

// =====================================================================
// MC2 - Sorted Array + Binary Search + Merge Sort
// =====================================================================

void CliApp::menuMC2() {
    std::string choice;
    while (true) {
        std::cout << "\n--- MC2: TRUY VAN THEO KHOANG THOI GIAN / TOP XE ---\n";
        std::cout << "1. Tra cuu don thue theo khoang ngay (Range Query)\n";
        std::cout << "2. Top-N xe duoc thue nhieu nhat\n";
        std::cout << "3. Benchmark: Binary Search vs Linear Scan\n";
        std::cout << "4. Xem 30 don dau (da sap theo ngay)\n";
        std::cout << "0. Quay lai\n";
        std::cout << "Chon: ";
        if (!readChoice(choice) || choice == "0") return;

        if (choice == "1" || choice == "3") {
            const std::string from = UnifiedSystem::normalizeDate(ask("Tu ngay (YYYY-MM-DD hoac DD/MM/YYYY): "));
            const std::string to = UnifiedSystem::normalizeDate(ask("Den ngay (YYYY-MM-DD hoac DD/MM/YYYY): "));
            if (from.empty() || to.empty()) { std::cout << "-> Ngay khong hop le.\n"; continue; }
            if (choice == "1") printRecords(sys.queryRange(from, to));
            else sys.benchmarkRange(from, to);
        }
        else if (choice == "2") {
            const int k = parseInt(ask("Nhap so luong Top xe muon xem (VD 5, 10): "), 5);
            const auto top = sys.topCars(k);
            printLine('-', 66);
            std::cout << std::left << std::setw(6) << "Hang" << std::setw(14) << "Bien so"
                << std::setw(14) << "Hang xe" << std::setw(16) << "Dong xe" << "Luot thue\n";
            printLine('-', 66);
            int rank = 1;
            for (const auto& s : top)
                std::cout << std::left << std::setw(6) << rank++ << std::setw(14) << s.carPlate
                << std::setw(14) << s.carBrand << std::setw(16) << s.carModel << s.rentCount << "\n";
            printLine('-', 66);
        }
        else if (choice == "4") {
            printRecords(sys.records(), 30);
        }
        else {
            std::cout << "-> Lua chon khong hop le.\n";
        }
    }
}

// =====================================================================
// RF1 - Trie (Autocomplete)
// =====================================================================

void CliApp::menuRF1() {
    std::cout << "\n--- RF1: GOI Y TEN HANG / DONG XE (Trie, " << sys.catalogSize() << " dong xe) ---\n";
    std::cout << "Go tien to theo Hang (Toyota, Hyu...) hoac Dong xe (Innova, i10...). Enter de quay lai.\n";

    while (true) {
        std::string keyword;
        if (!readLine("\nNhap tu khoa: ", keyword) || keyword.empty()) return;

        const auto matches = sys.suggest(keyword);
        if (matches.empty()) {
            std::cout << "   [X] Khong tim thay xe nao phu hop voi \"" << keyword << "\".\n";
            continue;
        }

        const std::size_t shown = std::min<std::size_t>(5, matches.size());
        std::cout << "   [OK] Goi y (" << shown << "/" << matches.size() << " ket qua):\n";
        for (std::size_t i = 0; i < shown; ++i) std::cout << "      " << (i + 1) << ". " << matches[i] << "\n";

        if (matches.size() > shown) {
            const std::string more = ask("   Con " + std::to_string(matches.size() - shown) + " goi y nua. Xem toan bo? (y/n): ");
            if (more == "y" || more == "Y")
                for (std::size_t i = shown; i < matches.size(); ++i) std::cout << "      " << (i + 1) << ". " << matches[i] << "\n";
        }
    }
}

// =====================================================================
// RF2 - Max-Heap (uu tien khi tranh chap xe)
// =====================================================================

void CliApp::menuRF2() {
    std::string choice;
    while (true) {
        std::cout << "\n--- RF2: XU LY TRANH CHAP XE (Max-Heap) ---\n";
        std::cout << "Uu tien: hang thanh vien cao hon thang; cung hang thi dat som hon thang.\n";
        std::cout << "Yeu cau dang cho: " << sys.disputeCount() << "\n\n";
        std::cout << "1. Gui yeu cau thue moi vao hang doi\n";
        std::cout << "2. Xem yeu cau uu tien cao nhat\n";
        std::cout << "3. Xu ly yeu cau tiep theo (tao don that)\n";
        std::cout << "4. Huy yeu cau theo Booking_ID\n";
        std::cout << "5. Danh sach yeu cau dang cho\n";
        std::cout << "6. Nap 5 yeu cau mau tranh chap cung 1 xe (de demo)\n";
        std::cout << "0. Quay lai\n";
        std::cout << "Chon: ";
        if (!readChoice(choice) || choice == "0") return;

        if (choice == "1") {
            Booking b;
            b.booking_id = ask("Booking_ID moi: ");
            b.ten_khach = ask("Ten khach hang: ");
            b.bien_so = ask("Bien so xe dang tranh chap: ");
            const Booking* existing = sys.findByPlate(b.bien_so);
            if (existing) {
                b.hang_xe = existing->hang_xe;
                b.dong_xe = existing->dong_xe;
                std::cout << "-> Xe: " << b.hang_xe << " " << b.dong_xe << "\n";
            }
            else {
                b.hang_xe = ask("Hang xe: ");
                b.dong_xe = ask("Dong xe: ");
            }
            b.ngay_bat_dau = ask("Ngay bat dau (YYYY-MM-DD hoac DD/MM/YYYY): ");
            b.ngay_ket_thuc = ask("Ngay ket thuc: ");
            b.hang_thanh_vien = parseInt(ask("Hang thanh vien (0-3, Enter = 0): "), 0);
            const std::string ts = ask("Thoi diem dat (ms, Enter = bay gio): ");
            b.thoi_diem_dat = ts.empty() ? 0 : static_cast<long long>(parseDouble(ts, 0));

            std::string err;
            if (sys.submitDispute(b, err)) std::cout << "-> Da dua vao hang doi uu tien.\n";
            else std::cout << "-> That bai: " << err << "\n";

        }
        else if (choice == "2") {
            RentalRequest top;
            if (!sys.peekDispute(top)) { std::cout << "-> Hang doi dang rong.\n"; continue; }
            std::cout << RED << "XU LY GAP: " << top.bookingId << " | xe " << top.carId
                << " | khach " << top.customerId << " | hang TV " << top.membershipTier
                << " | dat luc " << top.bookingTimestamp << RESET << "\n";

        }
        else if (choice == "3") {
            Booking winner;
            std::vector<std::string> rejected;
            std::string err;
            if (!sys.processNextDispute(winner, rejected, err)) { std::cout << "-> " << err << "\n"; continue; }
            std::cout << GREEN << "-> THANG: " << winner.booking_id << " (khach " << winner.ten_khach
                << ", hang TV " << winner.hang_thanh_vien << ") duoc thue xe " << winner.bien_so << RESET << "\n";
            printBooking(winner);
            for (const auto& id : rejected) std::cout << YELLOW << "-> Tu choi: " << id << " (cung xe)" << RESET << "\n";
            std::cout << "(Don da vao Bang bam + mang sap theo ngay, co the Undo o RF3.)\n";

        }
        else if (choice == "4") {
            const std::string id = ask("Booking_ID can huy: ");
            std::cout << (sys.cancelDispute(id) ? "-> Da huy yeu cau.\n" : "-> Khong tim thay yeu cau.\n");

        }
        else if (choice == "5") {
            const auto pending = sys.pendingDisputes();
            if (pending.empty()) { std::cout << "-> Hang doi dang rong.\n"; continue; }
            printDisputeHeader();
            for (const auto& b : pending) printDisputeRow(b);
            std::cout << "(Liet ke theo thu tu bam; dung muc 2 de xem yeu cau uu tien cao nhat.)\n";

        }
        else if (choice == "6") {
            struct Demo { const char* id; int tier; long long ts; };
            const Demo demos[] = {
                {"DEMO_A", 1, 1790000001000LL}, {"DEMO_B", 3, 1790000002000LL},
                {"DEMO_C", 3, 1790000001500LL}, {"DEMO_D", 0, 1790000000100LL},
                {"DEMO_E", 2, 1790000000500LL} };
            int added = 0;
            for (const auto& d : demos) {
                Booking b;
                b.booking_id = d.id;
                b.ten_khach = std::string("Khach_") + d.id;
                b.bien_so = "99Z-000.01";
                b.hang_xe = "Toyota";
                b.dong_xe = "Vios";
                b.ngay_bat_dau = "2026-12-20";
                b.ngay_ket_thuc = "2026-12-22";
                b.hang_thanh_vien = d.tier;
                b.thoi_diem_dat = d.ts;
                std::string err;
                if (sys.submitDispute(b, err)) ++added;
                else std::cout << "-> " << d.id << ": " << err << "\n";
            }
            std::cout << "-> Da nap " << added << " yeu cau cung xe 99Z-000.01. Thu muc 2 roi muc 3: DEMO_C phai thang (hang 3, dat som hon DEMO_B).\n";
        }
        else {
            std::cout << "-> Lua chon khong hop le.\n";
        }
    }
}

// =====================================================================
// RF3 - Undo (Mystack / UnoManager cua ban RF3, ap len he tong)
// =====================================================================

void CliApp::menuRF3() {
    std::string choice;
    while (true) {
        std::cout << "\n--- RF3: TAO / SUA / XOA / UNDO DON THUE ---\n";
        std::cout << "So thao tac co the Undo: " << sys.undoCount() << "\n\n";
        std::cout << "1. Tao don thue moi\n";
        std::cout << "2. Cap nhat don thue (ten khach / bien so / ngay)\n";
        std::cout << "3. Xoa don thue\n";
        std::cout << "4. Undo thao tac gan nhat\n";
        std::cout << "5. Xem thao tac gan nhat co the Undo\n";
        std::cout << "0. Quay lai\n";
        std::cout << "Chon: ";
        if (!readChoice(choice) || choice == "0") return;

        std::string err;
        if (choice == "1") {
            Booking b;
            b.booking_id = ask("Booking_ID moi: ");
            b.ten_khach = ask("Ten khach hang: ");
            b.bien_so = ask("Bien so: ");
            const Booking* existing = sys.findByPlate(b.bien_so);
            if (existing) {
                b.hang_xe = existing->hang_xe;
                b.dong_xe = existing->dong_xe;
                std::cout << "-> Xe da co trong he thong: " << b.hang_xe << " " << b.dong_xe << "\n";
            }
            else {
                b.hang_xe = ask("Hang xe: ");
                b.dong_xe = ask("Dong xe: ");
            }
            b.ngay_bat_dau = ask("Ngay bat dau (YYYY-MM-DD hoac DD/MM/YYYY): ");
            b.ngay_ket_thuc = ask("Ngay ket thuc: ");
            b.gia_tien = parseDouble(ask("Gia thue (VND, Enter = 0): "), 0.0);
            b.hang_thanh_vien = parseInt(ask("Hang thanh vien (0-3, Enter = 0): "), 0);

            if (sys.createBooking(b, err)) std::cout << "-> Them don thue thanh cong!\n";
            else std::cout << "-> Them that bai: " << err << "\n";

        }
        else if (choice == "2") {
            const std::string id = ask("Booking_ID can cap nhat: ");
            const Booking* cur = sys.findById(id);
            if (!cur) { std::cout << "-> Khong tim thay Booking_ID.\n"; continue; }
            std::cout << "(Enter de giu nguyen gia tri hien tai)\n";
            const std::string name = ask("Ten khach moi [" + cur->ten_khach + "]: ");
            const std::string plate = ask("Bien so moi [" + cur->bien_so + "]: ");
            const std::string start = ask("Ngay bat dau moi [" + cur->ngay_bat_dau + "]: ");
            const std::string end = ask("Ngay ket thuc moi [" + cur->ngay_ket_thuc + "]: ");
            if (sys.updateBooking(id, name, plate, start, end, err)) std::cout << "-> Cap nhat thanh cong!\n";
            else std::cout << "-> Cap nhat that bai: " << err << "\n";

        }
        else if (choice == "3") {
            const std::string id = ask("Booking_ID can xoa: ");
            if (sys.deleteBooking(id, err)) std::cout << "-> Xoa don thue thanh cong!\n";
            else std::cout << "-> " << err << "\n";

        }
        else if (choice == "4") {
            std::string msg;
            const bool ok = sys.undo(msg);
            std::cout << (ok ? GREEN : YELLOW) << "-> " << msg << RESET << "\n";

        }
        else if (choice == "5") {
            Action a;
            if (!sys.topUndo(a)) { std::cout << "-> Khong co lich su Undo.\n"; continue; }
            const char* name = a.type == ActionType::ADD ? "ADD"
                : a.type == ActionType::UPDATE ? "UPDATE" : "DELETE_ACTION";
            const std::string& id = a.type == ActionType::ADD ? a.newData.bookingID : a.oldData.bookingID;
            std::cout << "-> Thao tac gan nhat: " << name << " | Booking_ID: " << id << "\n";
        }
        else {
            std::cout << "-> Lua chon khong hop le.\n";
        }
    }
}
