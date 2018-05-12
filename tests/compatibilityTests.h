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

#include <etl/etlSupport.h>

#include <iterator>
#include <numeric>


template<class Cont>
void testAccumulate() {

    Cont c;

    c.push_back(1);
    c.push_back(2);
    c.push_back(3);
    c.push_back(4);
    c.push_back(5);

    static const int RES = 1 + 2 + 3 + 4 + 5;

    REQUIRE(std::accumulate(c.begin(), c.end(), 0) == RES);
}


template<class Cont>
void testPartialSum() {

    Cont c;
    Cont res;

    c.push_back(1);
    c.push_back(2);
    c.push_back(3);
    c.push_back(4);
    c.push_back(5);

    CHECK(c.getSize() == 5);

    static const int RES[] = {
        1,
        1 + 2,
        1 + 2 + 3,
        1 + 2 + 3 + 4,
        1 + 2 + 3 + 4 + 5
    };

    std::partial_sum(c.begin(), c.end(), std::back_inserter(res));

    REQUIRE(res.getSize() == c.getSize());

    typename Cont::const_iterator it = res.begin();

    for (uint32_t i = 0; i < res.getSize(); ++i) {
        REQUIRE(*it == RES[i]);
        ++it;
    }
}


template<class Cont1, class Cont2>
void testInnerProduct() {

    Cont1 c1;
    Cont2 c2;

    c1.push_back(1);
    c1.push_back(2);
    c1.push_back(3);
    c1.push_back(4);

    c2.push_back(-1);
    c2.push_back(-2);
    c2.push_back(-3);
    c2.push_back(-4);

    static const int RES = (1 * -1) + (2 * -2) + (3 * -3) + (4 * -4);

    REQUIRE(std::inner_product(c1.begin(), c1.end(), c2.begin(), 0) == RES);
}


