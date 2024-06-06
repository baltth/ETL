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

#ifndef ETL_TEST_COMPARISIONTESTS_H_
#define ETL_TEST_COMPARISIONTESTS_H_

#include <catch2/catch.hpp>

#include <etl/etlSupport.h>


template<class L, class R>
void assertEqual(const L& lhs, const R& rhs) {

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
    REQUIRE_FALSE(lhs < rhs);
    REQUIRE(lhs <= rhs);
    REQUIRE_FALSE(lhs > rhs);
    REQUIRE(lhs >= rhs);
}


template<class L, class R>
void assertLess(const L& lhs, const R& rhs) {

    REQUIRE_FALSE(lhs == rhs);
    REQUIRE(lhs != rhs);
    REQUIRE(lhs < rhs);
    REQUIRE(lhs <= rhs);
    REQUIRE_FALSE(lhs > rhs);
    REQUIRE_FALSE(lhs >= rhs);
}


template<class L, class R>
void assertGreater(const L& lhs, const R& rhs) {

    REQUIRE_FALSE(lhs == rhs);
    REQUIRE(lhs != rhs);
    REQUIRE_FALSE(lhs < rhs);
    REQUIRE_FALSE(lhs <= rhs);
    REQUIRE(lhs > rhs);
    REQUIRE(lhs >= rhs);
}


template<class L, class R, class LI, class RI>
void testComparision(L& lhs,
                     R& rhs,
                     LI lInserter,
                     RI rInserter) {

    CHECK(lhs.empty());
    CHECK(rhs.empty());

    assertEqual(lhs, rhs);
    assertEqual(rhs, lhs);

    rInserter(rhs, 1);
    rInserter(rhs, 2);

    assertLess(lhs, rhs);
    assertGreater(rhs, lhs);

    lInserter(lhs, 1);

    assertLess(lhs, rhs);
    assertGreater(rhs, lhs);

    lInserter(lhs, 2);

    assertEqual(lhs, rhs);
    assertEqual(rhs, lhs);

    lInserter(lhs, 3);
    rInserter(rhs, 4);

    assertLess(lhs, rhs);
    assertGreater(rhs, lhs);

    lInserter(lhs, 5);
    rInserter(rhs, 5);

    assertLess(lhs, rhs);
    assertGreater(rhs, lhs);
}


template<class L, class R, class LI, class RI>
void testEquivalence(L& lhs,
                     R& rhs,
                     LI lInserter,
                     RI rInserter) {

    auto eq = [&lhs, &rhs]() {
        REQUIRE(lhs == rhs);
        REQUIRE(rhs == lhs);
        REQUIRE_FALSE(lhs != rhs);
        REQUIRE_FALSE(rhs != lhs);
    };

    auto neq = [&lhs, &rhs]() {
        REQUIRE_FALSE(lhs == rhs);
        REQUIRE_FALSE(rhs == lhs);
        REQUIRE(lhs != rhs);
        REQUIRE(rhs != lhs);
    };

    CHECK(lhs.empty());
    CHECK(rhs.empty());

    eq();

    rInserter(rhs, 1);
    neq();
    rInserter(rhs, 2);
    neq();
    lInserter(lhs, 1);
    neq();
    lInserter(lhs, 2);

    eq();

    lInserter(lhs, 3);
    neq();
    rInserter(rhs, 4);
    lInserter(lhs, 5);
    rInserter(rhs, 5);
    neq();
    lInserter(lhs, 4);
    rInserter(rhs, 3);

    eq();
}

#endif  // ETL_TEST_COMPARISIONTESTS_H_
