/**
 * @file coroutine_task.hpp
 * @brief 现代 C++20 无栈协程 (Coroutine Task) 核心实现与 Awaiter 机制
 */

#pragma once

#if __cplusplus >= 202002L || defined(__cpp_impl_coroutine)

#include <coroutine>
#include <exception>
#include <utility>

namespace aq {

template <typename T>
class Task {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        T value_;
        std::exception_ptr exception_;

        Task get_return_object() {
            return Task(handle_type::from_promise(*this));
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(T value) {
            value_ = std::move(value);
        }

        void unhandled_exception() {
            exception_ = std::current_exception();
        }
    };

    explicit Task(handle_type h) : handle_(h) {}
    ~Task() {
        if (handle_) handle_.destroy();
    }

    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    T get() {
        if (!handle_.done()) {
            handle_.resume();
        }
        if (handle_.promise().exception_) {
            std::rethrow_exception(handle_.promise().exception_);
        }
        return std::move(handle_.promise().value_);
    }

private:
    handle_type handle_;
};

} // namespace aq

#endif
