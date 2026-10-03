#pragma once

#include <map>
#include <string>
#include <vector>
#include <cctype>

// Trie nho cho goi y tien to ten hang/dong xe (RF1).
class Trie {
    struct Node {
        std::map<char, Node*> children;
        bool terminal = false;
        std::string value;
        ~Node() { for (auto& child : children) delete child.second; }
    };
    Node root_;

    static void collect(const Node* node, std::string& word, std::vector<std::string>& out, std::size_t limit) {
        if (out.size() >= limit) return;
        if (node->terminal) out.push_back(node->value);
        for (const auto& child : node->children) {
            if (out.size() >= limit) break;
            word.push_back(child.first);
            collect(child.second, word, out, limit);
            word.pop_back();
        }
    }
public:
    Trie() = default;
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
    void insert(const std::string& value) {
        Node* node = &root_;
        for (char ch : value) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            Node*& next = node->children[ch];
            if (!next) next = new Node();
            node = next;
        }
        node->terminal = true;
        node->value = value;
    }
    std::vector<std::string> autocomplete(const std::string& prefix, std::size_t limit = 10) const {
        const Node* node = &root_;
        std::string normalized = prefix;
        for (char& ch : normalized) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        for (char ch : normalized) {
            auto found = node->children.find(ch);
            if (found == node->children.end()) return {};
            node = found->second;
        }
        std::vector<std::string> result;
        std::string word = normalized;
        collect(node, word, result, limit);
        return result;
    }
};
