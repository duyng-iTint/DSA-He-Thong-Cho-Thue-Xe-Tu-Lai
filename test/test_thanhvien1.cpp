// test_thanhvien1.cpp
// ---------------------
// Bo test tu dong cho phan Thanh vien 1 (D5 - bat buoc phai co, khong
// chi test thu cong). Khong dung framework ngoai (vd GoogleTest) de
// giu don gian, tu viet macro CHECK/EXPECT_EQ nho gon.
//
// Bien dich & chay:
//     make test_thanhvien1
//     ./test_thanhvien1
//
// Bao gom:
// - Test cac thao tac co ban cua MyHashTable (insert/search/remove).
// - Test va cham (2 key khac nhau bam trung bucket van tra dung).
// - Test resize khong lam mat du lieu.
// - Test edge case: key khong ton tai, xoa key khong ton tai, bang rong.
// - Test round-trip persistence: ghi ra CSV roi doc lai phai khop du lieu.

#include <iostream>
#include <string>
#include <cstdio>
#include <map>

#include "Booking.h"
#include "MyHashTable.h"
#include "Persistence.h"

namespace {
int g_passed = 0;
int g_failed = 0;

void reportPass(const std::string& name) {
    ++g_passed;
    std::cout << "[ OK ] " << name << "\n";
}

void reportFail(const std::string& name, const std::string& detail) {
    ++g_failed;
    std::cout << "[FAIL] " << name << " -- " << detail << "\n";
}
}

#define TEST_CASE(name) void name()
#define CHECK(name, cond) \
    do { if (cond) reportPass(name); else reportFail(name, #cond); } while (0)

// ---------------------------------------------------------------------
// MyHashTable
// ---------------------------------------------------------------------

TEST_CASE(test_insert_and_search_basic) {
    MyHashTable<std::string> t;
    t.insert("A001", "gia tri 1");
    t.insert("A002", "gia tri 2");
    std::string v1, v2;
    bool f1 = t.search("A001", v1);
    bool f2 = t.search("A002", v2);
    CHECK("test_insert_and_search_basic",
          f1 && v1 == "gia tri 1" && f2 && v2 == "gia tri 2");
}

TEST_CASE(test_search_key_not_found) {
    MyHashTable<std::string> t;
    t.insert("A001", "x");
    std::string v;
    CHECK("test_search_key_not_found", !t.search("KHONG_TON_TAI", v));
}

TEST_CASE(test_search_on_empty_table) {
    MyHashTable<std::string> t;
    std::string v;
    CHECK("test_search_on_empty_table", !t.search("BAT_KY", v) && t.size() == 0);
}

TEST_CASE(test_update_existing_key) {
    MyHashTable<std::string> t;
    t.insert("A001", "v1");
    t.insert("A001", "v2"); // ghi de, khong tao ban ghi moi
    std::string v;
    t.search("A001", v);
    CHECK("test_update_existing_key", v == "v2" && t.size() == 1);
}

TEST_CASE(test_remove_existing_key) {
    MyHashTable<std::string> t;
    t.insert("A001", "v1");
    bool removed = t.remove("A001");
    std::string v;
    CHECK("test_remove_existing_key",
          removed && !t.search("A001", v) && t.size() == 0);
}

TEST_CASE(test_remove_nonexistent_key) {
    MyHashTable<std::string> t;
    CHECK("test_remove_nonexistent_key", !t.remove("KHONG_TON_TAI"));
}

TEST_CASE(test_collision_handling) {
    // Ep nhieu key vao cung 1 bucket (capacity nho) de kiem tra
    // Separate Chaining van tra dung tung key rieng biet.
    MyHashTable<int> t(1); // moi key deu roi vao bucket 0
    t.insert("A", 1);
    t.insert("B", 2);
    t.insert("C", 3);
    int va = -1, vb = -1, vc = -1;
    t.search("A", va); t.search("B", vb); t.search("C", vc);
    CHECK("test_collision_handling", va == 1 && vb == 2 && vc == 3);
}

TEST_CASE(test_resize_preserves_data) {
    MyHashTable<int> t(4); // capacity nho -> resize som
    std::map<std::string, int> expected;
    for (int i = 0; i < 200; ++i) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "KEY_%04d", i);
        t.insert(buf, i);
        expected[buf] = i;
    }
    bool allMatch = true;
    for (const auto& kv : expected) {
        int v;
        if (!t.search(kv.first, v) || v != kv.second) { allMatch = false; break; }
    }
    CHECK("test_resize_preserves_data", allMatch && t.size() == 200);
}

TEST_CASE(test_all_items_returns_everything) {
    MyHashTable<int> t;
    t.insert("A", 1);
    t.insert("B", 2);
    auto items = t.allItems();
    bool hasA = false, hasB = false;
    for (auto& kv : items) {
        if (kv.first == "A" && kv.second == 1) hasA = true;
        if (kv.first == "B" && kv.second == 2) hasB = true;
    }
    CHECK("test_all_items_returns_everything", hasA && hasB && items.size() == 2);
}

// ---------------------------------------------------------------------
// Persistence round-trip
// ---------------------------------------------------------------------

TEST_CASE(test_save_then_load_matches) {
    const std::string csvPath = "_test_round_trip.csv";

    Booking b;
    b.booking_id = "RENT_HCM_00001";
    b.bien_so = "51H-123.45";
    b.ten_khach = "Nguyen Van A";
    b.hang_xe = "Hyundai";
    b.dong_xe = "Accent";
    b.ngay_bat_dau = "2026-08-01";
    b.ngay_ket_thuc = "2026-08-05";
    b.trang_thai = "DANG_THUE";

    PersistenceContext ctx1;
    MyHashTable<Booking*> tableById1;
    Booking* ptr = ctx1.addBooking(b);
    tableById1.insert(ptr->booking_id, ptr);

    saveFromHashTable(csvPath, tableById1);

    PersistenceContext ctx2;
    MyHashTable<Booking*> tableById2, tableByPlate2;
    int count = loadIntoHashTable(csvPath, ctx2, tableById2, tableByPlate2);

    Booking* result = nullptr;
    bool found = tableById2.search(b.booking_id, result);

    CHECK("test_save_then_load_matches",
          count == 1 && found &&
          result->bien_so == b.bien_so &&
          result->ten_khach == b.ten_khach);

    std::remove(csvPath.c_str());
}

TEST_CASE(test_load_from_nonexistent_file_returns_zero) {
    PersistenceContext ctx;
    MyHashTable<Booking*> tableById, tableByPlate;
    int count = loadIntoHashTable("_khong_ton_tai.csv", ctx, tableById, tableByPlate);
    CHECK("test_load_from_nonexistent_file_returns_zero", count == 0);
}

int main() {
    test_insert_and_search_basic();
    test_search_key_not_found();
    test_search_on_empty_table();
    test_update_existing_key();
    test_remove_existing_key();
    test_remove_nonexistent_key();
    test_collision_handling();
    test_resize_preserves_data();
    test_all_items_returns_everything();
    test_save_then_load_matches();
    test_load_from_nonexistent_file_returns_zero();

    std::cout << "\n" << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed == 0 ? 0 : 1;
}
