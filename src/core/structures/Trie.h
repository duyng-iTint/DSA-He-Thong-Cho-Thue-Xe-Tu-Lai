#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <algorithm>

using namespace std;

// Node của cây Trie
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    vector< string > matchedFullCarNames; // Lưu tên đầy đủ của các xe kết thúc tại nút này

    TrieNode() {
        isEndOfWord = false;
    }

    ~TrieNode() {
        for (auto& pair : children) {
            if (pair.second != nullptr) {
                delete pair.second;
                pair.second = nullptr;
            }
        }
    }
};

// Lớp quản lý Cây tiền tố xe
class CarTrie {
private:
    TrieNode* root;
    int totalCars;

    // Chuẩn hóa chữ thường và xóa khoảng trắng thừa
    string normalize(const string& str) const {
        string res = "";
        int start = 0;
        while (start < (int)str.length() && isspace((unsigned char)str[start])) {
            start++;
        }
        int end = (int)str.length() - 1;
        while (end >= start && isspace((unsigned char)str[end])) {
            end--;
        }
        for (int i = start; i <= end; i++) {
            res += (char)tolower((unsigned char)str[i]);
        }
        return res;
    }

    // Tách chuỗi thành các từ đơn để lập chỉ mục từ khóa phụ
    vector< string > splitWords(const string& str) const {
        vector< string > words;
        string current = "";
        for (char c : str) {
            if (isspace((unsigned char)c)) {
                if (!current.empty()) {
                    words.push_back(current);
                    current = "";
                }
            } else {
                current += c;
            }
        }
        if (!current.empty()) {
            words.push_back(current);
        }
        return words;
    }

    void insertKey(const string& key, const string& fullCarName) {
        if (key.empty()) return;

        TrieNode* current = root;
        for (char ch : key) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEndOfWord = true;

        bool exists = false;
        for (const string& name : current->matchedFullCarNames) {
            if (name == fullCarName) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            current->matchedFullCarNames.push_back(fullCarName);
        }
    }

    void collectAllWords(TrieNode* node, vector< string >& results) const {
        if (node == nullptr) return;

        if (node->isEndOfWord) {
            for (const string& carName : node->matchedFullCarNames) {
                if (find(results.begin(), results.end(), carName) == results.end()) {
                    results.push_back(carName);
                }
            }
        }

        for (auto const& pair : node->children) {
            collectAllWords(pair.second, results);
        }
    }

public:
    CarTrie() {
        root = new TrieNode();
        totalCars = 0;
    }

    ~CarTrie() {
        delete root;
        root = nullptr;
    }

    // Nạp cả tên hãng + dòng xe, đồng thời nạp riêng dòng xe để tìm linh hoạt
    void insertCar(const string& brand, const string& model) {
        string fullCarName = brand + " " + model;
        string lowerFull = normalize(fullCarName);

        // Nạp cả cụm đầy đủ (VD: "toyota innova")
        insertKey(lowerFull, fullCarName);

        // Nạp riêng từ khóa dòng xe (VD: "innova" -> trỏ về "Toyota Innova")
        vector< string > words = splitWords(lowerFull);
        for (size_t i = 1; i < words.size(); ++i) {
            string subKey = "";
            for (size_t j = i; j < words.size(); ++j) {
                if (!subKey.empty()) subKey += " ";
                subKey += words[j];
            }
            insertKey(subKey, fullCarName);
        }

        totalCars++;
    }

    // Tìm kiếm toàn bộ gợi ý khớp tiền tố
    vector< string > getAllSuggestions(const string& prefix) const {
        vector< string > results;
        string cleanPrefix = normalize(prefix);
        if (cleanPrefix.empty()) return results;

        TrieNode* current = root;
        for (char ch : cleanPrefix) {
            if (current->children.find(ch) == current->children.end()) {
                return results;
            }
            current = current->children[ch];
        }

        collectAllWords(current, results);
        // Sắp xếp lại theo thứ tự từ điển A-Z để đảm bảo đáp ứng chuẩn yêu cầu Q1
        sort(results.begin(), results.end());
        return results;
    }

    int getTotalCars() const {
        return totalCars;
    }
};

#endif