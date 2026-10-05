// benchmark_mc2.cpp
// -------------------
// Bang chung hieu nang cho MC2:
//
// So sanh truy van khoang thoi gian giua:
// (a) Sorted Array + Binary Search
//     -> Tim nhanh vi tri bat dau va ket thuc khoang
//     -> O(log N + k)
//
// (b) Linear Scan
//     -> Duyet toan bo du lieu
//     -> O(N)
//
// Quy mo:
// 1.000 -> 10.000 -> 100.000 ban ghi

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace chrono;


// cau truc du lieu

struct RentalRecord {
    int bookingId;
    int rentalDate;      // Dang YYYYMMDD
};


// tao du lieu


vector<RentalRecord> createData(int n) {

    vector<RentalRecord> data;
    data.reserve(n);

    for (int i = 0; i < n; i++) {

        RentalRecord record;

        record.bookingId = i;

        // Tao ngay tang dan
        // Vi du: 20240101, 20240102,...
        record.rentalDate = 20240101 + i;

        data.push_back(record);
    }

    return data;
}


// BINARY SEARCH
// Tim vi tri dau tien co date >= target

int lowerBoundDate(
    const vector<RentalRecord>& data,
    int target
) {

    int left = 0;
    int right = (int)data.size();

    while (left < right) {

        int mid = left + (right - left) / 2;

        if (data[mid].rentalDate < target) {
            left = mid + 1;
        }
        else {
            right = mid;
        }
    }

    return left;
}


// tim khoang bang binary search

int binaryRangeQuery(
    const vector<RentalRecord>& data,
    int startDate,
    int endDate
) {

    // Tim vi tri dau tien >= startDate
    int left = lowerBoundDate(data, startDate);

    // Tim vi tri dau tien > endDate
    int right = lowerBoundDate(data, endDate + 1);

    // So luong ban ghi trong khoang
    return right - left;
}


//liner scan

int linearRangeQuery(
    const vector<RentalRecord>& data,
    int startDate,
    int endDate
) {

    int count = 0;

    for (const auto& record : data) {

        if (record.rentalDate >= startDate &&
            record.rentalDate <= endDate) {

            count++;
        }
    }

    return count;
}

//benchmark

void benchmark(int n) {

    vector<RentalRecord> data = createData(n);

    // Khoang can tim
    int startDate = 20240101 + n / 3;
    int endDate = startDate + 100;

    const int N_QUERIES = 1000;

    volatile int checksum = 0;


    // test binary search

    auto startBinary = high_resolution_clock::now();

    for (int i = 0; i < N_QUERIES; i++) {

        checksum += binaryRangeQuery(
            data,
            startDate,
            endDate
        );
    }

    auto endBinary = high_resolution_clock::now();


    long long timeBinary =
        duration_cast<nanoseconds>(
            endBinary - startBinary
        ).count();

    //test linear scan

    auto startLinear = high_resolution_clock::now();

    for (int i = 0; i < N_QUERIES; i++) {

        checksum += linearRangeQuery(
            data,
            startDate,
            endDate
        );
    }

    auto endLinear = high_resolution_clock::now();


    long long timeLinear =
        duration_cast<nanoseconds>(
            endLinear - startLinear
        ).count();

    //ket qua

    double avgBinary =
        (double)timeBinary / N_QUERIES;

    double avgLinear =
        (double)timeLinear / N_QUERIES;

    double speedup =
        avgBinary > 0
        ? avgLinear / avgBinary
        : 0;


    cout << "So luong ban ghi: "
        << n << endl;

    cout << "Binary Search: "
        << fixed << setprecision(2)
        << avgBinary
        << " ns/query" << endl;

    cout << "Linear Scan: "
        << avgLinear
        << " ns/query" << endl;

    cout << "Speedup: "
        << speedup
        << "x" << endl;

    cout << "So ket qua trong khoang: "
        << binaryRangeQuery(
            data,
            startDate,
            endDate
        )
        << endl;

    cout << "-----------------------------"
        << endl;

    (void)checksum;
}


int main() {

    cout << "===== BENCHMARK MC2 ====="
        << endl;

    cout << "Sorted Array + Binary Search"
        << " VS Linear Scan"
        << endl;

    cout << "============================="
        << endl;


    benchmark(1000);

    benchmark(10000);

    benchmark(100000);


    cout << "===== KET THUC ====="
        << endl;

    return 0;
}