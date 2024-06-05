/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017-2024 Balazs Toth.

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

#include <catch2/catch.hpp>

#include <etl/List.h>

#include <memory>

#include "AtScopeEnd.h"
#include "ContainerTester.h"
#include "DummyAllocator.h"
#include "comparisionTests.h"
#include "compatibilityTests.h"
#include "constructorTests.h"
#include "sequenceTests.h"
#include "swapTests.h"

using Etl::Test::ContainerTester;
using Etl::Test::DummyAllocator;
using Etl::Test::AtScopeEnd;

namespace {

namespace CheckNoexcept {

using Etl::Detail::NothrowContract;

using SC = Etl::Static::List<int, 16U>;
using SCSC = Etl::Static::List<SC, 8U>;
using PC = Etl::Pooled::List<int, 16U>;
using DC = Etl::Dynamic::List<int>;

TEMPLATE_TEST_CASE("List nothrow contract",
                   "[list][etl][basic]",
                   SC,
                   SCSC,
                   PC,
                   DC) {

    static_assert(NothrowContract<TestType>::value, "nothrow contract violation");
    using std::swap;
    TestType c1;
    TestType c2;
    static_assert(noexcept(swap(c1, c2)), "swap() nothrow contract violation");
}

}  // namespace CheckNoexcept


TEMPLATE_TEST_CASE("Etl::List<> basic test",
                   "[list][etl][basic]",
                   (Etl::Dynamic::List<int>),
                   (Etl::Static::List<int, 16U>),
                   (Etl::Pooled::List<int, 16U>)) {

    TestType list;

    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());

    list.push_back(2);

    REQUIRE_FALSE(list.empty());

    list.push_front(1);

    REQUIRE(list.size() == 2);
    REQUIRE_FALSE(list.empty());

    auto it = list.begin();
    REQUIRE(it != list.end());
    REQUIRE(*it == 1);
    ++it;
    REQUIRE(it != list.end());
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(it == list.end());

    it = list.insert(list.begin(), 3);
    REQUIRE(list.size() == 3);
    REQUIRE(it != list.end());
    REQUIRE(*it == 3);

    it = list.emplace(list.end(), 4);
    REQUIRE(list.size() == 4);
    REQUIRE(it != list.end());
    REQUIRE(*it == 4);

    list.pop_front();
    list.erase(list.begin());
    list.pop_back();

    REQUIRE(list.size() == 1);
    REQUIRE_FALSE(list.empty());

    list.pop_back();

    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());
}


TEMPLATE_TEST_CASE("Etl::List<> constructor test",
                   "[list][etl][basic]",
                   (Etl::Dynamic::List<ContainerTester>),
                   (Etl::Static::List<ContainerTester, 16U>),
                   (Etl::Pooled::List<ContainerTester, 16U>)) {

    SECTION("L(size_t)") {
        Etl::Test::constructForSize<TestType>();
    }

    SECTION("L(size_t, const value_type&)") {
        Etl::Test::constructForSizeAndValue<TestType>(ContainerTester {17});
    }

    SECTION("L(InputIt, IntputIt)") {
        Etl::Test::constructWithIterators<TestType>();
    }

    SECTION("L(std::initializer_list)") {
        Etl::Test::constructWithInitList<TestType>();
    }
}


TEMPLATE_TEST_CASE("Etl::List<> push/pop test",
                   "[list][etl][basic]",
                   (Etl::Dynamic::List<int>),
                   (Etl::Static::List<int, 16U>),
                   (Etl::Pooled::List<int, 16U>)) {

    Etl::Test::testBackAccess<TestType>();
    Etl::Test::testFrontAccess<TestType>();
}


TEMPLATE_TEST_CASE("Etl::List<> iteration test",
                   "[list][etl][basic]",
                   (Etl::Dynamic::List<int>),
                   (Etl::Static::List<int, 16U>),
                   (Etl::Pooled::List<int, 16U>)) {

    SECTION("iterator") {
        Etl::Test::testIterationForward<TestType>();
        Etl::Test::testIterationBackward<TestType>();
    }

    SECTION("reverse_iterator") {
        Etl::Test::testReverseIterationForward<TestType>();
        Etl::Test::testReverseIterationBackward<TestType>();
    }
}


