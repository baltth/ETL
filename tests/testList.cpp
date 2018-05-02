/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Balazs Toth.

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

#include <etl/List.h>

#include "ContainerTester.h"
#include "DummyAllocator.h"

using ETL_NAMESPACE::Test::ContainerTester;
using ETL_NAMESPACE::Test::DummyAllocator;


template<class ListT>
void testListBasic() {

    ListT list;

    REQUIRE(list.getSize() == 0);
    REQUIRE(list.isEmpty());

    list.pushBack(2);

    REQUIRE_FALSE(list.isEmpty());

    list.pushFront(1);

    REQUIRE(list.getSize() == 2);
    REQUIRE_FALSE(list.isEmpty());

    typename ListT::Iterator it = list.begin();
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
    REQUIRE(list.isEmpty());
}

TEST_CASE("Etl::Dynamic::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Dynamic::List<ItemType> ListType;

    testListBasic<ListType>();
}

TEST_CASE("Etl::Static::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Static::List<ItemType, 16> ListType;

    testListBasic<ListType>();
}

TEST_CASE("Etl::Pooled::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Pooled::List<ItemType, 16> ListType;

    testListBasic<ListType>();
}


template<class ListT>
void testListLeak() {

    typedef typename ListT::ItemType ItemType;
    static const int PATTERN = 123;


    CHECK(ItemType::getObjectCount() == 0);

    {
        ListT list;

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

TEST_CASE("Etl::Dynamic::List<> leak test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> ListType;

    testListLeak<ListType>();
}

TEST_CASE("Etl::Static::List<> leak test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Static::List<ItemType, 16> ListType;

    testListLeak<ListType>();
}

TEST_CASE("Etl::Pooled::List<> leak test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Pooled::List<ItemType, 16> ListType;

    testListLeak<ListType>();
}


template<class ListT>
void testListCopy() {

    ListT list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);
    list.pushBack(4);

    ListT list2;

    list2.pushBack(1);
    list2.pushBack(5);

    CHECK(list.getSize() == 4);
    CHECK(list2.getSize() == 2);

    SECTION("copy assignment") {

        list2 = list;

        REQUIRE(list2.getSize() == 4);

        typename ListT::ConstIterator it = list2.begin();

        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);

        it = list2.end();
        --it;
        REQUIRE(*it == 4);
    }

    SECTION("copy constructor") {

        ListT list3 = list;

        REQUIRE(list3.getSize() == 4);

        typename ListT::ConstIterator it = list3.begin();

        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);

        it = list3.end();
        --it;
        REQUIRE(*it == 4);
    }

    SECTION("copyElements()") {

        list2.copyElements(list);

        REQUIRE(list2.getSize() == 6);

        typename ListT::ConstIterator it = list2.begin();

        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 5);
        ++it;
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
        ++it;
        REQUIRE(*it == 3);
        ++it;
        REQUIRE(*it == 4);
        ++it;
        REQUIRE(it == list2.end());
    }
}

TEST_CASE("Etl::Dynamic::List<> copy", "[list][etl]") {

    typedef Etl::Dynamic::List<int> ListType;

    testListCopy<ListType>();
}

TEST_CASE("Etl::Static::List<> copy", "[list][etl]") {

    typedef Etl::Static::List<int, 32> ListType;

    testListCopy<ListType>();
}

TEST_CASE("Etl::Pooled::List<> copy", "[list][etl]") {

    typedef Etl::Pooled::List<int, 32> ListType;

    testListCopy<ListType>();
}


