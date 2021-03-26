/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2021 Balazs Toth.

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

#include <etl/Set.h>
#include <etl/UnorderedMap.h>

#include "ContainerTester.h"
#include "DummyAllocator.h"
#include "comparisionTests.h"

using Etl::Test::ContainerTester;
using Etl::Test::DummyAllocator;


TEST_CASE("Etl::Dynamic::UnorderedMap<> basic test", "[unorderedmap][etl][basic]") {

    Etl::Dynamic::UnorderedMap<uint32_t, ContainerTester> map;

    REQUIRE(map.empty());
    REQUIRE(map.size() == 0);

    ContainerTester a(4);
    map.insert(std::make_pair(4, a));

    REQUIRE_FALSE(map.empty());
    REQUIRE(map.size() == 1);

    REQUIRE(map.find(4) != map.end());
    REQUIRE(map.find(4)->first == 4);
    REQUIRE(map.find(4)->second.getValue() == a.getValue());

    REQUIRE(map[4].getValue() == a.getValue());

    map.insert(5, ContainerTester(-5));

    REQUIRE(map.size() == 2);

    ContainerTester b(-4);
    map.insert_or_assign(4, b);

    REQUIRE(map.size() == 2);
    REQUIRE(map[4].getValue() == b.getValue());

    map.erase(5);

    REQUIRE(map.size() == 1);
}


TEST_CASE("Etl::Dynamic::UnorderedMap<> insert test", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<int, uint32_t> MapType;

    MapType map;

    auto res = map.insert(1, 2);

    REQUIRE(res.second == true);
    REQUIRE(res.first != map.end());
    REQUIRE(map.size() == 1);

    SECTION("first element") {

        REQUIRE(res.first->first == 1);
        REQUIRE(res.first->second == 2);
    }

    SECTION("second element") {

        res = map.insert(2, 2);

        REQUIRE(res.second == true);
        REQUIRE(map.size() == 2);
        REQUIRE(map[2] == 2);
    }

    SECTION("insert() of existing shall fail") {

        res = map.insert(1, 3);

        REQUIRE(res.second == false);
        REQUIRE(map.size() == 1);
        REQUIRE(map[1] == 2);
    }

    SECTION("insert_or_assign() of existing shall overwrite") {

        res = map.insert_or_assign(1, 3);

        REQUIRE(res.second == false);
        REQUIRE(map.size() == 1);
        REQUIRE(map[1] == 3);
    }
}


TEST_CASE("Etl::Dynamic::UnorderedMap<> erase tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    CHECK(map.size() == 4);

    SECTION("erase(Key)") {

        map.erase(2);

        REQUIRE(map.size() == 3);
        REQUIRE(map.find(2) == map.end());
    }

    SECTION("erase(iterator)") {

        MapType::iterator it = map.find(2);
        CHECK(it != map.end());

        it = map.erase(it);

        REQUIRE(map.size() == 3);
        REQUIRE(map.find(2) == map.end());
        REQUIRE(it == map.find(3));
    }
}


TEST_CASE("Etl::Dynamic::UnorderedMap<> iteration tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    CHECK(map.size() == 4);

    Etl::Dynamic::Set<int> seen;

    SECTION("with range for") {

        for (auto& item : map) {
            seen.insert(item.first);
        }

        REQUIRE(seen.find(1) != seen.end());
        REQUIRE(seen.find(2) != seen.end());
        REQUIRE(seen.find(3) != seen.end());
        REQUIRE(seen.find(4) != seen.end());
    }
}


TEST_CASE("Etl::Dynamic::UnorderedMap<> association tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<uint32_t, ContainerTester> MapType;

    MapType map;

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.size() == 4);

    SECTION("write existing") {

        // ContainerTester::enablePrint = true;
        map[4] = ContainerTester(-5);
        // ContainerTester::enablePrint = false;

        CAPTURE(map.find(4)->second.toString());
        CAPTURE(ContainerTester(-5).toString());
        REQUIRE(map.find(4)->second == ContainerTester(-5));
    }

    SECTION("write new") {

        // ContainerTester::enablePrint = true;
        map[5] = ContainerTester(-5);
        // ContainerTester::enablePrint = false;

        CAPTURE(map.find(5)->second.toString());
        CAPTURE(ContainerTester(-5).toString());
        REQUIRE(map.find(5)->second == ContainerTester(-5));
    }

    SECTION("read existing") {

        REQUIRE(map[4] == ContainerTester(-4));
        REQUIRE(&(map[4]) == &(map.find(4)->second));
    }

    SECTION("read new - default insertion") {

        // ContainerTester::enablePrint = true;
        REQUIRE(map[5] == ContainerTester());
        // ContainerTester::enablePrint = false;
    }
}


