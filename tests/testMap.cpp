/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Balazs Toth.

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

#include <ETL/Map.h>

#include "ContainerTester.h"
#include "DummyAllocator.h"

using ETL_NAMESPACE::Test::ContainerTester;
using ETL_NAMESPACE::Test::DummyAllocator;


TEST_CASE("Etl::Map<> basic test", "[map][etl][basic]") {

    Etl::Map<uint32_t, ContainerTester> map;

    REQUIRE(map.isEmpty());
    REQUIRE(map.getSize() == 0);

    ContainerTester a(4);
    map.insertOrAssign(4, a);

    REQUIRE_FALSE(map.isEmpty());
    REQUIRE(map.getSize() == 1);
    REQUIRE(map[4].getValue() == a.getValue());

    map.insert(5, ContainerTester(-5));

    REQUIRE(map.getSize() == 2);

    ContainerTester b(-4);
    map.insertOrAssign(4, b);

    REQUIRE(map.getSize() == 2);
    REQUIRE(map[4].getValue() == b.getValue());

    map.erase(5);

    REQUIRE(map.getSize() == 1);
}


TEST_CASE("Etl::Map<> insert test", "[map][etl]") {

    typedef Etl::Map<int, uint32_t> MapType;
    typedef std::pair<MapType::Iterator, bool> ResultType;

    MapType map;

    ResultType res = map.insert(1, 2);

    REQUIRE(res.second == true);
    REQUIRE(res.first != map.end());
    REQUIRE(map.getSize() == 1);

    SECTION("first element") {

        REQUIRE(res.first->getKey() == 1);
        REQUIRE(res.first->getElement() == 2);
    }

    SECTION("second element") {

        res = map.insert(2, 2);

        REQUIRE(res.second == true);
        REQUIRE(map.getSize() == 2);
        REQUIRE(map[2] == 2);
    }

    SECTION("insert() of existing shall fail") {

        res = map.insert(1, 3);

        REQUIRE(res.second == false);
        REQUIRE(map.getSize() == 1);
        REQUIRE(map[1] == 2);
    }

    SECTION("insertOrAssign() of existing shall overwrite") {

        res = map.insertOrAssign(1, 3);

        REQUIRE(res.second == false);
        REQUIRE(map.getSize() == 1);
        REQUIRE(map[1] == 3);
    }
}


TEST_CASE("Etl::Map<> erase tests", "[map][etl]") {

    typedef Etl::Map<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    CHECK(map.getSize() == 4);

    SECTION("erase(Key)") {

        map.erase(2);

        REQUIRE(map.getSize() == 3);
        REQUIRE(map.find(2) == map.end());
    }

    SECTION("erase(Iterator)") {

        MapType::Iterator it = map.find(2);
        CHECK(it != map.end());

        it = map.erase(it);

        REQUIRE(map.getSize() == 3);
        REQUIRE(map.find(2) == map.end());
        REQUIRE(it == map.find(3));
    }
}


TEST_CASE("Etl::Map<> iteration tests", "[map][etl]") {

    typedef Etl::Map<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    CHECK(map.getSize() == 4);

    SECTION("forward") {

        MapType::Iterator it = map.begin();

        REQUIRE(it->getKey() == 1);
        REQUIRE(it->getElement() == -1);

        ++it;

        REQUIRE(it->getKey() == 2);
        REQUIRE(it->getElement() == -2);
    }

    SECTION("backward") {

        MapType::Iterator it = map.end();

        --it;

        REQUIRE(it->getKey() == 4);
        REQUIRE(it->getElement() == -4);

        --it;

        REQUIRE(it->getKey() == 3);
        REQUIRE(it->getElement() == -3);
    }
}


TEST_CASE("Etl::Map<> element order", "[map][etl]") {

    typedef Etl::Map<int, uint32_t> MapType;

    MapType map;

    map.insert(3, -3);
    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(4, -4);

    CHECK(map.getSize() == 4);

    MapType::Iterator it = map.begin();

    REQUIRE(it->getKey() == 1);
    REQUIRE(it->getElement() == -1);

    ++it;
    REQUIRE(it->getKey() == 2);
    REQUIRE(it->getElement() == -2);

    ++it;
    REQUIRE(it->getKey() == 3);
    REQUIRE(it->getElement() == -3);

    ++it;
    REQUIRE(it->getKey() == 4);
    REQUIRE(it->getElement() == -4);

    ++it;
    REQUIRE(it == map.end());
}


