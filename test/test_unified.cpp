// test_unified.cpp

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "../src/core/services/UnifiedSystem.h"
#include "../src/core/services/DataGenerator.h"

namespace {

    int g_total = 0, g_failed = 0;

    void check(bool cond, const std::string& label) {
        ++g_total;
        if (!cond) ++g_failed;
        std::cout << (cond ? "[ OK ] " : "[FAIL] ") << label << "\n";
    }

    Booking makeBooking(const std::string& id, const std::string& plate, const std::string& start,
        const std::string& end, int tier = 0, long long ts = 0) {
        Booking b;
        b.booking_id = id;
        b.ten_khach = "Khach " + id;
        b.bien_so = plate;
        b.hang_xe = "Toyota";
        b.dong_xe = "Vios";
        b.ngay_bat_dau = start;
        b.ngay_ket_thuc = end;
        b.hang_thanh_vien = tier;
        b.thoi_diem_dat = ts;
        b.gia_tien = 800000;
        return b;
    }

    // Mang MC2 phai luon: dung so luong, sap theo ngay tang dan.
    bool recordsConsistent(const UnifiedSystem& s) {
        const auto& r = s.records();
        if (r.size() != s.size()) return false;
        for (std::size_t i = 1; i < r.size(); ++i)
            if (r[i - 1].rentDate > r[i].rentDate) return false;
        return true;
    }

    bool inRange(const UnifiedSystem& s, const std::string& from, const std::string& to, const std::string& id) {
        for (const auto& r : s.queryRange(from, to)) if (r.bookingId == id) return true;
        return false;
    }

} // namespace

