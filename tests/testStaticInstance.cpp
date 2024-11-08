/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2024 Balazs Toth.

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

#include <etl/List.h>
#include <etl/Map.h>
#include <etl/MultiMap.h>
#include <etl/Set.h>
#include <etl/UnorderedMap.h>
#include <etl/UnorderedMultiMap.h>
#include <etl/UnorderedSet.h>

namespace {

// Test to detect Static Initialization Order Fiasco with
// static instance of Etl::Pooled:: containers.
//
// As destructor of containers delete all container elements,
// there's a risk of SIOF if
// - the container instance is static,
// - the container is not empty, and
// - the element allocator is static (just like in case of Etl::Pooled)
//
// As the allocator is a static in function scope, SIOF when constructing
// is not an issue, but deinit time is critical.
//
// Note: current container versions should have no SIOF,
// this test is added as a regression test.
//
// More info about the fiasco: https://en.cppreference.com/w/cpp/language/siof

// A local type is introduced to guarantee that
// the common pool allocators used here are unique to this file.
struct Data {
    int a;
};

bool operator<(const Data& lhs, const Data& rhs) {
    return lhs.a < rhs.a;
}

bool operator==(const Data& lhs, const Data& rhs) {
    return lhs.a == rhs.a;
}

}  // namespace

namespace std {

template<>
struct hash<Data> {
    size_t operator()(const Data& val) const noexcept {
        return hash<int32_t> {}(val.a);
    }
};

}  // namespace std

namespace {

using PL1 = Etl::Pooled::List<Data, 36>;
using PL2 = Etl::Pooled::List<Data, 37U>;
using PL3 = Etl::Pooled::List<Data, 38U>;

PL1 list1 {};
PL2 list2 {Data {}};

TEST_CASE("Test destructor of static Etl::Pooled::List containers", "[list][siof][etl]") {

    static PL3 list3 {};

    list1.push_back(Data {});
    list3.push_back(Data {});

    CHECK_FALSE(list1.empty());
    CHECK_FALSE(list2.empty());
    CHECK_FALSE(list3.empty());

    // Leave the container as-is to have elements to destruct during static deinitialization.
}


using PS1 = Etl::Pooled::Set<Data, 46U>;
using PS2 = Etl::Pooled::Set<Data, 47U>;
using PS3 = Etl::Pooled::Set<Data, 48U>;

PS1 set1 {};
PS2 set2 {Data {}};

TEST_CASE("Test destructor of static Etl::Pooled::Set containers", "[set][siof][etl]") {

    static PS3 set3 {};

    set1.insert(Data {});
    set3.insert(Data {});

    CHECK_FALSE(set1.empty());
    CHECK_FALSE(set2.empty());
    CHECK_FALSE(set3.empty());

    // Leave the container as-is to have elements to destruct during static deinitialization.
}


using PM1 = Etl::Pooled::Map<int, Data, 56U>;
using PM2 = Etl::Pooled::Map<int, Data, 57U>;
using PM3 = Etl::Pooled::Map<int, Data, 58U>;

PM1 map1 {};
PM2 map2 {{0, Data {}}};

TEST_CASE("Test destructor of static Etl::Pooled::Map containers", "[map][siof][etl]") {

    static PM3 map3 {};

    map1.insert_or_assign(0, Data {});
    map3.insert_or_assign(0, Data {});

    CHECK_FALSE(map1.empty());
    CHECK_FALSE(map2.empty());
    CHECK_FALSE(map3.empty());

    // Leave the container as-is to have elements to destruct during static deinitialization.
}


using PMM1 = Etl::Pooled::MultiMap<int, Data, 66U>;
using PMM2 = Etl::Pooled::MultiMap<int, Data, 67U>;
using PMM3 = Etl::Pooled::MultiMap<int, Data, 68U>;

PMM1 mMap1 {};
PMM2 mMap2 {{0, Data {}}};

TEST_CASE("Test destructor of static Etl::Pooled::MultiMap containers", "[multimap][siof][etl]") {

    static PMM3 mMap3 {};

    mMap1.insert(0, Data {});
    mMap3.insert(0, Data {});

    CHECK_FALSE(mMap1.empty());
    CHECK_FALSE(mMap2.empty());
    CHECK_FALSE(mMap3.empty());

    // Leave the container as-is to have elements to destruct during static deinitialization.
}


using PUS1 = Etl::Pooled::UnorderedSet<Data, 76U, 100U>;
using PUS2 = Etl::Pooled::UnorderedSet<Data, 77U, 100U>;
using PUS3 = Etl::Pooled::UnorderedSet<Data, 78U, 100U>;

PUS1 uSet1 {};
PUS2 uSet2 {Data {}};

TEST_CASE("Test destructor of static Etl::Pooled::UnorderedSet containers",
          "[unorderedset][siof][etl]") {

    static PUS3 uSet3 {};

    uSet1.insert(Data {});
    uSet3.insert(Data {});

    CHECK_FALSE(uSet1.empty());
    CHECK_FALSE(uSet2.empty());
    CHECK_FALSE(uSet3.empty());

    // Leave the container as-is to have elements to destruct during static deinitialization.
}


using PUM1 = Etl::Pooled::UnorderedMap<int, Data, 86U, 100U>;
using PUM2 = Etl::Pooled::UnorderedMap<int, Data, 87U, 100U>;
using PUM3 = Etl::Pooled::UnorderedMap<int, Data, 88U, 100U>;

PUM1 uMap1 {};
PUM2 uMap2 {{0, Data {}}};

TEST_CASE("Test destructor of static Etl::Pooled::UnorderedMap containers",
          "[unorderedmap][siof][etl]") {

    static PUM3 uMap3 {};

    uMap1.insert_or_assign(0, Data {});
    uMap3.insert_or_assign(0, Data {});

    CHECK_FALSE(uMap1.empty());
    CHECK_FALSE(uMap2.empty());
    CHECK_FALSE(uMap3.empty());

    // Leave the container as-is to have elements to destruct during static deinitialization.
}


using PUMM1 = Etl::Pooled::UnorderedMultiMap<int, Data, 96U, 100U>;
using PUMM2 = Etl::Pooled::UnorderedMultiMap<int, Data, 97U, 100U>;
using PUMM3 = Etl::Pooled::UnorderedMultiMap<int, Data, 98U, 100U>;

PUMM1 uMMap1 {};
PUMM2 uMMap2 {{0, Data {}}};

TEST_CASE("Test destructor of static Etl::Pooled::UnorderedMultiMap containers",
          "[unorderedmultimap][siof][etl]") {

    static PUMM3 uMMap3 {};

    uMMap1.insert(0, Data {});
    uMMap3.insert(0, Data {});

    CHECK_FALSE(uMMap1.empty());
    CHECK_FALSE(uMMap2.empty());
    CHECK_FALSE(uMMap3.empty());

    // Leave the container as-is to have elements to destruct during static deinitialization.
}

}  // namespace
