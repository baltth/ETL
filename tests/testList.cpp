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
#include "compatibilityTests.h"

using ETL_NAMESPACE::Test::ContainerTester;
using ETL_NAMESPACE::Test::DummyAllocator;


template<class ListT>
void testListBasic() {

    ListT list;

    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());

    list.push_back(2);

    REQUIRE_FALSE(list.empty());

    list.push_front(1);

    REQUIRE(list.size() == 2);
    REQUIRE_FALSE(list.empty());

    typename ListT::iterator it = list.begin();
    REQUIRE(*it == 1);
    ++it;
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(it == list.end());

    it = list.insert(list.begin(), 3);
    REQUIRE(list.size() == 3);
    REQUIRE(*it == 3);

    list.pop_front();
    list.erase(list.begin());
    list.pop_back();

    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());
}

TEST_CASE("Etl::Dynamic::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Dynamic::List<ItemType> ListT;

    testListBasic<ListT>();
}

TEST_CASE("Etl::Static::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Static::List<ItemType, 16> ListT;

    testListBasic<ListT>();
}

TEST_CASE("Etl::Pooled::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Pooled::List<ItemType, 16> ListT;

    testListBasic<ListT>();
}


template<class ListT>
void testListLeak() {

    typedef typename ListT::value_type ItemType;
    static const int PATTERN = 123;

    CHECK(ItemType::getObjectCount() == 0);

    {
        ListT list;

        list.push_back(ContainerTester(PATTERN));
        list.push_back(ContainerTester(PATTERN));
        list.push_back(ContainerTester(PATTERN));
        list.push_back(ContainerTester(PATTERN));
        list.push_back(ContainerTester(PATTERN));
        list.push_back(ContainerTester(PATTERN));
        list.push_back(ContainerTester(PATTERN));
        list.push_back(ContainerTester(PATTERN));

        CHECK(list.size() == ItemType::getObjectCount());

        list.pop_back();
        REQUIRE(list.size() == ItemType::getObjectCount());

        list.erase(list.begin());
        REQUIRE(list.size() == ItemType::getObjectCount());
    }

    REQUIRE(ItemType::getObjectCount() == 0);
}

TEST_CASE("Etl::Dynamic::List<> leak test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> ListT;

    testListLeak<ListT>();
}

TEST_CASE("Etl::Static::List<> leak test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Static::List<ItemType, 16> ListT;

    testListLeak<ListT>();
}

TEST_CASE("Etl::Pooled::List<> leak test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Pooled::List<ItemType, 16> ListT;

    testListLeak<ListT>();
}


template<class ListT>
void testListCopy() {

    ListT list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);

    ListT list2;

    list2.push_back(1);
    list2.push_back(5);

    CHECK(list.size() == 4);
    CHECK(list2.size() == 2);

    SECTION("copy assignment") {

        list2 = list;

        REQUIRE(list2.size() == 4);

        typename ListT::const_iterator it = list2.begin();

        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);

        it = list2.end();
        --it;
        REQUIRE(*it == 4);
    }

    SECTION("copy constructor") {

        ListT list3 = list;

        REQUIRE(list3.size() == 4);

        typename ListT::const_iterator it = list3.begin();

        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);

        it = list3.end();
        --it;
        REQUIRE(*it == 4);
    }
}

TEST_CASE("Etl::Dynamic::List<> copy", "[list][etl]") {

    typedef Etl::Dynamic::List<int> ListT;

    testListCopy<ListT>();
}

TEST_CASE("Etl::Static::List<> copy", "[list][etl]") {

    typedef Etl::Static::List<int, 32> ListT;

    testListCopy<ListT>();
}

TEST_CASE("Etl::Pooled::List<> copy", "[list][etl]") {

    typedef Etl::Pooled::List<int, 32> ListT;

    testListCopy<ListT>();
}


