/**
\file
\date 2017.02.20. 11:49:49
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

#include <FixedVector.h>
#include "UnalignedTester.h"
#include "ContainerTester.h"


TEST_CASE("Etl::FixedVector<> basic test", "[fixedvector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::FixedVector<ItemType, 16> VectorType;

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


TEST_CASE("Etl::FixedVector<> push/pop test", "[fixedvector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::FixedVector<ItemType, 16> VectorType;

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

TEST_CASE("Etl::FixedVector<> insert/erase test", "[fixedvector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::FixedVector<ItemType, 16> VectorType;

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


TEST_CASE("Etl::FixedVector<> size/capacity test", "[fixedvector][etl][basic]") {

    static const uint32_t CAPACITY = 16;

    typedef int ItemType;
    typedef Etl::FixedVector<ItemType, CAPACITY> VectorType;

    CAPTURE(CAPACITY);

    VectorType vector;

    REQUIRE(vector.getSize() == 0);
    REQUIRE(vector.getCapacity() == CAPACITY);

    vector.reserve(16);
    REQUIRE(vector.getCapacity() == CAPACITY);
    REQUIRE(vector.getSize() == 0);

    vector.pushBack(1);
    vector.pushBack(2);
    REQUIRE(vector.getSize() == 2);

    /*
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
    */
}


TEST_CASE("Etl::FixedVector<> constructor test", "[fixedvector][etl][basic]") {

    static const uint32_t CAPACITY = 16;

    typedef int ItemType;
    typedef Etl::FixedVector<ItemType, CAPACITY> VectorType;

    static const ItemType INIT_VALUE = 123;

    CAPTURE(CAPACITY);

    VectorType vector1;
    REQUIRE(vector1.getSize() == 0);
    REQUIRE(vector1.getCapacity() == CAPACITY);

    VectorType vector2(4);
    REQUIRE(vector2.getSize() == 4);
    REQUIRE(vector2.getCapacity() == CAPACITY);

    CAPTURE(INIT_VALUE);
    VectorType vector3(4, INIT_VALUE);
    REQUIRE(vector3.getSize() == 4);
    REQUIRE(vector3.getCapacity() == CAPACITY);
    REQUIRE(vector3[0] == INIT_VALUE);
    REQUIRE(vector3[3] == INIT_VALUE);
    /*
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
    */
}

TEST_CASE("Etl::FixedVector<> assignment test", "[fixedvector][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::FixedVector<ItemType, 16> VectorType;

    static const int PATTERN1 = 123;
    static const int PATTERN2 = 321;

    VectorType vector1(4, ContainerTester(PATTERN1));
    VectorType vector2(8, ContainerTester(PATTERN2));

    CHECK(vector1[0] != vector2[0]);
    CHECK(vector1.getSize() != vector2.getSize());

    REQUIRE(ItemType::getObjectCount() == (vector1.getSize() + vector2.getSize()));

    vector1 = vector2;

    REQUIRE(vector1[0] == vector2[0]);
    REQUIRE(vector1.getSize() == vector2.getSize());
    REQUIRE(ItemType::getObjectCount() == (2 * vector2.getSize()));

}


TEST_CASE("Etl::FixedVector<> leak test", "[fixedvector][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::FixedVector<ItemType, 16> VectorType;

    static const int PATTERN = 123;

    CHECK(ItemType::getObjectCount() == 0);
    if (ItemType::getObjectCount() == 0) {

        VectorType vector(8, ContainerTester(PATTERN));
        CHECK(vector.getSize() == ItemType::getObjectCount());

        vector.popBack();
        REQUIRE(vector.getSize() == ItemType::getObjectCount());

        vector.erase(vector.begin());
        REQUIRE(vector.getSize() == ItemType::getObjectCount());

        vector.erase((vector.begin() + 1), (vector.begin() + 3));
        REQUIRE(vector.getSize() == ItemType::getObjectCount());
    }

    REQUIRE(ItemType::getObjectCount() == 0);
}


TEST_CASE("Etl::FixedVector<>::find(Etl::Matcher<>) test", "[fixedvector][etl]") {

    typedef int ItemType;
    typedef Etl::FixedVector<ItemType, 16> VectorType;

    class IntMatcher : public Etl::Matcher<ItemType> {
        const ItemType value;
      public:
        IntMatcher(ItemType val) :
            value(val) {};

        virtual bool call(const ItemType& ref) const OVERRIDE {
            return value == ref;
        }
    };

    static const ItemType REF_VALUE = 123;

    CAPTURE(REF_VALUE);

    VectorType vector;
    vector.pushBack(1);
    vector.pushBack(2);
    vector.pushBack(REF_VALUE);
    VectorType::Iterator it1 = vector.end() - 1;
    vector.pushBack(4);
    vector.pushBack(REF_VALUE);
    VectorType::Iterator it2 = vector.end() - 1;
    vector.pushBack(6);

    VectorType::Iterator found = vector.find(IntMatcher(REF_VALUE));
    REQUIRE(found == it1);
    found = vector.find((++found), vector.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == it2);
    found = vector.find((++found), vector.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == vector.end());

}

