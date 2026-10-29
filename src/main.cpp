/**
 * @file main.cpp
 * @brief C++ 核心机制与手写组件综合单元测试与运行演示
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "memory/smart_ptr.hpp"
#include "memory/memory_pool.hpp"
#include "concurrency/thread_pool.hpp"
#include "concurrency/lockfree_queue.hpp"
#include "structures/my_string.hpp"
#include "structures/lru_cache.hpp"
#include "structures/skip_list.hpp"

void test_smart_ptr() {
    std::cout << "[Testing Smart Pointers]...";
    // Test UniquePtr
    {
        auto uptr = aq::make_unique<int>(42);
        assert(*uptr == 42);
        auto uptr2 = std::move(uptr);
        assert(!uptr);
        assert(*uptr2 == 42);
    }

    // Test SharedPtr
    {
        auto s1 = aq::make_shared<std::string>("hello world");
        assert(s1.use_count() == 1);
        {
            auto s2 = s1;
            assert(s1.use_count() == 2);
            assert(*s2 == "hello world");
        }
        assert(s1.use_count() == 1);
    }
    std::cout << " PASSED\n";
}

void test_memory_pool() {
    std::cout << "[Testing Memory Pool]...";
    struct Item {
        int id;
        double score;
        Item(int i, double s) : id(i), score(s) {}
    };

    aq::ObjectPool<Item> pool;
    Item* item1 = pool.construct(101, 99.5);
    Item* item2 = pool.construct(102, 88.0);
    assert(item1->id == 101);
    assert(item2->id == 102);
    pool.destroy(item1);
    pool.destroy(item2);
    std::cout << " PASSED\n";
}

void test_thread_pool() {
    std::cout << "[Testing Thread Pool]...";
    aq::ThreadPool pool(4);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool.enqueue([i]() {
                return i * i;
            })
        );
    }

    for (int i = 0; i < 8; ++i) {
        assert(results[i].get() == i * i);
    }
    std::cout << " PASSED\n";
}

void test_lockfree_queue() {
    std::cout << "[Testing Lockfree Queue (SPSC)]...";
    aq::SPSCQueue<int, 16> queue;
    for (int i = 0; i < 10; ++i) {
        bool ok = queue.push(i);
        assert(ok);
    }
    for (int i = 0; i < 10; ++i) {
        int val = -1;
        bool ok = queue.pop(val);
        assert(ok && val == i);
    }
    std::cout << " PASSED\n";
}

void test_string() {
    std::cout << "[Testing Custom String with SSO]...";
    aq::String s1("short");
    assert(s1.is_sso());
    assert(s1.size() == 5);

    aq::String s2("this is a very long string designed to bypass the small string optimization buffer");
    assert(!s2.is_sso());
    assert(s2.size() > 15);

    aq::String s3 = s2;
    assert(s3 == s2);
    std::cout << " PASSED\n";
}

void test_lru_cache() {
    std::cout << "[Testing LRU Cache]...";
    aq::LRUCache<int, std::string> cache(2);
    cache.put(1, "one");
    cache.put(2, "two");
    assert(cache.get(1).value() == "one");
    cache.put(3, "three"); // 淘汰 2
    assert(!cache.get(2).has_value());
    assert(cache.get(3).value() == "three");
    std::cout << " PASSED\n";
}

void test_skip_list() {
    std::cout << "[Testing Skip List]...";
    aq::SkipList<int, std::string> sl;
    sl.insert(10, "ten");
    sl.insert(20, "twenty");
    sl.insert(5, "five");

    assert(sl.search(10).value() == "ten");
    assert(sl.search(20).value() == "twenty");
    assert(!sl.search(100).has_value());

    assert(sl.erase(10));
    assert(!sl.search(10).has_value());
    std::cout << " PASSED\n";
}

int main() {
    std::cout << "=== Running C++ Core Components & Verification Suite ===\n";
    test_smart_ptr();
    test_memory_pool();
    test_thread_pool();
    test_lockfree_queue();
    test_string();
    test_lru_cache();
    test_skip_list();
    std::cout << "All C++ module tests passed successfully!\n";
    return 0;
}
