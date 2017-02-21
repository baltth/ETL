/**
\file
\date 2017.02.16. 11:20:49
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

#include <Vector.h>
#include <Test/UnalignedTester.h>


TEST_CASE("Etl::Vector<> basic test", "[vector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Vector<ItemType> VectorType;
    
    VectorType vector;

    REQUIRE(vector.getSize() == 0);

    vector.reserve(16);
    REQUIRE(vector.getCapacity() >= 16);
    REQUIRE(vector.getSize() == 0);

    vector.pushBack(1);
    vector.pushBack(2);

    REQUIRE(vector.getSize() == 2);

    VectorType::Iterator it = vector.begin();
    REQUIRE(*it == 1);
    REQUIRE(vector[0] == *it);

    ++it;
    *it = 3;

    vector.insert(vector.begin(), 11);
    REQUIRE(vector[0] == 11);
    vector.popFront();
    vector.erase(vector.begin());

    REQUIRE(*vector.begin() == 3);
    REQUIRE(vector.getSize() == 1);

}

TEST_CASE("Etl::Vector<> push/pop test", "[vector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Vector<ItemType> VectorType;

    static const ItemType itemBack1 = 1;
    static const ItemType itemBack2 = 2;
    static const ItemType itemFront1 = 3;
    static const ItemType itemFront2 = 4;

    VectorType vector;

    vector.pushBack(itemBack1);
    vector.pushBack(itemBack2);
    vector.pushFront(itemFront1);
    vector.pushFront(itemFront2);
    
    REQUIRE(vector.getSize() == 4);

    REQUIRE(vector[0] == itemFront2);
    REQUIRE(vector[1] == itemFront1);
    REQUIRE(vector[2] == itemBack1);
    REQUIRE(vector[3] == itemBack2);

    REQUIRE(vector.back() == itemBack2);
    vector.popBack();
    REQUIRE(vector.back() == itemBack1);
    
    REQUIRE(vector.front() == itemFront2);
    vector.popFront();
    REQUIRE(vector.front() == itemFront1);

    vector.popBack();
    vector.popFront();

    REQUIRE(vector.getSize() == 0);
    
}

TEST_CASE("Etl::Vector<> insert/erase test", "[vector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Vector<ItemType> VectorType;

    VectorType vector(4, 0);

    CHECK(vector.getSize() == 4);

    VectorType::Iterator it = vector.begin() + 2;
    it = vector.insert(it, 2);
    REQUIRE(vector[2] == 2);
    REQUIRE(vector.getSize() == 5);
    REQUIRE(it == &vector[2]);

    ++it;
    it = vector.insert(it, 2, 3);
    REQUIRE(vector[2] == 2);
    REQUIRE(vector[3] == 3);
    REQUIRE(vector[4] == 3);
    REQUIRE(vector.getSize() == 7);
    REQUIRE(it == &vector[3]);
    
    CHECK(vector[1] == 0);
    CHECK(vector[5] == 0);

    it = vector.begin() + 2;
    it = vector.erase(it);
    REQUIRE(vector.getSize() == 6);
    REQUIRE(vector[2] == 3);
    REQUIRE(it == &vector[2]);

    it = vector.erase(it, it + 2);
    REQUIRE(vector.getSize() == 4);
    REQUIRE(vector[2] == 0);
    REQUIRE(it == &vector[2]);

}


TEST_CASE("Etl::Vector<> size/capacity test", "[vector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Vector<ItemType> VectorType;

    VectorType vector;

    REQUIRE(vector.getSize() == 0);
    REQUIRE(vector.getCapacity() == 0);

    vector.reserve(16);
    REQUIRE(vector.getCapacity() >= 16);
    REQUIRE(vector.getSize() == 0);

    vector.pushBack(1);
    vector.pushBack(2);
    REQUIRE(vector.getSize() == 2);

    vector.shrinkToFit();
    REQUIRE(vector.getCapacity() == 2);

    vector.reserveAtLeast(5);
    REQUIRE(vector.getSize() == 2);
    REQUIRE(vector.getCapacity() >= 5);

    uint32_t capacity = vector.getCapacity();
    uint32_t newSize = capacity + 3;

    vector.resize(newSize);
    REQUIRE(vector.getSize() == newSize);
    REQUIRE(vector.getCapacity() >= newSize);

    capacity = vector.getCapacity();
    vector.clear();
    REQUIRE(vector.getSize() == 0);
    REQUIRE(vector.getCapacity() == capacity);

}


TEST_CASE("Etl::Vector<> constructor test", "[vector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Vector<ItemType> VectorType;

    static const ItemType INIT_VALUE = 123;

    VectorType vector1;
    REQUIRE(vector1.getSize() == 0);
    REQUIRE(vector1.getCapacity() == 0);

    VectorType vector2(4);
    REQUIRE(vector2.getSize() == 4);
    REQUIRE(vector2.getCapacity() >= 4);

    CAPTURE(INIT_VALUE);
    VectorType vector3(4, INIT_VALUE);
    REQUIRE(vector3.getSize() == 4);
    REQUIRE(vector3.getCapacity() >= 4);
    REQUIRE(vector3[0] == INIT_VALUE);
    REQUIRE(vector3[3] == INIT_VALUE);

    VectorType vector4(vector3);
    REQUIRE(vector4.getSize() == 4);
    REQUIRE(vector4.getCapacity() >= 4);
    REQUIRE(vector4[0] == INIT_VALUE);
    REQUIRE(vector4[3] == INIT_VALUE);

    vector1 = vector4;
    REQUIRE(vector1.getSize() == 4);
    REQUIRE(vector1.getCapacity() >= 4);
    REQUIRE(vector1[0] == INIT_VALUE);
    REQUIRE(vector1[3] == INIT_VALUE);

}

/*
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
*/
