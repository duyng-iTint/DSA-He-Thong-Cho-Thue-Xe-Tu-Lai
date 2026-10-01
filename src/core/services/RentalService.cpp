// RentalService.cpp
#include "RentalService.h"
#include "../algorithms/MergeSort.h"
#include "../algorithms/BinarySearch.h"
#include <chrono>
#include <iostream>
#include <iomanip>

void RentalService::sortByRentDate(std::vector<RentalRecord>& records) {
    // Comparator: sap xep theo rentDate tang dan; neu trung ngay thi sap theo bookingId
    // de ket qua on dinh, de kiem tra lai (deterministic).
    mergeSort(records, [](const RentalRecord& a, const RentalRecord& b) {
        if (a.rentDate != b.rentDate) return a.rentDate < b.rentDate;
        return a.bookingId < b.bookingId;
    });
}

std::vector<RentalRecord> RentalService::queryByDateRange(
    const std::vector<RentalRecord>& sortedByDate,
    const std::string& fromDate,
    const std::string& toDate) {

    std::vector<RentalRecord> result;
    if (sortedByDate.empty()) return result;

    int startIdx = lowerBoundByDate(sortedByDate, fromDate); // O(log N)
    int endIdx   = upperBoundByDate(sortedByDate, toDate);   // O(log N), tro toi vi tri SAU phan tu cuoi hop le

    result.reserve(endIdx > startIdx ? (endIdx - startIdx) : 0);
    for (int i = startIdx; i < endIdx; ++i) {
        result.push_back(sortedByDate[i]);
    }
    return result;
}

std::vector<CarStat> RentalService::buildCarStats(const std::vector<RentalRecord>& records) {
    // Buoc 1: copy va Merge Sort theo carPlate de cac ban ghi cung 1 xe dung ke nhau
    std::vector<RentalRecord> byPlate = records;
    mergeSort(byPlate, [](const RentalRecord& a, const RentalRecord& b) {
        return a.carPlate < b.carPlate;
    });

    // Buoc 2: quet tuyen tinh 1 lan, gom nhom cac ban ghi lien tiep co cung carPlate
    std::vector<CarStat> stats;
    for (size_t i = 0; i < byPlate.size(); ) {
        size_t j = i;
        int count = 0;
        while (j < byPlate.size() && byPlate[j].carPlate == byPlate[i].carPlate) {
            ++count;
            ++j;
        }
        CarStat s;
        s.carPlate = byPlate[i].carPlate;
        s.carBrand = byPlate[i].carBrand;
        s.carModel = byPlate[i].carModel;
        s.rentCount = count;
        stats.push_back(s);
        i = j;
    }
    return stats;
}

std::vector<CarStat> RentalService::topRentedCars(std::vector<CarStat> stats, int topK) {
    // Merge Sort giam dan theo rentCount
    mergeSort(stats, [](const CarStat& a, const CarStat& b) {
        return a.rentCount > b.rentCount;
    });

    if (topK < 0) topK = 0;
    if (topK > static_cast<int>(stats.size())) topK = static_cast<int>(stats.size());

    return std::vector<CarStat>(stats.begin(), stats.begin() + topK);
}

void RentalService::benchmarkRangeQuery(
    const std::vector<RentalRecord>& sortedByDate,
    const std::string& fromDate,
    const std::string& toDate) {

    using Clock = std::chrono::high_resolution_clock;

    // Mot lan tim don le co the chi mat vai chuc nano-giay (qua nho, khong on dinh
    // de do dac). De co so lieu dang tin cay, lap lai truy van nhieu lan va lay TONG
    // thoi gian / trung binh - ky thuat pho bien khi benchmark cac thao tac O(log N).
    const int REPEAT = 20000;
    volatile long long sinkBinary = 0; // 'volatile' de trinh bien dich toi uu loai bo vong lap
    volatile long long sinkLinear = 0;

    // --- Binary Search ---
    auto t1 = Clock::now();
    for (int r = 0; r < REPEAT; ++r) {
        sinkBinary += lowerBoundByDate(sortedByDate, fromDate);
    }
    auto t2 = Clock::now();
    double binaryTotalMs = std::chrono::duration<double, std::milli>(t2 - t1).count();

    // --- Linear Scan (de doi chieu) ---
    auto t3 = Clock::now();
    for (int r = 0; r < REPEAT; ++r) {
        sinkLinear += linearScanFirstDateGE(sortedByDate, fromDate);
    }
    auto t4 = Clock::now();
    double linearTotalMs = std::chrono::duration<double, std::milli>(t4 - t3).count();

    double binaryAvgUs = (binaryTotalMs * 1000.0) / REPEAT;
    double linearAvgUs = (linearTotalMs * 1000.0) / REPEAT;

    std::cout << "\n--- BENCHMARK: Tim vi tri bat dau khoang [" << fromDate << ", " << toDate << "] ---\n";
    std::cout << "  So luong ban ghi (N)        : " << sortedByDate.size() << "\n";
    std::cout << "  So lan lap de lay trung binh: " << REPEAT << "\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Binary Search O(log N)  - trung binh/lan: " << binaryAvgUs << " micro-giay"
              << "  (tong: " << binaryTotalMs << " ms)\n";
    std::cout << "  Linear Scan   O(N)      - trung binh/lan: " << linearAvgUs << " micro-giay"
              << "  (tong: " << linearTotalMs << " ms)\n";
    if (binaryAvgUs > 0.0) {
        std::cout << "  => Binary Search nhanh hon khoang " << (linearAvgUs / binaryAvgUs)
                  << " lan so voi Linear Scan (voi N = " << sortedByDate.size() << ").\n";
    }
}
