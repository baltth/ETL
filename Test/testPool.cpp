/**
\file
\date 2017.05.07.
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Tóth Balázs.

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

#include "catch.hpp"

#include <MemoryPool.h>


TEST_CASE("Etl::MemoryPool<> basic test", "[pool][etl][basic]") {

    Etl::MemoryPool<16, 16> pool;

    uint32_t freeCnt = pool.getFreeCount();
    REQUIRE(pool.getSize() == freeCnt);
    REQUIRE(freeCnt == 16);
    REQUIRE_FALSE(pool.isEmpty());

    void* item0 = NULL;
    item0 = pool.pop();
    REQUIRE(item0 != NULL);
    --freeCnt;
    REQUIRE(pool.getFreeCount() == freeCnt);
    REQUIRE_FALSE(pool.isEmpty());

    void* item1 = NULL;
    item1 = pool.pop();
    REQUIRE(item1 != NULL);
    --freeCnt;
    REQUIRE(pool.getFreeCount() == freeCnt);

    REQUIRE(pool.push(item0));
    ++freeCnt;
    CHECK(pool.getFreeCount() == freeCnt);
}


TEST_CASE("Etl::MemoryPool<> alloc fail", "[pool][etl]") {

    Etl::MemoryPool<16, 8> pool;
    Etl::Array<void*, 8> items;

    items.fill(NULL);

    for (Etl::Array<void*, 8>::Iterator it = items.begin(); it < items.end(); ++it) {
        CAPTURE(pool.getFreeCount());
        *it = pool.pop();
        CHECK(*it != NULL);
    }

    REQUIRE(pool.isEmpty());
    REQUIRE(pool.pop() == NULL);

    for (Etl::Array<void*, 8>::Iterator it = items.begin(); it < items.end(); ++it) {
        CAPTURE(pool.getFreeCount());
        CAPTURE(*it);
        CHECK(pool.push(*it));
    }
}


TEST_CASE("Etl::MemoryPool<> free fail", "[pool][etl]") {

    Etl::MemoryPool<16, 8> pool;
    void* item0 = NULL;
    void* item1 = static_cast<void*>(&item0);

    REQUIRE(pool.push(item0) == false);
    REQUIRE(pool.push(item1) == false);

    void* item2 = pool.pop();

    REQUIRE(pool.push(item0) == false);
    REQUIRE(pool.push(item0) == false);

    REQUIRE(pool.push(item2));
}


TEST_CASE("Etl::MemoryPool<> random access", "[pool][etl][basic]") {

    Etl::MemoryPool<16, 8> pool;
    Etl::Array<void*, 8> items;

    items.fill(NULL);

    for (Etl::Array<void*, 8>::Iterator it = items.begin(); it < items.end(); ++it) {
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


TEST_CASE("Etl::PoolBase multi-pop", "[pool][etl]") {

    Etl::Array<uint8_t[8], 8> data;
    Etl::Array<uint8_t, 8> registry;
    Etl::PoolBase base(data, registry);

    REQUIRE(base.pop(3) == NULL);
}


