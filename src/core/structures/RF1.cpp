#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
using namespace std;

// CẤU TRÚC NÚT CỦA CÂY TIỀN TỐ (TRIE NODE)
class TrieNode {
public:
    static const int ALPHABET_SIZE = 256; // Bảng mã ASCII (chữ hoa/thường, số, khoảng trắng, gạch nối)
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;
    string originalCarName; // Tên xe gốc (ví dụ: "Toyota Vios 1.5G")
    string brand;           // Tên thương hiệu (ví dụ: "Toyota")

    TrieNode() {
        isEndOfWord = false;
        originalCarName = "";
        brand = "";
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            children[i] = nullptr;
        }
    }

    ~TrieNode() {
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (children[i] != nullptr) {
                delete children[i];
                children[i] = nullptr;
            }
        }
    }
};
// LỚP ĐIỀU KHIỂN CÂY TIỀN TỐ (CAR TRIE)
class CarTrie {
private:
    TrieNode* root;
    int totalCars;

    // Chuyển chuỗi về chữ thường để tra cứu không phân biệt hoa thường
    string toLower(const string& str) const {
        string result = str;
        for (char& c : result) {
            c = tolower(static_cast< unsigned char >(c));
        }
        return result;
    }

    // Duyệt cây con theo chiều sâu (DFS) để lấy danh sách gợi ý
    void collectSuggestions(TrieNode* node, vector< string >& results, int limit) const {
        if (node == nullptr) return;

        // Dừng khi đã đủ số lượng kết quả cần lấy
        if (results.size() >= static_cast< size_t >(limit)) {
            return;
        }

        // Nếu nút hiện tại là điểm kết thúc của một tên xe
        if (node->isEndOfWord) {
            results.push_back(node->originalCarName);
        }

        // Duyệt thứ tự từ điển qua tất cả các ký tự con
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            if (node->children[i] != nullptr) {
                collectSuggestions(node->children[i], results, limit);
                if (results.size() >= static_cast< size_t >(limit)) {
                    return;
                }
            }
        }
    }

public:
    CarTrie() {
        root = new TrieNode();
        totalCars = 0;
    }

    ~CarTrie() {
        delete root;
    }

    // 1. Thêm mẫu xe vào cây Trie
    void insert(const string& carName, const string& brand = "") {
        if (carName.empty()) return;

        TrieNode* current = root;
        string lowerName = toLower(carName);

        for (char ch : lowerName) {
            unsigned char idx = static_cast< unsigned char >(ch);
            if (current->children[idx] == nullptr) {
                current->children[idx] = new TrieNode();
            }
            current = current->children[idx];
        }

        if (!current->isEndOfWord) {
            current->isEndOfWord = true;
            current->originalCarName = carName;
            current->brand = brand;
            totalCars++;
        }
    }

    // 2. Gợi ý tên xe tự động theo tiền tố (Autocomplete)
    vector< string > autocomplete(const string& prefix, int limit = 5) const {
        vector< string > suggestions;
        if (prefix.empty()) {
            return suggestions;
        }

        TrieNode* current = root;
        string lowerPrefix = toLower(prefix);

        // Di chuyển đến nút đại diện cho tiền tố
        for (char ch : lowerPrefix) {
            unsigned char idx = static_cast< unsigned char >(ch);
            if (current->children[idx] == nullptr) {
                return suggestions; // Không có dòng xe nào bắt đầu bằng tiền tố này
            }
            current = current->children[idx];
        }

        // Thu thập tối đa 'limit' gợi ý từ nút tiền tố
        collectSuggestions(current, suggestions, limit);
        return suggestions;
    }

    int getTotalCars() const {
        return totalCars;
    }
};
// CHƯƠNG TRÌNH CHÍNH (GIAO DIỆN)
int main() {
    CarTrie carCatalog;

    // 1. Nạp danh mục xe giả lập vào cây Trie
    vector< pair< string, string > > sampleCars = {
        {"Toyota Vios 1.5E", "Toyota"},
        {"Toyota Vios 1.5G", "Toyota"},
        {"Toyota Camry 2.0Q", "Toyota"},
        {"Toyota Camry 2.5Q", "Toyota"},
        {"Toyota Corolla Cross 1.8V", "Toyota"},
        {"Toyota Fortuner Legender", "Toyota"},
        {"Hyundai Accent 1.4 AT", "Hyundai"},
        {"Hyundai Accent 1.4 MT", "Hyundai"},
        {"Hyundai Grand i10 Hatchback", "Hyundai"},
        {"Hyundai Grand i10 Sedan", "Hyundai"},
        {"Hyundai Tucson 2.0 AT", "Hyundai"},
        {"Hyundai Santa Fe 2.2D", "Hyundai"},
        {"Honda City RS", "Honda"},
        {"Honda City L", "Honda"},
        {"Honda Civic RS", "Honda"},
        {"Honda CR-V G", "Honda"},
        {"Kia Morning AT", "Kia"},
        {"Kia Morning MT", "Kia"},
        {"Kia Cerato 1.6 AT", "Kia"},
        {"Kia Carnival 2.2D", "Kia"},
        {"Mazda 3 Luxury", "Mazda"},
        {"Mazda CX-5 2.0 Premium", "Mazda"}
    };

    for (const auto& car : sampleCars) {
        carCatalog.insert(car.first, car.second);
    }

    cout << "========================================================\n";
    cout << "  HE THONG CHO THUE XE TU LAI - DEMO AUTOCOMPLETE (RF1) \n";
    cout << "========================================================\n";
    cout << "-> Da nap thanh cong " << carCatalog.getTotalCars() << " mau xe vao bo nho (RAM).\n";
    cout << "-> Huong dan: Nhap vai ky tu dau de xem goi y.\n";
    cout << "-> Go 'exit' de thoat chuong trinh.\n";
    cout << "========================================================\n\n";

    string userInput;

    // 2. Vòng lặp nhận dữ liệu từ bàn phím
    while (true) {
        cout << "Nhap tu khoa tim kiem (vd: hyu, toy, kia, mazda...): ";
        getline(cin, userInput);

        // Thoát chương trình nếu người dùng nhập 'exit'
        if (userInput == "exit") {
            cout << "\nCam on ban da su dung he thong!\n";
            break;
        }

        // Bỏ qua nếu nhấn Enter mà không gõ gì
        if (userInput.empty()) {
            continue;
        }

        // Tìm kiếm và hiển thị tối đa 5 gợi ý
        vector< string > results = carCatalog.autocomplete(userInput, 5);

        if (results.empty()) {
            cout << "   [X] Khong tim thay dong xe nao phu hop voi \"" << userInput << "\"!\n\n";
        } else {
            cout << "   [OK] Ket qua goi y (Top " << results.size() << "):\n";
            for (size_t i = 0; i < results.size(); ++i) {
                cout << "      " << (i + 1) << ". " << results[i] << "\n";
            }
            cout << "\n";
        }
    }

    return 0;
}