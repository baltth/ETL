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

#include <iostream>
#include <random>

namespace {

Etl::Dynamic::Set<uint32_t> getInput(size_t n) {

    Etl::Dynamic::Set<uint32_t> res;

    std::mt19937 mt;
    for (uint32_t i = 0; i < n; ++i) {
        res.insert(mt());
    }

    return res;
}


void validateInput(const Etl::Set<uint32_t>& input) {

    REQUIRE_FALSE(input.empty());
    uint32_t prev = *input.begin();
    for (auto it = ++input.begin(); it != input.end(); ++it) {
        REQUIRE(*it > prev);
        prev = *it;
    }
}


template<typename C, size_t N>
void testRandomContent() {

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


TEMPLATE_TEST_CASE("Random content",
                   "[set][unorderedmap][map][etl][complex]",
                   (Etl::Static::UnorderedMap<uint32_t, uint32_t, 10000U, 1000U>),
                   (Etl::Pooled::UnorderedMap<uint32_t, uint32_t, 10000U, 1000U>),
                   (Etl::Static::Map<uint32_t, uint32_t, 10000U>),
                   (Etl::Pooled::Map<uint32_t, uint32_t, 10000U>)) {

    SECTION("with 100 elements") {
        static const size_t N = 100U;
        testRandomContent<TestType, N>();
    }

    SECTION("with 10000 elements") {
        static const size_t N = 10000U;
        testRandomContent<TestType, N>();
    }
}

}  // namespace
