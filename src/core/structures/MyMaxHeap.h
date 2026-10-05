#ifndef MY_MAX_HEAP_H
#define MY_MAX_HEAP_H

#include <string>
#include <vector>
#include "MyHashTable.h"
#include <stdexcept>

using namespace std;

struct RentalRequest {
    string bookingId;
    string customerId;
    string carId;
    int membershipTier;
    long long bookingTimestamp;
};

class MyMaxHeap {
private:
    vector<RentalRequest> heap;
    MyHashTable<int> indexMap;

    bool HigherPriority(const RentalRequest& a, const RentalRequest& b) {
    if (a.membershipTier != b.membershipTier) {
        return a.membershipTier > b.membershipTier;
    }
    if (a.bookingTimestamp != b.bookingTimestamp) {
        return a.bookingTimestamp < b.bookingTimestamp;
    }
    return a.bookingId < b.bookingId;
}

    void SwapAndSync(int i, int j) {
        RentalRequest temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;

        indexMap.insert(heap[i].bookingId, i);
        indexMap.insert(heap[j].bookingId, j);
    }

    void SiftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;

            if (!HigherPriority(heap[i], heap[parent])) {
                break;
            }

            SwapAndSync(i, parent);
            i = parent;
        }
    }

    void SiftDown(int i) {
        int n = static_cast<int>(heap.size());

        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int best = i;

            if (left < n && HigherPriority(heap[left], heap[best])) {
                best = left;
            }

            if (right < n && HigherPriority(heap[right], heap[best])) {
                best = right;
            }

            if (best == i) {
                break;
            }

            SwapAndSync(i, best);
            i = best;
        }
    }

    void RemoveAtIndex(int idx) {
        int lastIdx = static_cast<int>(heap.size()) - 1;

        indexMap.remove(heap[idx].bookingId);

        if (idx != lastIdx) {
            heap[idx] = heap.back();
            indexMap.insert(heap[idx].bookingId, idx);
        }

        heap.pop_back();

        if (idx < static_cast<int>(heap.size())) {
            SiftUp(idx);
            SiftDown(idx);
        }
    }

public:
    bool InsertRequest(const RentalRequest& req) {
        // Khong cho phep trung BookingID
        if (indexMap.contains(req.bookingId)) {
            return false;
        }

        heap.push_back(req);

        int newIndex = static_cast<int>(heap.size()) - 1;
        indexMap.insert(req.bookingId, newIndex);

        SiftUp(newIndex);

        return true;
    }

    RentalRequest ExtractMax() {
        if (heap.empty()) {
            throw runtime_error("Heap rong");
        }

        RentalRequest top = heap[0];
        RemoveAtIndex(0);

        return top;
    }
    bool RemoveById(const string& bookingId) {
    int index;

    if (!indexMap.search(bookingId, index)) {
        return false;
    }

    RemoveAtIndex(index);
    return true;
    }

    bool BuildHeap(const vector<RentalRequest>& initial) {
        heap.clear();
        indexMap.clear();

        for (const RentalRequest& req : initial) {
            // Khong cho phep trung BookingID
            if (indexMap.contains(req.bookingId)) {
                return false;
            }

            heap.push_back(req);

            int index = static_cast<int>(heap.size()) - 1;
            indexMap.insert(req.bookingId, index);
        }

        for (int i = static_cast<int>(heap.size()) / 2 - 1; i >= 0; i--) {
            SiftDown(i);
        }

        return true;
    }

    bool Empty() const {
        return heap.empty();
    }

    int Size() const {
        return static_cast<int>(heap.size());
    }

    const RentalRequest& Top() const {
        if (heap.empty()) {
            throw runtime_error("Heap rong");
        }

        return heap[0];
    }
};

#endif
