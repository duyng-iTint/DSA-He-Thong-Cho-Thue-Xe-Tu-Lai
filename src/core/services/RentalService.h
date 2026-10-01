// RentalService.h
// Trai tim cua module MC2:
//   MC2 - Truy van xe theo Khoang thoi gian thue / Top xe duoc thue nhieu nhat
//         (Range Query + Extremes) - cau truc: Sorted Array + Binary Search,
//         xay dung bang Merge Sort.
//
// Khong su dung std::sort, std::map, std::lower_bound... cho phan xu ly du lieu
// chinh (dung MergeSort.h / BinarySearch.h tu cai dat), dung theo yeu cau do an.
#pragma once
#include <vector>
#include <string>
#include "../model/RentalRecord.h"

// Thong ke so lan thue theo tung xe (dung cho yeu cau "Top xe hot")
struct CarStat {
    std::string carPlate;
    std::string carBrand;
    std::string carModel;
    int rentCount;
};

class RentalService {
public:
    // Sap xep vector RentalRecord theo rentDate tang dan (Merge Sort).
    // Day la buoc "bulk load" bat buoc truoc khi Range Query / Binary Search hoat dong dung.
    static void sortByRentDate(std::vector<RentalRecord>& records);

    // MC2 (a): Loc toan bo don thue co rentDate trong [fromDate, toDate] (dinh dang YYYY-MM-DD).
    // Yeu cau: 'records' PHAI da duoc sortByRentDate() truoc do.
    // Do phuc tap: O(log N) de tim bien + O(k) de lay k ket qua trong khoang => O(log N + k).
    static std::vector<RentalRecord> queryByDateRange(
        const std::vector<RentalRecord>& sortedByDate,
        const std::string& fromDate,
        const std::string& toDate);

    // Xay dung bang thong ke so lan thue cho tung xe (gom nhom theo carPlate).
    // Ky thuat: Merge Sort theo carPlate (O(N log N)) roi quet tuyen tinh de dem nhom (O(N))
    // => khong dung hash table (do la pham vi cua Thanh vien 1 / MC1).
    static std::vector<CarStat> buildCarStats(const std::vector<RentalRecord>& records);

    // MC2 (b): Tra ve Top-K xe duoc thue nhieu nhat, sap xep giam dan theo so lan thue.
    // Ky thuat: Merge Sort giam dan tren vector CarStat (O(M log M), M = so xe khac nhau).
    static std::vector<CarStat> topRentedCars(std::vector<CarStat> stats, int topK);

    // Benchmark: so sanh thoi gian truy van Range Query bang Binary Search
    // so voi Linear Scan tren cung 1 tap du lieu (dung de chung minh O(log N) vs O(N)).
    // In ket qua ra console, tra ve (thoiGianBinarySearch_ms, thoiGianLinearScan_ms).
    static void benchmarkRangeQuery(
        const std::vector<RentalRecord>& sortedByDate,
        const std::string& fromDate,
        const std::string& toDate);
};
