/**
 * @file memory_pool.hpp
 * @brief 定长对象池与内存块分配器 (Slab / Fixed-size Memory Pool)
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>
#include <mutex>

namespace aq {

template <size_t BlockSize, size_t BlocksPerPage = 128>
class FixedMemoryPool {
    static_assert(BlockSize >= sizeof(void*), "BlockSize must be at least sizeof(void*)");

    union FreeNode {
        FreeNode* next;
        char storage[BlockSize];
    };

    struct Page {
        Page* next{nullptr};
        FreeNode nodes[BlocksPerPage];
    };

public:
    FixedMemoryPool() = default;

    ~FixedMemoryPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Page* curr = page_head_;
        while (curr) {
            Page* next = curr->next;
            delete curr;
            curr = next;
        }
        page_head_ = nullptr;
        free_head_ = nullptr;
    }

    FixedMemoryPool(const FixedMemoryPool&) = delete;
    FixedMemoryPool& operator=(const FixedMemoryPool&) = delete;

    void* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!free_head_) {
            allocate_page();
        }
        FreeNode* node = free_head_;
        free_head_ = free_head_->next;
        total_allocated_++;
        return static_cast<void*>(node);
    }

    void deallocate(void* ptr) {
        if (!ptr) return;
        std::lock_guard<std::mutex> lock(mutex_);
        FreeNode* node = static_cast<FreeNode*>(ptr);
        node->next = free_head_;
        free_head_ = node;
        total_allocated_--;
    }

    size_t active_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return total_allocated_;
    }

private:
    void allocate_page() {
        Page* new_page = new Page();
        new_page->next = page_head_;
        page_head_ = new_page;

        for (size_t i = 0; i < BlocksPerPage - 1; ++i) {
            new_page->nodes[i].next = &new_page->nodes[i + 1];
        }
        new_page->nodes[BlocksPerPage - 1].next = free_head_;
        free_head_ = &new_page->nodes[0];
    }

    Page* page_head_{nullptr};
    FreeNode* free_head_{nullptr};
    size_t total_allocated_{0};
    mutable std::mutex mutex_;
};

template <typename T, size_t Capacity = 256>
class ObjectPool {
public:
    template <typename... Args>
    T* construct(Args&&... args) {
        void* mem = pool_.allocate();
        try {
            return new (mem) T(std::forward<Args>(args)...);
        } catch (...) {
            pool_.deallocate(mem);
            throw;
        }
    }

    void destroy(T* ptr) {
        if (!ptr) return;
        ptr->~T();
        pool_.deallocate(static_cast<void*>(ptr));
    }

private:
    FixedMemoryPool<sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T), Capacity> pool_;
};

} // namespace aq
