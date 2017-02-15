/*
 * Copyright (C) Commsignia Ltd. - All Rights Reserved
 * Unauthorised copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Balazs Toth <balazs.toth@commsignia.com>, 2017
 */

#include "catch.hpp"

#include "Array.h"
#include "ArrayProxy.h"
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
    uint64_t refDiff = refItem1- refItem0;

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

TEST_CASE("Etl::ArrayProxy test", "[array][etl][basic]") {

    typedef int ItemType;
   
    Etl::Array<ItemType, 16> array;
    Etl::ArrayProxy proxy(array);

    REQUIRE(proxy.getCapacity() == array.getSize());
    REQUIRE(proxy.getItemSize() == sizeof(ItemType));
    REQUIRE(proxy.getData() == array.getData());
    REQUIRE(proxy.getItemPointer(0) == array.begin());
    REQUIRE(proxy.getItemPointer(proxy.getCapacity()) == array.end());

}

TEST_CASE("Etl::TypedArrayProxy<> test", "[array][etl][basic]") {

    typedef int ItemType;
    static const int PATTERN1 = 123;
    static const int PATTERN2 = 321;
    
    Etl::Array<ItemType, 16> array;
    Etl::TypedArrayProxy<ItemType> proxy(array);

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

