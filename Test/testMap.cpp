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


TEST_CASE("Etl::Map<> basic test", "[map][etl][basic]" ) {

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

