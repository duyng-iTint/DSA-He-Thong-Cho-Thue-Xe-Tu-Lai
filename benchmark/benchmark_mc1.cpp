// benchmark_mc1.cpp
// -------------------
// Bang chung hieu nang cho MC1 (Muc 5.3):
// So sanh thoi gian tra cuu Booking_ID giua:
//   (a) MyHashTable - ky vong O(1) trung binh, gan nhu khong doi khi N tang
//   (b) Linear Scan tren vector - O(N), tang tuyen tinh theo N
//
// Do o cac moc quy mo cach nhau >= 1 bac do lon: 1.000, 10.000, 100.000.
//
// Bien dich & chay:
//     make benchmark_mc1
//     ./benchmark_mc1

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <random>

#include "Booking.h"
#include "MyHashTable.h"
#include "Persistence.h"
#include "DataGenerator.h"

namespace {

const std::vector<int> SIZES = {1000, 10000, 100000};
const int N_QUERIES = 500; // so luot tra cuu de lay trung binh, tranh nhieu

// Duyet tuan tu toan bo vector - baseline O(N) de so sanh.
Booking* linearScanSearch(const std::vector<Booking*>& records, const std::string& key) {
    for (Booking* r : records) {
        if (r->booking_id == key) return r;
    }
    return nullptr;
}

struct BenchResult {
    double tHashMs;
    double tLinearMs;
    MyHashTable<Booking*>::Stats stats;
};

BenchResult runBenchmarkForSize(int n) {
    std::string csvPath = "_bench_" + std::to_string(n) + ".csv";
    generateCsv(csvPath, n, 123);

    PersistenceContext ctx;
    MyHashTable<Booking*> tableById;
    MyHashTable<Booking*> tableByPlate;
    loadIntoHashTable(csvPath, ctx, tableById, tableByPlate);

    std::vector<Booking*> records;
    records.reserve(tableById.size());
    for (auto& kv : tableById.allItems()) records.push_back(kv.second);

    std::mt19937 rng(7);
    std::uniform_int_distribution<std::size_t> dist(0, records.size() - 1);
    std::vector<std::string> sampleIds;
    sampleIds.reserve(N_QUERIES);
    for (int i = 0; i < N_QUERIES; ++i) {
        sampleIds.push_back(records[dist(rng)]->booking_id);
    }

    // volatile de ngan trinh bien dich toi uu hoa (loai bo) vong lap
    // do ket qua tra cuu khong duoc su dung o dau khac.
    volatile std::size_t checksum = 0;

    // Do MyHashTable
    auto t0 = std::chrono::high_resolution_clock::now();
    for (const auto& key : sampleIds) {
        Booking* out = nullptr;
        if (tableById.search(key, out)) checksum += reinterpret_cast<std::size_t>(out);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double tHash = std::chrono::duration<double, std::milli>(t1 - t0).count() / N_QUERIES;

    // Do Linear Scan
    t0 = std::chrono::high_resolution_clock::now();
    for (const auto& key : sampleIds) {
        Booking* out = linearScanSearch(records, key);
        if (out != nullptr) checksum += reinterpret_cast<std::size_t>(out);
    }
    t1 = std::chrono::high_resolution_clock::now();
    double tLinear = std::chrono::duration<double, std::milli>(t1 - t0).count() / N_QUERIES;
    (void)checksum;

    return BenchResult{tHash, tLinear, tableById.stats()};
}

} // namespace

int main() {
    std::cout << std::right
              << std::setw(12) << "N ban ghi" << " | "
              << std::setw(16) << "HashTable (ms)" << " | "
              << std::setw(18) << "Linear Scan (ms)" << " | "
              << std::setw(8) << "Speedup" << "\n";
    std::cout << std::string(65, '-') << "\n";

    for (int n : SIZES) {
        BenchResult r = runBenchmarkForSize(n);
        double speedup = r.tHashMs > 0 ? r.tLinearMs / r.tHashMs : 0.0;

        std::cout << std::setw(12) << n << " | "
                  << std::setw(16) << std::fixed << std::setprecision(5) << r.tHashMs << " | "
                  << std::setw(18) << r.tLinearMs << " | "
                  << std::setw(7) << std::setprecision(1) << speedup << "x\n";
        std::cout << "             (thong ke bang bam: capacity=" << r.stats.capacity
                  << ", size=" << r.stats.size
                  << ", load_factor=" << std::setprecision(3) << r.stats.loadFactor
                  << ", max_chain_length=" << r.stats.maxChainLength << ")\n";
    }
    return 0;
}
