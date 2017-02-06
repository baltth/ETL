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

#include <iostream>

#include "Map.h"
#include "Test/ContainerTester.h"

#include "testMap.h"


void testMap() {

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
    numMap.emplace(5, -5);
    printMap();
    ContainerTester b(-4);
    numMap.insertOrAssign(4, b);
    printMap();
    numMap.emplace(2, -2);
    printMap();
    numMap.emplace(3, -3);
    printMap();
    numMap.erase(3);
    printMap();

    std::cout << "[4]" << std::endl;
    numMap[4].reportValue();
    ContainerTester &c = numMap[4];
    c.reportValue();

    std::cout << std::endl;
}

