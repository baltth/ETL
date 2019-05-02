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

#include <etl/Set.h>

#include "ContainerTester.h"
#include "DummyAllocator.h"

using ETL_NAMESPACE::Test::ContainerTester;
using ETL_NAMESPACE::Test::DummyAllocator;


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

        res = set.insert(2);

        REQUIRE(res.second == true);
        REQUIRE(set.size() == 2);
        REQUIRE(*(res.first) == 2);
    }

    SECTION("insert() of existing shall fail") {

        res = set.insert(1);

        REQUIRE(res.second == false);
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

    typedef ContainerTester ItemType;
    typedef Etl::Custom::Set<ItemType, DummyAllocator> SetType;
    typedef SetType::Allocator::Allocator AllocatorType;

    AllocatorType::reset();
    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    SetType set;
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
    CHECK(SetType::Allocator::Allocator::getDeleteCount() == SetType::Allocator::Allocator::getAllocCount());
}

