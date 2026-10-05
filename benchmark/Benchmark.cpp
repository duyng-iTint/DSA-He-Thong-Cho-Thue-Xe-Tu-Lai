#include "Benchmark.h"
#include <chrono>

using namespace chrono;

size_t MyHashTable::hashFunction(const string& key) const {
    size_t hash = 0;

    for (unsigned char c : key)
        hash = hash * 131 + c;

    return hash % table.size();
}

MyHashTable::MyHashTable(size_t capacity) {
    table.resize(capacity);
}

void MyHashTable::insert(const string& key) {
    int index = hashFunction(key);
    table[index].push_back(key);
}

bool MyHashTable::search(const string& key) const {
    int index = hashFunction(key);

    for (const string& item : table[index]) {
        if (item == key)
            return true;
    }

    return false;
}

int linearSearch(
    const vector<string>& data,
    const string& target
) {
    for (int i = 0; i < static_cast<int>(data.size()); i++) {
        if (data[i] == target)
            return i;
    }

    return -1;
}

vector<string> generateData(int N) {
    vector<string> data;
    data.reserve(N);

    for (int i = 0; i < N; i++) {
        data.push_back(
            "RENT_" + to_string(100000 + i).substr(1)
        );
    }

    return data;
}

double benchmarkLinear(
    const vector<string>& data,
    const string& target,
    int repeat
) {
    volatile int result = -1;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < repeat; i++)
        result = linearSearch(data, target);

    auto finish = high_resolution_clock::now();

    (void)result;

    return duration<double, milli>(
        finish - start
    ).count();
}

double benchmarkHash(
    const MyHashTable& hashTable,
    const string& target,
    int repeat
) {
    volatile bool result = false;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < repeat; i++)
        result = hashTable.search(target);

    auto finish = high_resolution_clock::now();

    (void)result;

    return duration<double, milli>(
        finish - start
    ).count();
}

void runBenchmark() {
    cout << "\n";
    cout << "====================================\n";
    cout << " BENCHMARK\n";
    cout << "====================================\n";

    int testSizes[] = {1000, 10000, 100000};

    for (int N : testSizes) {
        vector<string> data = generateData(N);

        // Tim phan tu cuoi cung
        string target = data[N - 1];

        MyHashTable hashTable(
            static_cast<size_t>(N) * 2 + 1
        );

        for (const string& key : data)
            hashTable.insert(key);

        int repeat = (N <= 10000) ? 1000 : 200;

        double linearTime =
            benchmarkLinear(data, target, repeat);

        double hashTime =
            benchmarkHash(hashTable, target, repeat);

        cout << "\nN = " << N << endl;
        cout << "Linear Search: "
             << linearTime << " ms" << endl;
        cout << "Hash Table: "
             << hashTime << " ms" << endl;
    }

    cout << "\nLinear Search: O(N)\n";
    cout << "Hash Table: trung binh O(1)\n";
    cout << "====================================\n";
}
