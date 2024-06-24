/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2022-2024 Balazs Toth.

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

#include <algorithm>
#include <iterator>

#include <etl/List.h>
#include <etl/Set.h>
#include <etl/UnorderedMultiMap.h>

#include "AtScopeEnd.h"
#include "ContainerTester.h"
#include "DummyAllocator.h"
#include "comparisionTests.h"
#include "swapTests.h"

using Etl::Test::ContainerTester;
using Etl::Test::DummyAllocator;
using Etl::Test::AtScopeEnd;

namespace {

namespace CompileTimeChecks {

using Etl::Detail::NothrowContract;

using StaticUnorderedMultiMap = Etl::Static::UnorderedMultiMap<int, int, 16U>;
using StaticUnorderedMultiMapNested =
    Etl::Static::UnorderedMultiMap<int, StaticUnorderedMultiMap, 8U>;
using PooledUnorderedMultiMap = Etl::Pooled::UnorderedMultiMap<int, int, 16U, 4U>;
using DynamicUnorderedMultiMap = Etl::Dynamic::UnorderedMultiMap<int, int>;

TEMPLATE_TEST_CASE("UnorderedMultiMap noexcept default constructor",
                   "[unorderedmultimap][etl]",
                   StaticUnorderedMultiMap,
                   StaticUnorderedMultiMapNested,
                   PooledUnorderedMultiMap) {  // Dynamic skipped intentionally
    REQUIRE(NothrowContract<TestType>::nothrowIfDefaultConstructible);
    REQUIRE(NothrowContract<TestType>::nothrowIfDestructible);
}

TEMPLATE_TEST_CASE("UnorderedMultiMap noexcept move",
                   "[unorderedmultimap][etl]",
                   StaticUnorderedMultiMap,
                   StaticUnorderedMultiMapNested,
                   PooledUnorderedMultiMap,
                   DynamicUnorderedMultiMap) {
    CAPTURE(NothrowContract<TestType>::nothrowIfMoveConstructible);
    CAPTURE(NothrowContract<TestType>::nothrowIfMoveAssignable);
    REQUIRE(NothrowContract<TestType>::nothrowIfMovable);
}

TEMPLATE_TEST_CASE("UnorderedMultiMap noexcept swap",
                   "[unorderedmultimap][etl]",
                   StaticUnorderedMultiMap,
                   StaticUnorderedMultiMapNested,
                   PooledUnorderedMultiMap,
                   DynamicUnorderedMultiMap) {

    using std::swap;
    TestType c1;
    TestType c2;
    REQUIRE(noexcept(swap(c1, c2)));
}


static_assert(
    std::is_same<std::iterator_traits<PooledUnorderedMultiMap::iterator>::iterator_category,
                 std::forward_iterator_tag>::value,
    "Wrong iterator category for UnorderedMultiMap<>::iterator");

static_assert(
    std::is_same<std::iterator_traits<PooledUnorderedMultiMap::const_iterator>::iterator_category,
                 std::forward_iterator_tag>::value,
    "Wrong iterator category for UnorderedMultiMap<>::const_iterator");

}  // namespace CompileTimeChecks


template<class It>
Etl::Dynamic::List<typename It::value_type::second_type> fetchValues(It first, It last) {
    using Value = typename It::value_type;
    using Res = Etl::Dynamic::List<typename Value::second_type>;
    Res res;
    std::for_each(first,
                  last,
                  [&res](const Value& v) { res.push_back(v.second); });
    return res;
}


TEST_CASE("Etl::Dynamic::UnorderedMultiMap<> basic test", "[unorderedmultimap][etl]") {

    using MapType = Etl::Dynamic::UnorderedMultiMap<uint32_t, ContainerTester>;
    MapType map;

    REQUIRE(map.empty());
    REQUIRE(map.size() == 0);
    REQUIRE(map.max_size() == static_cast<uint32_t>(std::allocator<uint32_t>().max_size()));

    ContainerTester a(4);
    map.insert(std::make_pair(4, a));

    REQUIRE_FALSE(map.empty());
    REQUIRE(map.size() == 1);

    REQUIRE(map.find(4) != map.end());
    REQUIRE(map.find(4)->first == 4);
    REQUIRE(map.find(4)->second.getValue() == a.getValue());

    map.insert(5, ContainerTester(-5));

    REQUIRE(map.size() == 2);

    auto range = map.equal_range(4);
    REQUIRE(range.first == map.find(4));
    REQUIRE(std::distance(range.first, range.second) == 1);

    map.insert(std::make_pair(4, ContainerTester(-6)));
    REQUIRE(map.size() == 3);

    range = map.equal_range(4);
    REQUIRE(range.first == map.find(4));
    REQUIRE(std::distance(range.first, range.second) == 2);
    std::initializer_list<MapType::value_type> expected {{4, ContainerTester {-6}},
                                                         {4, ContainerTester {4}}};
    REQUIRE(std::is_permutation(range.first, range.second, expected.begin()));

    map.erase(5);

    REQUIRE(map.size() == 2);
}


TEST_CASE("Etl::Dynamic::UnorderedMultiMap<> insert test", "[unorderedmultimap][etl]") {

    typedef Etl::Dynamic::UnorderedMultiMap<int, uint32_t> MapType;

    MapType map;

    auto res = map.insert(1, 2);

    REQUIRE(res != map.end());
    REQUIRE(map.size() == 1);

    SECTION("first element") {

        REQUIRE(res->first == 1);
        REQUIRE(res->second == 2);
    }

    SECTION("second element") {

        res = map.insert(2, 2);

        REQUIRE(map.size() == 2);
        REQUIRE(map.find(2) != map.end());
        REQUIRE(map.find(2)->second == 2);
    }

    SECTION("insert with an existing key") {

        res = map.insert(1, 3);

        REQUIRE(map.size() == 2);
        REQUIRE(map.find(1) != map.end());
        auto range = map.equal_range(1);
        REQUIRE(std::distance(range.first, range.second) == 2);

        std::initializer_list<MapType::value_type> expected {{1, 2},
                                                             {1, 3}};
        REQUIRE(std::is_permutation(range.first, range.second, expected.begin()));
    }
}


TEST_CASE("Etl::Dynamic::UnorderedMultiMap<> erase tests", "[unorderedmultimap][etl]") {

    typedef Etl::Dynamic::UnorderedMultiMap<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    CHECK(map.size() == 4);

    SECTION("erase(Key)") {

        map.erase(2);

        REQUIRE(map.size() == 3);
        REQUIRE(map.find(2) == map.end());
    }

    SECTION("erase(iterator)") {

        MapType::iterator it = map.find(2);
        CHECK(it != map.end());

        it = map.erase(it);

        REQUIRE(map.size() == 3);
        REQUIRE(map.find(2) == map.end());
        REQUIRE(it == map.find(3));
    }
}


TEST_CASE("Etl::Dynamic::UnorderedMultiMap<> clear tests", "[unorderedmultimap][etl]") {

    typedef Etl::Dynamic::UnorderedMultiMap<int, uint32_t> MapType;

    auto testClear = [](MapType& map) {
        CHECK_FALSE(map.empty());

        map.clear();

        REQUIRE(map.empty());
        REQUIRE(map.find(2) == map.end());

        auto bucketInspector = [](size_t ix, const void* b) {
            CAPTURE(ix);
            REQUIRE(b == nullptr);
        };

        auto nodeInspector = [](size_t hash, size_t ix, const void* node) {
            (void)hash;
            (void)ix;
            (void)node;
            REQUIRE(false);
        };

        map.ht().inspectBuckets(bucketInspector);
        map.ht().inspectNodes(nodeInspector);
    };

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    {
        MapType m2 {map};
        MapType m3;
        m3.swap(m2);
        testClear(m3);
    }

    testClear(map);
}


TEST_CASE("Etl::Dynamic::UnorderedMultiMap<> iteration tests", "[unorderedmultimap][etl]") {

    typedef Etl::Dynamic::UnorderedMultiMap<int, uint32_t> MapType;

    MapType map;

    map.insert(1, -1);
    map.insert(2, -2);
    map.insert(3, -3);
    map.insert(4, -4);

    CHECK(map.size() == 4);

    Etl::Dynamic::Set<int> seen;

    SECTION("with range for") {

        for (auto& item : map) {
            seen.insert(item.first);
        }

        REQUIRE(seen.find(1) != seen.end());
        REQUIRE(seen.find(2) != seen.end());
        REQUIRE(seen.find(3) != seen.end());
        REQUIRE(seen.find(4) != seen.end());
    }
}


template<typename M1, typename M2, typename K>
void assertEqualElements(const M1& m1, const M2& m2, const K& key) {

    auto range1 = m1.equal_range(key);
    auto range2 = m2.equal_range(key);

    REQUIRE(std::distance(range1.first, range1.second)
            == std::distance(range2.first, range2.second));
    REQUIRE(std::is_permutation(range1.first, range1.second, range2.first));
}


template<typename M1, typename M2>
void assertEqualElements(const M1& m1, const M2& m2) {

    auto m1It = m1.begin();
    while (m1It != m1.end()) {
        auto m1Er = m1.equal_range(m1It->first);
        REQUIRE(m1Er.first != m1.end());
        assertEqualElements(m1, m2, m1Er.first->first);
        m1It = m1Er.second;
    }
}


template<typename M>
void assertEqualMap(const M& m1, const M& m2) {

    REQUIRE(m1.size() == m2.size());
    REQUIRE(m1.max_load_factor() == Approx(m2.max_load_factor()));
    REQUIRE(m1.bucket_count() == m2.bucket_count());
    REQUIRE(m1.load_factor() == Approx(m2.load_factor()));

    assertEqualElements(m1, m2);
}


template<typename M1, typename M2>
void assertEqualMap(const M1& m1, const M2& m2) {

    REQUIRE(m1.size() == m2.size());

    assertEqualElements(m1, m2);
}

template<class M1, class M2>
void testOpEq() {

    std::initializer_list<typename M1::value_type> INIT_1 {{1, 4},
                                                           {1, 5},
                                                           {2, -5},
                                                           {3, -5}};
    std::initializer_list<typename M1::value_type> INIT_2 {{1, 4},
                                                           {1, 5},
                                                           {2, -5},
                                                           {3, -6}};

    SECTION("==") {
        M1 m1 {INIT_1};
        M2 m2 {INIT_1};
        assertEqualMap(m1, m2);
        REQUIRE(m1 == m2);
        REQUIRE_FALSE(m1 != m2);
    }

    SECTION("!=") {
        M1 m1 {INIT_1};
        M2 m2 {INIT_2};
        REQUIRE(m1 != m2);
        REQUIRE_FALSE(m1 == m2);
    }
}


TEST_CASE("Etl::UnorderedMultiMap<> operator==", "[unorderedmultimap][op][etl]") {

    using DM = Etl::Dynamic::UnorderedMultiMap<int, int32_t>;
    using SM = Etl::Static::UnorderedMultiMap<int, int32_t, 64U, 16U>;

    SECTION("D <- D") {
        testOpEq<DM, DM>();
    }

    SECTION("D <- S") {
        testOpEq<DM, SM>();
    }

    SECTION("S <- D") {
        testOpEq<SM, DM>();
    }

    SECTION("S <- S") {
        testOpEq<SM, SM>();
    }
}


template<typename DST, typename SRC>
void testCopy() {

    SRC src;
    src.insert(1, -1);
    src.insert(2, -2);
    src.insert(3, -3);
    src.insert(4, -4);

    CHECK(src.size() == 4);

    SECTION("A {const B&}") {
        DST other {src};
        assertEqualMap(src, other);
    }

    SECTION("A {const Base&}") {
        DST other {static_cast<const typename SRC::Base&>(src)};
        assertEqualMap(src, other);
    }

    SECTION("A = const B&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        other = src;
        assertEqualMap(src, other);
    }

    SECTION("A = const Base&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        other = static_cast<const typename SRC::Base&>(src);
        assertEqualMap(src, other);
    }

    SECTION("Base = const B&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        static_cast<typename DST::Base&>(other) = src;
        assertEqualMap(src, other);
    }

    SECTION("Base = const Base&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        static_cast<typename DST::Base&>(other) = static_cast<const typename SRC::Base&>(src);
        assertEqualMap(src, other);
    }
}


TEST_CASE("Etl::UnorderedMultiMap<> copy", "[unorderedmultimap][copy][etl]") {

    using DM = Etl::Dynamic::UnorderedMultiMap<int, int32_t>;
    using SM = Etl::Static::UnorderedMultiMap<int, int32_t, 64U, 16U>;

    SECTION("D <- D") {
        testCopy<DM, DM>();
    }

    SECTION("D <- S") {
        testCopy<DM, SM>();
    }

    SECTION("S <- D") {
        testCopy<SM, DM>();
    }

    SECTION("S <- S") {
        testCopy<SM, SM>();
    }
}


template<typename DST, typename SRC>
void testMove() {

    SRC src;
    src.insert(1, -1);
    src.insert(2, -2);
    src.insert(3, -3);
    src.insert(4, -4);

    CHECK(src.size() == 4);

    SRC srcAlias = src;
    assertEqualMap(src, srcAlias);

    SECTION("A {B&&}") {
        DST other {std::move(src)};
        assertEqualMap(srcAlias, other);
    }

    SECTION("A {Base&&}") {
        DST other {std::move(static_cast<typename SRC::Base&>(src))};
        assertEqualMap(srcAlias, other);
    }

    SECTION("A = B&&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        other = std::move(src);
        assertEqualMap(srcAlias, other);
    }

    SECTION("A = Base&&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        other = std::move(static_cast<typename SRC::Base&>(src));
        assertEqualMap(srcAlias, other);
    }

    SECTION("Base = B&&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        static_cast<typename DST::Base&>(other) = std::move(src);
        assertEqualMap(srcAlias, other);
    }

    SECTION("Base = Base&&") {
        DST other;
        CHECK(other.empty());  // to avoid optimizations
        static_cast<typename DST::Base&>(other) = std::move(static_cast<typename SRC::Base&>(src));
        assertEqualMap(srcAlias, other);
    }
}


TEST_CASE("Etl::UnorderedMultiMap<> move", "[unorderedmultimap][move][etl]") {

    using DM = Etl::Dynamic::UnorderedMultiMap<int, int32_t>;
    using SM = Etl::Static::UnorderedMultiMap<int, int32_t, 64U, 16U>;

    SECTION("D <- D") {
        testMove<DM, DM>();
    }

    SECTION("D <- S") {
        testMove<DM, SM>();
    }

    SECTION("S <- D") {
        testMove<SM, DM>();
    }

    SECTION("S <- S") {
        testMove<SM, SM>();
    }
}


TEST_CASE("Etl::UnorderedMultiMap<> swap", "[unorderedmultimap][etl]") {

    using Etl::Test::NonAssignable;

    using SIC = Etl::Static::UnorderedMultiMap<int, int, 4>;
    using PIC = Etl::Pooled::UnorderedMultiMap<int, int, 8, 4>;
    using DIC = Etl::Dynamic::UnorderedMultiMap<int, int>;
    using SNMC = Etl::Static::UnorderedMultiMap<int, NonAssignable, 4>;
    using PNMC = Etl::Pooled::UnorderedMultiMap<int, NonAssignable, 8, 4>;
    using DNMC = Etl::Dynamic::UnorderedMultiMap<int, NonAssignable>;

    SECTION("with assignable type") {

        auto insert = [](Etl::UnorderedMultiMap<int, int>& map, int v) {
            map.insert(std::make_pair(v, v));
        };

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

        auto insert = [](Etl::UnorderedMultiMap<int, NonAssignable>& map, int v) {
            map.emplace(v, NonAssignable {v});
        };

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


TEMPLATE_TEST_CASE("Etl::UnorderedMultiMap<> with std::initializer_list<>",
                   "[unorderedmultimap][etl]",
                   (Etl::Dynamic::UnorderedMultiMap<int, int>),
                   (Etl::Static::UnorderedMultiMap<int, int, 32U>),
                   (Etl::Pooled::UnorderedMultiMap<int, int, 32U, 8U>)) {

    std::initializer_list<typename TestType::value_type> init {{1, -1}, {2, -2}, {3, -3}, {4, -4}};
    auto test = [&init](const TestType& map) {
        REQUIRE(map.size() == init.size());
        for (auto& item : init) {
            REQUIRE(map.find(item.first) != map.end());
            REQUIRE(map.find(item.first)->second == item.second);
        }
    };

    SECTION("M(std::initializer_list<>)") {
        TestType map(init);
        test(map);
    }

    SECTION("M = std::initializer_list<>") {
        TestType map;
        map.insert({1, -7});

        map = init;
        test(map);
    }
}


TEMPLATE_TEST_CASE("Etl::UnorderedMultiMap<> search tests",
                   "[unorderedmultimap][etl]",
                   (Etl::Dynamic::UnorderedMultiMap<int, ContainerTester>),
                   (Etl::Static::UnorderedMultiMap<int, ContainerTester, 32U>),
                   (Etl::Pooled::UnorderedMultiMap<int, ContainerTester, 32U, 8U>)) {

    TestType map;

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.size() == 4);

    SECTION("find(Key)") {

        auto it = map.find(3);

        REQUIRE(it != map.end());
        REQUIRE(it->first == 3);
        REQUIRE(it->second == ContainerTester(-3));
    }

    SECTION("find(Key) non-existing") {

        auto it = map.find(7);

        REQUIRE(it == map.end());
    }

    SECTION("equal_range(Key)") {

        auto res = map.equal_range(3);

        REQUIRE(res.first != map.end());
        REQUIRE(res.first->first == 3);
        REQUIRE(res.first->second == ContainerTester(-3));

        REQUIRE(res.first != res.second);
        ++res.first;
        REQUIRE(res.first == res.second);
    }

    SECTION("equal_range(Key) non-existing") {

        auto res = map.equal_range(7);

        REQUIRE(res.first == map.end());
        REQUIRE(res.second == map.end());
    }
}


TEMPLATE_TEST_CASE("Etl::UnorderedMultiMap<> bucket interface tests",
                   "[unorderedmultimap][etl]",
                   (Etl::Static::UnorderedMultiMap<int, int, 64U, 16U>),
                   (Etl::Pooled::UnorderedMultiMap<int, int, 64U, 16U>)) {

    static const size_t BUCKETS {16};
    static const size_t MOD {BUCKETS};

    TestType map;

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

    CHECK(map.hash_function()(34) == 34);

    CHECK_FALSE(inBucket1.empty());
    CHECK_FALSE(inBucket2.empty());
    CHECK_FALSE(inBucket4.empty());

    for (auto item : inBucket1) {
        map.insert(item, -item);
    }

    for (auto item : inBucket2) {
        map.insert(item, -item);
    }

    for (auto item : inBucket4) {
        map.insert(item, -item);
    }

    CHECK(map.size() == (inBucket1.size() + inBucket2.size() + inBucket4.size()));

    SECTION("bucket()") {
        REQUIRE(map.bucket(*inBucket1.begin()) != map.bucket(*inBucket2.begin()));
    }

    SECTION("bucket_size()") {

#if 0
        auto& ht = map.ht();
        size_t cnt = 0;
        ht.inspectNodes([&cnt](size_t hash, size_t ix, const void* node) {
            std::cout << "#" << cnt << " ix " << ix << " @ " << node << std::endl;
            ++cnt;
        });
#endif

        CHECK(map.bucket_size(0) == 0);
        CHECK(map.bucket_size(1) == inBucket1.size());
        CHECK(map.bucket_size(2) == inBucket2.size());
        CHECK(map.bucket_size(3) == 0);
        CHECK(map.bucket_size(4) == inBucket4.size());
        CHECK(map.bucket_size(5) == 0);

        REQUIRE(map.max_bucket_count() == BUCKETS);
    }

    SECTION("bucket iteration") {

        auto checkBucket = [](const TestType& map, uint32_t ix, const Input& input) {
            CAPTURE(ix);
            CHECK(map.bucket_size(ix) == input.size());
            auto it = map.begin(ix);
            while (it != map.end(ix)) {
                CAPTURE(it->first);
                REQUIRE(input.find(it->first) != input.end());
                ++it;
            }
        };

        checkBucket(map, 1, inBucket1);
        checkBucket(map, 2, inBucket2);
        checkBucket(map, 4, inBucket4);
    }
}


TEST_CASE("Etl::Dynamic::UnorderedMultiMap<> hash policy tests", "[unorderedmultimap][etl]") {

    typedef Etl::Dynamic::UnorderedMultiMap<uint32_t, ContainerTester> MapType;
    MapType map;

    SECTION("max_load_factor()") {
        REQUIRE(map.max_load_factor() == Approx(1.0));
    }

    SECTION("load_factor()") {

        CHECK(map.empty());
        REQUIRE(map.load_factor() == Approx(0.0));

        map.insert(1, ContainerTester(-1));
        CHECK(map.size() == 1U);
        auto lf1 = map.load_factor();
        REQUIRE(lf1 != Approx(0.0));

        map.insert(2, ContainerTester(-1));
        CHECK(map.size() == 2U);

        auto lf2 = map.load_factor();
        REQUIRE(lf2 > lf1);
    }

    SECTION("rehash()") {

        map.insert(1, ContainerTester(-1));
        map.insert(2, ContainerTester(-2));
        map.insert(3, ContainerTester(-3));
        map.insert(4, ContainerTester(-4));
        CHECK(map.size() == 4U);
        auto lf = map.load_factor();
        REQUIRE(lf != Approx(0.0));

        auto bc = map.bucket_count();
        CHECK(lf > 0U);

#if 0
        auto bucketInspector = [](size_t ix, const void* b) {
            using Etl::Detail::AHashTable;
            auto bucket = static_cast<AHashTable::ConstBucketItem>(b);
            std::cout << "#" << ix << " -> " << bucket;
            if (bucket) {
                std::cout << " -> " << bucket->next;
            }
            std::cout << std::endl;
        };

        auto nodeInspector = [](size_t hash, size_t ix, const void* node) {
            std::cout << "h " << hash << " ix " << ix << " @ " << node << std::endl;
        };

        std::cout << "Original:" << std::endl;
        map.ht().inspectBuckets(bucketInspector);
        map.ht().inspectNodes(nodeInspector);
#endif

        SECTION("to greater bucket count") {

            auto newBc = bc * 2U;
            map.rehash(newBc);

#if 0
            std::cout << "After rehash():" << std::endl;
            map.ht().inspectBuckets(bucketInspector);
            map.ht().inspectNodes(nodeInspector);
#endif

            REQUIRE(map.size() == 4U);
            REQUIRE(map.bucket_count() == newBc);
            REQUIRE(map.load_factor() == Approx(lf / 2.0));
        }

        SECTION("to smaller bucket count") {

            auto newBc = bc / 2U;
            map.rehash(newBc);

            REQUIRE(map.size() == 4U);
            REQUIRE(map.bucket_count() == newBc);
            REQUIRE(map.load_factor() == Approx(lf * 2.0));
        }

        SECTION("rehash(0)") {

            map.rehash(0);

            REQUIRE(map.size() == 4U);
            REQUIRE(map.bucket_count() == 4U);
            REQUIRE(map.load_factor() == Approx(map.max_load_factor()));
        }
    }

    SECTION("insert() with rehashing") {

        map.rehash(4U);

        map.insert(1, ContainerTester(-1));
        map.insert(2, ContainerTester(-2));
        map.insert(3, ContainerTester(-3));
        map.insert(4, ContainerTester(-4));
        CHECK(map.size() == 4U);

        auto bc = map.bucket_count();
        CHECK(bc == 4U);
        auto lf = map.load_factor();
        CHECK(lf == Approx(1.0));

        map.insert(5, ContainerTester(-5));
        CHECK(map.size() == 5U);

        REQUIRE(map.bucket_count() > bc);
        REQUIRE(map.load_factor() < lf);
    }
}


TEST_CASE("Etl::Static::UnorderedMultiMap<> parameter tests", "[unorderedmultimap][etl]") {

    SECTION("with default number of buckets") {

        Etl::Static::UnorderedMultiMap<int, int, 55U> map;

        REQUIRE(map.max_size() == 55U);
        REQUIRE(map.bucket_count() == 55U);
        REQUIRE(map.max_load_factor() == Approx(1.0f));
    }

    SECTION("with custom number of buckets") {

        Etl::Static::UnorderedMultiMap<int, int, 55U, 11U> map;

        REQUIRE(map.max_size() == 55U);
        REQUIRE(map.bucket_count() == 11U);
        REQUIRE(map.max_load_factor() == Approx(5.0f));
    }
}


TEST_CASE("Etl::Custom::UnorderedMultiMap<> allocator test", "[unorderedmultimap][etl]") {

    using ItemType = ContainerTester;
    using MapType =
        Etl::Custom::UnorderedMultiMap<uint32_t, ItemType, DummyAllocator, DummyAllocator>;
    using NodeAllocatorType = MapType::NodeAllocator::Allocator;
    using BucketAllocatorType = MapType::BucketImpl::Allocator;

    auto end = AtScopeEnd([]() {
        NodeAllocatorType::reset();
        BucketAllocatorType::reset();
    });

    CHECK(NodeAllocatorType::getAllocCount() == 0);
    CHECK(NodeAllocatorType::getDeleteCount() == 0);
    CHECK(BucketAllocatorType::getAllocCount() == 0);
    CHECK(BucketAllocatorType::getDeleteCount() == 0);

    MapType map;
    map.insert(5, ContainerTester(-5));

    MapType::iterator it = map.begin();
    REQUIRE(it.operator->() == &(NodeAllocatorType::ptrOfAllocation(0)->item));

    map.insert(6, ContainerTester(-6));
    ++it;
    REQUIRE(it.operator->() == &(NodeAllocatorType::ptrOfAllocation(1)->item));

    CHECK(NodeAllocatorType::getDeleteCount() == 0);

    map.erase(5);
    REQUIRE(NodeAllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Static::UnorderedMultiMap<> test", "[unorderedmultimap][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Static::UnorderedMultiMap<uint32_t, ItemType, NUM> MapType;

    MapType map;

    SECTION("Basic allocation") {

        map.insert(5, ContainerTester(-5));

        MapType::iterator it = map.begin();
        REQUIRE(it.operator->() != NULL);

        map.insert(6, ContainerTester(-6));
        MapType::iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            map.insert(i, ContainerTester(i));
        }

        CHECK(map.size() == NUM);

        auto res = map.insert(NUM, ContainerTester(NUM));
        REQUIRE(map.size() == NUM);
        REQUIRE(res == map.end());
    }
}


TEST_CASE("Etl::UnorderedMultiMap<> test cleanup", "[unorderedmultimap][etl]") {

    typedef Etl::Custom::UnorderedMultiMap<uint32_t, ContainerTester, DummyAllocator> MapType;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(MapType::NodeAllocator::Allocator::getDeleteCount()
          == MapType::NodeAllocator::Allocator::getAllocCount());
}


// Etl::UnorderedMultiMap comparision tests ----------------------------------------------


TEST_CASE("Etl::UnorderedMultiMap<> comparision", "[unorderedmultimap][etl]") {

    SECTION("Etl::UnorderedMultiMap<> vs Etl::UnorderedMultiMap<>") {

        using MapType = Etl::Dynamic::UnorderedMultiMap<int, int>;
        using Base = Etl::UnorderedMultiMap<int, int>;

        MapType lhs;
        MapType rhs;

        auto inserter = [](Base& cont, int val) { cont.emplace(val, val); };

        testEquivalence(static_cast<Base&>(lhs),
                        static_cast<Base&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::UnorderedMultiMap<> vs Etl::Static::UnorderedMultiMap<>") {

        using LType = Etl::Dynamic::UnorderedMultiMap<int, int>;
        using RType = Etl::Static::UnorderedMultiMap<int, int, 32U>;

        LType lhs;
        RType rhs;

        auto lInserter = [](LType& cont, int val) { cont.emplace(val, val); };
        auto rInserter = [](RType& cont, int val) { cont.emplace(val, val); };

        testEquivalence(lhs,
                        rhs,
                        lInserter,
                        rInserter);
    }
}


}  // namespace
