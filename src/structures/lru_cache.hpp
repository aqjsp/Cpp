/**
 * @file lru_cache.hpp
 * @brief 高性能线程安全 LRU (Least Recently Used) 缓存实现
 */

#pragma once

#include <unordered_map>
#include <list>
#include <mutex>
#include <optional>
#include <utility>

namespace aq {

template <typename Key, typename Value>
class LRUCache {
public:
    using KeyValuePair = std::pair<Key, Value>;
    using ListIterator = typename std::list<KeyValuePair>::iterator;

    explicit LRUCache(size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            capacity_ = 1;
        }
    }

    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            // 更新节点值并移到头部
            it->second->second = value;
            list_.splice(list_.begin(), list_, it->second);
            return;
        }

        // 超出容量淘汰尾部
        if (list_.size() >= capacity_) {
            auto& last = list_.back();
            map_.erase(last.first);
            list_.pop_back();
        }

        list_.emplace_front(key, value);
        map_[key] = list_.begin();
    }

    std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return std::nullopt;
        }

        // 移至链表头部
        list_.splice(list_.begin(), list_, it->second);
        return it->second->second;
    }

    bool remove(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }
        list_.erase(it->second);
        map_.erase(it);
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return list_.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        map_.clear();
        list_.clear();
    }

private:
    size_t capacity_;
    mutable std::mutex mutex_;
    std::list<KeyValuePair> list_;
    std::unordered_map<Key, ListIterator> map_;
};

} // namespace aq
