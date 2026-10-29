/**
 * @file my_string.hpp
 * @brief 自实现 String 类，演示深拷贝、SSO (短字符串优化) 与移动语义
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <string_view>

namespace aq {

class String {
public:
    static constexpr size_t SSO_CAPACITY = 15;

    // 默认构造
    String() noexcept : size_(0) {
        sso_buffer_[0] = '\0';
    }

    // C 风格字符串构造
    String(const char* s) {
        if (!s) {
            size_ = 0;
            sso_buffer_[0] = '\0';
            return;
        }
        size_ = std::strlen(s);
        if (is_sso()) {
            std::memcpy(sso_buffer_, s, size_ + 1);
        } else {
            capacity_ = size_;
            heap_buffer_ = new char[capacity_ + 1];
            std::memcpy(heap_buffer_, s, size_ + 1);
        }
    }

    // 拷贝构造
    String(const String& other) : size_(other.size_) {
        if (other.is_sso()) {
            std::memcpy(sso_buffer_, other.sso_buffer_, size_ + 1);
        } else {
            capacity_ = other.capacity_;
            heap_buffer_ = new char[capacity_ + 1];
            std::memcpy(heap_buffer_, other.heap_buffer_, size_ + 1);
        }
    }

    // 移动构造
    String(String&& other) noexcept : size_(other.size_) {
        if (other.is_sso()) {
            std::memcpy(sso_buffer_, other.sso_buffer_, size_ + 1);
        } else {
            capacity_ = other.capacity_;
            heap_buffer_ = other.heap_buffer_;
            other.heap_buffer_ = nullptr;
        }
        other.size_ = 0;
        other.sso_buffer_[0] = '\0';
    }

    // 析构
    ~String() {
        if (!is_sso() && heap_buffer_) {
            delete[] heap_buffer_;
            heap_buffer_ = nullptr;
        }
    }

    // 拷贝赋值
    String& operator=(const String& other) {
        if (this != &other) {
            String temp(other);
            swap(temp);
        }
        return *this;
    }

    // 移动赋值
    String& operator=(String&& other) noexcept {
        if (this != &other) {
            if (!is_sso() && heap_buffer_) {
                delete[] heap_buffer_;
            }
            size_ = other.size_;
            if (other.is_sso()) {
                std::memcpy(sso_buffer_, other.sso_buffer_, size_ + 1);
            } else {
                capacity_ = other.capacity_;
                heap_buffer_ = other.heap_buffer_;
                other.heap_buffer_ = nullptr;
            }
            other.size_ = 0;
            other.sso_buffer_[0] = '\0';
        }
        return *this;
    }

    void swap(String& other) noexcept {
        // 分四种情况或者借助临时对象转移
        String temp(std::move(other));
        other = std::move(*this);
        *this = std::move(temp);
    }

    const char* c_str() const noexcept {
        return is_sso() ? sso_buffer_ : heap_buffer_;
    }

    size_t size() const noexcept { return size_; }
    size_t length() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    char operator[](size_t index) const {
        return c_str()[index];
    }

    bool is_sso() const noexcept {
        return size_ <= SSO_CAPACITY;
    }

    bool operator==(const String& other) const noexcept {
        if (size_ != other.size_) return false;
        return std::strcmp(c_str(), other.c_str()) == 0;
    }

    bool operator!=(const String& other) const noexcept {
        return !(*this == other);
    }

private:
    size_t size_{0};
    union {
        struct {
            char* heap_buffer_;
            size_t capacity_;
        };
        char sso_buffer_[SSO_CAPACITY + 1];
    };
};

inline std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.c_str();
}

} // namespace aq