#if 0
TEST_CASE("Etl::Dynamic::UnorderedMap<> copy", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    MapType map2;

    map2.insert(1, 1);
    map2.insert(5, -5);

    CHECK(map.size() == 4);
    CHECK(map2.size() == 2);

    SECTION("copy assignment") {

        map2 = map;

        REQUIRE(map2.size() == 4);
        REQUIRE(map2[1] == map[1]);
        REQUIRE(map2[4] == map[4]);
    }

    SECTION("copy constructor") {

        MapType map3 = map;

        REQUIRE(map3.size() == 4);
        REQUIRE(map3[1] == map[1]);
        REQUIRE(map3[4] == map[4]);
    }

    SECTION("swap()") {

        map.swap(map2);

        REQUIRE(map2.size() == 4);
        REQUIRE(map.size() == 2);

        REQUIRE(map[1] == 1);
        REQUIRE(map[5] == -5);

        REQUIRE(map2[1] == -1);
        REQUIRE(map2[4] == -4);
    }
}
#endif


TEST_CASE("Etl::Dynamic::UnorderedMap<> search tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<uint32_t, ContainerTester> MapType;
    MapType map;

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.size() == 4);

    SECTION("find(Key)") {

        MapType::iterator it = map.find(3);

        REQUIRE(it != map.end());
        REQUIRE(it->first == 3);
        REQUIRE(it->second == ContainerTester(-3));
    }

    SECTION("find(Key) non-existing") {

        MapType::iterator it = map.find(7);

        REQUIRE(it == map.end());
    }
}


#if 0
TEST_CASE("Etl::UnorderedMap<> custom compare tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<uint32_t, ContainerTester, std::greater<int>> MapType;
    MapType map;

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.size() == 4);
    REQUIRE(map.begin()->first == 4);
}


TEST_CASE("Etl::Custom::UnorderedMap<> allocator test", "[unorderedmap][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Custom::UnorderedMap<uint32_t, ItemType, DummyAllocator> MapType;
    typedef MapType::Allocator::Allocator AllocatorType;

    AllocatorType::reset();
    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    MapType map;
    map.insert(5, ContainerTester(-5));

    MapType::iterator it = map.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    map.insert(6, ContainerTester(-6));
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    map.erase(5);
    REQUIRE(AllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Static::UnorderedMap<> test", "[unorderedmap][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Static::UnorderedMap<uint32_t, ItemType, NUM> MapType;

    MapType map;

    SECTION("Basic allocation") {

        map.insert(5, ContainerTester(-5));

        MapType::iterator it = map.begin();
        REQUIRE(it.operator->() != NULL);

        map.insert(6, ContainerTester(-6));
        MapType::iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            map.insert(i, ContainerTester(i));
        }

        CHECK(map.size() == NUM);

        std::pair<MapType::iterator, bool> res = map.insert(NUM, ContainerTester(NUM));
        REQUIRE(map.size() == NUM);
        REQUIRE(res.first == map.end());
        REQUIRE(res.second == false);
    }
}


TEST_CASE("Etl::UnorderedMap<> test cleanup", "[unorderedmap][etl]") {

    typedef Etl::Custom::UnorderedMap<uint32_t, ContainerTester, DummyAllocator> MapType;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(MapType::Allocator::Allocator::getDeleteCount()
          == MapType::Allocator::Allocator::getAllocCount());
}


// Etl::UnorderedMap comparision tests ----------------------------------------------


TEST_CASE("Etl::UnorderedMap<> comparision", "[unorderedmap][etl]") {

    SECTION("Etl::UnorderedMap<> vs Etl::UnorderedMap<>") {

        using MapType = Etl::Dynamic::UnorderedMap<int, int>;
        using Base = Etl::UnorderedMap<int, int>;

        MapType lhs;
        MapType rhs;

        auto inserter = [](Base& cont, int val) {
            cont.emplace(val, val);
        };

        testComparision(static_cast<Base&>(lhs),
                        static_cast<Base&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::UnorderedMap<> vs Etl::Static::UnorderedMap<>") {

        using LType = Etl::Dynamic::UnorderedMap<int, int>;
        using RType = Etl::Static::UnorderedMap<int, int, 32U>;

        LType lhs;
        RType rhs;

        auto lInserter = [](LType& cont, int val) {
            cont.emplace(val, val);
        };

        auto rInserter = [](RType& cont, int val) {
            cont.emplace(val, val);
        };

        testComparision(lhs,
                        rhs,
                        lInserter,
                        rInserter);
    }
}
#endif
