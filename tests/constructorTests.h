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

#ifndef ETL_TEST_CONSTRUCTORTESTS_H_
#define ETL_TEST_CONSTRUCTORTESTS_H_

#include <catch2/catch.hpp>

#include <etl/etlSupport.h>

#include <array>
#include <iterator>
#include <numeric>

namespace Etl {
namespace Test {


template<typename Cont, typename Val>
void constructForSizeAndValue(Val v) {

    static constexpr std::size_t N {5U};

    Cont c {N, v};

    REQUIRE_FALSE(c.empty());
    REQUIRE(c.size() == N);

    for (const auto& item : c) {
        REQUIRE(item == v);
    }
}


template<class Cont>
void constructForSize() {

    static constexpr std::size_t N {5U};

    Cont c {N};

    REQUIRE_FALSE(c.empty());
    REQUIRE(c.size() == N);

    const auto DEF = typename Cont::value_type {};

    for (const auto& item : c) {
        REQUIRE(item == DEF);
    }
}


template<class Cont>
void constructWithIterators() {

    using Val = typename Cont::value_type;

    const std::array<Val, 7U> input {Val {1},
                                     Val {2},
                                     Val {3},
                                     Val {4},
                                     Val {5},
                                     Val {6},
                                     Val {7}};

    Cont c {input.cbegin(), input.cend()};

    REQUIRE(c.size() == input.size());

    auto it = input.cbegin();

    for (const auto& item : c) {
        REQUIRE(item == *it);
        ++it;
    }
}


template<class Cont>
void constructWithInitList() {

    using Val = typename Cont::value_type;

    std::initializer_list<Val> input {Val {1},
                                      Val {2},
                                      Val {3},
                                      Val {4},
                                      Val {5},
                                      Val {6},
                                      Val {7}};

    Cont c {input};

    REQUIRE(c.size() == input.size());

    auto it = input.begin();

    for (const auto& item : c) {
        REQUIRE(item == *it);
        ++it;
    }
}

}  // namespace Test
}  // namespace Etl

#endif  // ETL_TEST_CONSTRUCTORTESTS_H_