template<class ListT1, class ListT2>
void testListSwap() {

    ListT1 list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);
    list.pushBack(4);

    ListT2 list2;

    list2.pushBack(1);
    list2.pushBack(5);

    CHECK(list.getSize() == 4);
    CHECK(list2.getSize() == 2);
    SECTION("swap()") {

        list.swap(list2);

        REQUIRE(list.getSize() == 2);
        REQUIRE(list2.getSize() == 4);

        typename ListT1::ConstIterator it = list.begin();
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 5);
        ++it;
        REQUIRE(it == list.end());
        --it;
        REQUIRE(*it == 5);
        --it;
        REQUIRE(it == list.begin());

        it = list2.begin();
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
        ++it;
        REQUIRE(*it == 3);
        ++it;
        REQUIRE(*it == 4);
        ++it;
        REQUIRE(it == list2.end());
        --it;
        REQUIRE(*it == 4);
        --it;
        --it;
        --it;
        REQUIRE(it == list2.begin());
    }

    SECTION("swap() with empty") {

        ListT2 list3;

        CHECK(list.getSize() == 4);
        CHECK(list3.getSize() == 0);

        list.swap(list3);

        REQUIRE(list.getSize() == 0);
        REQUIRE(list3.getSize() == 4);

        typename ListT1::ConstIterator it = list.begin();

        REQUIRE(it == list.end());

        it = list3.begin();
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
        ++it;
        REQUIRE(*it == 3);
        ++it;
        REQUIRE(*it == 4);
        ++it;
        REQUIRE(it == list3.end());
        --it;
        REQUIRE(*it == 4);

        list.swap(list3);

        REQUIRE(list.getSize() == 4);
        REQUIRE(list3.getSize() == 0);

        it = list.begin();
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
        ++it;
        REQUIRE(*it == 3);
        ++it;
        REQUIRE(*it == 4);
        ++it;
        REQUIRE(it == list.end());
        --it;
        REQUIRE(*it == 4);

        it = list3.begin();

        REQUIRE(it == list3.end());
    }
}

TEST_CASE("Etl::Dynamic::List<> swap", "[list][etl]") {

    typedef Etl::Dynamic::List<int> ListType;

    testListSwap<ListType, ListType>();
}

TEST_CASE("Etl::Static::List<> swap", "[list][etl]") {

    typedef Etl::Static::List<int, 32> ListType;

    testListSwap<ListType, ListType>();
}

TEST_CASE("Etl::Pooled::List<> swap", "[list][etl]") {

    typedef Etl::Pooled::List<int, 32> ListType;

    testListSwap<ListType, ListType>();
}


template<class ListT1, class ListT2>
void testListSplice() {

    typedef typename ListT1::ItemType ItemType;

    ListT1 list1;
    ListT2 list2;

    for (uint32_t i = 0; i < 8; ++i) {
        list1.pushBack(ItemType(i));
    }

    CHECK(list1.getSize() == 8);
    CHECK(list2.getSize() == 0);

    SECTION("Splice to empty") {

        list2.splice(list2.end(), list1, list1.begin());

        REQUIRE(list1.getSize() == 7);
        REQUIRE(list2.getSize() == 1);

        REQUIRE(ContainerTester::getObjectCount() == 8);

        REQUIRE(*list1.begin() == ItemType(1));
        REQUIRE(*list2.begin() == ItemType(0));
    }

    SECTION("Splice to existing") {

        list2.pushBack(ItemType(8));
        CHECK(list2.getSize() == 1);

        typename ListT1::ConstIterator it = list1.begin();
        ++it;
        ++it;
        ++it;

        CHECK(*it == ItemType(3));

        list2.splice(list2.begin(), list1, list1.begin(), it);

        REQUIRE(list1.getSize() == 5);
        REQUIRE(list2.getSize() == 4);

        list2.splice(list2.end(), list1);

        REQUIRE(list1.getSize() == 0);
        REQUIRE(list2.getSize() == 9);

        REQUIRE(ContainerTester::getObjectCount() == 9);

        it = list2.begin();
        REQUIRE(*it == ItemType(0));
        ++it;
        REQUIRE(*it == ItemType(1));
        ++it;
        REQUIRE(*it == ItemType(2));
        ++it;
        REQUIRE(*it == ItemType(8));
        ++it;
        REQUIRE(*it == ItemType(3));
        ++it;
        REQUIRE(*it == ItemType(4));
        ++it;
        REQUIRE(*it == ItemType(5));
        ++it;
        REQUIRE(*it == ItemType(6));
        ++it;
        REQUIRE(*it == ItemType(7));
        ++it;
        REQUIRE(it == list2.end());
    }
}

TEST_CASE("Etl::Dynamic::List<>::splice() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> ListType;

    testListSplice<ListType, ListType>();
}

TEST_CASE("Etl::Static::List<>::splice() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Static::List<ItemType, 16> ListType;

    testListSplice<ListType, ListType>();
}

