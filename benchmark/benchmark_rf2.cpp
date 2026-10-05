#include <iostream>
#include <vector>
#include <chrono>
#include "../src/core/structures/MyMaxHeap.h"

using namespace std;
using namespace chrono;


// Tìm request có priority cao nhất trong mảng thường
int findMaxIndex(const vector<RentalRequest>& a) {
    if (a.empty()) {
        return -1;
    }

    int maxIndex = 0;

    for (int i = 1; i < (int)a.size(); i++) {
        if (a[i].membershipTier > a[maxIndex].membershipTier) {
            maxIndex = i;
        }
        else if (a[i].membershipTier == a[maxIndex].membershipTier) {

            if (a[i].bookingTimestamp < a[maxIndex].bookingTimestamp) {
                maxIndex = i;
            }
            else if (a[i].bookingTimestamp ==
                     a[maxIndex].bookingTimestamp) {

                if (a[i].bookingId < a[maxIndex].bookingId) {
                    maxIndex = i;
                }
            }
        }
    }

    return maxIndex;
}


// Tạo dữ liệu test
vector<RentalRequest> createData(int n) {

    vector<RentalRequest> data;

    for (int i = 0; i < n; i++) {

        RentalRequest request;

        request.bookingId = "B" + to_string(i);
        request.customerId = "C" + to_string(i);
        request.carId = "CAR01";

        // Tạo Tier từ 1 đến 3
        request.membershipTier = (i % 3) + 1;

        // Timestamp
        request.bookingTimestamp = 100000 + i;

        data.push_back(request);
    }

    return data;
}


void benchmark(int n) {

    vector<RentalRequest> data = createData(n);


    // =========================
    // TEST MYMAXHEAP
    // =========================

    MyMaxHeap heap;

    for (int i = 0; i < n; i++) {
        heap.InsertRequest(data[i]);
    }

    auto startHeap = high_resolution_clock::now();

    for (int i = 0; i < 1000; i++) {
        heap.Top();
    }

    auto endHeap = high_resolution_clock::now();

    long long timeHeap =
        duration_cast<nanoseconds>(
            endHeap - startHeap
        ).count();


    // =========================
    // TEST MẢNG THƯỜNG
    // =========================

    auto startArray = high_resolution_clock::now();

    for (int i = 0; i < 1000; i++) {
        findMaxIndex(data);
    }

    auto endArray = high_resolution_clock::now();

    long long timeArray =
        duration_cast<nanoseconds>(
            endArray - startArray
        ).count();


    // =========================
    // OUTPUT
    // =========================

    cout << "So luong request: " << n << endl;

    cout << "MyMaxHeap Top(): "
         << timeHeap
         << " ns" << endl;

    cout << "Mang thuong tim Max: "
         << timeArray
         << " ns" << endl;

    if (timeHeap > 0) {
        cout << "Mang thuong / MyMaxHeap: "
             << (double)timeArray / timeHeap
             << " lan" << endl;
    }

    cout << "-----------------------------" << endl;
}


int main() {

    cout << "===== BENCHMARK MYMAXHEAP VS MANG THUONG ====="
         << endl;

    benchmark(1000);
    benchmark(10000);
    benchmark(50000);

    cout << "===== KET THUC =====" << endl;

    return 0;
}