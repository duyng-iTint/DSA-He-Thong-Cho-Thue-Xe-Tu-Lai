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

template <typename Query>
double measureNanosecondsPerQuery(Query query) {
    volatile int checksum = 0;
    size_t queryCount = 0;
    const auto start = steady_clock::now();
    do {
        checksum += query();
        ++queryCount;
    } while (steady_clock::now() - start < milliseconds(100));

    const double elapsedNs = duration<double, nano>(steady_clock::now() - start).count();
    (void)checksum;
    return elapsedNs / queryCount;
}

//benchmark

void benchmark(int n) {

    vector<RentalRecord> data = createData(n);

    // Khoang can tim
    int startDate = 20240101 + n / 3;
    int endDate = startDate + 100;

    const int binaryResult = binaryRangeQuery(data, startDate, endDate);
    const int linearResult = linearRangeQuery(data, startDate, endDate);
    if (binaryResult != linearResult) {
        cerr << "Loi: Binary Search va Linear Scan tra ket qua khac nhau.\n";
        return;
    }

    const double timeBinaryNs = measureNanosecondsPerQuery([&]() {
        return binaryRangeQuery(data, startDate, endDate);
    });
    const double timeLinearNs = measureNanosecondsPerQuery([&]() {
        return linearRangeQuery(data, startDate, endDate);
    });

    //ket qua

    double speedup =
        timeBinaryNs > 0
        ? timeLinearNs / timeBinaryNs
        : 0;


    cout << "So luong ban ghi: "
        << n << endl;

    cout << "Binary Search: "
        << fixed << setprecision(2)
        << timeBinaryNs
        << " ns/query" << endl;

    cout << "Linear Scan: "
        << timeLinearNs
        << " ns/query" << endl;

    cout << "Speedup: "
        << speedup
        << "x" << endl;

    cout << "So ket qua trong khoang: "
        << binaryResult
        << endl;

    cout << "-----------------------------"
        << endl;

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
