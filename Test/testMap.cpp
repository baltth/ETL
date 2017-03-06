/**
\file
\date 2016.03.16. 12:27:49
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

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

#include <Map.h>
#include <Test/ContainerTester.h>
#include <Test/DummyAllocator.h>


TEST_CASE("Etl::Map<> basic test", "[map][etl][basic]") {

    Etl::Map<uint32_t, ContainerTester> map;

    ContainerTester a(4);
    map.insertOrAssign(4, a);

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

TEST_CASE("Etl::Map<> insert() test", "[map][etl]") {

    typedef Etl::Map<int, uint32_t> MapType;
    typedef std::pair<MapType::Iterator, bool> ResultType;

    MapType map;

    ResultType res = map.insert(1, 1);

    REQUIRE(res.second == true);
    REQUIRE(res.first != map.end());
    REQUIRE(map.getSize() == 1);

    SECTION("insert() of existing shall fail") {

        res = map.insert(1, 2);

        REQUIRE(res.second == false);
        REQUIRE(map.getSize() == 1);
        REQUIRE(map[1] == 1);
    }

    SECTION("insertOrAssign() of existing shall overwrite") {

        res = map.insertOrAssign(1, 2);

        REQUIRE(res.second == false);
        REQUIRE(map.getSize() == 1);
        REQUIRE(map[1] == 2);
    }
}

TEST_CASE("Etl::Map<> search tests", "[map][etl]") {

    typedef Etl::Map<uint32_t, ContainerTester> MapType;
    MapType map;

    class Matcher : public MapType::ElementMatcher {
        virtual bool call(const ContainerTester& c) const {
            return (c.getValue() >= -3) && (c.getValue() <= -2);
        }
    };

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.getSize() == 4);

    SECTION("find(ElementMatcher) method") {

        Matcher matchCall;

        MapType::Iterator it = map.find(matchCall);
        REQUIRE(it->getKey() == 2);

        ++it;
        it = map.find(it, map.end(), matchCall);
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

    map.erase(6);
    REQUIRE(AllocatorType::getDeleteCount() == 2);

}

TEST_CASE("Etl::Map<> test cleanup", "[map][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}

