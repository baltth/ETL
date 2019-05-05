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

#include "catch.hpp"
#include "ContainerTester.h"

#include <etl/Vector.h>
#include <etl/BufStr.h>

#include <ctime>
#include <cstdlib>
#include <iostream>

using ETL_NAMESPACE::Test::ContainerTester;

// Etl::Vector tests -----------------------------------------------------------

TEST_CASE("Etl::Vector<> insert performance", "[vec][perf][etl]") {

    static const uint32_t CYCLES = 100000UL;

    SECTION("with int32_t") {

        typedef Etl::Static::Vector<int32_t, 128> VecT;

        VecT vec;
        vec.push_back(-2);
        vec.push_back(-1);

        std::clock_t start = std::clock();

        for (uint32_t i = 0; i < CYCLES; ++i) {

            vec.push_back(i);
            VecT::iterator pos = vec.begin();
            vec.insert(++pos, 40U, i + 1);
            vec.push_front(i);
            vec.erase(vec.end() - 42, vec.end());
        }

        std::clock_t end = std::clock();

        CHECK(vec.size() == 2);

        std::cout << "Etl::Static::Vector<int32_t> insertion: "
                  << ((end - start) * 1000.0 / CLOCKS_PER_SEC) << " ms" << std::endl;
    }

    SECTION("with ContainerTester") {

        typedef Etl::Static::Vector<ContainerTester, 128> VecT;

        VecT vec;
        vec.push_back(ContainerTester(-2));
        vec.push_back(ContainerTester(-1));

        std::clock_t start = std::clock();

        for (uint32_t i = 0; i < CYCLES; ++i) {

            vec.push_back(ContainerTester(i));
            VecT::iterator pos = vec.begin();
            vec.insert(++pos, 40U, ContainerTester(i + 1));
            vec.push_front(ContainerTester(i));
            vec.erase(vec.end() - 42, vec.end());
        }

        std::clock_t end = std::clock();

        CHECK(vec.size() == 2);

        std::cout << "Etl::Static::Vector<ContainerTester> insertion: "
                  << ((end - start) * 1000.0 / CLOCKS_PER_SEC) << " ms" << std::endl;
    }
}


// Etl::BufStr performance tests ---------------------------------------------


TEST_CASE("Etl::BufStr performance", "[bufstr][perf][etl]") {

    static const uint32_t CYCLES = 100000UL;

    typedef Etl::Static::BufStr<128> BufT;

    SECTION("with double") {

        std::clock_t sum = 0;

        for (uint32_t i = 0; i < CYCLES; ++i) {

            BufT bs;
            bs << i;

            double val = (std::rand() * 100.0 / RAND_MAX) - 50.0;

            std::clock_t start = std::clock();
            bs << val;
            std::clock_t end = std::clock();

            sum += (end - start);
        }

        CHECK(sum > 0);

        std::cout << "Etl::BufStr << double: "
                  << (sum * 1000.0 / CLOCKS_PER_SEC) << " ms" << std::endl;
    }

    SECTION("with hex") {

        std::clock_t sum = 0;

        for (uint32_t i = 0; i < CYCLES; ++i) {

            BufT bs;
            bs << "0x";

            std::clock_t start = std::clock();
            bs << Etl::BufStr::Hex(i, 10);
            std::clock_t end = std::clock();

            sum += (end - start);
        }

        CHECK(sum > 0);

        std::cout << "Etl::BufStr << hex: "
                  << (sum * 1000.0 / CLOCKS_PER_SEC) << " ms" << std::endl;
    }

}