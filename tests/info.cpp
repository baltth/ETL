/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019 Balazs Toth.

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

#include "ContainerTester.h"
#include <catch2/catch.hpp>

#include <etl/BufStr.h>
#include <etl/Vector.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using Etl::Test::ContainerTester;

// Etl::Vector tests -----------------------------------------------------------

namespace {

template<typename V>
void testRandom(V& vec) {

    static const uint32_t CYCLES = 100000UL;

    for (size_t i = 0; i < CYCLES; ++i) {

        vec.push_back(i);
        auto pos = vec.begin();
        vec.insert(++pos, 40U, i + 1);
        vec.insert(vec.begin(), i);
        vec.erase(vec.end() - 42, vec.end());
    }
}

TEST_CASE("Etl::Vector<> insert performance", "[vec][perf][etl]") {

    typedef Etl::Static::Vector<int32_t, 128> SVec;

    SVec sVec;
    sVec.push_back(-2);
    sVec.push_back(-1);

    BENCHMARK("Static::Vector<int32_t>") {
        testRandom(sVec);
    };

    typedef Etl::Dynamic::Vector<int32_t> DVec;

    DVec dVec;
    dVec.push_back(-2);
    dVec.push_back(-1);

    BENCHMARK("Dynamic::Vector<int32_t>") {
        testRandom(dVec);
    };

    typedef std::vector<int32_t> StdVec;

    StdVec stdVec;
    stdVec.push_back(-2);
    stdVec.push_back(-1);

    BENCHMARK("std::vector<int32_t>") {
        testRandom(stdVec);
    };
}

// Etl::BufStr performance tests ---------------------------------------------


TEST_CASE("Etl::BufStr performance", "[bufstr][perf][etl]") {

    typedef Etl::Static::BufStr<128> BufT;

    BENCHMARK_ADVANCED("with double")(Catch::Benchmark::Chronometer meter) {
        double val = (std::rand() * 100.0 / RAND_MAX) - 50.0;
        meter.measure([val] {
            BufT bs;
            bs << val;
        });
    };

    BENCHMARK_ADVANCED("with hex")(Catch::Benchmark::Chronometer meter) {
        auto i = static_cast<uint32_t>(std::rand());
        meter.measure([i] {
            BufT bs;
            bs << "0x";
            bs << Etl::BufStr::Hex(i, 10);
        });
    };
}

}  // namespace
