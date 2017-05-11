/**
\file
\date 2017.02.06.
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

#include "etlSupport.h"

#include "Array.h"
#include "ContainerProxy.h"
#include "Test/UnalignedTester.h"


TEST_CASE("Etl::Array<> basic test", "[array][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Array<ItemType, 16> ArrayType;

    ArrayType array;

    uint32_t size = array.getSize();
    REQUIRE(size == 16);

    ArrayType::Iterator it = array.begin();
    *it = 1;
    CAPTURE(*it);
    REQUIRE(array[0] == *it);

}

TEST_CASE("Etl::Array<> iterators", "[array][etl]") {

    typedef int ItemType;
    typedef Etl::Array<ItemType, 16> ArrayType;

    ArrayType array;

    ArrayType::Iterator it = array.begin();
    *it = 1;
    ++it;
    *it = 2;

    REQUIRE(array[0] == 1);
    REQUIRE(array[1] == 2);

    array[1] = 1;
    REQUIRE(*it == 1);

    it = array.end();
    --it;
    *it = 16;
    --it;
    *it = 15;

    REQUIRE(array[15] == 16);
    REQUIRE(array[14] == 15);
}

#if ETL_USE_EXCEPTIONS

TEST_CASE("Etl::Array<> exceptions", "[array][etl]") {

    static const uint32_t COUNT = 16;
    typedef int ItemType;
    typedef Etl::Array<ItemType, COUNT> ArrayType;

    ArrayType array;

    ItemType val;
    CHECK_NOTHROW(val = array[0]);
    CHECK_NOTHROW(val = array[COUNT - 1]);

    REQUIRE_NOTHROW(val = array.at(0));
    REQUIRE_NOTHROW(val = array.at(COUNT - 1));

    REQUIRE_THROWS_AS(val = array.at(COUNT), Etl::OutOfRangeException);
    REQUIRE_THROWS_AS(val = array.at(COUNT + 100), Etl::OutOfRangeException);

}

#endif

TEST_CASE("Etl::Array<> alignment", "[array][etl]") {

    typedef UnalignedTester ItemType;
    typedef Etl::Array<ItemType, 16> ArrayType;

    ArrayType array;
    UnalignedTester refArray[16];

    const size_t itemSize = sizeof(ItemType);
    CAPTURE(itemSize);
    CHECK((itemSize % sizeof(uint32_t)) != 0);

    ArrayType::Iterator it = array.begin();
    uint64_t item0 = reinterpret_cast<uint64_t>(it);
    ++it;
    uint64_t item1 = reinterpret_cast<uint64_t>(it);
    uint64_t diff = item1 - item0;
    CAPTURE(diff);

    uint64_t refItem0 = reinterpret_cast<uint64_t>(&refArray[0]);
    uint64_t refItem1 = reinterpret_cast<uint64_t>(&refArray[1]);
    uint64_t refDiff = refItem1 - refItem0;

    REQUIRE(diff == refDiff);
}

TEST_CASE("Etl::Array<> features", "[array][etl]") {

    typedef int ItemType;
    typedef Etl::Array<ItemType, 16> ArrayType;
    static const int PATTERN1 = 123;
    static const int PATTERN2 = 321;

    ArrayType array;
    array.fill(PATTERN1);

    CAPTURE(PATTERN1);
    REQUIRE(array[0] == PATTERN1);
    REQUIRE(array[15] == PATTERN1);

    array.fill(PATTERN2);
    CAPTURE(PATTERN2);
    REQUIRE(array[0] == PATTERN2);
    REQUIRE(array[15] == PATTERN2);

    ArrayType array2(array);
    REQUIRE(array2[15] == PATTERN2);

    ArrayType array3 = array2;
    REQUIRE(array3[15] == PATTERN2);

}

TEST_CASE("Etl::ContainerProxy - Array<> test", "[array][etl][basic]") {

    typedef int ItemType;

    Etl::Array<ItemType, 16> array;
    Etl::ContainerProxy proxy(array);

    REQUIRE(proxy.getCapacity() == array.getSize());
    REQUIRE(proxy.getItemSize() == sizeof(ItemType));
    REQUIRE(proxy.getData() == array.getData());
    REQUIRE(proxy.getItemPointer(0) == array.begin());
    REQUIRE(proxy.getItemPointer(proxy.getCapacity()) == array.end());

}

TEST_CASE("Etl::TypedContainerProxy - Array<> test", "[array][etl][basic]") {

    typedef int ItemType;
    static const int PATTERN1 = 123;
    static const int PATTERN2 = 321;

    Etl::Array<ItemType, 16> array;
    Etl::TypedContainerProxy<ItemType> proxy(array);

    REQUIRE(proxy.getCapacity() == array.getSize());
    REQUIRE(proxy.getItemSize() == sizeof(ItemType));
    REQUIRE(proxy.getData() == array.getData());
    REQUIRE(proxy.getItemPointer(0) == array.begin());
    REQUIRE(proxy.getItemPointer(proxy.getCapacity()) == array.end());

    CAPTURE(PATTERN1);
    array.fill(PATTERN1);

    CAPTURE(PATTERN2);
    proxy.fill(PATTERN2);

    REQUIRE(array[0] == PATTERN2);
    REQUIRE(array[15] == PATTERN2);

}

