/**
 * @file lockfree_queue.hpp
 * @brief 单生产者单消费者 (SPSC) 无锁环形队列，利用 memory_order_acquire/release 消除锁开销
 */

#pragma once

#include <vector>
#include <atomic>
#include <cstddef>
#include <utility>
#include <new>

namespace aq {

#if defined(__cpp_lib_hardware_interference_size)
    using std::hardware_destructive_interference_size;
#else
    constexpr size_t hardware_destructive_interference_size = 64;
#endif

template <typename T, size_t Capacity = 1024>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

public:
    SPSCQueue() : buffer_(static_cast<T*>(::operator new(sizeof(T) * Capacity))) {}

    ~SPSCQueue() {
        T item;
        while (pop(item)) {}
        ::operator delete(buffer_);
    }

    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue& operator=(const SPSCQueue&) = delete;

    template <typename... Args>
    bool emplace(Args&&... args) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const size_t head = head_cached_;

        if (tail - head >= Capacity) {
            head_cached_ = head_.load(std::memory_order_acquire);
            if (tail - head_cached_ >= Capacity) {
                return false; // 队列满
            }
        }

        new (&buffer_[tail & BufferMask]) T(std::forward<Args>(args)...);
        tail_.store(tail + 1, std::memory_order_release);
        return true;
    }

    bool push(const T& item) {
        return emplace(item);
    }

    bool push(T&& item) {
        return emplace(std::move(item));
    }

    bool pop(T& val) {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t tail = tail_cached_;

        if (head == tail) {
            tail_cached_ = tail_.load(std::memory_order_acquire);
            if (head == tail_cached_) {
                return false; // 队列空
            }
        }

        T* ptr = &buffer_[head & BufferMask];
        val = std::move(*ptr);
        ptr->~T();
        head_.store(head + 1, std::memory_order_release);
        return true;
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed);
    }

    size_t size() const noexcept {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t tail = tail_.load(std::memory_order_relaxed);
        return (tail >= head) ? (tail - head) : 0;
    }

private:
    static constexpr size_t BufferMask = Capacity - 1;
    T* const buffer_;

    alignas(hardware_destructive_interference_size) std::atomic<size_t> tail_{0};
    alignas(hardware_destructive_interference_size) size_t head_cached_{0};

    alignas(hardware_destructive_interference_size) std::atomic<size_t> head_{0};
    alignas(hardware_destructive_interference_size) size_t tail_cached_{0};
};

} // namespace aq
