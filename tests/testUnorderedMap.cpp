/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2022 Balazs Toth.

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
#include <etl/UnorderedMap.h>

#include "ContainerTester.h"
#include "DummyAllocator.h"
#include "comparisionTests.h"

using Etl::Test::ContainerTester;
using Etl::Test::DummyAllocator;


TEST_CASE("Etl::Dynamic::UnorderedMap<> basic test", "[unorderedmap][etl][basic]") {

    Etl::Dynamic::UnorderedMap<uint32_t, ContainerTester> map;

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

    REQUIRE(map[4].getValue() == a.getValue());

    map.insert(5, ContainerTester(-5));

    REQUIRE(map.size() == 2);

    ContainerTester b(-4);
    map.insert_or_assign(4, b);

    REQUIRE(map.size() == 2);
    REQUIRE(map[4].getValue() == b.getValue());

    map.erase(5);

    REQUIRE(map.size() == 1);
}


TEST_CASE("Etl::Dynamic::UnorderedMap<> insert test", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<int, uint32_t> MapType;

    MapType map;

    auto res = map.insert(1, 2);

    REQUIRE(res.second == true);
    REQUIRE(res.first != map.end());
    REQUIRE(map.size() == 1);

    SECTION("first element") {

        REQUIRE(res.first->first == 1);
        REQUIRE(res.first->second == 2);
    }

    SECTION("second element") {

        res = map.insert(2, 2);

        REQUIRE(res.second == true);
        REQUIRE(map.size() == 2);
        REQUIRE(map[2] == 2);
    }

    SECTION("insert() of existing shall fail") {

        res = map.insert(1, 3);

        REQUIRE(res.second == false);
        REQUIRE(map.size() == 1);
        REQUIRE(map[1] == 2);
    }

    SECTION("insert_or_assign() of existing shall overwrite") {

        res = map.insert_or_assign(1, 3);

        REQUIRE(res.second == false);
        REQUIRE(map.size() == 1);
        REQUIRE(map[1] == 3);
    }
}