template<class ListT1, class ListT2>
void testListSwap() {

    ListT1 list1;
    list1.push_back(1);
    list1.push_back(2);

    ListT2 list2;
    list2.push_back(-1);
    list2.push_back(-2);

    CHECK(list1.size() == 2);
    CHECK(list2.size() == 2);

    SECTION("Swap equal length") {

        list1.swap(list2);

        REQUIRE(list1.size() == 2);
        REQUIRE(list2.size() == 2);

        typename ListT1::const_iterator it1 = list1.begin();
        REQUIRE(*it1 == -1);
        ++it1;
        REQUIRE(*it1 == -2);
        ++it1;
        REQUIRE(it1 == list1.end());

        typename ListT2::const_iterator it2 = list2.begin();
        REQUIRE(*it2 == 1);
        ++it2;
        REQUIRE(*it2 == 2);
        ++it2;
        REQUIRE(it2 == list2.end());

        list1.swap(list2);

        REQUIRE(list1.size() == 2);
        REQUIRE(list2.size() == 2);

        REQUIRE(*list1.begin() == 1);
        REQUIRE(*list2.begin() == -1);
    }

    SECTION("Swap different length") {

        list2.push_back(-3);
        list2.push_back(-4);

        CHECK(list2.size() == 4);

        list1.swap(list2);

        REQUIRE(list1.size() == 4);
        REQUIRE(list2.size() == 2);

        typename ListT1::const_iterator it1 = list1.begin();
        REQUIRE(*it1 == -1);
        ++it1;
        REQUIRE(*it1 == -2);
        ++it1;
        REQUIRE(*it1 == -3);
        ++it1;
        REQUIRE(*it1 == -4);
        ++it1;
        REQUIRE(it1 == list1.end());

        typename ListT2::const_iterator it2 = list2.begin();
        REQUIRE(*it2 == 1);
        ++it2;
        REQUIRE(*it2 == 2);
        ++it2;
        REQUIRE(it2 == list2.end());

        list1.swap(list2);

        REQUIRE(list1.size() == 2);
        REQUIRE(list2.size() == 4);

        REQUIRE(*list1.begin() == 1);
        REQUIRE(*list2.begin() == -1);
    }

    SECTION("Swap with empty") {

        list2.clear();

        CHECK(list2.empty());

        list1.swap(list2);

        REQUIRE(list1.size() == 0);
        REQUIRE(list2.size() == 2);

        typename ListT1::const_iterator it1 = list1.begin();
        REQUIRE(it1 == list1.end());

        typename ListT2::const_iterator it2 = list2.begin();
        REQUIRE(*it2 == 1);
        ++it2;
        REQUIRE(*it2 == 2);
        ++it2;
        REQUIRE(it2 == list2.end());

        list1.swap(list2);

        REQUIRE(list1.size() == 2);
        REQUIRE(list2.size() == 0);

        REQUIRE(*list1.begin() == 1);
    }
}

template<class ListT>
void testListSwapIsNoCopy() {

    SECTION("Swap is no-copy") {

        ListT list1;

        list1.push_back(ContainerTester(1));
        list1.push_back(ContainerTester(2));

        ListT list2;

        list2.push_back(ContainerTester(-1));

        const uint32_t copyCnt = ContainerTester::getCopyCount();

        list1.swap(list2);

        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
    }
}

TEST_CASE("Etl::Dynamic::List<> swap", "[list][etl]") {

    typedef Etl::Dynamic::List<int> DListT;
    typedef Etl::Static::List<int, 16> SListT;
    typedef Etl::Pooled::List<int, 16> PListT;
    typedef Etl::Dynamic::List<ContainerTester> ListTNC;

    SECTION("D.swap(D)") {
        testListSwap<DListT, DListT>();
    }
    SECTION("D.swap(S)") {
        testListSwap<DListT, SListT>();
    }
    SECTION("D.swap(P)") {
        testListSwap<DListT, PListT>();
    }

    testListSwapIsNoCopy<ListTNC>();
}

