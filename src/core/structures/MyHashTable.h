// MyHashTable.

#pragma once

#include <string>
#include <vector>
#include <cstddef>

template <typename V>
class MyHashTable {
private:
    struct Node {
        std::string key;
        V value;
        Node* next;
        Node(const std::string& k, const V& v, Node* n)
            : key(k), value(v), next(n) {}
    };

    std::vector<Node*> buckets_;
    std::size_t capacity_;
    std::size_t size_;
    double loadFactorThreshold_ = 0.75;

    // Ham bam tu cai dat (polynomial rolling hash), tuong tu cach Java
    std::size_t hashKey(const std::string& key) const {
        std::size_t h = 0;
        for (char ch : key) {
            h = (h * 31 + static_cast<unsigned char>(ch)) % capacity_;
        }
        return h;
    }

    double loadFactor() const {
        return static_cast<double>(size_) / static_cast<double>(capacity_);
    }

    // Tang gap doi so bucket roi bam lai (rehash) toan bo phan tu.
    void resize() {
        std::vector<Node*> oldBuckets = buckets_;
        capacity_ = capacity_ * 2 + 1; // giu le, gan so nguyen to
        buckets_.assign(capacity_, nullptr);
        size_ = 0;

        for (Node* node : oldBuckets) {
            while (node != nullptr) {
                Node* nextNode = node->next;
                insert(node->key, node->value); // bam lai vao bang moi
                delete node; // giai phong node cu
                node = nextNode;
            }
        }
    }

public:
    explicit MyHashTable(std::size_t initialCapacity = 1009)
        : capacity_(initialCapacity), size_(0) {
        buckets_.assign(capacity_, nullptr);
    }

    // Khong cho phep copy (tranh double-free) - chi cho phep move.
    MyHashTable(const MyHashTable&) = delete;
    MyHashTable& operator=(const MyHashTable&) = delete;

    MyHashTable(MyHashTable&& other) noexcept
        : buckets_(std::move(other.buckets_)),
          capacity_(other.capacity_),
          size_(other.size_) {
        other.buckets_.clear();
        other.size_ = 0;
    }

    ~MyHashTable() {
        clear();
    }

    void clear() {
        for (Node* head : buckets_) {
            Node* node = head;
            while (node != nullptr) {
                Node* nextNode = node->next;
                delete node;
                node = nextNode;
            }
        }
        buckets_.assign(capacity_, nullptr);
        size_ = 0;
    }

    // Them moi hoac cap nhat value neu key da ton tai. O(1) trung binh.
    void insert(const std::string& key, const V& value) {
        std::size_t idx = hashKey(key);
        Node* node = buckets_[idx];
        while (node != nullptr) {
            if (node->key == key) {
                node->value = value; // key da co -> cap nhat (upsert)
                return;
            }
            node = node->next;
        }
        // Key chua co -> chen vao dau danh sach lien ket cua bucket do
        buckets_[idx] = new Node(key, value, buckets_[idx]);
        ++size_;

        if (loadFactor() > loadFactorThreshold_) {
            resize();
        }
    }

    // Tim key. Tra ve true/false qua outValue neu tim thay. O(1) trung binh.
    bool search(const std::string& key, V& outValue) const {
        std::size_t idx = hashKey(key);
        Node* node = buckets_[idx];
        while (node != nullptr) {
            if (node->key == key) {
                outValue = node->value;
                return true;
            }
            node = node->next;
        }
        return false;
    }

    bool contains(const std::string& key) const {
        V dummy;
        return search(key, dummy);
    }

    // Xoa key khoi bang bam. Tra ve true neu xoa duoc, false neu khong co.
    bool remove(const std::string& key) {
        std::size_t idx = hashKey(key);
        Node* node = buckets_[idx];
        Node* prev = nullptr;
        while (node != nullptr) {
            if (node->key == key) {
                if (prev == nullptr) {
                    buckets_[idx] = node->next;
                } else {
                    prev->next = node->next;
                }
                delete node;
                --size_;
                return true;
            }
            prev = node;
            node = node->next;
        }
        return false;
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }

    // Duyet toan bo (key, value) - dung cho viec ghi ra CSV (persistence).
    std::vector<std::pair<std::string, V>> allItems() const {
        std::vector<std::pair<std::string, V>> result;
        result.reserve(size_);
        for (Node* head : buckets_) {
            Node* node = head;
            while (node != nullptr) {
                result.emplace_back(node->key, node->value);
                node = node->next;
            }
        }
        return result;
    }

    struct Stats {
        std::size_t capacity;
        std::size_t size;
        double loadFactor;
        std::size_t maxChainLength;
    };

    // Thong tin phuc vu giai thich/benchmark: capacity, size, load factor,
    Stats stats() const {
        std::size_t maxChain = 0;
        for (Node* head : buckets_) {
            std::size_t length = 0;
            Node* node = head;
            while (node != nullptr) {
                ++length;
                node = node->next;
            }
            if (length > maxChain) maxChain = length;
        }
        return Stats{capacity_, size_, loadFactor(), maxChain};
    }
};
