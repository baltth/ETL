/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2022-2023 Balazs Toth.

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
#include <etl/UnorderedSet.h>

#include "AtScopeEnd.h"
#include "ContainerTester.h"
#include "DummyAllocator.h"
#include "comparisionTests.h"
#include "swapTests.h"

using Etl::Test::ContainerTester;
using Etl::Test::DummyAllocator;
using Etl::Test::AtScopeEnd;

namespace CheckNoexcept {

using Etl::Detail::NothrowContract;

using StaticUnorderedSet = Etl::Static::UnorderedSet<int, 16U>;
using StaticUnorderedSetNested = Etl::Static::UnorderedSet<StaticUnorderedSet, 8U>;
using PooledUnorderedSet = Etl::Pooled::UnorderedSet<int, 16U, 8U>;
using DynamicUnorderedSet = Etl::Dynamic::UnorderedSet<int>;

TEMPLATE_TEST_CASE("UnorderedSet noexcept default constructor",
                   "[unorderedset][etl][basic]",
                   StaticUnorderedSet,
                   StaticUnorderedSetNested,
                   PooledUnorderedSet) {  // Dynamic skipped intentionally
    REQUIRE(NothrowContract<TestType>::nothrowIfDefaultConstructible);
    REQUIRE(NothrowContract<TestType>::nothrowIfDestructible);
}

TEMPLATE_TEST_CASE("UnorderedSet noexcept move",
                   "[unorderedset][etl][basic]",
                   StaticUnorderedSet,
                   StaticUnorderedSetNested,
                   PooledUnorderedSet,
                   DynamicUnorderedSet) {
    CAPTURE(NothrowContract<TestType>::nothrowIfMoveConstructible);
    CAPTURE(NothrowContract<TestType>::nothrowIfMoveAssignable);
    REQUIRE(NothrowContract<TestType>::nothrowIfMovable);
}

TEMPLATE_TEST_CASE("UnorderedSet noexcept swap",
                   "[unorderedset][etl][basic]",
                   StaticUnorderedSet,
                   StaticUnorderedSetNested,
                   PooledUnorderedSet,
                   DynamicUnorderedSet) {

    using std::swap;
    TestType c1;
    TestType c2;
    REQUIRE(noexcept(swap(c1, c2)));
}

}  // namespace CheckNoexcept

