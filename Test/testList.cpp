/**
\file
\date 2017.02.22.
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

#include "List.h"
#include "Test/ContainerTester.h"
#include "Test/DummyAllocator.h"


TEST_CASE("Etl::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::List<ItemType> ListType;

    ListType list;

    REQUIRE(list.getSize() == 0);

    list.pushBack(2);
    list.pushFront(1);

    REQUIRE(list.getSize() == 2);

    ListType::Iterator it = list.begin();
    REQUIRE(*it == 1);
    ++it;
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(it == list.end());

    it = list.insert(list.begin(), 3);
    REQUIRE(list.getSize() == 3);
    REQUIRE(*it == 3);

    list.popFront();
    list.erase(list.begin());
    list.popBack();

    REQUIRE(list.getSize() == 0);
}


TEST_CASE("Etl::List<> leak test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::List<ItemType> ListType;

    static const int PATTERN = 123;

    CHECK(ItemType::getObjectCount() == 0);
    if (ItemType::getObjectCount() == 0) {

        ListType list;

        list.pushBack(ContainerTester(PATTERN));
        list.pushBack(ContainerTester(PATTERN));
        list.pushBack(ContainerTester(PATTERN));
        list.pushBack(ContainerTester(PATTERN));
        list.pushBack(ContainerTester(PATTERN));
        list.pushBack(ContainerTester(PATTERN));
        list.pushBack(ContainerTester(PATTERN));
        list.pushBack(ContainerTester(PATTERN));

        CHECK(list.getSize() == ItemType::getObjectCount());

        list.popBack();
        REQUIRE(list.getSize() == ItemType::getObjectCount());

        list.erase(list.begin());
        REQUIRE(list.getSize() == ItemType::getObjectCount());
    }

    REQUIRE(ItemType::getObjectCount() == 0);
}


TEST_CASE("Etl::List<>::find(Etl::Matcher<>) test", "[list][etl]") {

    typedef int ItemType;
    typedef Etl::List<ItemType> ListType;

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

    ListType list;
    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(REF_VALUE);
    ListType::Iterator it1 = --list.end();
    list.pushBack(4);
    list.pushBack(REF_VALUE);
    ListType::Iterator it2 = --list.end();
    list.pushBack(6);

    ListType::Iterator found = list.find(IntMatcher(REF_VALUE));
    REQUIRE(found == it1);
    found = list.find((++found), list.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == it2);
    found = list.find((++found), list.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == list.end());
}


TEST_CASE("Etl::List<>::find<F, V>() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::List<ItemType> ListType;

    static const int REF_VALUE = 123;
    const ItemType REF_ITEM(REF_VALUE);

    CAPTURE(REF_ITEM.getValue());

    ListType list;
    list.pushBack(ItemType(1));
    list.pushBack(ItemType(2));
    list.pushBack(REF_ITEM);
    ListType::Iterator it1 = --list.end();
    list.pushBack(ItemType(4));
    list.pushBack(REF_ITEM);
    ListType::Iterator it2 = --list.end();
    list.pushBack(ItemType(6));

    ListType::Iterator found = list.find(&ItemType::getValue, REF_VALUE);
    REQUIRE(found == it1);
    found = list.find((++found), list.end(), &ItemType::getValue, REF_VALUE);
    REQUIRE(found == it2);
    found = list.find((++found), list.end(), &ItemType::getValue, REF_VALUE);
    REQUIRE(found == list.end());
}


TEST_CASE("Etl::List<> allocator test", "[list][etl]") {

    typedef int ItemType;
    typedef Etl::List<ItemType, DummyAllocator> ListType;
    typedef ListType::Allocator AllocatorType;

    ListType list;
    list.pushBack(1);

    ListType::Iterator it = list.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    list.pushBack(2);
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    list.popFront();
    REQUIRE(AllocatorType::getDeleteCount() == 1);

    list.popBack();
    REQUIRE(AllocatorType::getDeleteCount() == 2);
}


TEST_CASE("Etl::List<> test cleanup", "[list][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}