int main() {
    const std::string csv = "_test_unified.csv";
    generateCsv(csv, 2000);

    UnifiedSystem sys;
    check(sys.loadFromCsv(csv) == 2000, "Nap CSV 2000 don");
    check(recordsConsistent(sys), "Mang MC2 dung so luong va da sap theo ngay sau khi nap");
    check(sys.findById("RENT_HCM_000001") != nullptr, "MC1 tra cuu Booking_ID co san");
    check(!sys.suggest("hyu").empty() && sys.suggest("zzz").empty(), "RF1 goi y theo tien to");

    std::string err, msg;

    // ---------- RF3: THEM ----------
    check(sys.createBooking(makeBooking("T1", "99T-001.01", "25/12/2026", "27/12/2026"), err), "RF3 them don (ngay DD/MM/YYYY)");
    check(sys.findById("T1") && sys.findById("T1")->ngay_bat_dau == "2026-12-25", "Ngay duoc chuan hoa ve YYYY-MM-DD");
    check(sys.findByPlate("99T-001.01") != nullptr, "MC1 tra theo bien so thay don moi");
    check(inRange(sys, "2026-12-25", "2026-12-25", "T1"), "MC2 range query thay don moi");
    check(recordsConsistent(sys), "Mang MC2 van dung thu tu sau khi them");
    check(!sys.createBooking(makeBooking("T1", "99T-002.02", "2026-12-25", "2026-12-26"), err), "Tu choi trung Booking_ID");
    check(!sys.createBooking(makeBooking("T2", "99T-002.02", "2026-12-30", "2026-12-26"), err), "Tu choi ngay ket thuc < ngay bat dau");
    check(!sys.createBooking(makeBooking("T2", "99T-002.02", "31/02/2026", "2026-12-26"), err), "Tu choi ngay khong ton tai");

    // ---------- RF3: SUA ----------
    check(sys.updateBooking("T1", "", "99T-777.77", "2026-11-05", "2026-11-06", err), "RF3 sua bien so + ngay");
    check(sys.findByPlate("99T-001.01") == nullptr && sys.findByPlate("99T-777.77") != nullptr, "Bang bam bien so doi theo");
    check(!inRange(sys, "2026-12-25", "2026-12-25", "T1") && inRange(sys, "2026-11-05", "2026-11-05", "T1"), "Mang MC2 doi vi tri theo ngay moi");
    check(recordsConsistent(sys), "Mang MC2 dung thu tu sau khi sua");

    // ---------- RF3: XOA ----------
    const std::size_t before = sys.size();
    check(sys.deleteBooking("T1", err), "RF3 xoa don");
    check(sys.findById("T1") == nullptr && sys.findByPlate("99T-777.77") == nullptr, "MC1 khong con thay don da xoa");
    check(!inRange(sys, "2026-11-05", "2026-11-05", "T1") && sys.size() == before - 1, "MC2 khong con thay don da xoa");
    check(recordsConsistent(sys), "Mang MC2 dung sau khi xoa");

    // ---------- RF3: UNDO (LIFO) ----------
    check(sys.undoCount() == 3, "Lich su Undo co 3 thao tac");
    check(sys.undo(msg) && sys.findById("T1") != nullptr, "Undo XOA khoi phuc don");
    check(sys.findById("T1")->gia_tien == 800000 && sys.findById("T1")->hang_xe == "Toyota", "Undo XOA giu du truong (gia, hang xe), khong chi 5 truong");
    check(inRange(sys, "2026-11-05", "2026-11-05", "T1") && sys.findByPlate("99T-777.77") != nullptr, "Undo XOA dong bo lai MC1 + MC2");
    check(sys.undo(msg) && sys.findById("T1")->bien_so == "99T-001.01" && sys.findById("T1")->ngay_bat_dau == "2026-12-25", "Undo SUA tra lai gia tri cu");
    check(inRange(sys, "2026-12-25", "2026-12-25", "T1") && sys.findByPlate("99T-001.01") != nullptr, "Undo SUA dong bo lai MC1 + MC2");
    check(sys.undo(msg) && sys.findById("T1") == nullptr && sys.size() == 2000, "Undo THEM xoa don vua them");
    check(!sys.undo(msg), "Undo khi het lich su tra ve false");
    check(recordsConsistent(sys), "Mang MC2 dung sau chuoi Undo");

    // ---------- RF3: xoa -> them lai cung ma -> undo ve dung thu tu ----------
    check(sys.createBooking(makeBooking("S1", "99S-001.01", "2026-10-01", "2026-10-02"), err), "Them S1");
    check(sys.deleteBooking("S1", err), "Xoa S1");
    check(sys.createBooking(makeBooking("S1", "99S-002.02", "2026-10-05", "2026-10-06"), err), "Them lai S1 (ban moi)");
    check(sys.deleteBooking("S1", err), "Xoa S1 (ban moi)");
    check(sys.undo(msg) && sys.findById("S1")->bien_so == "99S-002.02", "Undo xoa khoi phuc DUNG ban moi");
    check(sys.undo(msg) && sys.findById("S1") == nullptr, "Undo them ban moi");
    check(sys.undo(msg) && sys.findById("S1") != nullptr && sys.findById("S1")->bien_so == "99S-001.01", "Undo xoa khoi phuc DUNG ban cu");
    check(sys.undo(msg) && sys.findById("S1") == nullptr, "Undo them ban cu");
    check(recordsConsistent(sys), "Mang MC2 dung sau chuoi thao tac trung ma");

    // ---------- RF2: tranh chap ----------
    check(sys.submitDispute(makeBooking("D_A", "99D-001.01", "2026-12-20", "2026-12-22", 1, 3000), err), "RF2 gui yeu cau A (hang 1)");
    check(sys.submitDispute(makeBooking("D_B", "99D-001.01", "2026-12-20", "2026-12-22", 3, 2000), err), "RF2 gui yeu cau B (hang 3, dat luc 2000)");
    check(sys.submitDispute(makeBooking("D_C", "99D-001.01", "2026-12-20", "2026-12-22", 3, 1000), err), "RF2 gui yeu cau C (hang 3, dat som nhat)");
    check(sys.submitDispute(makeBooking("D_X", "99D-009.09", "2026-12-21", "2026-12-23", 0, 500), err), "RF2 gui yeu cau X (xe khac)");
    check(!sys.submitDispute(makeBooking("D_A", "99D-001.01", "2026-12-20", "2026-12-22", 1, 1), err), "RF2 tu choi trung ma yeu cau");
    check(!sys.submitDispute(makeBooking("RENT_HCM_000001", "99D-001.01", "2026-12-20", "2026-12-22"), err), "RF2 tu choi ma da la don that");
    check(sys.disputeCount() == 4, "Hang doi co 4 yeu cau");

    RentalRequest top;
    check(sys.peekDispute(top) && top.bookingId == "D_C", "Uu tien cao nhat: hang 3 + dat som nhat = D_C");

    Booking winner;
    std::vector<std::string> rejected;
    check(sys.processNextDispute(winner, rejected, err) && winner.booking_id == "D_C", "Xu ly: D_C thang");
    check(rejected.size() == 2, "D_A va D_B (cung xe) bi tu choi");
    check(sys.findById("D_C") != nullptr && sys.findByPlate("99D-001.01") != nullptr, "Don thang co trong bang bam (MC1)");
    check(inRange(sys, "2026-12-20", "2026-12-20", "D_C"), "Don thang co trong mang sap theo ngay (MC2)");
    check(sys.disputeCount() == 1, "Con lai yeu cau X (xe khac)");
    check(sys.undo(msg) && sys.findById("D_C") == nullptr, "Undo don vua thang tranh chap");
    check(sys.cancelDispute("D_X") && sys.disputeCount() == 0, "Huy yeu cau X");
    check(!sys.processNextDispute(winner, rejected, err), "Hang doi rong -> khong xu ly duoc");
    check(recordsConsistent(sys), "Mang MC2 dung sau RF2");

    // ---------- Persistence ----------
    check(sys.createBooking(makeBooking("P1", "99P-001.01", "2026-09-09", "2026-09-10"), err), "Them P1 truoc khi luu");
    check(sys.dirty(), "Co co dirty sau khi thay doi");
    check(sys.saveToCsv(csv) == 2001, "Luu CSV 2001 don");
    UnifiedSystem sys2;
    check(sys2.loadFromCsv(csv) == 2001 && sys2.findById("P1") != nullptr && recordsConsistent(sys2), "Nap lai file da luu: day du, nhat quan");

    std::remove(csv.c_str());
    std::cout << "\nKET QUA: " << (g_total - g_failed) << "/" << g_total << " test PASS\n";
    return g_failed == 0 ? 0 : 1;
}
