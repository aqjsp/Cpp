/**
 * @file smart_ptr.hpp
 * @brief 自实现 shared_ptr 与 unique_ptr，深度契合现代 C++ RAII 与内存管理原理
 */

#pragma once

#include <cstddef>
#include <atomic>
#include <utility>
#include <algorithm>

namespace aq {

// ==========================================
// 1. UniquePtr 实现 (独占语义，支持移动，禁止拷贝)
// ==========================================
template <typename T>
class UniquePtr {
public:
    constexpr UniquePtr() noexcept : ptr_(nullptr) {}
    explicit UniquePtr(T* ptr) noexcept : ptr_(ptr) {}

    ~UniquePtr() {
        reset();
    }

    // 禁止拷贝构造与拷贝赋值
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 移动构造函数
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.release()) {}

    // 移动赋值运算符
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    // 释放所有权但不销毁资源
    T* release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    // 重置指针，销毁旧资源
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            delete ptr_;
            ptr_ = ptr;
        }
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
    }

    T* get() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }

private:
    T* ptr_{nullptr};
};

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

// ==========================================
// 2. SharedPtr 控制块与引用计数实现
// ==========================================
struct ControlBlock {
    std::atomic<long> shared_count{1};
    std::atomic<long> weak_count{0};

    virtual ~ControlBlock() = default;
    virtual void destroy_resource() noexcept = 0;
};

template <typename T>
struct PointerControlBlock : public ControlBlock {
    T* ptr;

    explicit PointerControlBlock(T* p) : ptr(p) {}

    void destroy_resource() noexcept override {
        delete ptr;
        ptr = nullptr;
    }
};

template <typename T>
class SharedPtr {
public:
    constexpr SharedPtr() noexcept : ptr_(nullptr), ctrl_(nullptr) {}

    explicit SharedPtr(T* ptr) {
        if (ptr) {
            ptr_ = ptr;
            ctrl_ = new PointerControlBlock<T>(ptr);
        } else {
            ptr_ = nullptr;
            ctrl_ = nullptr;
        }
    }

    ~SharedPtr() {
        decrement_and_destroy();
    }

    // 拷贝构造函数：引用计数 +1
    SharedPtr(const SharedPtr& other) noexcept : ptr_(other.ptr_), ctrl_(other.ctrl_) {
        if (ctrl_) {
            ctrl_->shared_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // 拷贝赋值运算符：copy-and-swap 习惯用法
    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this != &other) {
            SharedPtr temp(other);
            swap(temp);
        }
        return *this;
    }

    // 移动构造函数：零成本转移
    SharedPtr(SharedPtr&& other) noexcept : ptr_(other.ptr_), ctrl_(other.ctrl_) {
        other.ptr_ = nullptr;
        other.ctrl_ = nullptr;
    }

    // 移动赋值运算符
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            decrement_and_destroy();
            ptr_ = other.ptr_;
            ctrl_ = other.ctrl_;
            other.ptr_ = nullptr;
            other.ctrl_ = nullptr;
        }
        return *this;
    }

    void reset(T* ptr = nullptr) {
        SharedPtr temp(ptr);
        swap(temp);
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(ctrl_, other.ctrl_);
    }

    long use_count() const noexcept {
        return ctrl_ ? ctrl_->shared_count.load(std::memory_order_relaxed) : 0;
    }

    T* get() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }

private:
    void decrement_and_destroy() noexcept {
        if (ctrl_) {
            if (ctrl_->shared_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                ctrl_->destroy_resource();
                delete ctrl_;
            }
            ptr_ = nullptr;
            ctrl_ = nullptr;
        }
    }

    T* ptr_{nullptr};
    ControlBlock* ctrl_{nullptr};
};

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

} // namespace aq
