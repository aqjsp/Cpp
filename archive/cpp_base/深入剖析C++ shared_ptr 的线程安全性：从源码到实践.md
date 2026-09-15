# 深入剖析C++ `shared_ptr` 的线程安全性：从源码到实践

先说结论：

> - `std::shared_ptr` 是"半线程安全"的：它保证控制块的操作是并发安全的，所以不同的 `shared_ptr` 实例（即便它们共享同一个对象）可以在多个线程上并发地复制/销毁而不会导致控制块破坏或双重删除。
> - 但对同一个 `shared_ptr` 对象实例（即同一内存位置）在多个线程上没有同步就同时进行写操作（例如同时 `reset()`、赋值）会产生数据竞争 —— 这是未定义行为。你必须对同一实例的并发访问加锁或使用原子接口。

现代C++编程中，智能指针 `std::shared_ptr` 极大地简化了内存管理，有效避免了内存泄漏和悬空指针等问题。

然而，关于 `shared_ptr` 的线程安全性，许多开发者存在误解。它究竟是完全线程安全的，还是部分线程安全的？

本文将从 `shared_ptr` 的源码实现角度出发，结合实例和图表，详细解析其线程安全性，助力大家在多线程环境中更安全、高效地使用 `shared_ptr`。

## `shared_ptr` 的核心机制：控制块与原子操作

`std::shared_ptr` 的强大之处在于其内部的引用计数机制。每个 `shared_ptr` 实例都指向一个控制块，这个控制块存储着关键信息：

*   强引用计数：记录有多少个 `shared_ptr` 实例正在管理对象。
*   弱引用计数：记录有多少个 `std::weak_ptr` 实例正在观察对象。
*   原始指针：指向被管理的对象。
*   自定义删除器（Deleter）和分配器（Allocator）。

关键在于，`shared_ptr` 保证了对这些引用计数的增减操作是线程安全的。C++标准要求 `shared_ptr` 的实现必须使用原子操作来修改引用计数。

也就是说，即使多个线程同时对同一个 `shared_ptr` 实例进行拷贝、赋值或销毁操作，引用计数也能正确地增加或减少，不会出现数据竞争导致计数错误的问题。这通常通过 `std::atomic<long>` 或类似的原子类型来实现，例如 `fetch_add` 和 `fetch_sub` 等原子函数。

下图展示了 `shared_ptr` 及其控制块的内部结构：