TEST_CASE("Etl::Static::List<> swap", "[list][etl]") {

    typedef Etl::Dynamic::List<int> DListT;
    typedef Etl::Static::List<int, 16> SListT;
    typedef Etl::Pooled::List<int, 16> PListT;

    SECTION("S.swap(D)") {
        testListSwap<SListT, DListT>();
    }
    SECTION("S.swap(S)") {
        testListSwap<SListT, SListT>();
    }
    SECTION("S.swap(P)") {
        testListSwap<SListT, PListT>();
    }
}

TEST_CASE("Etl::Pooled::List<> swap", "[list][etl]") {

    typedef Etl::Dynamic::List<int> DListT;
    typedef Etl::Static::List<int, 32> SListT;
    typedef Etl::Pooled::List<int, 32> PListT;
    typedef Etl::Pooled::List<ContainerTester, 32> ListTNC;

    SECTION("P.swap(D)") {
        testListSwap<PListT, DListT>();
    }
    SECTION("P.swap(S)") {
        testListSwap<PListT, SListT>();
    }
    SECTION("P.swap(P)") {
        testListSwap<PListT, PListT>();
    }

    testListSwapIsNoCopy<ListTNC>();
}


template<class ListT1, class ListT2>
void testListSplice() {

    typedef typename ListT1::value_type ItemType;

    ListT1 list1;
    ListT2 list2;

    for (uint32_t i = 0; i < 8; ++i) {
        list2.push_back(ItemType(i));
    }

    CHECK(list1.size() == 0);
    CHECK(list2.size() == 8);

    SECTION("Splice to empty") {

        SECTION("Splice one item") {

            list1.splice(list1.end(), list2, list2.begin());

            REQUIRE(list1.size() == 1);
            REQUIRE(list2.size() == 7);

            REQUIRE(ContainerTester::getObjectCount() == 8);

            REQUIRE(*list1.begin() == ItemType(0));
            REQUIRE(*list2.begin() == ItemType(1));
        }

        SECTION("Splice range") {

            typename ListT2::const_iterator first = list2.begin();
            ++first;
            ++first;
            typename ListT2::const_iterator last = first;
            ++last;
            ++last;

            list1.splice(list1.end(), list2, first, last);

            REQUIRE(list1.size() == 2);
            REQUIRE(list2.size() == 6);

            REQUIRE(ContainerTester::getObjectCount() == 8);

            typename ListT1::const_iterator it1 = list1.begin();
            REQUIRE(*it1 == ItemType(2));
            ++it1;
            REQUIRE(*it1 == ItemType(3));
            ++it1;
            REQUIRE(it1 == list1.end());

            typename ListT2::const_iterator it2 = list2.begin();
            REQUIRE(*it2 == ItemType(0));
            ++it2;
            REQUIRE(*it2 == ItemType(1));
            ++it2;
            REQUIRE(*it2 == ItemType(4));
            ++it2;
            REQUIRE(*it2 == ItemType(5));
            ++it2;
            REQUIRE(*it2 == ItemType(6));
            ++it2;
            REQUIRE(*it2 == ItemType(7));
            ++it2;
            REQUIRE(it2 == list2.end());
        }

        SECTION("Splice all") {

            list1.splice(list1.end(), list2);

            REQUIRE(list1.size() == 8);
            REQUIRE(list2.size() == 0);

            REQUIRE(ContainerTester::getObjectCount() == 8);

            typename ListT1::const_iterator it = list1.begin();

            for (uint32_t i = 0; i < 8; ++i) {
                CAPTURE(i);
                CAPTURE(it->getValue());
                REQUIRE(*it == ItemType(i));
                ++it;
            }
        }
    }

    SECTION("Splice to existing") {

        list1.push_back(ItemType(8));
        CHECK(list1.size() == 1);

        typename ListT1::const_iterator pos = list1.end();

        typename ListT2::const_iterator it = list2.begin();
        ++it;
        ++it;
        ++it;

        CHECK(*it == ItemType(3));

        list1.splice(pos, list2, list2.begin(), it);

        REQUIRE(list1.size() == 4);
        REQUIRE(list2.size() == 5);

        --pos;

        REQUIRE(*pos == ItemType(2));

        list1.splice(pos, list2);

        REQUIRE(list1.size() == 9);
        REQUIRE(list2.size() == 0);

        REQUIRE(ContainerTester::getObjectCount() == 9);

        it = list1.begin();
        REQUIRE(*it == ItemType(8));
        ++it;
        REQUIRE(*it == ItemType(0));
        ++it;
        REQUIRE(*it == ItemType(1));
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
        REQUIRE(*it == ItemType(2));
        ++it;
        REQUIRE(it == list1.end());
    }
}

