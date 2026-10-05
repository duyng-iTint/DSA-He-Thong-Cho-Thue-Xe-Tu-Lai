//TT

// BinarySearch.h
// Tu cai dat Binary Search (khong dung std::lower_bound / std::upper_bound).
// Ap dung tren mang RentalRecord DA duoc Merge Sort theo rentDate tang dan.
// Muc tieu MC2: tim nhanh vi tri bat dau / ket thuc cua 1 khoang ngay thue,
// do phuc tap O(log N), thay vi phai duyet tuyen tinh O(N).
#pragma once
#include <vector>
#include <string>
#include "../model/RentalRecord.h"

inline int lowerBoundByDate(const std::vector<RentalRecord>& arr, const std::string& target) {
    int lo = 0;
    int hi = static_cast<int>(arr.size()); // hi == size() nghia la "khong tim thay / vuot qua cuoi"

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid].rentDate < target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

inline int upperBoundByDate(const std::vector<RentalRecord>& arr, const std::string& target) {
    int lo = 0;
    int hi = static_cast<int>(arr.size());

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid].rentDate <= target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

inline int linearScanFirstDateGE(const std::vector<RentalRecord>& arr, const std::string& target) {
    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        if (arr[i].rentDate >= target) return i;
    }
    return static_cast<int>(arr.size());
}