template<class ListT>
void testListLeak() {

    typedef typename ListT::value_type ItemType;
    static const int PATTERN = 123;

    CHECK(ItemType::getObjectCount() == 0);

    SECTION("basic ops") {

        ListT list;

        for (int i = 0; i < 8; ++i) {
            list.push_back(ContainerTester(PATTERN));
        }

        CHECK(list.size() == 8);
        CHECK(list.size() == ItemType::getObjectCount());

        list.pop_back();
        REQUIRE(list.size() == ItemType::getObjectCount());

        list.erase(list.begin());
        REQUIRE(list.size() == ItemType::getObjectCount());
    }

    SECTION("copy") {

        ListT list1;

        for (int i = 0; i < 8; ++i) {
            list1.push_back(ContainerTester(PATTERN));
        }

        ListT list2;

        for (int i = 0; i < 3; ++i) {
            list2.push_back(ContainerTester(PATTERN));
        }

        CHECK(list1.size() == 8);
        CHECK(list2.size() == 3);
        CHECK((list1.size() + list2.size()) == ItemType::getObjectCount());

        list2 = list1;

        CHECK(list2.size() == list1.size());
        REQUIRE((list1.size() + list2.size()) == ItemType::getObjectCount());
    }

    REQUIRE(ItemType::getObjectCount() == 0);
}


