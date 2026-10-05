#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "../src/core/structures/Trie.h"

using namespace std;

void runAutomatedTests() {
    cout << "=== D5: DANG CHAY BO UNIT TEST CHO TRIE (RF1) ===" << endl;
    CarTrie trie;

    // Nạp dữ liệu mẫu
    trie.insertCar("Toyota", "Vios");
    trie.insertCar("Toyota", "Innova");
    trie.insertCar("Hyundai", "i10");
    trie.insertCar("Hyundai", "Grand i10");
    trie.insertCar("Kia", "Morning");

    // 1. Tìm theo tên Hãng xe (Brand prefix)
    vector< string > resToy = trie.getAllSuggestions("toy");
    assert(resToy.size() == 2);
    cout << "Test 1: Tim theo tien to Hang xe thanh cong." << endl;

    // 2. Tìm theo tên Dòng xe độc lập (Model prefix)
    vector< string > resInnova = trie.getAllSuggestions("inno");
    assert(resInnova.size() == 1);
    assert(resInnova[0] == "Toyota Innova");
    cout << "Test 2: Tim bang ten Dong xe (Innova -> Toyota Innova) thanh cong." << endl;

    // 3. Tiền tố khớp nhiều dòng xe con
    vector< string > resI10 = trie.getAllSuggestions("i10");
    assert(resI10.size() >= 2);
    cout << "Test 3: Goi y ca 'Hyundai i10' va 'Hyundai Grand i10' thanh cong." << endl;

    // 4. Kiểm tra thứ tự từ điển A-Z (Cam kết Q1 trong D3)
    // "Toyota Innova" phai dung truoc "Toyota Vios"
    assert(resToy[0] == "Toyota Innova");
    assert(resToy[1] == "Toyota Vios");
    cout << "Test 4: Ket qua tra ve dam bao thu tu tu dien (A-Z) thanh cong." << endl;

    // 5. Kiểm tra không phân biệt chữ HOA / chữ thường
    vector< string > resUpper = trie.getAllSuggestions("TOY");
    vector< string > resMixed = trie.getAllSuggestions("tOy");
    assert(resUpper == resToy);
    assert(resMixed == resToy);
    cout << "Test 5: Khong phan biet hoa/thuong (toy == TOY == tOy) thanh cong." << endl;

    // 6. Edge Case: Khoảng trắng thừa ở hai đầu (Trim spaces)
    vector< string > resSpace = trie.getAllSuggestions("   inno   ");
    assert(resSpace == resInnova);
    cout << "Test 6: Tu dong cat khoang trang o hai dau tu khoa thanh cong." << endl;

    // 7. Edge Case: Chuỗi rỗng hoặc toàn dấu cách
    vector< string > resEmpty = trie.getAllSuggestions("");
    vector< string > resBlank = trie.getAllSuggestions("     ");
    assert(resEmpty.empty());
    assert(resBlank.empty());
    cout << "Test 7: Xu ly an toan khi nhap chuoi rong/dau cach thanh cong." << endl;

    // 8. Edge Case: Tiền tố không tồn tại
    vector< string > resNone = trie.getAllSuggestions("Ferrari");
    assert(resNone.empty());
    cout << "Test 8: Tra ve danh sach rong an toan khi khong tim thay thanh cong." << endl;

    cout << "=== TAT CA 8/8 UNIT TEST CASES DA PASS 100%! ===" << endl << endl;
}

int main() {
    runAutomatedTests();
    return 0;
}