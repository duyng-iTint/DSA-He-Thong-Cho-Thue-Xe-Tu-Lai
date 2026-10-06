//TT

// MergeSort.h
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
