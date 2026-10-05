#include <iostream>
#include <string>
#include <vector>
#include "../core/structures/Trie.h"

using namespace std;

// Dữ liệu danh mục dòng xe ban đầu
const vector< pair< string, string > > HANG_DONG_XE = {
    {"Toyota", "Vios"},
    {"Toyota", "Innova"},
    {"Hyundai", "Accent"},
    {"Hyundai", "i10"},
    {"Hyundai", "Grand i10"},
    {"Kia", "Morning"},
    {"Kia", "Seltos"},
    {"Honda", "City"},
    {"Mazda", "CX-5"},
    {"Mitsubishi", "Xpander"},
    {"Ford", "Everest"},
    {"VinFast", "VF5"}
};

int main() {
    CarTrie carCatalog;

    // Nạp toàn bộ danh mục xe vào Cây Trie trong RAM
    for (const auto& item : HANG_DONG_XE) {
        carCatalog.insertCar(item.first, item.second);
    }

    cout << "========================================================\n";
    cout << "  HE THONG CHO THUE XE TU LAI - DEMO AUTOCOMPLETE (RF1) \n";
    cout << "========================================================\n";
    cout << "-> Da nap " << HANG_DONG_XE.size() << " dong xe vao Cay Trie (RAM).\n";
    cout << "-> Ban co the tim theo Hang (Toyota, Hyundai...) hoac Dong xe (Innova, i10...).\n";
    cout << "-> Go 'exit' de thoat chuong trinh.\n";
    cout << "========================================================\n\n";

    string userInput;

    while (true) {
        cout << "Nhap tu khoa tim kiem: ";
        getline(cin, userInput);

        if (userInput == "exit") {
            cout << "\nCam on ban da su dung he thong!\n";
            break;
        }

        if (userInput.empty()) {
            continue;
        }

        vector< string > allMatches = carCatalog.getAllSuggestions(userInput);

        if (allMatches.empty()) {
            cout << "   [X] Khong tim thay xe nao phu hop voi tu khoa \"" << userInput << "\"!\n\n";
        } 
        else if (allMatches.size() <= 5) {
            // Có tối đa 5 gợi ý: hiển thị toàn bộ
            cout << "   [OK] Ket qua goi y (" << allMatches.size() << " ket qua):\n";
            for (size_t i = 0; i < allMatches.size(); ++i) {
                cout << "      " << (i + 1) << ". " << allMatches[i] << "\n";
            }
            cout << "\n";
        } 
        else {
            // Nhiều hơn 5 gợi ý: hiển thị 5 cái đầu và hỏi người dùng
            cout << "   [OK] Ket qua goi y (Top 5 / " << allMatches.size() << " ket qua):\n";
            for (size_t i = 0; i < 5; ++i) {
                cout << "      " << (i + 1) << ". " << allMatches[i] << "\n";
            }

            int remaining = (int)allMatches.size() - 5;
            cout << "   [!] Con " << remaining << " goi y khac phu hop voi \"" << userInput << "\".\n";
            cout << "   Ban co muon xem toan bo " << allMatches.size() << " goi y nay khong? (y/n): ";
            
            string choice;
            getline(cin, choice);

            if (choice == "y" || choice == "Y") {
                cout << "   --- TOAN BO " << allMatches.size() << " GOI Y CHO \"" << userInput << "\" ---\n";
                for (size_t i = 0; i < allMatches.size(); ++i) {
                    cout << "      " << (i + 1) << ". " << allMatches[i] << "\n";
                }
            }
            cout << "\n";
        }
    }

    return 0;
}