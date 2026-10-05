// test_correctness.cpp
// Bo test don gian (khong dung framework ngoai) de tu kiem chung module MC2:
//   - Merge Sort sap xep dung thu tu
//   - Binary Search (lowerBound/upperBound) tra ve dung chi so
//   - Range Query tra ve dung tap ket qua so voi cach loc "brute-force" (doi chieu)
//   - Top xe hot tra ve dung thu tu giam dan
//
// Bien dich rieng file nay de chay test (xem huong dan trong README.md, muc "Kiem thu"):
//   g++ -std=c++17 -I ..\src demo\test_correctness.cpp ..\src\RentalService.cpp ..\src\CSVUtils.cpp -o test_mc2.exe
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "../src/core/model/RentalRecord.h"
#include "../src/core/algorithms/MergeSort.h"
#include "../src/core/algorithms/BinarySearch.h"
#include "../src/core/services/RentalService.h"

namespace {

int totalChecks = 0;
int failedChecks = 0;

void check(bool condition, const std::string& label) {
    ++totalChecks;
    if (!condition) {
        ++failedChecks;
        std::cout << "[THAT BAI] " << label << "\n";
    } else {
        std::cout << "[OK]       " << label << "\n";
    }
}

RentalRecord makeRecord(const std::string& id, const std::string& plate,
                         const std::string& brand, const std::string& model,
                         const std::string& rentDate) {
    RentalRecord r;
    r.bookingId = id;
    r.carPlate = plate;
    r.carBrand = brand;
    r.carModel = model;
    r.rentDate = rentDate;
    r.returnDate = rentDate;
    r.price = 1000000;
    r.customerName = "Test";
    r.memberRank = 0;
    return r;
}

} // namespace

int main() {
    std::cout << "===== BO TEST TU KIEM CHUNG MODULE MC2 =====\n\n";

    // ---------- Test 1: Merge Sort dung thu tu ----------
    std::vector<int> nums = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
    mergeSort(nums, [](int a, int b) { return a < b; });
    bool sortedAsc = true;
    for (size_t i = 1; i < nums.size(); ++i) {
        if (nums[i - 1] > nums[i]) { sortedAsc = false; break; }
    }
    check(sortedAsc, "Merge Sort sap xep tang dan dung (so nguyen)");

    // ---------- Test 2: Merge Sort + Binary Search tren RentalRecord ----------
    std::vector<RentalRecord> records = {
        makeRecord("B1", "P1", "Toyota", "Vios",   "2024-03-10"),
        makeRecord("B2", "P2", "Kia",    "Morning","2024-01-05"),
        makeRecord("B3", "P3", "Honda",  "City",   "2024-02-20"),
        makeRecord("B4", "P4", "Toyota", "Vios",   "2024-03-01"),
        makeRecord("B5", "P5", "Kia",    "Morning","2024-05-15"),
        makeRecord("B6", "P1", "Toyota", "Vios",   "2024-06-01"),
        makeRecord("B7", "P2", "Kia",    "Morning","2024-06-10"),
    };

    RentalService::sortByRentDate(records);
    bool dateSorted = true;
    for (size_t i = 1; i < records.size(); ++i) {
        if (records[i - 1].rentDate > records[i].rentDate) { dateSorted = false; break; }
    }
    check(dateSorted, "RentalService::sortByRentDate() sap xep dung theo rentDate");

    // ---------- Test 3: Range Query so voi brute-force ----------
    std::string from = "2024-02-01", to = "2024-05-31";
    auto rangeResult = RentalService::queryByDateRange(records, from, to);

    std::vector<std::string> bruteForceIds;
    for (const auto& r : records) {
        if (r.rentDate >= from && r.rentDate <= to) bruteForceIds.push_back(r.bookingId);
    }
    bool rangeMatches = (rangeResult.size() == bruteForceIds.size());
    if (rangeMatches) {
        for (size_t i = 0; i < rangeResult.size(); ++i) {
            if (rangeResult[i].bookingId != bruteForceIds[i]) { rangeMatches = false; break; }
        }
    }
    check(rangeMatches, "Range Query (Binary Search) khop 100% voi loc brute-force");

    // Khoang tinh ca hai moc, ke ca nhieu don cung ngay.
    std::vector<RentalRecord> boundaryRecords = {
        makeRecord("START_2", "P8", "Toyota", "Vios", "2024-02-01"),
        makeRecord("END", "P9", "Honda", "City", "2024-05-31"),
        makeRecord("START_1", "P7", "Kia", "Morning", "2024-02-01"),
        makeRecord("BEFORE", "P6", "Mazda", "3", "2024-01-31"),
        makeRecord("AFTER", "P10", "Ford", "Ranger", "2024-06-01"),
    };
    RentalService::sortByRentDate(boundaryRecords);
    const auto boundaryRange = RentalService::queryByDateRange(boundaryRecords, "2024-02-01", "2024-05-31");
    check(boundaryRange.size() == 3,
          "Range Query lay du ca don dung ngay bat dau va ket thuc");
    check(RentalService::queryByDateRange(boundaryRecords, "2024-06-01", "2024-02-01").empty(),
          "Range Query rong khi ngay bat dau sau ngay ket thuc");

    // ---------- Test 4: Bien tren/duoi cua khoang (edge case) ----------
    auto emptyResult = RentalService::queryByDateRange(records, "2030-01-01", "2030-12-31");
    check(emptyResult.empty(), "Range Query tra ve rong khi khong co du lieu trong khoang");

    auto allResult = RentalService::queryByDateRange(records, "2000-01-01", "2099-12-31");
    check(allResult.size() == records.size(), "Range Query tra ve toan bo khi khoang bao trum het");

    // ---------- Test 5: Top xe hot ----------
    auto stats = RentalService::buildCarStats(records);
    auto top = RentalService::topRentedCars(stats, 3);
    bool topDescending = true;
    for (size_t i = 1; i < top.size(); ++i) {
        if (top[i - 1].rentCount < top[i].rentCount) { topDescending = false; break; }
    }
    check(topDescending, "Top xe hot sap xep giam dan dung theo so luot thue");
    // P1 va P2 moi xe xuat hien 2 lan trong du lieu test -> phai dung dau bang (>=2)
    check(!top.empty() && top[0].rentCount >= 2, "Xe co nhieu luot thue nhat duoc xep hang 1");
    check(stats.size() == 5 && top.size() == 3 && top[0].rentCount == 2 && top[1].rentCount == 2,
          "Thong ke gom dung theo bien so va tra dung Top-K");
    check(RentalService::topRentedCars(stats, 0).empty(),
          "Top-K bang 0 tra ve danh sach rong");
    check(RentalService::topRentedCars(stats, -1).empty(),
          "Top-K am duoc xu ly nhu 0");
    check(RentalService::topRentedCars(stats, 99).size() == stats.size(),
          "Top-K lon hon so xe tra ve toan bo xe");
    check(RentalService::buildCarStats({}).empty() &&
          RentalService::topRentedCars({}, 5).empty(),
          "Thong ke va Top-K xu ly du lieu rong");

    std::cout << "\n===============================================\n";
    std::cout << "KET QUA: " << (totalChecks - failedChecks) << "/" << totalChecks << " test PASS\n";
    std::cout << "===============================================\n";

    return failedChecks == 0 ? 0 : 1;
}
