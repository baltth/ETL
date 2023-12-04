/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2023 Balazs Toth.

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

#ifndef __ETL_TEST_SWAPTESTS_H__
#define __ETL_TEST_SWAPTESTS_H__

#include <catch2/catch.hpp>

#include <etl/etlSupport.h>


namespace Etl {
namespace Test {


template<class L, class R, class I>
void testSwapAssociative(I insert) {

    using Key = typename L::key_type;

    L lhs {};

    SECTION("swap() empties") {

        R rhs {};

        CHECK(lhs.empty());
        CHECK(rhs.empty());

        lhs.swap(rhs);

        REQUIRE(lhs.empty());
        REQUIRE(rhs.empty());
    }

    insert(lhs, 1);
    insert(lhs, 2);
    insert(lhs, 3);

    CHECK(lhs.size() == 3);

    SECTION("swap() with empty") {

        R rhs {};

        CHECK(rhs.empty());

        lhs.swap(rhs);

        REQUIRE(lhs.empty());
        REQUIRE(rhs.size() == 3);

        CHECK(rhs.find(Key {1}) != rhs.end());
        CHECK(rhs.find(Key {2}) != rhs.end());
        CHECK(rhs.find(Key {3}) != rhs.end());

        lhs.swap(rhs);

        REQUIRE(lhs.size() == 3);
        REQUIRE(rhs.empty());

        CHECK(lhs.find(Key {1}) != lhs.end());
        CHECK(lhs.find(Key {2}) != lhs.end());
        CHECK(lhs.find(Key {3}) != lhs.end());
    }

    SECTION("swap() with different size") {

        R rhs {};

        insert(rhs, 1);
        insert(rhs, 5);

        CHECK(rhs.size() == 2);

        lhs.swap(rhs);

        REQUIRE(rhs.size() == 3);
        REQUIRE(lhs.size() == 2);

#if 0
        auto bucketInspector = [](size_t ix, const void* b) {
            using Etl::Detail::AHashTable;
            auto bucket = static_cast<AHashTable::ConstBucketItem>(b);
            std::cout << "#" << ix << " -> " << bucket;
            if (bucket) {
                auto* next = bucket->next;
                while(next != nullptr) {
                    std::cout << " -> " << next;
                    next = next->next;
                }
            }
            std::cout << std::endl;
        };

        auto nodeInspector = [](size_t hash, size_t ix, const void* node) {
            std::cout << "h " << hash << " ix " << ix << " @ " << node << std::endl;
        };

        std::cout << "lhs:" << std::endl;
        lhs.ht().inspectBuckets(bucketInspector);
        lhs.ht().inspectNodes(nodeInspector);
        std::cout << "rhs:" << std::endl;
        rhs.ht().inspectBuckets(bucketInspector);
        rhs.ht().inspectNodes(nodeInspector);
#endif

        CHECK(lhs.find(Key {1}) != lhs.end());
        CHECK(lhs.find(Key {5}) != lhs.end());

        CHECK(rhs.find(Key {1}) != rhs.end());
        CHECK(rhs.find(Key {2}) != rhs.end());
        CHECK(rhs.find(Key {3}) != rhs.end());

        lhs.swap(rhs);

        REQUIRE(rhs.size() == 2);
        REQUIRE(lhs.size() == 3);

        CHECK(lhs.find(Key {1}) != lhs.end());
        CHECK(lhs.find(Key {2}) != lhs.end());
        CHECK(lhs.find(Key {3}) != lhs.end());

        CHECK(rhs.find(Key {1}) != rhs.end());
        CHECK(rhs.find(Key {5}) != rhs.end());
    }

    SECTION("swap() with equal size") {

        insert(lhs, 4);

        CHECK(lhs.size() == 4);

        R rhs {};
        insert(rhs, 5);
        insert(rhs, 6);
        insert(rhs, 7);
        insert(rhs, 8);

        CHECK(rhs.size() == 4);

        lhs.swap(rhs);

        REQUIRE(rhs.size() == 4);
        REQUIRE(lhs.size() == 4);

        CHECK(lhs.find(Key {5}) != lhs.end());
        CHECK(lhs.find(Key {6}) != lhs.end());
        CHECK(lhs.find(Key {7}) != lhs.end());
        CHECK(lhs.find(Key {8}) != lhs.end());

        CHECK(rhs.find(Key {1}) != rhs.end());
        CHECK(rhs.find(Key {2}) != rhs.end());
        CHECK(rhs.find(Key {3}) != rhs.end());
        CHECK(rhs.find(Key {4}) != rhs.end());
    }

    SECTION("swap(lhs, rhs)") {

        insert(lhs, 4);

        CHECK(lhs.size() == 4);

        R rhs {};
        insert(rhs, 5);
        insert(rhs, 6);
        insert(rhs, 7);
        insert(rhs, 8);

        CHECK(rhs.size() == 4);

        using std::swap;
        swap(lhs, rhs);

        REQUIRE(rhs.size() == 4);
        REQUIRE(lhs.size() == 4);

        CHECK(lhs.find(Key {5}) != lhs.end());
        CHECK(lhs.find(Key {6}) != lhs.end());
        CHECK(lhs.find(Key {7}) != lhs.end());
        CHECK(lhs.find(Key {8}) != lhs.end());

        CHECK(rhs.find(Key {1}) != rhs.end());
        CHECK(rhs.find(Key {2}) != rhs.end());
        CHECK(rhs.find(Key {3}) != rhs.end());
        CHECK(rhs.find(Key {4}) != rhs.end());
    }
}


template<class L, class R, class I>
void testSwapOrdered(I insert) {

    using Val = typename L::value_type;

    L lhs {};

    SECTION("swap() empties") {

        R rhs {};

        CHECK(lhs.empty());
        CHECK(rhs.empty());

        lhs.swap(rhs);

        REQUIRE(lhs.empty());
        REQUIRE(rhs.empty());
    }

    insert(lhs, 1);
    insert(lhs, 2);
    insert(lhs, 3);

    CHECK(lhs.size() == 3);

    SECTION("swap() with empty") {

        R rhs {};

        CHECK(rhs.empty());

        lhs.swap(rhs);

        REQUIRE(lhs.empty());
        REQUIRE(rhs.size() == 3);

        auto rIt = rhs.begin();
        CHECK(*rIt == Val {1});
        ++rIt;
        CHECK(*rIt == Val {2});
        ++rIt;
        CHECK(*rIt == Val {3});
        ++rIt;
        CHECK(rIt == rhs.end());

        lhs.swap(rhs);

        REQUIRE(lhs.size() == 3);
        REQUIRE(rhs.empty());

        auto lIt = lhs.begin();
        CHECK(*lIt == Val {1});
        ++lIt;
        CHECK(*lIt == Val {2});
        ++lIt;
        CHECK(*lIt == Val {3});
        ++lIt;
        CHECK(lIt == lhs.end());
    }

    SECTION("swap() with different size") {

        R rhs {};

        insert(rhs, 1);
        insert(rhs, 5);

        CHECK(rhs.size() == 2);

        lhs.swap(rhs);

        REQUIRE(lhs.size() == 2);
        REQUIRE(rhs.size() == 3);

        auto lIt = lhs.begin();
        CHECK(*lIt == Val {1});
        ++lIt;
        CHECK(*lIt == Val {5});
        ++lIt;
        CHECK(lIt == lhs.end());

        auto rIt = rhs.begin();
        CHECK(*rIt == Val {1});
        ++rIt;
        CHECK(*rIt == Val {2});
        ++rIt;
        CHECK(*rIt == Val {3});
        ++rIt;
        CHECK(rIt == rhs.end());

        lhs.swap(rhs);

        REQUIRE(lhs.size() == 3);
        REQUIRE(rhs.size() == 2);

        lIt = lhs.begin();
        CHECK(*lIt == Val {1});
        ++lIt;
        CHECK(*lIt == Val {2});
        ++lIt;
        CHECK(*lIt == Val {3});
        ++lIt;
        CHECK(lIt == lhs.end());

        rIt = rhs.begin();
        CHECK(*rIt == Val {1});
        ++rIt;
        CHECK(*rIt == Val {5});
        ++rIt;
        CHECK(rIt == rhs.end());
    }

    SECTION("swap() with equal size") {

        insert(lhs, 4);

        CHECK(lhs.size() == 4);

        R rhs {};
        insert(rhs, 5);
        insert(rhs, 6);
        insert(rhs, 7);
        insert(rhs, 8);

        CHECK(rhs.size() == 4);

        lhs.swap(rhs);

        REQUIRE(rhs.size() == 4);
        REQUIRE(lhs.size() == 4);

        auto lIt = lhs.begin();
        CHECK(*lIt == Val {5});
        ++lIt;
        CHECK(*lIt == Val {6});
        ++lIt;
        CHECK(*lIt == Val {7});
        ++lIt;
        CHECK(*lIt == Val {8});
        ++lIt;
        CHECK(lIt == lhs.end());

        auto rIt = rhs.begin();
        CHECK(*rIt == Val {1});
        ++rIt;
        CHECK(*rIt == Val {2});
        ++rIt;
        CHECK(*rIt == Val {3});
        ++rIt;
        CHECK(*rIt == Val {4});
        ++rIt;
        CHECK(rIt == rhs.end());
    }

    SECTION("swap(lhs, rhs)") {

        insert(lhs, 4);

        CHECK(lhs.size() == 4);

        R rhs {};
        insert(rhs, 5);
        insert(rhs, 6);
        insert(rhs, 7);
        insert(rhs, 8);

        CHECK(rhs.size() == 4);

        using std::swap;
        swap(lhs, rhs);

        REQUIRE(rhs.size() == 4);
        REQUIRE(lhs.size() == 4);

        auto lIt = lhs.begin();
        CHECK(*lIt == Val {5});
        ++lIt;
        CHECK(*lIt == Val {6});
        ++lIt;
        CHECK(*lIt == Val {7});
        ++lIt;
        CHECK(*lIt == Val {8});
        ++lIt;
        CHECK(lIt == lhs.end());

        auto rIt = rhs.begin();
        CHECK(*rIt == Val {1});
        ++rIt;
        CHECK(*rIt == Val {2});
        ++rIt;
        CHECK(*rIt == Val {3});
        ++rIt;
        CHECK(*rIt == Val {4});
        ++rIt;
        CHECK(rIt == rhs.end());
    }
}
}  // namespace Test
}  // namespace Etl

#endif  // __ETL_TEST_SWAPTESTS_H__