TEMPLATE_TEST_CASE("Etl::List<> leak test",
                   "[list][etl]",
                   (Etl::Dynamic::List<ContainerTester>),
                   (Etl::Static::List<ContainerTester, 16U>),
                   (Etl::Pooled::List<ContainerTester, 16U>)) {

    testListLeak<TestType>();

    if (!TestType::Allocator::uniqueAllocator) {
        TestType l;
        REQUIRE(l.getAllocator().size() == 0);
    }
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


TEMPLATE_TEST_CASE("Etl::List<> copy",
                   "[list][etl]",
                   (Etl::Dynamic::List<int>),
                   (Etl::Static::List<int, 32U>),
                   (Etl::Pooled::List<int, 32U>)) {

    testListCopy<TestType>();
}


template<class ListT>
void testSwapIsNoCopy() {

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


TEST_CASE("Etl::List<> swap", "[list][etl]") {

    using Etl::Test::NonAssignable;

    using SIC = Etl::Static::List<int, 4>;
    using PIC = Etl::Pooled::List<int, 8>;
    using DIC = Etl::Dynamic::List<int>;
    using SNMC = Etl::Static::List<NonAssignable, 4>;
    using PNMC = Etl::Pooled::List<NonAssignable, 8>;
    using DNMC = Etl::Dynamic::List<NonAssignable>;

    SECTION("with assignable type") {

        auto insert = [](Etl::List<int>& list, int v) { list.push_back(v); };

        SECTION("self: Static") {
            using Self = SIC;

            SECTION("other: Static") {
                Etl::Test::testSwapOrdered<Self, SIC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapOrdered<Self, PIC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapOrdered<Self, DIC>(insert);
            }
        }

        SECTION("self: Pooled") {
            using Self = PIC;

            SECTION("other: Static") {
                Etl::Test::testSwapOrdered<Self, SIC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapOrdered<Self, PIC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapOrdered<Self, DIC>(insert);
            }
        }

        SECTION("self: Dynamic") {
            using Self = DIC;

            SECTION("other: Static") {
                Etl::Test::testSwapOrdered<Self, SIC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapOrdered<Self, PIC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapOrdered<Self, DIC>(insert);
            }
        }
    }

    SECTION("with non-assignable type") {

        auto insert = [](Etl::List<NonAssignable>& list, int v) { list.emplace_back(v); };

        SECTION("self: Static") {
            using Self = SNMC;

            SECTION("other: Static") {
                Etl::Test::testSwapOrdered<Self, SNMC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapOrdered<Self, PNMC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapOrdered<Self, DNMC>(insert);
            }
        }

        SECTION("self: Pooled") {
            using Self = PNMC;

            SECTION("other: Static") {
                Etl::Test::testSwapOrdered<Self, SNMC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapOrdered<Self, PNMC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapOrdered<Self, DNMC>(insert);
            }
        }

        SECTION("self: Dynamic") {
            using Self = DNMC;

            SECTION("other: Static") {
                Etl::Test::testSwapOrdered<Self, SNMC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapOrdered<Self, PNMC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapOrdered<Self, DNMC>(insert);
            }
        }
    }

    testSwapIsNoCopy<Etl::Pooled::List<ContainerTester, 32>>();
    testSwapIsNoCopy<Etl::Dynamic::List<ContainerTester>>();
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


TEST_CASE("Etl::Custom::List<> allocator test", "[list][etl]") {

    using ItemType = ContainerTester;
    using ListT = Etl::Custom::List<ItemType, DummyAllocator>;
    using AllocatorType = ListT::Allocator::Allocator;

    auto end = AtScopeEnd([]() {
        REQUIRE(AllocatorType::getDeleteCount() == AllocatorType::getAllocCount());
        AllocatorType::reset();
    });

    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    ListT list;
    REQUIRE(AllocatorType::getAllocCount() == 0);
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

    CHECK(list.getAllocator().max_size() == NUM);
    CHECK(list.getAllocator().size() == 0);

    SECTION("Basic allocation") {

        list.push_back(ContainerTester(1));

        typename ListT::iterator it = list.begin();
        REQUIRE(it.operator->() != NULL);

        list.push_back(ContainerTester(1));
        typename ListT::iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());

        REQUIRE_FALSE(list.getAllocator().size() == 0);
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
    REQUIRE(list.getAllocator().size() == 0);
}


TEST_CASE("Etl::Static::List<> test", "[list][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Static::List<ItemType, NUM> ListT;

    testSizedListAllocation<ListT, NUM>();

    SECTION("Unique pool checks") {

        ListT list;
        CHECK(list.getAllocator().max_size() == NUM);
        CHECK(list.getAllocator().size() == 0);

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

        CHECK(list.getAllocator().max_size() == NUM);
        CHECK(list.getAllocator().size() == 0);

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

    typedef Etl::Custom::List<ContainerTester, DummyAllocator> ListT;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(ListT::Allocator::Allocator::getDeleteCount()
          == ListT::Allocator::Allocator::getAllocCount());
}


// Etl::List comparision tests ---------------------------------------------


TEST_CASE("Etl::List<> comparision", "[list][etl]") {

    SECTION("Etl::List<> vs Etl::List<>") {

        using ListType = Etl::Dynamic::List<int>;
        using Base = Etl::List<int>;

        ListType lhs;
        ListType rhs;

        auto inserter = [](Base& cont, int val) { cont.push_back(val); };

        testComparision(static_cast<Base&>(lhs),
                        static_cast<Base&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::List<> vs Etl::Static::List<>") {

        using LType = Etl::Dynamic::List<int>;
        using RType = Etl::Static::List<int, 32U>;

        LType lhs;
        RType rhs;

        auto lInserter = [](LType& cont, int val) { cont.push_back(val); };
        auto rInserter = [](RType& cont, int val) { cont.push_back(val); };

        testComparision(lhs,
                        rhs,
                        lInserter,
                        rInserter);
    }
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


TEST_CASE("Etl::List<reference_wrapper<T>> tests", "[list][comp][etl]") {

    typedef std::reference_wrapper<int> ItemT;
    typedef Etl::Dynamic::List<ItemT> ListT;

    int i0 = 0;
    int i1 = 1;
    int i2 = 2;
    int i3 = 3;

    ListT list;
    list.push_back(std::ref(i0));
    list.push_back(std::ref(i1));
    list.push_back(std::ref(i2));
    list.push_back(std::ref(i3));

    REQUIRE(list.size() == 4);

    ListT::iterator it = list.begin();

    for (size_t i = 0; i < list.size(); ++i) {
        REQUIRE(it->get() == static_cast<int>(i));
        ++it;
    }

    it = list.begin();
    *it = std::ref(i3);
    ++it;
    *it = std::ref(i2);
    ++it;
    *it = std::ref(i1);
    ++it;
    *it = std::ref(i0);

    it = list.begin();
    for (size_t i = 0; i < list.size(); ++i) {
        REQUIRE(it->get() == static_cast<int>(list.size() - i - 1));
        ++it;
    }

    list.back().get() = -1;
    REQUIRE(i0 == -1);
}

}  // namespace
