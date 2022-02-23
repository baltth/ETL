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

std::mt19937 mt;

std::set<uint32_t> getInput(size_t n) {

    std::set<uint32_t> res;

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


template<typename C>
typename C::const_iterator selectRandomElement(const C& cont) {
    CHECK(cont.size() > 1U);
    auto dist = std::uniform_int_distribution<> {0,
                                                 static_cast<int>(cont.size()) - 1};
    auto it = cont.begin();
    std::advance(it, dist(mt));
    return it;
}

template<typename I, typename C>
void testMapFill(I& input, C& map) {

    size_t cnt {map.size()};

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
}

template<typename I, typename C>
void testMapFind(I& input, C& map) {

    size_t cnt = 0U;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        auto it = map.find(item);
        REQUIRE(it != map.end());
        REQUIRE(it->first == item);
        REQUIRE(it->second == item);
    }
}

template<typename I, typename C>
void testRemove(I& input, C& map, size_t n) {

    auto origSize = map.size();
    CHECK(origSize > n);

    for (size_t i = 1; i <= n; ++i) {
        CAPTURE(i);

        auto it = selectRandomElement(input);
        CHECK(it != input.end());
        CAPTURE(*it);

        map.erase(*it);
        REQUIRE(map.size() == (origSize - i));

        input.erase(it);
    }

    REQUIRE(input.size() == map.size());
}

template<typename I, typename C>
void testMapRemoveAndAdd(I& input, C& map, size_t n) {

    testRemove(input, map, n);

    auto newInput = getInput(n);
    testMapFill(newInput, map);

    input.insert(newInput.begin(), newInput.end());
    REQUIRE(input.size() == map.size());
}


template<typename C, size_t N, size_t NRA>
void testRandomMapContent() {

    auto input = getInput(N);

    CAPTURE(input.size());
    validateInput(input);

    C map;
    testMapFill(input, map);

    REQUIRE(map.size() == input.size());
    testMapFind(input, map);

    for (size_t i = 0; i < NRA; ++i) {
        testMapRemoveAndAdd(input, map, static_cast<size_t>(N * 0.6));
    }

    map.clear();
    REQUIRE(map.empty());

    testMapFill(input, map);
    testRemove(input, map, input.size() - 1U);

    CHECK(input.size() == 1U);
    REQUIRE(map.size() == 1U);
    REQUIRE(map.find(*input.begin()) != map.end());
}


TEMPLATE_TEST_CASE("Random content - unordered maps",
                   "[set][unorderedmap][etl][complex]",
                   (Etl::Static::UnorderedMap<uint32_t, uint32_t, 10000U, 100U>),
                   (Etl::Pooled::UnorderedMap<uint32_t, uint32_t, 10000U, 100U>)) {

    SECTION("with 100 elements") {
        testRandomMapContent<TestType, 100U, 10U>();
    }

    SECTION("with 10000 elements") {
        testRandomMapContent<TestType, 10000U, 3U>();
    }
}


TEMPLATE_TEST_CASE("Random content - maps",
                   "[set][map][etl][complex]",
                   (Etl::Static::Map<uint32_t, uint32_t, 10000U>),
                   (Etl::Pooled::Map<uint32_t, uint32_t, 10000U>)) {

    SECTION("with 100 elements") {
        testRandomMapContent<TestType, 100U, 10U>();
    }
}


template<typename I, typename C>
void testSetFill(I& input, C& set) {

    size_t cnt {set.size()};

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
}

template<typename I, typename C>
void testSetFind(I& input, C& set) {

    size_t cnt = 0U;
    for (auto& item : input) {
        ++cnt;
        CAPTURE(cnt);
        CAPTURE(item);

        auto it = set.find(item);
        REQUIRE(it != set.end());
        REQUIRE(*it == item);
    }
}

template<typename C, size_t N>
void testRandomSetContent() {

    auto input = getInput(N);

    CAPTURE(input.size());
    validateInput(input);

    C set;
    testSetFill(input, set);

    REQUIRE(set.size() == input.size());

    testSetFind(input, set);

    set.clear();
    REQUIRE(set.empty());

    testSetFill(input, set);
    testRemove(input, set, input.size() - 1U);

    CHECK(input.size() == 1U);
    REQUIRE(set.size() == 1U);
    REQUIRE(set.find(*input.begin()) != set.end());
}


TEMPLATE_TEST_CASE("Random content - unordered sets",
                   "[set][unorderedset][etl][complex]",
                   (Etl::Static::UnorderedSet<uint32_t, 10000U, 100U>),
                   (Etl::Pooled::UnorderedSet<uint32_t, 10000U, 100U>)) {
    SECTION("with 100 elements") {
        testRandomSetContent<TestType, 100U>();
    }

    SECTION("with 10000 elements") {
        testRandomSetContent<TestType, 10000U>();
    }
}

TEMPLATE_TEST_CASE("Random content - sets",
                   "[set][etl][complex]",
                   (Etl::Static::Set<uint32_t, 10000U>),
                   (Etl::Pooled::Set<uint32_t, 10000U>)) {
    SECTION("with 100 elements") {
        testRandomSetContent<TestType, 100U>();
    }
}

}  // namespace
