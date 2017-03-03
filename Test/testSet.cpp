/**
\file
\date 2017.02.22. 16:52:09
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Tóth Balázs.

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

#include "Set.h"
#include "Test/ContainerTester.h"


TEST_CASE("Etl::Set<> basic test", "[set][etl][basic]" ) {

    typedef Etl::Set<ContainerTester> SetType;
    SetType set;

    ContainerTester a(4);
    set.insert(a);

    REQUIRE(set.getSize() == 1);
    SetType::Iterator it = set.begin();
    REQUIRE(*it == a);

    set.insert(ContainerTester(5));

    REQUIRE(set.getSize() == 2);
    ++it;
    REQUIRE(*it == ContainerTester(5));

    ContainerTester b(4);
    set.insert(b);

    REQUIRE(set.getSize() == 2);

    set.erase(ContainerTester(5));

    REQUIRE(set.getSize() == 1);

}

TEST_CASE("Etl::Set<> insert() test", "[set][etl]") {

    typedef Etl::Set<int> SetType;
    typedef std::pair<SetType::Iterator, bool> ResultType;

    SetType set;

    ResultType res = set.insert(1);

    REQUIRE(res.second == true);
    REQUIRE(res.first != set.end());
    REQUIRE(set.getSize() == 1);

    SECTION("insert() of existing shall fail") {

        res = set.insert(1);

        REQUIRE(res.second == false);
        REQUIRE(set.getSize() == 1);
    }
}

TEST_CASE("Etl::Set<> test cleanup", "[set][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}