TEST_CASE("Etl::Dynamic::UnorderedMap<> erase tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<int, uint32_t> MapType;

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


TEST_CASE("Etl::Dynamic::UnorderedMap<> iteration tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<int, uint32_t> MapType;

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


TEST_CASE("Etl::Dynamic::UnorderedMap<> association tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<uint32_t, ContainerTester> MapType;

    MapType map;

    map.insert(1, ContainerTester(-1));
    map.insert(2, ContainerTester(-2));
    map.insert(3, ContainerTester(-3));
    map.insert(4, ContainerTester(-4));

    CHECK(map.size() == 4);

    SECTION("write existing") {

        // ContainerTester::enablePrint = true;
        map[4] = ContainerTester(-5);
        // ContainerTester::enablePrint = false;

        CAPTURE(map.find(4)->second.toString());
        CAPTURE(ContainerTester(-5).toString());
        REQUIRE(map.find(4)->second == ContainerTester(-5));
    }

    SECTION("write new") {

        // ContainerTester::enablePrint = true;
        map[5] = ContainerTester(-5);
        // ContainerTester::enablePrint = false;

        CAPTURE(map.find(5)->second.toString());
        CAPTURE(ContainerTester(-5).toString());
        REQUIRE(map.find(5)->second == ContainerTester(-5));
    }

    SECTION("read existing") {

        REQUIRE(map[4] == ContainerTester(-4));
        REQUIRE(&(map[4]) == &(map.find(4)->second));
    }

    SECTION("read new - default insertion") {

        // ContainerTester::enablePrint = true;
        REQUIRE(map[5] == ContainerTester());
        // ContainerTester::enablePrint = false;
    }
}


template<typename M1, typename M2, typename K>
void assertEqualElement(const M1& m1, const M2& m2, const K& key) {

    REQUIRE(m1.find(key) != m1.end());
    REQUIRE(m2.find(key) != m2.end());
    REQUIRE(m1.find(key)->second == m2.find(key)->second);
}


template<typename M>
void assertEqualMap(const M& m1, const M& m2) {

    REQUIRE(m1.size() == m2.size());
    REQUIRE(m1.max_load_factor() == Approx(m2.max_load_factor()));
    REQUIRE(m1.bucket_count() == m2.bucket_count());
    REQUIRE(m1.load_factor() == Approx(m2.load_factor()));

    assertEqualElement(m1, m2, 1);
    assertEqualElement(m1, m2, 4);
}


template<typename M1, typename M2>
void assertEqualMap(const M1& m1, const M2& m2) {

    REQUIRE(m1.size() == m2.size());

    assertEqualElement(m1, m2, 1);
    assertEqualElement(m1, m2, 4);
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


TEST_CASE("Etl::UnorderedMap<> copy", "[unorderedmap][copy][etl]") {

    using DM = Etl::Dynamic::UnorderedMap<int, int32_t>;
    using SM = Etl::Static::UnorderedMap<int, int32_t, 64U, 16U>;

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


TEST_CASE("Etl::UnorderedMap<> move", "[unorderedmap][move][etl]") {

    using DM = Etl::Dynamic::UnorderedMap<int, int32_t>;
    using SM = Etl::Static::UnorderedMap<int, int32_t, 64U, 16U>;

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


template<typename M1, typename M2>
void testSwapOp(std::function<void(M1&, M2&)> swapFunc) {

    M1 src;
    src.insert(1, -1);
    src.insert(2, -2);
    src.insert(3, -3);
    src.insert(4, -4);

    CHECK(src.size() == 4);

    M2 other;
    other.insert(1, 1);
    other.insert(5, -5);
    CHECK(other.size() == 2);

    swapFunc(src, other);

    REQUIRE(other.size() == 4);
    REQUIRE(src.size() == 2);

    REQUIRE(src[1] == 1);
    REQUIRE(src[5] == -5);

    REQUIRE(other[1] == -1);
    REQUIRE(other[4] == -4);
}

template<typename M1, typename M2>
void testSwap() {

    SECTION("lhs.swap(rhs)") {
        auto memberSwap = [](M1& lhs, M2& rhs) { lhs.swap(rhs); };
        testSwapOp<M1, M2>(memberSwap);
    }

    SECTION("swap(lhs, rhs)") {
        auto standaloneSwap = [](M1& lhs, M2& rhs) { swap(lhs, rhs); };
        testSwapOp<M1, M2>(standaloneSwap);
    }
}


TEST_CASE("Etl::UnorderedMap<> swap", "[unorderedmap][etl]") {

    using DM = Etl::Dynamic::UnorderedMap<int, int32_t>;
    using SM = Etl::Static::UnorderedMap<int, int32_t, 64U, 16U>;

    SECTION("D with D") {
        testSwap<DM, DM>();
    }

    SECTION("D with S") {
        testSwap<DM, SM>();
    }

    SECTION("S with D") {
        testSwap<SM, DM>();
    }

    SECTION("S with S") {
        testSwap<SM, SM>();
    }
}


TEMPLATE_TEST_CASE("Etl::UnorderedMap<> with std::initializer_list<>",
                   "[unorderedmap][etl]",
                   (Etl::Dynamic::UnorderedMap<int, int>),
                   (Etl::Static::UnorderedMap<int, int, 32U>),
                   (Etl::Pooled::UnorderedMap<int, int, 32U>)) {

    std::initializer_list<std::pair<int, int>> init {{1, -1}, {2, -2}, {3, -3}, {4, -4}};
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


TEMPLATE_TEST_CASE("Etl::UnorderedMap<> search tests",
                   "[unorderedmap][etl]",
                   (Etl::Dynamic::UnorderedMap<int, ContainerTester>),
                   (Etl::Static::UnorderedMap<int, ContainerTester, 32U>),
                   (Etl::Pooled::UnorderedMap<int, ContainerTester, 32U>)) {

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


TEMPLATE_TEST_CASE("Etl::UnorderedMap<> bucket interface tests",
                   "[unorderedmap][etl]",
                   (Etl::Static::UnorderedMap<int, int, 64U, 16U>),
                   (Etl::Pooled::UnorderedMap<int, int, 64U, 16U>)) {

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


TEST_CASE("Etl::Dynamic::UnorderedMap<> hash policy tests", "[unorderedmap][etl]") {

    typedef Etl::Dynamic::UnorderedMap<uint32_t, ContainerTester> MapType;
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


TEST_CASE("Etl::Static::UnorderedMap<> parameter tests", "[unorderedmap][etl]") {

    SECTION("with default number of buckets") {

        Etl::Static::UnorderedMap<int, int, 55U> map;

        REQUIRE(map.max_size() == 55U);
        REQUIRE(map.bucket_count() == 55U);
        REQUIRE(map.max_load_factor() == Approx(1.0f));
    }

    SECTION("with custom number of buckets") {

        Etl::Static::UnorderedMap<int, int, 55U, 11U> map;

        REQUIRE(map.max_size() == 55U);
        REQUIRE(map.bucket_count() == 11U);
        REQUIRE(map.max_load_factor() == Approx(5.0f));
    }
}


TEST_CASE("Etl::Custom::UnorderedMap<> allocator test", "[unorderedmap][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Custom::UnorderedMap<uint32_t, ItemType, DummyAllocator> MapType;
    typedef MapType::NodeAllocator::Allocator AllocatorType;

    AllocatorType::reset();
    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    MapType map;
    map.insert(5, ContainerTester(-5));

    MapType::iterator it = map.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    map.insert(6, ContainerTester(-6));
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    map.erase(5);
    REQUIRE(AllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Static::UnorderedMap<> test", "[unorderedmap][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Static::UnorderedMap<uint32_t, ItemType, NUM> MapType;

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

        std::pair<MapType::iterator, bool> res = map.insert(NUM, ContainerTester(NUM));
        REQUIRE(map.size() == NUM);
        REQUIRE(res.first == map.end());
        REQUIRE(res.second == false);
    }
}


TEST_CASE("Etl::UnorderedMap<> test cleanup", "[unorderedmap][etl]") {

    typedef Etl::Custom::UnorderedMap<uint32_t, ContainerTester, DummyAllocator> MapType;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(MapType::NodeAllocator::Allocator::getDeleteCount()
          == MapType::NodeAllocator::Allocator::getAllocCount());
}


// Etl::UnorderedMap comparision tests ----------------------------------------------


TEST_CASE("Etl::UnorderedMap<> comparision", "[unorderedmap][etl]") {

    SECTION("Etl::UnorderedMap<> vs Etl::UnorderedMap<>") {

        using MapType = Etl::Dynamic::UnorderedMap<int, int>;
        using Base = Etl::UnorderedMap<int, int>;

        MapType lhs;
        MapType rhs;

        auto inserter = [](Base& cont, int val) { cont.emplace(val, val); };

        testEquivalence(static_cast<Base&>(lhs),
                        static_cast<Base&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::UnorderedMap<> vs Etl::Static::UnorderedMap<>") {

        using LType = Etl::Dynamic::UnorderedMap<int, int>;
        using RType = Etl::Static::UnorderedMap<int, int, 32U>;

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
