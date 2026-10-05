#include <iostream>
#include "../src/core/structures/MyMaxHeap.h"

using namespace std;

int main() {
    MyMaxHeap heap;

    RentalRequest a = {
        "B001", "C001", "CAR01", 1, 100
    };

    RentalRequest b = {
        "B002", "C002", "CAR01", 3, 200
    };

    RentalRequest c = {
        "B003", "C003", "CAR01", 3, 100
    };

    cout << "===== TEST INSERT =====" << endl;

    heap.InsertRequest(a);
    heap.InsertRequest(b);
    heap.InsertRequest(c);

    cout << "Size: " << heap.Size() << endl;
    cout << "Top: " << heap.Top().bookingId << endl;


    cout << endl;
    cout << "===== TEST EXTRACT MAX =====" << endl;

    RentalRequest x = heap.ExtractMax();

    cout << "Extract: " << x.bookingId << endl;
    cout << "Top sau Extract: "
         << heap.Top().bookingId << endl;


    cout << endl;
    cout << "===== TEST REMOVE BY ID =====" << endl;

    bool result = heap.RemoveById("B001");

    cout << "Remove B001: ";

    if (result) {
        cout << "Thanh cong" << endl;
    }
    else {
        cout << "Khong tim thay" << endl;
    }

    cout << "Size sau Remove: "
         << heap.Size() << endl;


    cout << endl;
    cout << "===== TEST REMOVE ID KHONG TON TAI =====" << endl;

    result = heap.RemoveById("B999");

    cout << "Remove B999: ";

    if (result) {
        cout << "Thanh cong" << endl;
    }
    else {
        cout << "Khong tim thay" << endl;
    }


    cout << endl;
    cout << "===== TEST EMPTY =====" << endl;

    cout << "Heap rong: ";

    if (heap.Empty()) {
        cout << "Co" << endl;
    }
    else {
        cout << "Khong" << endl;
    }


    cout << endl;
    cout << "===== TEST HEAP RONG =====" << endl;

    MyMaxHeap emptyHeap;

    cout << "Empty: "
         << emptyHeap.Empty() << endl;

    cout << "Size: "
         << emptyHeap.Size() << endl;

    try {
        emptyHeap.Top();
    }
    catch (runtime_error& e) {
        cout << "Top(): Heap rong" << endl;
    }

    try {
        emptyHeap.ExtractMax();
    }
    catch (runtime_error& e) {
        cout << "ExtractMax(): Heap rong" << endl;
    }


    cout << endl;
    cout << "===== TEST CUNG TIER CUNG TIMESTAMP =====" << endl;

    MyMaxHeap heap2;

    RentalRequest d = {
        "B004", "C004", "CAR01", 3, 300
    };

    RentalRequest e = {
        "B002", "C005", "CAR01", 3, 300
    };

    heap2.InsertRequest(d);
    heap2.InsertRequest(e);

    cout << "Top: "
         << heap2.Top().bookingId << endl;

    cout << endl;
    cout << "===== TEST KET THUC =====" << endl;

    return 0;
}