TEST_CASE("Etl::Dynamic::List<>::splice() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> DListT;
    typedef Etl::Static::List<ItemType, 16> SListT;
    typedef Etl::Pooled::List<ItemType, 16> PListT;

    SECTION("D.splice(D)") {
        testListSplice<DListT, DListT>();
    }
    SECTION("D.splice(S)") {
        testListSplice<DListT, SListT>();
    }
    SECTION("D.splice(P)") {
        testListSplice<DListT, PListT>();
    }
}

TEST_CASE("Etl::Static::List<>::splice() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> DListT;
    typedef Etl::Static::List<ItemType, 16> SListT;
    typedef Etl::Pooled::List<ItemType, 16> PListT;

    SECTION("S.splice(D)") {
        testListSplice<SListT, DListT>();
    }
    SECTION("S.splice(S)") {
        testListSplice<SListT, SListT>();
    }
    SECTION("S.splice(P)") {
        testListSplice<SListT, PListT>();
    }
}

TEST_CASE("Etl::Pooled::List<>::splice() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> DListT;
    typedef Etl::Static::List<ItemType, 16> SListT;
    typedef Etl::Pooled::List<ItemType, 16> PListT;

    SECTION("P.splice(D)") {
        testListSplice<PListT, DListT>();
    }
    SECTION("P.splice(S)") {
        testListSplice<PListT, SListT>();
    }
    SECTION("P.splice(P)") {
        testListSplice<PListT, PListT>();
    }
}


TEST_CASE("Etl::Dynamic::List<>::find(Etl::Matcher<>) test", "[list][etl]") {

    typedef int ItemType;
    typedef Etl::Dynamic::List<ItemType> ListT;

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

    ListT list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(REF_VALUE);
    ListT::iterator it1 = --list.end();
    list.push_back(4);
    list.push_back(REF_VALUE);
    ListT::iterator it2 = --list.end();
    list.push_back(6);

    ListT::iterator found = list.find(IntMatcher(REF_VALUE));
    REQUIRE(found == it1);
    found = list.find((++found), list.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == it2);
    found = list.find((++found), list.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == list.end());
}


TEST_CASE("Etl::Dynamic::List<>::find<F, V>() test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Dynamic::List<ItemType> ListT;

    static const int REF_VALUE = 123;
    const ItemType REF_ITEM(REF_VALUE);

    CAPTURE(REF_ITEM.getValue());

    ListT list;
    list.push_back(ItemType(1));
    list.push_back(ItemType(2));
    list.push_back(REF_ITEM);
    ListT::iterator it1 = --list.end();
    list.push_back(ItemType(4));
    list.push_back(REF_ITEM);
    ListT::iterator it2 = --list.end();
    list.push_back(ItemType(6));

    ListT::iterator found = list.find(&ItemType::getValue, REF_VALUE);
    REQUIRE(found == it1);
    found = list.find((++found), list.end(), &ItemType::getValue, REF_VALUE);
    REQUIRE(found == it2);
    found = list.find((++found), list.end(), &ItemType::getValue, REF_VALUE);
    REQUIRE(found == list.end());
}


