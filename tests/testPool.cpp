/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017-2023 Balazs Toth.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
\endparblock
*/

#include <catch2/catch.hpp>

#include <etl/List.h>
#include <etl/MemoryPool.h>
#include <etl/PoolAllocator.h>
#include <etl/UnorderedSet.h>
#include <etl/Vector.h>

#include <mutex>
#include <thread>

namespace {

TEST_CASE("Etl::MemoryPool<> basic test", "[pool][etl][basic]") {

    Etl::MemoryPool<16, 16> pool;

    auto freeCnt = pool.getFreeCount();
    REQUIRE(pool.capacity() == freeCnt);
    REQUIRE(freeCnt == 16);
    REQUIRE(pool.getCount() == 0);

    void* item0 = NULL;
    item0 = pool.pop();
    REQUIRE(item0 != NULL);
    --freeCnt;
    REQUIRE(pool.getFreeCount() == freeCnt);
    REQUIRE(pool.getCount() == (pool.capacity() - freeCnt));

    void* item1 = NULL;
    item1 = pool.pop();
    REQUIRE(item1 != NULL);
    --freeCnt;
    REQUIRE(pool.getFreeCount() == freeCnt);
    REQUIRE(pool.getCount() == (pool.capacity() - freeCnt));

    REQUIRE(pool.push(item0));
    ++freeCnt;
    REQUIRE(pool.getFreeCount() == freeCnt);
    REQUIRE(pool.getCount() == (pool.capacity() - freeCnt));
}


TEST_CASE("Etl::MemoryPool<> alloc fail", "[pool][etl]") {

    Etl::MemoryPool<16, 8> pool;
    Etl::Array<void*, 8> items;

    items.fill(NULL);

    for (auto it = items.begin(); it < items.end(); ++it) {
        CAPTURE(pool.getFreeCount());
        *it = pool.pop();
        CHECK(*it != NULL);
    }

    REQUIRE(pool.getCount() == pool.capacity());
    REQUIRE(pool.pop() == NULL);

    for (auto it = items.begin(); it < items.end(); ++it) {
        CAPTURE(pool.getFreeCount());
        CAPTURE(*it);
        CHECK(pool.push(*it));
    }

    CHECK(pool.getFreeCount() == pool.capacity());
    CHECK(pool.getCount() == 0);
}


TEST_CASE("Etl::MemoryPool<> free fail", "[pool][etl]") {

    Etl::MemoryPool<16, 8> pool;
    void* item0 = NULL;
    void* item1 = static_cast<void*>(&item0);

    REQUIRE_FALSE(pool.push(item0));
    REQUIRE_FALSE(pool.push(item1));

    void* item2 = pool.pop();

    REQUIRE_FALSE(pool.push(item0));
    REQUIRE_FALSE(pool.push(item1));

    REQUIRE(pool.push(item2));
}


template<size_t S, uint32_t N>
void testMemoryPoolRandomAccess() {

    REQUIRE(N >= 8);

    typedef Etl::Array<void*, N> Items;

    Etl::MemoryPool<S, N> pool;
    Items items;

    items.fill(NULL);

    for (typename Items::iterator it = items.begin(); it < items.end(); ++it) {
        *it = pool.pop();
    }

    void* item4 = items[4];
    void* item6 = items[6];
    void* item7 = items[7];

    CAPTURE(pool.getFreeCount());
    REQUIRE(pool.push(items[6]));

    CAPTURE(pool.getFreeCount());
    REQUIRE(pool.push(items[7]));

    CAPTURE(pool.getFreeCount());
    items[7] = pool.pop();
    REQUIRE(items[7] != NULL);

    CAPTURE(pool.getFreeCount());
    REQUIRE(pool.push(items[4]));

    CAPTURE(pool.getFreeCount());
    items[6] = pool.pop();
    REQUIRE(items[6] != NULL);

    CAPTURE(pool.getFreeCount());
    items[4] = pool.pop();
    REQUIRE(items[4] != NULL);

    REQUIRE(items[4] != items[6]);
    REQUIRE(items[4] != items[7]);
    REQUIRE(items[6] != items[4]);
    REQUIRE(items[6] != items[7]);
    REQUIRE(items[7] != items[4]);
    REQUIRE(items[7] != items[6]);

    REQUIRE(((items[4] == item4) || (items[4] == item6) || (items[4] == item7)));
    REQUIRE(((items[6] == item4) || (items[6] == item6) || (items[6] == item7)));
    REQUIRE(((items[7] == item4) || (items[7] == item6) || (items[7] == item7)));
}

TEST_CASE("Etl::MemoryPool<> random access", "[pool][etl][basic]") {

    SECTION("for size 16") {
        testMemoryPoolRandomAccess<16, 8>();
    }

    SECTION("for size 17") {
        testMemoryPoolRandomAccess<17, 8>();
    }

    SECTION("for size 1") {
        testMemoryPoolRandomAccess<1, 8>();
    }
}


TEST_CASE("Etl::PoolAllocator basic tests", "[poolallocator][etl][basic]") {

    static const std::size_t SIZE {16U};
    Etl::PoolHelperForSize<SIZE>::Allocator<int> pool;

    REQUIRE(pool.max_size() == SIZE);
    REQUIRE(pool.size() == 0);

    Etl::Dynamic::Vector<int*> allocated;

    for (size_t i = 0; i < pool.max_size(); ++i) {

        allocated.push_back(pool.allocate(1));

        REQUIRE(allocated.back() != nullptr);
        REQUIRE(pool.size() == (i + 1));
    }

    REQUIRE(pool.allocate(1) == nullptr);

    Etl::Dynamic::UnorderedSet<int*> uniqueElements;
    uniqueElements.insert(allocated.begin(), allocated.end());
    REQUIRE(uniqueElements.size() == SIZE);

    for (auto ptr : allocated) {
        pool.deallocate(ptr, 1);
    }

    REQUIRE(pool.size() == 0);
}


TEST_CASE("Etl::CommonPoolAllocator basic tests", "[poolallocator][etl][basic]") {

    static const std::size_t SIZE {16U};
    Etl::PoolHelperForSize<SIZE>::CommonAllocator<int> pool1;
    Etl::PoolHelperForSize<SIZE>::CommonAllocator<int> pool2;

    REQUIRE(pool1.max_size() == SIZE);
    REQUIRE(pool1.size() == 0);

    REQUIRE(pool2.max_size() == SIZE);
    REQUIRE(pool2.size() == 0);

    Etl::Dynamic::Vector<int*> allocated1;
    Etl::Dynamic::Vector<int*> allocated2;

    static const std::size_t HALF_SIZE {SIZE / 2};
    for (size_t i = 0; i < HALF_SIZE; ++i) {

        allocated1.push_back(pool1.allocate(1));
        REQUIRE(allocated1.back() != nullptr);

        allocated2.push_back(pool2.allocate(1));
        REQUIRE(allocated2.back() != nullptr);

        REQUIRE(pool1.size() == (2 * (i + 1)));
        REQUIRE(pool2.size() == pool1.size());
    }

    REQUIRE(pool1.allocate(1) == nullptr);
    REQUIRE(pool2.allocate(1) == nullptr);

    Etl::Dynamic::UnorderedSet<int*> uniqueElements;
    uniqueElements.insert(allocated1.begin(), allocated1.end());
    uniqueElements.insert(allocated2.begin(), allocated2.end());
    REQUIRE(uniqueElements.size() == SIZE);

    // Release to the other pool
    for (auto ptr : allocated1) {
        pool2.deallocate(ptr, 1);
    }

    for (auto ptr : allocated2) {
        pool1.deallocate(ptr, 1);
    }

    REQUIRE(pool1.size() == 0);
    REQUIRE(pool2.size() == 0);
}


TEST_CASE("Etl::CommonPoolAllocator concurrency test", "[poolallocator][etl][basic]") {

    static const std::size_t NUM_THREADS {10000U};
    static const std::size_t SIZE {100000U};

    Etl::Dynamic::UnorderedSet<int*> uniqueElements;
    std::mutex mx {};

    auto proc = [&mx, &uniqueElements]() {
        Etl::PoolHelperForSize<SIZE>::CommonAllocator<int> pool;
        for (std::size_t i = 0; i < (SIZE / NUM_THREADS); ++i) {

            auto* p = pool.allocate(1);
            ETL_ASSERT(p != nullptr);

            auto lg = Etl::Detail::lock(mx);
            uniqueElements.insert(p);
        }
    };

    Etl::Dynamic::List<std::thread> threads;
    for (std::size_t i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(proc);
    }

    CHECK(threads.size() == NUM_THREADS);
    for (auto& t : threads) {
        t.join();
    }

    REQUIRE(uniqueElements.size() == SIZE);
}

}  // namespace
