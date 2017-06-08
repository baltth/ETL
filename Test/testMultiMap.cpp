﻿/**
\file
\date 2016.03.16.
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

#include <MultiMap.h>

#include "ContainerTester.h"
#include "DummyAllocator.h"


TEST_CASE("Etl::MultiMap<> basic test", "[multimap][etl][basic]") {

    typedef Etl::MultiMap<int, ContainerTester> MapType;
    MapType map;

    ContainerTester a(4);
    map.insert(4, a);

    REQUIRE(map.getSize() == 1);

    ContainerTester b(-4);
    map.insert(4, b);

    REQUIRE(map.getSize() == 2);

    map.insert(5, ContainerTester(-5));

    REQUIRE(map.getSize() == 3);

    MapType::ConstIterator it = map.find(5);
    REQUIRE(it != map.end());
    REQUIRE(it->getElement().getValue() == -5);

    uint32_t cnt = map.erase(4);

    REQUIRE(map.getSize() == 1);
    REQUIRE(cnt == 2);
}


TEST_CASE("Etl::MultiMap<> insert test", "[multimap][etl]") {

    typedef Etl::MultiMap<int, uint32_t> MapType;

    MapType map;
    MapType::Iterator it = map.insert(1, 2);

    REQUIRE(it != map.end());
    REQUIRE(map.getSize() == 1);

    SECTION("first element") {

        REQUIRE(it->getKey() == 1);
        REQUIRE(it->getElement() == 2);
    }

    SECTION("second element") {

        it = map.insert(2, 2);
    
        REQUIRE(it != map.end());
        REQUIRE(it->getKey() == 2);
        REQUIRE(map.getSize() == 2);
    }

    SECTION("insert() of existing") {

        MapType::Iterator it2 = map.insert(1, 3);

        REQUIRE(it2 != map.end());
        REQUIRE(it2 != it);
        REQUIRE(it2->getKey() == 1);
        REQUIRE(it2->getElement() == 3);
        REQUIRE(it->getElement() == 2);
        REQUIRE(map.getSize() == 2);
    }

    SECTION("insertUnique()") {

        std::pair<MapType::Iterator, bool> res = map.insertUnique(1, 3);

        REQUIRE(res.second == false);
        REQUIRE(map.getSize() == 1);
        REQUIRE(res.first->getElement() == 2);

        res = map.insertUnique(2, 2);

        REQUIRE(res.second == true);
        REQUIRE(map.getSize() == 2);
        REQUIRE(res.first != map.end());
        REQUIRE(res.first->getKey() == 2);
        REQUIRE(res.first->getElement() == 2);
    }
}


TEST_CASE("Etl::MultiMap<> erase tests", "[multimap][etl]") {

    typedef Etl::MultiMap<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(2, -2);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    CHECK(map.getSize() == 6);

    SECTION("erase(Key)") {

        uint32_t cnt = map.erase(2);

        REQUIRE(cnt == 3);
        REQUIRE(map.getSize() == 3);
        REQUIRE(map.find(2) == map.end());
    }

    SECTION("erase(Iterator)") {

        MapType::Iterator it = map.find(2);
        CHECK(it != map.end());

        it = map.erase(it);

        REQUIRE(map.getSize() == 5);
        REQUIRE(map.find(2) != map.end());
    }
}


TEST_CASE("Etl::MultiMap<> iteration tests", "[multimap][etl]") {

    typedef Etl::MultiMap<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(3, -3);
    map.insert(3, -9);
    map.insert(2, -2);
    map.insert(4, -4);

    CHECK(map.getSize() == 5);

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
        REQUIRE(it->getElement() == -9);
    }
}


TEST_CASE("Etl::MultiMap<> element order", "[multimap][etl]") {

    typedef Etl::MultiMap<int, uint32_t> MapType;

    MapType map;

    map.insert(3, -3);
    map.insert(1, -1);
    map.insert(3, -9);
    map.insert(2, -2);
    map.insert(4, -4);

    CHECK(map.getSize() == 5);

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
    REQUIRE(it->getKey() == 3);
    REQUIRE(it->getElement() == -9);

    ++it;
    REQUIRE(it->getKey() == 4);
    REQUIRE(it->getElement() == -4);

    ++it;
    REQUIRE(it == map.end());
}


TEST_CASE("Etl::MultiMap<> copy", "[multimap][etl]") {

    typedef Etl::MultiMap<int, uint32_t> MapType;

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
       
        MapType::Iterator it1 = map.begin();
        MapType::Iterator it2 = map2.begin();

        REQUIRE(it2->getKey() == it1->getKey());
        REQUIRE(it2->getElement() == it1->getElement());
        ++it2;
        ++it2;
        ++it2;
        ++it1;
        ++it1;
        ++it1;
        REQUIRE(it2->getKey() == it1->getKey());
        REQUIRE(it2->getElement() == it1->getElement());
    }

    SECTION("copy constructor") {

        MapType map3 = map;

        REQUIRE(map3.getSize() == 4);
        
        MapType::Iterator it1 = map.begin();
        MapType::Iterator it3 = map3.begin();
        
        REQUIRE(it3->getKey() == it1->getKey());
        REQUIRE(it3->getElement() == it1->getElement());
        ++it3;
        ++it3;
        ++it3;
        ++it1;
        ++it1;
        ++it1;
        REQUIRE(it3->getKey() == it1->getKey());
        REQUIRE(it3->getElement() == it1->getElement());
    }

    SECTION("copyElementsFrom()") {

        map2.copyElementsFrom(map);

        REQUIRE(map2.getSize() == 6);
    }
}


TEST_CASE("Etl::MultiMap<> search tests", "[multimap][etl]") {

    typedef Etl::MultiMap<uint32_t, int> MapType;
    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(3, -9);
    map.insert(4, -4);

    CHECK(map.getSize() == 5);

    SECTION("find(Key)") {

        MapType::Iterator it = map.find(3);

        REQUIRE(it != map.end());
        REQUIRE(it->getKey() == 3);
        REQUIRE(it->getElement() == -9);
    }

    SECTION("find(Key) non-existing") {

        MapType::Iterator it = map.find(7);

        REQUIRE(it == map.end());
    }

    SECTION("equalRange(Key) for one") {

        std::pair<MapType::ConstIterator, MapType::ConstIterator> res = map.equalRange(2);

        REQUIRE(res.first != map.end());
        REQUIRE(res.second != map.end());
        REQUIRE(res.first != res.second);
        REQUIRE(res.first->getKey() == 2);
        REQUIRE(res.first->getElement() == -2);
        REQUIRE(res.second->getKey() == 3);
        REQUIRE(res.second->getElement() == -3);
        REQUIRE(++res.first == res.second);
    }
    
    SECTION("equalRange(Key) for more") {

        std::pair<MapType::ConstIterator, MapType::ConstIterator> res = map.equalRange(3);

        REQUIRE(res.first != map.end());
        REQUIRE(res.second != map.end());
        REQUIRE(res.first != res.second);
        REQUIRE(res.first->getKey() == 3);
        REQUIRE(res.first->getElement() == -3);
        REQUIRE(res.second->getKey() == 4);
        REQUIRE(res.second->getElement() == -4);
        ++res.first;
        REQUIRE(res.first->getKey() == 3);
        REQUIRE(res.first->getElement() == -9);
        REQUIRE(++res.first == res.second);
    }

    SECTION("equalRange(Key) non-existing") {

        std::pair<MapType::ConstIterator, MapType::ConstIterator> res = map.equalRange(12);

        REQUIRE(res.first == map.end());
        REQUIRE(res.second == map.end());
    }
}


TEST_CASE("Etl::MultiMap<> allocator test", "[multimap][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::MultiMap<uint32_t, ItemType, DummyAllocator> MapType;
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


TEST_CASE("Etl::Pooled::MultiMap<> test", "[multimap][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Pooled::MultiMap<uint32_t, ItemType, NUM> MapType;

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

        MapType::Iterator it = map.insert(NUM, ContainerTester(NUM));
        REQUIRE(map.getSize() == NUM);
        REQUIRE(it == map.end());
    }
}


TEST_CASE("Etl::MultiMap<> test cleanup", "[multimap][etl]") {

    typedef Etl::MultiMap<uint32_t, ContainerTester, DummyAllocator> MapType;
    
    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(MapType::Allocator::getDeleteCount() == MapType::Allocator::getAllocCount());
}
