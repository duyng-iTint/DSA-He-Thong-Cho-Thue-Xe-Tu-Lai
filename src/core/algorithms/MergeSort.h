//TT

// MergeSort.h
// Dung template + comparator de tai su dung cho nhieu loai du lieu:
//   - Sap xep RentalRecord theo rentDate  (phuc vu MC2 - Range Query)
//   - Sap xep RentalRecord theo carPlate  (buoc trung gian de gom nhom thong ke xe)
//   - Sap xep CarStat theo rentCount giam dan (phuc vu MC2 - Top xe hot)
//
// Do phuc tap: O(N log N) moi truong hop (best/worst/average) - on dinh (stable sort),
// phu hop de "bulk load" du lieu lich su thue xe mot lan dau vao Sorted Array.
#pragma once
#include <vector>

template <typename T, typename Compare>
void mergeInPlace(std::vector<T>& arr, int left, int mid, int right, Compare cmp) {
    std::vector<T> temp;
    temp.reserve(right - left + 1);

    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {
        if (cmp(arr[i], arr[j])) {
            temp.push_back(arr[i]);
            ++i;
        } else {
            temp.push_back(arr[j]);
            ++j;
        }
    }
    while (i <= mid) { temp.push_back(arr[i]); ++i; }
    while (j <= right) { temp.push_back(arr[j]); ++j; }

    for (int k = 0; k < static_cast<int>(temp.size()); ++k) {
        arr[left + k] = temp[k];
    }
}

template <typename T, typename Compare>
void mergeSortRange(std::vector<T>& arr, int left, int right, Compare cmp) {
    if (left >= right) return; // 0 hoac 1 phan tu - da sap xep

    int mid = left + (right - left) / 2;
    mergeSortRange(arr, left, mid, cmp);
    mergeSortRange(arr, mid + 1, right, cmp);
    mergeInPlace(arr, left, mid, right, cmp);
}

template <typename T, typename Compare>
void mergeSort(std::vector<T>& arr, Compare cmp) {
    if (arr.size() < 2) return;
    mergeSortRange(arr, 0, static_cast<int>(arr.size()) - 1, cmp);
}
