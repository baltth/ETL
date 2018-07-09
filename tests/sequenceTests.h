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

#ifndef __ETL_TEST_SEQUENCETESTS_H__
#define __ETL_TEST_SEQUENCETESTS_H__

#include "catch.hpp"

#include <etl/etlSupport.h>

#include <iterator>
#include <numeric>


template<class Cont>
void testBackAccess() {

    Cont c;
    typename Cont::const_iterator it = c.end();

    CHECK(c.empty());

    c.push_back(1);

    REQUIRE(c.size() == 1);
    REQUIRE(c.back() == 1);
    it = c.end();
    REQUIRE(c.back() == *(--it));

    c.push_back(2);

    REQUIRE(c.size() == 2);
    REQUIRE(c.back() == 2);
    it = c.end();
    REQUIRE(c.back() == *(--it));

    c.push_back(3);

    REQUIRE(c.size() == 3);
    REQUIRE(c.back() == 3);
    it = c.end();
    REQUIRE(c.back() == *(--it));

    it = c.begin();
    REQUIRE(*it == 1);
    ++it;
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(*it == 3);

    c.pop_back();

    REQUIRE(c.size() == 2);
    REQUIRE(c.back() == 2);
    it = c.end();
    REQUIRE(c.back() == *(--it));

    c.pop_back();

    REQUIRE(c.size() == 1);
    REQUIRE(c.back() == 1);
    it = c.end();
    REQUIRE(c.back() == *(--it));

    c.pop_back();

    REQUIRE(c.empty());
}


template<class Cont>
void testFrontAccess() {

    Cont c;

    CHECK(c.empty());

    c.push_front(1);

    REQUIRE(c.size() == 1);
    REQUIRE(c.front() == 1);
    REQUIRE(c.front() == *c.begin());

    c.push_front(2);

    REQUIRE(c.size() == 2);
    REQUIRE(c.front() == 2);
    REQUIRE(c.front() == *c.begin());

    c.push_front(3);

    REQUIRE(c.size() == 3);
    REQUIRE(c.front() == 3);
    REQUIRE(c.front() == *c.begin());

    typename Cont::const_iterator it = c.end();
    --it;
    REQUIRE(*it == 1);
    --it;
    REQUIRE(*it == 2);
    --it;
    REQUIRE(*it == 3);

    c.pop_front();

    REQUIRE(c.size() == 2);
    REQUIRE(c.front() == 2);
    REQUIRE(c.front() == *c.begin());

    c.pop_front();

    REQUIRE(c.size() == 1);
    REQUIRE(c.front() == 1);
    REQUIRE(c.front() == *c.begin());

    c.pop_front();

    REQUIRE(c.empty());
}


template<class Cont>
void testIterationForward() {

    Cont c;

    c.push_back(1);
    c.push_back(2);
    c.push_back(3);

    CHECK(c.size() == 3);

    typename Cont::const_iterator it = c.begin();

    REQUIRE(*it == 1);
    ++it;
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(*it == 3);
    ++it;
    REQUIRE(it == c.end());
}


template<class Cont>
void testIterationBackward() {

    Cont c;

    c.push_back(1);
    c.push_back(2);
    c.push_back(3);

    CHECK(c.size() == 3);

    typename Cont::const_iterator it = c.end();
    --it;

    REQUIRE(*it == 3);
    --it;
    REQUIRE(*it == 2);
    --it;
    REQUIRE(*it == 1);
    REQUIRE(it == c.begin());
}


template<class Cont>
void testReverseIterationForward() {

    Cont c;

    c.push_back(1);
    c.push_back(2);
    c.push_back(3);

    CHECK(c.size() == 3);

    typename Cont::const_reverse_iterator rit = c.rbegin();

    REQUIRE(*rit == 3);
    ++rit;
    REQUIRE(*rit == 2);
    ++rit;
    REQUIRE(*rit == 1);
    ++rit;
    REQUIRE(rit == c.rend());
}


template<class Cont>
void testReverseIterationBackward() {

    Cont c;

    c.push_back(1);
    c.push_back(2);
    c.push_back(3);

    CHECK(c.size() == 3);

    typename Cont::const_reverse_iterator rit = c.rend();
    --rit;

    REQUIRE(*rit == 1);
    --rit;
    REQUIRE(*rit == 2);
    --rit;
    REQUIRE(*rit == 3);
    REQUIRE(rit == c.rbegin());
}

#endif /* __ETL_TEST_SEQUENCETESTS_H__ */