TEST_CASE("Etl::Map<> association tests", "[map][etl]") {

    typedef Etl::Map<uint32_t, ContainerTester> MapType;

    MapType map;

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.getSize() == 4);

    SECTION("write existing") {

        map[4] = ContainerTester(-5);
        REQUIRE(map.find(4)->getElement() == ContainerTester(-5));
    }

    SECTION("write new") {

        map[5] = ContainerTester(-5);
        REQUIRE(map.find(5)->getElement() == ContainerTester(-5));
    }

    SECTION("read existing") {

        REQUIRE(map[4] == ContainerTester(-4));
    }

    SECTION("read new - default insertion") {

        REQUIRE(map[5] == ContainerTester());
    }
}


TEST_CASE("Etl::Map<> copy", "[map][etl]") {

    typedef Etl::Map<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    MapType map2;

    map2.insert(1, 1);
    map2.insert(5, -5);

    CHECK(map.getSize() == 4);
    CHECK(map2.getSize() == 2);

    SECTION("copy assignment") {

        map2 = map;

        REQUIRE(map2.getSize() == 4);
        REQUIRE(map2[1] == map[1]);
        REQUIRE(map2[4] == map[4]);
    }

    SECTION("copy constructor") {

        MapType map3 = map;

        REQUIRE(map3.getSize() == 4);
        REQUIRE(map3[1] == map[1]);
        REQUIRE(map3[4] == map[4]);
    }

    SECTION("copyElementsFrom()") {

        map2.copyElementsFrom(map);

        REQUIRE(map2.getSize() == 5);
        REQUIRE(map2[1] == map[1]);
        REQUIRE(map2[4] == map[4]);
        REQUIRE(map2[5] == -5);
    }

    SECTION("swap()") {

        map.swap(map2);

        REQUIRE(map2.getSize() == 4);
        REQUIRE(map.getSize() == 2);

        REQUIRE(map[1] == 1);
        REQUIRE(map[5] == -5);

        REQUIRE(map2[1] == -1);
        REQUIRE(map2[4] == -4);
    }
}


TEST_CASE("Etl::Map<> search tests", "[map][etl]") {

    typedef Etl::Map<uint32_t, ContainerTester> MapType;
    MapType map;

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.getSize() == 4);

    SECTION("find(Key)") {

        MapType::Iterator it = map.find(3);

        REQUIRE(it != map.end());
        REQUIRE(it->getKey() == 3);
        REQUIRE(it->getElement() == ContainerTester(-3));
    }

    SECTION("find(Key) non-existing") {

        MapType::Iterator it = map.find(7);

        REQUIRE(it == map.end());
    }

    SECTION("find(ItemMatcher)") {

        struct Matcher : MapType::ItemMatcher {
            virtual bool call(const MapType::ItemType& item) const {
                return (item.getKey() == 3) && (item.getElement().getValue() == -3);
            }
        };

        Matcher matchCall;

        MapType::Iterator it = map.find(matchCall);
        REQUIRE(it->getKey() == 3);

        ++it;
        it = map.find(it, map.end(), matchCall);
        REQUIRE(it == map.end());
    }
}


TEST_CASE("Etl::Map<> allocator test", "[map][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Map<uint32_t, ItemType, DummyAllocator> MapType;
    typedef MapType::Allocator AllocatorType;

    AllocatorType::reset();
    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    MapType map;
    map.insert(5, ContainerTester(-5));

    MapType::Iterator it = map.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    map.insert(6, ContainerTester(-6));
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    map.erase(5);
    REQUIRE(AllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Pooled::Map<> test", "[map][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Pooled::Map<uint32_t, ItemType, NUM> MapType;

    MapType map;

    SECTION("Basic allocation") {

        map.insert(5, ContainerTester(-5));

        MapType::Iterator it = map.begin();
        REQUIRE(it.operator->() != NULL);

        map.insert(6, ContainerTester(-6));
        MapType::Iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            map.insert(i, ContainerTester(i));
        }

        CHECK(map.getSize() == NUM);

        std::pair<MapType::Iterator, bool> res = map.insert(NUM, ContainerTester(NUM));
        REQUIRE(map.getSize() == NUM);
        REQUIRE(res.first == map.end());
        REQUIRE(res.second == false);
    }
}


TEST_CASE("Etl::Map<> test cleanup", "[map][etl]") {

    typedef Etl::Map<uint32_t, ContainerTester, DummyAllocator> MapType;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(MapType::Allocator::getDeleteCount() == MapType::Allocator::getAllocCount());
}