TEST_CASE("Etl::List<> allocator test", "[list][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::List<ItemType, DummyAllocator> ListT;
    typedef ListT::Allocator AllocatorType;

    AllocatorType::reset();
    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    ListT list;
    list.push_back(ContainerTester(1));

    ListT::iterator it = list.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    list.push_back(ContainerTester(2));
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    list.pop_front();
    REQUIRE(AllocatorType::getDeleteCount() == 1);
}


template<class ListT, uint32_t NUM>
void testSizedListAllocation() {

    ListT list;

    CHECK(list.getAllocator().capacity() == NUM);
    CHECK(list.getAllocator().empty());

    SECTION("Basic allocation") {

        list.push_back(ContainerTester(1));

        typename ListT::iterator it = list.begin();
        REQUIRE(it.operator->() != NULL);

        list.push_back(ContainerTester(1));
        typename ListT::iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());

        REQUIRE_FALSE(list.getAllocator().empty());
        REQUIRE(list.getAllocator().size() == 2);
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            list.push_back(ContainerTester(i));
        }

        CHECK(list.size() == NUM);

        typename ListT::iterator it = list.insert(list.begin(), ContainerTester(NUM));
        REQUIRE(list.size() == NUM);
        REQUIRE(it == list.end());
    }

    list.clear();
    REQUIRE(list.getAllocator().empty());
}

TEST_CASE("Etl::Static::List<> test", "[list][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Static::List<ItemType, NUM> ListT;

    testSizedListAllocation<ListT, NUM>();

    SECTION("Unique pool checks") {

        ListT list;
        CHECK(list.getAllocator().capacity() == NUM);
        CHECK(list.getAllocator().empty());

        list.push_back(ContainerTester(1));
        list.push_back(ContainerTester(2));

        CHECK(list.size() == list.getAllocator().size());

        ListT list2;
        list2.push_back(ContainerTester(3));
        list2.push_back(ContainerTester(4));

        REQUIRE(list.size() == list.getAllocator().size());
        REQUIRE(list2.size() == list2.getAllocator().size());

        list2.splice(list2.begin(), list);

        CHECK(list.empty());
        REQUIRE(list.getAllocator().size() == 0);
        REQUIRE(list2.size() == list2.getAllocator().size());
    }
}


TEST_CASE("Etl::Pooled::List<> test", "[list][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Pooled::List<ItemType, NUM> ListT;

    testSizedListAllocation<ListT, NUM>();

    SECTION("Common pool checks") {

        ListT list;

        CHECK(list.getAllocator().capacity() == NUM);
        CHECK(list.getAllocator().empty());

        list.push_back(ContainerTester(1));
        list.push_back(ContainerTester(2));

        CHECK(list.size() == list.getAllocator().size());

        ListT list2;
        list2.push_back(ContainerTester(3));
        list2.push_back(ContainerTester(4));

        REQUIRE((list.size() + list2.size()) == list.getAllocator().size());

        list2.splice(list2.begin(), list);

        CHECK(list.empty());
        REQUIRE(list2.size() == list.getAllocator().size());
    }
}


TEST_CASE("Etl::List<> test cleanup", "[list][etl]") {

    typedef Etl::List<ContainerTester, DummyAllocator> ListT;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(ListT::Allocator::getDeleteCount() == ListT::Allocator::getAllocCount());
}


// Etl::List compatibility tests ---------------------------------------------


TEST_CASE("Etl::List<> with std::accumulate()", "[list][comp][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::List<ItemT> ListT;

    testAccumulate<ListT>();
}

TEST_CASE("Etl::List<> with std::partial_sum()", "[list][comp][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::List<ItemT> ListT;

    testPartialSum<ListT>();
}

TEST_CASE("Etl::List<> with std::inner_product()", "[list][comp][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::List<ItemT> ListT;

    testInnerProduct<ListT, ListT>();
}

