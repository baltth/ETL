/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017-2023 Balazs Toth.

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

#include <etl/Set.h>

#include "AtScopeEnd.h"
#include "ContainerTester.h"
#include "DummyAllocator.h"
#include "comparisionTests.h"
#include "swapTests.h"

using Etl::Test::ContainerTester;
using Etl::Test::DummyAllocator;
using Etl::Test::AtScopeEnd;

namespace {

namespace CheckNoexcept {

using Etl::Detail::NothrowContract;

using SC = Etl::Static::Set<int, 16U>;
using SCSC = Etl::Static::Set<SC, 8U>;
using PC = Etl::Pooled::Set<int, 16U>;
using DC = Etl::Dynamic::Set<int>;

TEMPLATE_TEST_CASE("Set nothrow contract",
                   "[set][etl][basic]",
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


TEST_CASE("Etl::Dynamic::Set<> basic test", "[set][etl][basic]") {

    typedef Etl::Dynamic::Set<ContainerTester> SetType;
    SetType set;

    REQUIRE(set.empty());
    REQUIRE(set.size() == 0);

    ContainerTester a(4);
    set.insert(a);

    REQUIRE_FALSE(set.empty());
    REQUIRE(set.size() == 1);
    SetType::iterator it = set.begin();
    REQUIRE(*it == a);

    set.insert(ContainerTester(5));

    REQUIRE(set.size() == 2);
    ++it;
    REQUIRE(*it == ContainerTester(5));

    ContainerTester b(4);
    set.insert(b);

    REQUIRE(set.size() == 2);

    set.erase(ContainerTester(5));

    REQUIRE(set.size() == 1);
}


TEST_CASE("Etl::Dynamic::Set<> insert() test", "[set][etl]") {

    typedef Etl::Dynamic::Set<int> SetType;
    typedef std::pair<SetType::iterator, bool> ResultType;

    SetType set;

    ResultType res = set.insert(1);

    REQUIRE(res.second == true);
    REQUIRE(res.first != set.end());
    REQUIRE(set.size() == 1);

    SECTION("first element") {

        REQUIRE(*(res.first) == 1);
    }

    SECTION("second element") {

        ResultType res2 = set.insert(2);

        REQUIRE(res2.first != set.end());
        REQUIRE(res2.second == true);
        REQUIRE(set.size() == 2);
        REQUIRE(*(res2.first) == 2);
    }

    SECTION("insert() of existing shall fail") {

        ResultType res2 = set.insert(1);

        REQUIRE(res2.first != set.end());
        REQUIRE(res2.second == false);
        REQUIRE(set.size() == 1);
    }
}


TEST_CASE("Etl::Dynamic::Set<> emplace() test", "[set][etl]") {

    typedef Etl::Dynamic::Set<int> SetType;
    typedef std::pair<SetType::iterator, bool> ResultType;

    SetType set;

    ResultType res = set.emplace(1);

    REQUIRE(res.second == true);
    REQUIRE(res.first != set.end());
    REQUIRE(set.size() == 1);

    SECTION("first element") {

        REQUIRE(*(res.first) == 1);
    }

    SECTION("second element") {

        ResultType res2 = set.emplace(2);

        REQUIRE(res2.first != set.end());
        REQUIRE(res2.second == true);
        REQUIRE(set.size() == 2);
        REQUIRE(*(res2.first) == 2);
    }

    SECTION("emplace() of existing shall fail") {

        ResultType res2 = set.emplace(1);

        REQUIRE(res2.first != set.end());
        REQUIRE(res2.second == false);
        REQUIRE(set.size() == 1);
    }
}


TEST_CASE("Etl::Dynamic::Set<> erase tests", "[set][etl]") {

    typedef Etl::Dynamic::Set<int> SetType;

    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    CHECK(set.size() == 4);

    SECTION("erase(Element)") {

        set.erase(2);

        REQUIRE(set.size() == 3);
        REQUIRE(set.find(2) == set.end());
    }

    SECTION("erase(iterator)") {

        SetType::iterator it = set.find(2);
        CHECK(it != set.end());

        it = set.erase(it);

        REQUIRE(set.size() == 3);
        REQUIRE(set.find(2) == set.end());
        REQUIRE(it == set.find(3));
    }
}


TEST_CASE("Etl::Dynamic::Set<> iteration tests", "[set][etl]") {

    typedef Etl::Dynamic::Set<int> SetType;

    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    CHECK(set.size() == 4);

    SECTION("forward") {

        SetType::iterator it = set.begin();

        REQUIRE(*it == 1);

        ++it;

        REQUIRE(*it == 2);
    }

    SECTION("backward") {

        SetType::iterator it = set.end();

        --it;

        REQUIRE(*it == 4);

        --it;

        REQUIRE(*it == 3);
    }
}


TEST_CASE("Etl::Dynamic::Set<> element order", "[set][etl]") {

    typedef Etl::Dynamic::Set<int> SetType;

    SetType set;

    set.insert(3);
    set.insert(1);
    set.insert(2);
    set.insert(4);

    CHECK(set.size() == 4);

    SetType::iterator it = set.begin();

    REQUIRE(*it == 1);

    ++it;
    REQUIRE(*it == 2);

    ++it;
    REQUIRE(*it == 3);

    ++it;
    REQUIRE(*it == 4);

    ++it;
    REQUIRE(it == set.end());
}


TEST_CASE("Etl::Dynamic::Set<> copy", "[set][etl]") {

    typedef Etl::Dynamic::Set<int> SetType;

    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    SetType set2;

    set2.insert(1);
    set2.insert(5);

    CHECK(set.size() == 4);
    CHECK(set2.size() == 2);

    SECTION("copy assignment") {

        set2 = set;

        REQUIRE(set2.size() == 4);
        REQUIRE(set2.find(1) != set2.end());
        REQUIRE(set2.find(4) != set2.end());
    }

    SECTION("copy constructor") {

        SetType set3 = set;

        REQUIRE(set3.size() == 4);
        REQUIRE(set3.find(1) != set3.end());
        REQUIRE(set3.find(4) != set3.end());
    }
}


TEST_CASE("Etl::Set<> swap", "[set][etl]") {

    using Etl::Test::NonAssignable;

    using SIC = Etl::Static::Set<int, 4>;
    using PIC = Etl::Pooled::Set<int, 8>;
    using DIC = Etl::Dynamic::Set<int>;
    using SNMC = Etl::Static::Set<NonAssignable, 4>;
    using PNMC = Etl::Pooled::Set<NonAssignable, 8>;
    using DNMC = Etl::Dynamic::Set<NonAssignable>;

    SECTION("with assignable type") {

        auto insert = [](Etl::Set<int>& set, int v) { set.insert(v); };

        SECTION("self: Static") {
            using Self = SIC;

            SECTION("other: Static") {
                Etl::Test::testSwapAssociative<Self, SIC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapAssociative<Self, PIC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapAssociative<Self, DIC>(insert);
            }
        }

        SECTION("self: Pooled") {
            using Self = PIC;

            SECTION("other: Static") {
                Etl::Test::testSwapAssociative<Self, SIC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapAssociative<Self, PIC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapAssociative<Self, DIC>(insert);
            }
        }

        SECTION("self: Dynamic") {
            using Self = DIC;

            SECTION("other: Static") {
                Etl::Test::testSwapAssociative<Self, SIC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapAssociative<Self, PIC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapAssociative<Self, DIC>(insert);
            }
        }
    }

    SECTION("with non-assignable type") {

        auto insert = [](Etl::Set<NonAssignable>& set, int v) { set.emplace(v); };

        SECTION("self: Static") {
            using Self = SNMC;

            SECTION("other: Static") {
                Etl::Test::testSwapAssociative<Self, SNMC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapAssociative<Self, PNMC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapAssociative<Self, DNMC>(insert);
            }
        }

        SECTION("self: Pooled") {
            using Self = PNMC;

            SECTION("other: Static") {
                Etl::Test::testSwapAssociative<Self, SNMC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapAssociative<Self, PNMC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapAssociative<Self, DNMC>(insert);
            }
        }

        SECTION("self: Dynamic") {
            using Self = DNMC;

            SECTION("other: Static") {
                Etl::Test::testSwapAssociative<Self, SNMC>(insert);
            }

            SECTION("other: Pooled") {
                Etl::Test::testSwapAssociative<Self, PNMC>(insert);
            }

            SECTION("other: Dynamic") {
                Etl::Test::testSwapAssociative<Self, DNMC>(insert);
            }
        }
    }
}


TEST_CASE("Etl::Dynamic::Set<> search tests", "[set][etl]") {

    typedef Etl::Dynamic::Set<int> SetType;
    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    CHECK(set.size() == 4);

    SECTION("find(Key)") {

        SetType::iterator it = set.find(3);

        REQUIRE(it != set.end());
        REQUIRE(*it == 3);
    }

    SECTION("find(Key) non-existing") {

        SetType::iterator it = set.find(7);

        REQUIRE(it == set.end());
    }
}


TEST_CASE("Etl::Set<> custom compare tests", "[set][etl]") {

    typedef Etl::Dynamic::Set<int, std::greater<int>> SetType;
    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    CHECK(set.size() == 4);
    REQUIRE(*set.begin() == 4);
}


TEST_CASE("Etl::Custom::Set<> allocator test", "[set][etl]") {

    using ItemType = ContainerTester;
    using SetType = Etl::Custom::Set<ItemType, DummyAllocator>;
    using AllocatorType = SetType::Allocator::Allocator;

    auto end = AtScopeEnd([]() {
        REQUIRE(AllocatorType::getDeleteCount() == AllocatorType::getAllocCount());
        AllocatorType::reset();
    });

    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    SetType set;
    REQUIRE(AllocatorType::getAllocCount() == 0);
    set.insert(ContainerTester(5));

    SetType::iterator it = set.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    set.insert(ContainerTester(6));
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    set.erase(ContainerTester(5));
    REQUIRE(AllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Pooled::Set<> test", "[set][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Pooled::Set<ItemType, NUM> SetType;

    SetType set;

    SECTION("Basic allocation") {

        set.insert(ContainerTester(5));

        SetType::iterator it = set.begin();
        REQUIRE(it.operator->() != NULL);

        set.insert(ContainerTester(6));
        SetType::iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            set.insert(ContainerTester(i));
        }

        CHECK(set.size() == NUM);

        std::pair<SetType::iterator, bool> res = set.insert(ContainerTester(NUM));
        REQUIRE(set.size() == NUM);
        REQUIRE(res.first == set.end());
        REQUIRE(res.second == false);
    }
}


TEST_CASE("Etl::Set<> test cleanup", "[set][etl]") {

    typedef Etl::Custom::Set<ContainerTester, DummyAllocator> SetType;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(SetType::Allocator::Allocator::getDeleteCount()
          == SetType::Allocator::Allocator::getAllocCount());
}


// Etl::Set comparision tests ----------------------------------------------


TEST_CASE("Etl::Set<> comparision", "[set][etl]") {

    SECTION("Etl::Set<> vs Etl::Set<>") {

        using SetType = Etl::Dynamic::Set<int>;
        using Base = Etl::Set<int>;

        SetType lhs;
        SetType rhs;

        auto inserter = [](Base& cont, int val) { cont.emplace(val); };

        testComparision(static_cast<Base&>(lhs),
                        static_cast<Base&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::Set<> vs Etl::Static::Set<>") {

        using LType = Etl::Dynamic::Set<int>;
        using RType = Etl::Static::Set<int, 32U>;

        LType lhs;
        RType rhs;

        auto lInserter = [](LType& cont, int val) { cont.emplace(val); };

        auto rInserter = [](RType& cont, int val) { cont.emplace(val); };

        testComparision(lhs,
                        rhs,
                        lInserter,
                        rInserter);
    }
}

}  // namespace
