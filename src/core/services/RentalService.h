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

struct CarStat {
    std::string carPlate;
    std::string carBrand;
    std::string carModel;
    int rentCount;
};

class RentalService {
public:
    static void sortByRentDate(std::vector<RentalRecord>& records);

    static std::vector<RentalRecord> queryByDateRange(
        const std::vector<RentalRecord>& sortedByDate,
        const std::string& fromDate,
        const std::string& toDate);

    static std::vector<CarStat> buildCarStats(const std::vector<RentalRecord>& records);

    static std::vector<CarStat> topRentedCars(std::vector<CarStat> stats, int topK);

    static void benchmarkRangeQuery(
        const std::vector<RentalRecord>& sortedByDate,
        const std::string& fromDate,
        const std::string& toDate);
};
