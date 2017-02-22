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

#include "Map.h"
#include "Test/ContainerTester.h"


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

/*
TEST_CASE("Map test", "[map]" ) {

    Etl::Map<uint32_t, ContainerTester> numMap;

    auto printMap = [&numMap]() {

        std::cout << std::endl << "Map<uint32_t, ContainerTester> test:" << std::endl;

        for(auto &item : numMap) {
            std::cout << item.getKey() << ": ";
            item.getElement().reportValue();
        }

        std::cout << std::endl;
    };

    ContainerTester a(4);
    numMap.insertOrAssign(4, a);
    printMap();

    REQUIRE(numMap.getSize() == 1);
    REQUIRE(numMap[4].getValue() == a.getValue());

    numMap.emplace(5, -5);
    printMap();

    REQUIRE(numMap.getSize() == 2);

    ContainerTester b(-4);
    numMap.insertOrAssign(4, b);
    printMap();

    REQUIRE(numMap.getSize() == 2);
    REQUIRE(numMap[4].getValue() == b.getValue());

    numMap.emplace(2, -2);
    printMap();
    numMap.emplace(3, -3);
    printMap();

    REQUIRE(numMap.getSize() == 4);

    numMap.erase(3);
    printMap();

    REQUIRE(numMap.getSize() == 3);

}
*/

