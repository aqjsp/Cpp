/**
 * @file skip_list.hpp
 * @brief 跳表 (Skip List) 模板类实现，对标 Redis zset 底层核心结构
 */

#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <functional>
#include <optional>

namespace aq {

template <typename Key, typename Value, size_t MaxLevel = 16>
class SkipList {
    struct Node {
        Key key;
        Value value;
        std::vector<Node*> forward;

        Node(const Key& k, const Value& v, size_t level)
            : key(k), value(v), forward(level, nullptr) {}
    };

public:
    explicit SkipList(float probability = 0.5f)
        : probability_(probability),
          current_level_(1),
          size_(0),
          head_(new Node(Key{}, Value{}, MaxLevel)),
          dist_(0.0f, 1.0f) {
        rng_.seed(std::random_device{}());
    }

    ~SkipList() {
        Node* curr = head_;
        while (curr) {
            Node* next = curr->forward[0];
            delete curr;
            curr = next;
        }
    }

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    std::optional<Value> search(const Key& key) const {
        Node* curr = head_;
        for (int i = static_cast<int>(current_level_) - 1; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->key < key) {
                curr = curr->forward[i];
            }
        }
        curr = curr->forward[0];
        if (curr && curr->key == key) {
            return curr->value;
        }
        return std::nullopt;
    }

    void insert(const Key& key, const Value& value) {
        std::vector<Node*> update(MaxLevel, nullptr);
        Node* curr = head_;

        for (int i = static_cast<int>(current_level_) - 1; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->key < key) {
                curr = curr->forward[i];
            }
            update[i] = curr;
        }

        curr = curr->forward[0];
        if (curr && curr->key == key) {
            // 已存在则更新
            curr->value = value;
            return;
        }

        size_t new_level = random_level();
        if (new_level > current_level_) {
            for (size_t i = current_level_; i < new_level; ++i) {
                update[i] = head_;
            }
            current_level_ = new_level;
        }

        Node* new_node = new Node(key, value, new_level);
        for (size_t i = 0; i < new_level; ++i) {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }
        ++size_;
    }

    bool erase(const Key& key) {
        std::vector<Node*> update(MaxLevel, nullptr);
        Node* curr = head_;

        for (int i = static_cast<int>(current_level_) - 1; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->key < key) {
                curr = curr->forward[i];
            }
            update[i] = curr;
        }

        curr = curr->forward[0];
        if (!curr || curr->key != key) {
            return false;
        }

        for (size_t i = 0; i < current_level_; ++i) {
            if (update[i]->forward[i] != curr) break;
            update[i]->forward[i] = curr->forward[i];
        }

        delete curr;
        while (current_level_ > 1 && head_->forward[current_level_ - 1] == nullptr) {
            --current_level_;
        }
        --size_;
        return true;
    }

    size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

private:
    size_t random_level() {
        size_t lvl = 1;
        while (dist_(rng_) < probability_ && lvl < MaxLevel) {
            ++lvl;
        }
        return lvl;
    }

    float probability_;
    size_t current_level_;
    size_t size_;
    Node* head_;
    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_;
};

} // namespace aq