TEST_CASE("Etl::Pooled::List<>::splice() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Pooled::List<ItemType, 16> ListType;

    testListSplice<ListType, ListType>();
}


TEST_CASE("Etl::Dynamic::List<>::find(Etl::Matcher<>) test", "[list][etl]") {

    typedef int ItemType;
    typedef Etl::Dynamic::List<ItemType> ListType;

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


TEST_CASE("Etl::Dynamic::List<>::find<F, V>() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> ListType;

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

    typedef ContainerTester ItemType;
    typedef Etl::List<ItemType, DummyAllocator> ListType;
    typedef ListType::Allocator AllocatorType;

    AllocatorType::reset();
    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    ListType list;
    list.pushBack(ContainerTester(1));

    ListType::Iterator it = list.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    list.pushBack(ContainerTester(2));
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    list.popFront();
    REQUIRE(AllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Static::List<> test", "[list][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Static::List<ItemType, NUM> ListType;

    ListType list;

    CHECK(list.getAllocator().getCapacity() == NUM);
    REQUIRE(list.getAllocator().isEmpty());

    SECTION("Basic allocation") {

        list.pushBack(ContainerTester(1));

        ListType::Iterator it = list.begin();
        REQUIRE(it.operator->() != NULL);

        list.pushBack(ContainerTester(1));
        ListType::Iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());

        REQUIRE_FALSE(list.getAllocator().isEmpty());
        REQUIRE(list.getAllocator().getSize() == 2);
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            list.pushBack(ContainerTester(i));
        }

        CHECK(list.getSize() == NUM);

        ListType::Iterator it = list.insert(list.begin(), ContainerTester(NUM));
        REQUIRE(list.getSize() == NUM);
        REQUIRE(it == list.end());
    }

    SECTION("Unique pool checks") {

        list.pushBack(ContainerTester(1));
        list.pushBack(ContainerTester(2));

        CHECK(list.getSize() == list.getAllocator().getSize());

        ListType list2;
        list2.pushBack(ContainerTester(3));
        list2.pushBack(ContainerTester(4));

        REQUIRE(list.getSize() == list.getAllocator().getSize());
        REQUIRE(list2.getSize() == list2.getAllocator().getSize());

        list2.splice(list2.begin(), list);

        CHECK(list.isEmpty());
        REQUIRE(list.getAllocator().getSize() == 0);
        REQUIRE(list2.getSize() == list2.getAllocator().getSize());
    }

    list.clear();
    REQUIRE(list.getAllocator().isEmpty());
}


TEST_CASE("Etl::Pooled::List<> test", "[list][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Pooled::List<ItemType, NUM> ListType;

    ListType list;

    CHECK(list.getAllocator().getCapacity() == NUM);
    REQUIRE(list.getAllocator().isEmpty());

    SECTION("Basic allocation") {

        list.pushBack(ContainerTester(1));

        ListType::Iterator it = list.begin();
        REQUIRE(it.operator->() != NULL);

        list.pushBack(ContainerTester(1));
        ListType::Iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());

        REQUIRE_FALSE(list.getAllocator().isEmpty());
        REQUIRE(list.getAllocator().getSize() == 2);
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            list.pushBack(ContainerTester(i));
        }

        CHECK(list.getSize() == NUM);

        ListType::Iterator it = list.insert(list.begin(), ContainerTester(NUM));
        REQUIRE(list.getSize() == NUM);
        REQUIRE(it == list.end());
    }

    SECTION("Common pool checks") {

        list.pushBack(ContainerTester(1));
        list.pushBack(ContainerTester(2));

        CHECK(list.getSize() == list.getAllocator().getSize());

        ListType list2;
        list2.pushBack(ContainerTester(3));
        list2.pushBack(ContainerTester(4));

        REQUIRE((list.getSize() + list2.getSize()) == list.getAllocator().getSize());

        list2.splice(list2.begin(), list);

        CHECK(list.isEmpty());
        REQUIRE(list2.getSize() == list.getAllocator().getSize());
    }

    list.clear();
    REQUIRE(list.getAllocator().isEmpty());
}


TEST_CASE("Etl::List<> test cleanup", "[list][etl]") {

    typedef Etl::List<ContainerTester, DummyAllocator> ListType;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(ListType::Allocator::getDeleteCount() == ListType::Allocator::getAllocCount());
}