namespace {

TEST_CASE("Etl::Dynamic::UnorderedSet<> basic test", "[unorderedset][etl][basic]") {

    using SetType = Etl::Dynamic::UnorderedSet<ContainerTester>;
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


TEST_CASE("Etl::Dynamic::UnorderedSet<> insert() test", "[unorderedset][etl]") {

    using SetType = Etl::Dynamic::UnorderedSet<int>;
    using ResultType = std::pair<SetType::iterator, bool>;

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


TEST_CASE("Etl::Dynamic::UnorderedSet<> emplace() test", "[unorderedset][etl]") {

    using SetType = Etl::Dynamic::UnorderedSet<int>;
    using ResultType = std::pair<SetType::iterator, bool>;

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


TEST_CASE("Etl::Dynamic::UnorderedSet<> erase tests", "[unorderedset][etl]") {

    using SetType = Etl::Dynamic::UnorderedSet<int>;

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


TEST_CASE("Etl::Dynamic::UnorderedSet<> iteration tests", "[unorderedset][etl]") {

    using SetType = Etl::Dynamic::UnorderedSet<int>;

    SetType set;

    Etl::Dynamic::Set<int> input = {1, 2, 3, 4};

    for (auto item : input) {
        set.insert(item);
    }

    CHECK(set.size() == input.size());

    Etl::Dynamic::Set<int> seen {};

    SECTION("with iterators") {

        auto it = set.begin();
        while (it != set.end()) {
            seen.insert(*it);
            ++it;
        }

        REQUIRE(seen == input);
    }

    SECTION("with range-based for") {

        for (auto& item : set) {
            seen.insert(item);
        }

        REQUIRE(seen == input);
    }
}


TEST_CASE("Etl::Dynamic::UnorderedSet<> copy", "[unorderedset][etl]") {

    using SetType = Etl::Dynamic::UnorderedSet<int>;

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

    SECTION("swap()") {

        set.swap(set2);

        REQUIRE(set2.size() == 4);
        REQUIRE(set.size() == 2);

        REQUIRE(set.find(1) != set.end());
        REQUIRE(set.find(5) != set.end());

        REQUIRE(set2.find(1) != set2.end());
        REQUIRE(set2.find(2) != set2.end());
        REQUIRE(set2.find(3) != set2.end());
        REQUIRE(set2.find(4) != set2.end());
    }
}


TEST_CASE("Etl::UnorderedSet<> swap", "[unorderedset][etl]") {

    using Etl::Test::NonAssignable;

    using SIC = Etl::Static::UnorderedSet<int, 4>;
    using PIC = Etl::Pooled::UnorderedSet<int, 8, 4>;
    using DIC = Etl::Dynamic::UnorderedSet<int>;
    using SNMC = Etl::Static::UnorderedSet<NonAssignable, 4>;
    using PNMC = Etl::Pooled::UnorderedSet<NonAssignable, 8, 4>;
    using DNMC = Etl::Dynamic::UnorderedSet<NonAssignable>;

    SECTION("with assignable type") {

        auto insert = [](Etl::UnorderedSet<int>& set, int v) { set.insert(v); };

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

        auto insert = [](Etl::UnorderedSet<NonAssignable>& set, int v) { set.emplace(v); };

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


TEST_CASE("Etl::Dynamic::UnorderedSet<> search tests", "[unorderedset][etl]") {

    using SetType = Etl::Dynamic::UnorderedSet<int>;
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


TEMPLATE_TEST_CASE("Etl::UnorderedSet<> bucket interface tests",
                   "[unorderedset][etl]",
                   (Etl::Static::UnorderedSet<int, 64U, 16U>),
                   (Etl::Pooled::UnorderedSet<int, 64U, 16U>)) {

    static const size_t BUCKETS {16};
    static const size_t MOD {BUCKETS};

    TestType set;

    using Input = Etl::Dynamic::Set<int>;

    auto fill = [](Input& input, int base) {
        for (size_t i = 0; i < 5; ++i) {
            input.insert(base + (i * MOD));
        }
    };

    Input inBucket1;
    fill(inBucket1, 1);
    Input inBucket2;
    fill(inBucket2, 2);
    Input inBucket4;
    fill(inBucket4, 4);

    CHECK(set.hash_function()(34) == 34);

    CHECK_FALSE(inBucket1.empty());
    CHECK_FALSE(inBucket2.empty());
    CHECK_FALSE(inBucket4.empty());

    for (auto item : inBucket1) {
        set.insert(item);
    }

    for (auto item : inBucket2) {
        set.insert(item);
    }

    for (auto item : inBucket4) {
        set.insert(item);
    }

    CHECK(set.size() == (inBucket1.size() + inBucket2.size() + inBucket4.size()));

    SECTION("bucket()") {
        REQUIRE(set.bucket(*inBucket1.begin()) != set.bucket(*inBucket2.begin()));
    }

    SECTION("bucket_size()") {

#if 0
        auto& ht = set.ht();
        size_t cnt = 0;
        ht.inspectNodes([&cnt](size_t hash, size_t ix, const void* node) {
            std::cout << "#" << cnt << " ix " << ix << " @ " << node << std::endl;
            ++cnt;
        });
#endif

        CHECK(set.bucket_size(0) == 0);
        CHECK(set.bucket_size(1) == inBucket1.size());
        CHECK(set.bucket_size(2) == inBucket2.size());
        CHECK(set.bucket_size(3) == 0);
        CHECK(set.bucket_size(4) == inBucket4.size());
        CHECK(set.bucket_size(5) == 0);

        REQUIRE(set.max_bucket_count() == BUCKETS);
    }

    SECTION("bucket iteration") {

        auto checkBucket = [](const TestType& set, uint32_t ix, const Input& input) {
            CAPTURE(ix);
            CHECK(set.bucket_size(ix) == input.size());
            auto it = set.begin(ix);
            while (it != set.end(ix)) {
                CAPTURE(*it);
                REQUIRE(input.find(*it) != input.end());
                ++it;
            }
        };

        checkBucket(set, 1, inBucket1);
        checkBucket(set, 2, inBucket2);
        checkBucket(set, 4, inBucket4);
    }
}


TEST_CASE("Etl::Custom::UnorderedSet<> allocator test", "[unorderedset][etl]") {

    using ItemType = ContainerTester;
    using SetType = Etl::Custom::UnorderedSet<ItemType, DummyAllocator>;
    using NodeAllocatorType = SetType::NodeAllocator::Allocator;
    using BucketAllocatorType = SetType::BucketImpl::Allocator;

    auto end = AtScopeEnd([]() {
        NodeAllocatorType::reset();
        BucketAllocatorType::reset();
    });

    CHECK(NodeAllocatorType::getAllocCount() == 0);
    CHECK(NodeAllocatorType::getDeleteCount() == 0);
    CHECK(BucketAllocatorType::getAllocCount() == 0);
    CHECK(BucketAllocatorType::getDeleteCount() == 0);

    SetType set;
    CHECK(BucketAllocatorType::getAllocCount() > 0);
    CHECK(NodeAllocatorType::getDeleteCount() == 0);

    set.insert(ContainerTester(5));

    SetType::iterator it = set.begin();
    REQUIRE(it.operator->() == &(NodeAllocatorType::ptrOfAllocation(0)->item));

    set.insert(ContainerTester(6));
    ++it;
    REQUIRE(it.operator->() == &(NodeAllocatorType::ptrOfAllocation(1)->item));

    CHECK(NodeAllocatorType::getDeleteCount() == 0);

    set.erase(ContainerTester(5));
    REQUIRE(NodeAllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Pooled::UnorderedSet<> test", "[unorderedset][etl]") {

    static const size_t NUM {16U};
    using ItemType = ContainerTester;
    using SetType = Etl::Pooled::UnorderedSet<ItemType, NUM, NUM / 2U>;

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

        for (size_t i = 0; i < NUM; ++i) {
            set.insert(ContainerTester(i));
        }

        CHECK(set.size() == NUM);

        std::pair<SetType::iterator, bool> res = set.insert(ContainerTester(NUM));
        REQUIRE(set.size() == NUM);
        REQUIRE(res.first == set.end());
        REQUIRE(res.second == false);
    }
}


TEST_CASE("Etl::UnorderedSet<> test cleanup", "[unorderedset][etl]") {

    using SetType = Etl::Custom::UnorderedSet<ContainerTester, DummyAllocator>;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(SetType::NodeAllocator::Allocator::getDeleteCount()
          == SetType::NodeAllocator::Allocator::getAllocCount());
}


// Etl::UnorderedSet comparision tests ----------------------------------------------


TEST_CASE("Etl::UnorderedSet<> equivalence", "[unorderedset][etl]") {

    SECTION("Etl::UnorderedSet<> vs Etl::UnorderedSet<>") {

        using SetType = Etl::Dynamic::UnorderedSet<int>;
        using Base = Etl::UnorderedSet<int>;

        SetType lhs;
        SetType rhs;

        auto inserter = [](Base& cont, int val) { cont.emplace(val); };

        testEquivalence(static_cast<Base&>(lhs),
                        static_cast<Base&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::UnorderedSet<> vs Etl::Static::UnorderedSet<>") {

        using LType = Etl::Dynamic::UnorderedSet<int>;
        using RType = Etl::Static::UnorderedSet<int, 32U>;

        LType lhs;
        RType rhs;

        auto lInserter = [](LType& cont, int val) { cont.emplace(val); };

        auto rInserter = [](RType& cont, int val) { cont.emplace(val); };

        testEquivalence(lhs,
                        rhs,
                        lInserter,
                        rInserter);
    }
}

}  // namespace