![shared_ptr 控制块结构](https://cdn.jsdelivr.net/gh/aqjsp/photos/shared_ptr_control_block.png)

## `shared_ptr` 源码剖析：控制块的实现

为了更深入地理解 `shared_ptr` 如何实现引用计数的线程安全，我们来看一下GCC `libstdc++` 中 `shared_ptr` 内部控制块（`_Sp_counted_base`）的真实源码片段。这个基类负责管理强引用计数（`_M_use_count`）和弱引用计数（`_M_weak_count`）。

```cpp
// 基于 GCC libstdc++ (shared_ptr_base.h) 的真实实现
template<_Lock_policy _Lp>
class _Sp_counted_base : public _Mutex_base<_Lp>
{
public:
    // 构造函数，初始化引用计数
    _Sp_counted_base() noexcept
    : _M_use_count(1), _M_weak_count(1) { }

    virtual ~_Sp_counted_base() noexcept { }

    // 纯虚函数：销毁被管理的对象
    virtual void _M_dispose() noexcept = 0;

    // 虚函数：销毁控制块自身
    virtual void _M_destroy() noexcept { delete this; }

    // 纯虚函数：获取删除器
    virtual void* _M_get_deleter(const std::type_info&) noexcept = 0;

    // 增加强引用计数（原子操作）
    void _M_add_ref_copy() noexcept
    { 
        __gnu_cxx::__atomic_add_dispatch(&_M_use_count, 1); 
    }

    // 尝试增加强引用计数（用于weak_ptr::lock()）
    void _M_add_ref_lock();

    // 减少强引用计数（原子操作）
    void _M_release() noexcept
    {
        // 使用 exchange_and_add 进行原子递减并获取旧值
        _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(&_M_use_count);
        if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, -1) == 1)
        {
            _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&_M_use_count);
            _M_dispose(); // 销毁被管理对象
            
            // 内存屏障确保dispose()的效果在destroy()之前可见
            if (_Mutex_base<_Lp>::_S_need_barriers)
            {
                _GLIBCXX_READ_MEM_BARRIER;
                _GLIBCXX_WRITE_MEM_BARRIER;
            }

            // 递减弱引用计数
            _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(&_M_weak_count);
            if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
            {
                _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&_M_weak_count);
                _M_destroy(); // 销毁控制块自身
            }
        }
    }

    // 增加弱引用计数（原子操作）
    void _M_weak_add_ref() noexcept
    { 
        __gnu_cxx::__atomic_add_dispatch(&_M_weak_count, 1); 
    }

    // 减少弱引用计数（原子操作）
    void _M_weak_release() noexcept
    {
        _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(&_M_weak_count);
        if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
        {
            _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(&_M_weak_count);
            if (_Mutex_base<_Lp>::_S_need_barriers)
            {
                _GLIBCXX_READ_MEM_BARRIER;
                _GLIBCXX_WRITE_MEM_BARRIER;
            }
            _M_destroy(); // 销毁控制块自身
        }
    }

    // 获取强引用计数（原子读取）
    long _M_get_use_count() const noexcept
    { 
        // 使用 volatile 确保每次都从内存读取
        return const_cast<const volatile _Atomic_word&>(_M_use_count);
    }

private:
    // 禁止拷贝构造和赋值
    _Sp_counted_base(_Sp_counted_base const&);
    _Sp_counted_base& operator=(_Sp_counted_base const&);

    _Atomic_word _M_use_count;  // 强引用计数 (#shared)
    _Atomic_word _M_weak_count; // 弱引用计数 (#weak + (#shared != 0))
};
```

### 关键技术细节

1. `_Atomic_word _M_use_count;` 和 `_Atomic_word _M_weak_count;`：
   这是 `shared_ptr` 引用计数线程安全的核心。在GCC的实现中，强引用计数和弱引用计数被声明为 `_Atomic_word` 类型。`_Atomic_word` 是GCC内部定义的一个类型，它本质上是一个整数类型，但编译器会确保对它的操作是原子的。这等同于C++11及更高版本中的 `std::atomic<long>`。

2. `__gnu_cxx::__atomic_add_dispatch()` 和 `__gnu_cxx::__exchange_and_add_dispatch()`：
   - `_M_add_ref_copy()` 和 `_M_weak_add_ref()` 函数使用 `__gnu_cxx::__atomic_add_dispatch(&count, 1)` 来原子性地增加引用计数。这是一个GCC内部的原子操作函数，它会根据平台和编译选项选择最高效的原子指令（例如 `fetch_add`）。
   - `_M_release()` 和 `_M_weak_release()` 函数中，引用计数通过 `__gnu_cxx::__exchange_and_add_dispatch(&count, -1)` 进行原子递减并返回递减前的值。当返回值是1时，表示这是最后一个引用，从而触发资源的释放或控制块的销毁。
   - `_M_get_use_count()` 函数使用 `const_cast<const volatile _Atomic_word&>(_M_use_count)` 来原子性地读取引用计数。这确保了即使在多线程环境下，读取到的计数也是一个完整且一致的值。

3. 内存屏障和同步：
   - 在真实的GCC实现中，使用了 `_GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE` 和 `_GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER` 宏来标记同步点，这些主要用于线程安全检测工具（如ThreadSanitizer）。
   - `_Mutex_base<_Lp>::_S_need_barriers` 检查是否需要内存屏障。当需要时，会插入读写内存屏障（`_GLIBCXX_READ_MEM_BARRIER` 和 `_GLIBCXX_WRITE_MEM_BARRIER`）来确保 `_M_dispose()` 的效果在 `_M_destroy()` 之前对所有线程可见。

4. `_M_dispose()` 和 `_M_destroy()`：
   - 当 `_M_use_count` 减为0时，`_M_release()` 会调用纯虚函数 `_M_dispose()` 来销毁被 `shared_ptr` 管理的原始对象。这个操作只会在最后一个强引用消失时发生，因此是线程安全的。
   - 当 `_M_weak_count` 减为0时，`_M_release()` 或 `_M_weak_release()` 会调用虚函数 `_M_destroy()` 来销毁控制块自身。这个操作只会在最后一个弱引用（包括强引用转换为弱引用）消失时发生，也是线程安全的。

5. `_M_add_ref_lock()` 函数：
   这是一个关键函数，用于 `weak_ptr::lock()` 操作。它尝试原子性地增加强引用计数，但只有在当前强引用计数不为0时才成功。如果强引用计数已经为0（对象已被销毁），则抛出 `bad_weak_ptr` 异常。

6. 继承自 `_Mutex_base<_Lp>`：
   `_Sp_counted_base` 继承自 `_Mutex_base<_Lp>`，这个基类根据锁策略（`_Lock_policy`）提供不同的同步机制。对于原子操作策略（`_S_atomic`），它不需要额外的互斥锁；对于互斥锁策略（`_S_mutex`），它提供互斥锁保护。

## `shared_ptr` 的线程安全性边界

理解 `shared_ptr` 的线程安全性，需要区分两个层面：

1.  `shared_ptr` 自身的生命周期管理（引用计数）是线程安全的。
    *   多个线程可以同时拷贝、赋值、销毁同一个 `shared_ptr` 对象，其内部的引用计数会通过原子操作正确维护。
    *   当最后一个 `shared_ptr` 实例被销毁时，它所管理的资源（即原始对象）会被安全地释放，这个过程也是线程安全的。

2.  `shared_ptr` 所管理的原始对象本身的访问不是线程安全的。
    *   `shared_ptr` 仅仅保证了它对内存的管理是线程安全的，但它不保证多个线程同时访问或修改其内部指向的原始对象时不会发生数据竞争。
    *   如果原始对象的数据成员在多线程环境下被并发修改，仍然需要程序员自行添加同步机制（如 `std::mutex`、读写锁等）来保护对原始对象的访问。

下图概括了 `shared_ptr` 在多线程环境下的线程安全性概览：

![shared_ptr 线程安全性概览](https://cdn.jsdelivr.net/gh/aqjsp/photos/shared_ptr_thread_safety_overview.png)

## 实例：验证 `shared_ptr` 的线程安全性

为了更直观地理解上述概念，我们通过C++代码进行验证。

演示三个场景：

1.  引用计数的线程安全性：多个线程同时拷贝 `shared_ptr`，观察引用计数是否正确。
2.  对象访问的非线程安全性（无锁）：多个线程同时修改 `shared_ptr` 管理的同一个对象，但不加锁，观察是否出现数据竞争。
3.  对象访问的线程安全性（有锁）：多个线程通过互斥锁保护，安全地修改 `shared_ptr` 管理的同一个对象。

```cpp
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <cassert>

// 模拟一个简单的资源类，用于shared_ptr管理
class MyResource {
public:
    int value;
    MyResource(int v = 0) : value(v) {
        std::cout << "MyResource created with value: " << value << "\n";
    }
    ~MyResource() {
        std::cout << "MyResource destroyed with value: " << value << "\n";
    }
    void increment() {
        value++;
    }
};

// 场景1: 验证shared_ptr引用计数的线程安全性
void test_ref_count_thread_safety() {
    std::cout << "\n--- 场景1: 验证shared_ptr引用计数的线程安全性 ---\n";
    std::shared_ptr<MyResource> shared_res = std::make_shared<MyResource>(100);
    std::cout << "Initial use_count: " << shared_res.use_count() << "\n";

    const int num_threads = 5;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([shared_res]() {
            // 每个线程都拷贝一份shared_ptr，增加引用计数
            std::shared_ptr<MyResource> local_res = shared_res;
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 模拟一些工作
            std::cout << "Thread " << std::this_thread::get_id() << " use_count: " << local_res.use_count() << "\n";
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    std::cout << "After all threads joined, final use_count: " << shared_res.use_count() << "\n";
    // 预期：初始shared_res + num_threads个local_res = 1 + num_threads
    // 但由于local_res在线程函数结束后会析构，所以最终use_count应该回到1
    assert(shared_res.use_count() == 1);
    std::cout << "Reference count test passed. shared_ptr's internal reference count is thread-safe.\n";
}

// 场景2: 验证shared_ptr管理的对象访问的非线程安全性 (无锁)
void test_object_access_non_thread_safety() {
    std::cout << "\n--- 场景2: 验证shared_ptr管理的对象访问的非线程安全性 (无锁) ---\n";
    std::shared_ptr<MyResource> shared_res = std::make_shared<MyResource>(0);
    const int num_increments_per_thread = 100000;
    const int num_threads = 4;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([shared_res, num_increments_per_thread]() {
            for (int j = 0; j < num_increments_per_thread; ++j) {
                shared_res->increment(); // 多个线程同时修改同一个对象，无锁保护
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    int expected_value = num_increments_per_thread * num_threads;
    std::cout << "Expected value: " << expected_value << "\n";
    std::cout << "Actual value: " << shared_res->value << "\n";
    if (shared_res->value != expected_value) {
        std::cout << "Object access non-thread-safety demonstrated: Data corruption occurred!\n";
    } else {
        std::cout << "Object access non-thread-safety test passed (unexpectedly, likely due to specific execution environment or low contention). Data corruption is expected in general.\n";
    }
    // assert(shared_res->value != expected_value); // 预期会失败
}

// 场景3: 验证shared_ptr管理的对象访问的线程安全性 (有锁)
void test_object_access_thread_safety_with_mutex() {
    std::cout << "\n--- 场景3: 验证shared_ptr管理的对象访问的线程安全性 (有锁) ---\n";
    std::shared_ptr<MyResource> shared_res = std::make_shared<MyResource>(0);
    std::mutex mtx;
    const int num_increments_per_thread = 100000;
    const int num_threads = 4;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([shared_res, &mtx, num_increments_per_thread]() {
            for (int j = 0; j < num_increments_per_thread; ++j) {
                std::lock_guard<std::mutex> lock(mtx);
                shared_res->increment(); // 多个线程通过互斥锁保护访问同一个对象
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    int expected_value = num_increments_per_thread * num_threads;
    std::cout << "Expected value: " << expected_value << "\n";
    std::cout << "Actual value: " << shared_res->value << "\n";
    assert(shared_res->value == expected_value);
    std::cout << "Object access thread-safety with mutex demonstrated: Data is consistent.\n";
}

int main() {
    test_ref_count_thread_safety();
    test_object_access_non_thread_safety();
    test_object_access_thread_safety_with_mutex();
    return 0;
}
```

### 测试结果分析

运行上述代码，得到以下输出：

![运行结果](https://cdn.jsdelivr.net/gh/aqjsp/photos/image-20251007154050115.png)

结果解读：

*   场景1：引用计数在多线程环境下表现正常，最终 `use_count` 回到1，证明 `shared_ptr` 的引用计数是线程安全的。
*   场景2：在没有锁保护的情况下，多个线程同时对 `MyResource::value` 进行 `increment()` 操作，最终结果 `105734` 远小于预期值 `400000`，明确发生了数据竞争和数据损坏。这验证了 `shared_ptr` 不保证其管理的对象内容的线程安全性。
*   场景3：通过 `std::mutex` 对 `MyResource::increment()` 操作进行保护，最终结果 `400000` 与预期值一致，证明通过适当的同步机制可以确保对象访问的线程安全。

## 总结

| 特性           | `shared_ptr` 自身（引用计数） | `shared_ptr` 管理的原始对象    |
| :------------- | :---------------------------- | :----------------------------- |
| 线程安全性     | 是 (通过原子操作)             | 否 (需要用户自行保护)          |
| 保护机制       | C++标准库内部实现             | `std::mutex`, `std::atomic` 等 |
| 可能导致的问题 | 无 (若实现符合标准)           | 数据竞争、不一致状态、崩溃     |

`std::shared_ptr` 是一个强大的工具，它通过原子操作确保了自身生命周期管理（引用计数）的线程安全性。

可以放心地在不同线程间传递、复制和销毁 `shared_ptr` 实例，而无需担心引用计数出错。

但是，它不提供对其所管理对象的线程安全性保证。当多个线程需要并发访问或修改 `shared_ptr` 所指向的原始对象时，必须像处理普通指针一样，使用互斥锁（`std::mutex`）、原子变量（`std::atomic`）或其他同步原语来保护对原始对象的访问，以避免数据竞争和不确定的行为。