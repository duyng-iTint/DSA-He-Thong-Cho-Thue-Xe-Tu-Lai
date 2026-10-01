#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "RentalSystem.h"

class MyHashTable {
private:
    vector<vector<string>> table;

    size_t hashFunction(const string& key) const;

public:
    MyHashTable(size_t capacity);
    void insert(const string& key);
    bool search(const string& key) const;
};

int linearSearch(
    const vector<string>& data,
    const string& target
);

vector<string> generateData(int N);

double benchmarkLinear(
    const vector<string>& data,
    const string& target,
    int repeat
);

double benchmarkHash(
    const MyHashTable& hashTable,
    const string& target,
    int repeat
);

void runBenchmark();

#endif
