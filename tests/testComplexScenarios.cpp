/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2022 Balazs Toth.

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

#include <etl/Map.h>
#include <etl/Set.h>
#include <etl/UnorderedMap.h>
#include <etl/UnorderedSet.h>

#include <iostream>
#include <random>
#include <set>

namespace {

std::set<uint32_t> getInput(size_t n) {

    std::set<uint32_t> res;

    std::mt19937 mt;
    for (uint32_t i = 0; i < n; ++i) {
        res.insert(mt());
    }

    return res;
}


void validateInput(const std::set<uint32_t>& input) {

    REQUIRE_FALSE(input.empty());
    uint32_t prev = *input.begin();
    for (auto it = ++input.begin(); it != input.end(); ++it) {
        REQUIRE(*it > prev);
        prev = *it;
    }
}


template<typename C, size_t N>
void testRandomMapContent() {

    auto input = getInput(N);

    CAPTURE(input.size());
    validateInput(input);

    size_t cnt {0U};
    C map;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        auto inserted = map.insert(std::make_pair(item, item));
        REQUIRE(inserted.second);
        REQUIRE(inserted.first != map.end());
        REQUIRE(inserted.first->first == item);
        REQUIRE(inserted.first->second == item);

        REQUIRE(map.size() == cnt);

        auto it = map.find(item);
        REQUIRE(it == inserted.first);

        auto secondTry = map.insert(std::make_pair(item, item));
        REQUIRE_FALSE(secondTry.second);
        REQUIRE(map.size() == cnt);
    }

    REQUIRE(map.size() == input.size());

    cnt = 0U;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        auto it = map.find(item);
        REQUIRE(it != map.end());
        REQUIRE(it->first == item);
        REQUIRE(it->second == item);
    }

    cnt = 0U;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        map.erase(item);
        REQUIRE(map.size() == (N - cnt));
    }

    REQUIRE(map.empty());
}


TEMPLATE_TEST_CASE("Random content - maps",
                   "[set][unorderedmap][map][etl][complex]",
                   (Etl::Static::UnorderedMap<uint32_t, uint32_t, 10000U, 1000U>),
                   (Etl::Pooled::UnorderedMap<uint32_t, uint32_t, 10000U, 1000U>),
                   (Etl::Static::Map<uint32_t, uint32_t, 10000U>),
                   (Etl::Pooled::Map<uint32_t, uint32_t, 10000U>)) {

    SECTION("with 100 elements") {
        static const size_t N = 100U;
        testRandomMapContent<TestType, N>();
    }

    SECTION("with 10000 elements") {
        static const size_t N = 10000U;
        testRandomMapContent<TestType, N>();
    }
}

template<typename C, size_t N>
void testRandomSetContent() {

    auto input = getInput(N);

    CAPTURE(input.size());
    validateInput(input);

    size_t cnt {0U};
    C set;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        auto inserted = set.insert(item);
        REQUIRE(inserted.second);
        REQUIRE(inserted.first != set.end());
        REQUIRE(*inserted.first == item);

        REQUIRE(set.size() == cnt);

        auto it = set.find(item);
        REQUIRE(it == inserted.first);

        auto secondTry = set.insert(item);
        REQUIRE_FALSE(secondTry.second);
        REQUIRE(set.size() == cnt);
    }

    REQUIRE(set.size() == input.size());

    cnt = 0U;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        auto it = set.find(item);
        REQUIRE(it != set.end());
        REQUIRE(*it == item);
    }

    cnt = 0U;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        set.erase(item);
        REQUIRE(set.size() == (N - cnt));
    }

    REQUIRE(set.empty());
}


TEMPLATE_TEST_CASE("Random content - sets",
                   "[set][unorderedset][etl][complex]",
                   (Etl::Static::UnorderedSet<uint32_t, 10000U, 1000U>),
                   (Etl::Pooled::UnorderedSet<uint32_t, 10000U, 1000U>),
                   (Etl::Static::Set<uint32_t, 10000U>),
                   (Etl::Pooled::Set<uint32_t, 10000U>)) {

    SECTION("with 100 elements") {
        static const size_t N = 100U;
        testRandomSetContent<TestType, N>();
    }

    SECTION("with 10000 elements") {
        static const size_t N = 10000U;
        testRandomSetContent<TestType, N>();
    }
}
}  // namespace
