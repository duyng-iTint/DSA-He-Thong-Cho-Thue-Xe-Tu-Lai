#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cassert>
#include "../src/core/structures/Trie.h"

using namespace std;

// Hàm tìm kiếm tuyến tính đối chứng
vector< string > linearSearchPrefix(const vector< string >& database, const string& prefix, int limit = 5) {
    vector< string > results;
    string pLower = "";
    for (char c : prefix) pLower += (char)tolower((unsigned char)c);

    for (const string& item : database) {
        string itemLower = "";
        for (char c : item) itemLower += (char)tolower((unsigned char)c);

        if (itemLower.rfind(pLower, 0) == 0) {
            results.push_back(itemLower);
            if ((int)results.size() >= limit) break;
        }
    }
    return results;
}

void runAutomatedTests() {
    cout << "=== D5: DANG CHAY BO UNIT TEST CHO TRIE ===" << endl;
    CarTrie trie;

    trie.insertCar("Toyota", "Vios");
    trie.insertCar("Toyota", "Innova");
    trie.insertCar("Hyundai", "i10");
    trie.insertCar("Hyundai", "Grand i10");

    // 1. Tìm theo tên hãng
    vector< string > resToy = trie.getAllSuggestions("toy");
    assert(resToy.size() == 2);
    cout << "Test 1: Tim theo tien to Hang xe thanh cong." << endl;

    // 2. Tìm theo tên dòng xe riêng lẻ
    vector< string > resInnova = trie.getAllSuggestions("inno");
    assert(resInnova.size() == 1);
    assert(resInnova[0] == "Toyota Innova");
    cout << "Test 2: Tim bang ten Dong xe (Innova -> Toyota Innova) thanh cong." << endl;

    // 3. Tiền tố khớp nhiều dòng xe
    vector< string > resI10 = trie.getAllSuggestions("i10");
    assert(resI10.size() >= 2);
    cout << "Test 3: Goi y ca 'Hyundai i10' va 'Hyundai Grand i10' thanh cong." << endl;

    // 4. Edge Case: Tiền tố không tồn tại
    vector< string > resNone = trie.getAllSuggestions("Ferrari");
    assert(resNone.empty());
    cout << "Test 4: Tra ve rong an toan khi khong tim thay thanh cong." << endl;

    cout << "=== TAT CA UNIT TEST DA PASS 100%! ===" << endl << endl;
}

void runBenchmark() {
    cout << "=== D5: CHAY BENCHMARK DO TOC DO TRIE VS LINEAR SCAN ===" << endl;
    vector< int > sizes = {1000, 10000, 100000};

    for (int N : sizes) {
        CarTrie trieBench;
        vector< string > rawList;

        for (int i = 0; i < N; i++) {
            string brand = "Toyota";
            string model = "Model_" + to_string(i);
            trieBench.insertCar(brand, model);
            rawList.push_back(brand + " " + model);
        }

        int QUERIES = 2000;
        string q = "toyota model_1";

        // Đo Trie
        auto t1 = chrono::high_resolution_clock::now();
        for (int k = 0; k < QUERIES; k++) {
            volatile auto r = trieBench.getAllSuggestions(q);
        }
        auto t2 = chrono::high_resolution_clock::now();
        chrono::duration< double, milli > msTrie = t2 - t1;

        // Đo Linear Scan
        auto t3 = chrono::high_resolution_clock::now();
        for (int k = 0; k < QUERIES; k++) {
            volatile auto r = linearSearchPrefix(rawList, q, 5);
        }
        auto t4 = chrono::high_resolution_clock::now();
        chrono::duration< double, milli > msLinear = t4 - t3;

        cout << "N = " << N << " | Trie: " << msTrie.count() << " ms | Linear: " << msLinear.count() << " ms" << endl;
        cout << ">> Toc do Trie gap: " << (msLinear.count() / msTrie.count()) << " lan\n" << endl;
    }
}

int main() {
    runAutomatedTests();
    runBenchmark();
    return 0;
}