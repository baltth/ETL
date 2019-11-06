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

#include <catch2/catch.hpp>

#include <etl/Fifo.h>
#include <etl/FifoAccess.h>

#include <etl/Array.h>
#include <etl/Vector.h>

#include "ContainerTester.h"

using ETL_NAMESPACE::Test::ContainerTester;


template<class FifoT>
void testFifoBasic(FifoT& fifo, uint32_t capacity) {

    REQUIRE(fifo.capacity() == capacity);
    REQUIRE(fifo.size() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.size() == 2);
    REQUIRE(fifo[0] == 1);
    REQUIRE(fifo[-1] == 2);

    REQUIRE(fifo.pop() == 1);
    REQUIRE(fifo.pop() == 2);

    REQUIRE(fifo.size() == 0);
}


TEST_CASE("Etl::Fifo<> basic test with Array<>", "[fifo][array][etl][basic]") {

    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Array<int, SIZE>> FifoType;

    FifoType fifo;
    testFifoBasic(fifo, SIZE);
}


TEST_CASE("Etl::Fifo<> basic test with Dynamic::Vector<>", "[fifo][vector][dynamic][etl][basic]") {

    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Dynamic::Vector<int>> FifoType;

    FifoType fifo(SIZE);
    testFifoBasic(fifo, SIZE);
}


TEST_CASE("Etl::Fifo<> basic test with Static::Vector<>", "[fifo][vector][static][etl][basic]") {

    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Static::Vector<int, SIZE>> FifoType;

    FifoType fifo(SIZE);
    testFifoBasic(fifo, SIZE);
}


TEST_CASE("Etl::Fifo<> element access", "[fifo][etl]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE>> FifoType;

    FifoType fifo;

    fifo.push(1);
    fifo.push(2);
    fifo.push(3);
    fifo.push(4);
    fifo.push(5);

    CHECK(fifo.size() == 5);

    REQUIRE(fifo[0] == 1);
    REQUIRE(fifo[4] == 5);
    REQUIRE(fifo[-1] == 5);
    REQUIRE(fifo[-5] == 1);

    CHECK(fifo.pop() == 1);

    REQUIRE(fifo[0] == 2);
    REQUIRE(fifo[3] == 5);
    REQUIRE(fifo[-1] == 5);
    REQUIRE(fifo[-4] == 2);
}


TEST_CASE("Etl::Fifo<> push with move", "[fifo][etl]") {

    typedef ContainerTester ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE>> FifoType;

    FifoType fifo;

    auto pMoves = ContainerTester::getMoveCount();
    auto pCopies = ContainerTester::getCopyCount();

    fifo.push(ContainerTester(1));
    fifo.push(ContainerTester(2));

    ContainerTester ct(3);
    fifo.push(std::move(ct));

    REQUIRE(fifo.size() == 3);

    REQUIRE(ContainerTester::getMoveCount() > pMoves);
    REQUIRE(ContainerTester::getCopyCount() == pCopies);

    REQUIRE(fifo[0] == ContainerTester(1));
    REQUIRE(fifo[1] == ContainerTester(2));
    REQUIRE(fifo[2] == ContainerTester(3));
}


TEST_CASE("Etl::Fifo<> iteration", "[fifo][etl]") {

    typedef int ItemType;
    static const uint32_t SIZE = 4;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE>> FifoType;

    FifoType fifo;

    fifo.push(1);
    fifo.push(2);
    fifo.push(3);

    CHECK(fifo.size() == 3);

    int i = 0;
    for (FifoType::iterator it = fifo.begin(); it != fifo.end(); ++it) {
        REQUIRE(*it == fifo[i]);
        ++i;
    }

    fifo.push(4);
    fifo.push(5);
    fifo.push(6);

    i = 0;
    for (FifoType::const_iterator it = fifo.cbegin(); it != fifo.cend(); ++it) {
        REQUIRE(*it == fifo[i]);
        ++i;
    }

    i = 0;
    FifoType::iterator it = fifo.end();
    while (it != fifo.begin()) {
        --i;
        --it;
        REQUIRE(*it == fifo[i]);
    }
}


TEST_CASE("Etl::Fifo<> overflow", "[fifo][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 4;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE>> FifoType;

    FifoType fifo;

    fifo.push(1);
    fifo.push(2);
    fifo.push(3);
    fifo.push(4);

    CHECK(fifo.size() == SIZE);
    CHECK(fifo[0] == 1);
    CHECK(fifo[-1] == 4);

    fifo.push(5);

    REQUIRE(fifo.size() == SIZE);
    REQUIRE(fifo[0] == 2);
    REQUIRE(fifo[-1] == 5);

    fifo.push(6);

    REQUIRE(fifo.size() == SIZE);
    REQUIRE(fifo[0] == 3);
    REQUIRE(fifo[-1] == 6);

    CHECK(fifo.pop() == 3);

    REQUIRE(fifo[0] == 4);
    REQUIRE(fifo[-1] == 6);
}


TEST_CASE("Etl::Fifo<> resize", "[fifo][etl]") {

    typedef int ItemType;
    static const uint32_t SIZE1 = 4;
    static const uint32_t SIZE2 = 32;
    typedef Etl::Fifo<Etl::Dynamic::Vector<ItemType>> FifoType;

    FifoType fifo;

    REQUIRE(fifo.capacity() == 0);
    REQUIRE(fifo.size() == 0);

    fifo.setupFifo(SIZE1);

    REQUIRE(fifo.capacity() == SIZE1);
    REQUIRE(fifo.size() == 0);

    fifo.push(1);
    fifo.push(2);
    CHECK(fifo.size() == 2);

    fifo.setupFifo(SIZE2);

    REQUIRE(fifo.capacity() == SIZE2);
    REQUIRE(fifo.size() == 0);
}


TEST_CASE("Etl::Fifo<> length", "[fifo][etl]") {

    typedef int ItemType;
    static const uint32_t SIZE = 8;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE>> FifoType;

    FifoType fifo;

    CHECK(fifo.capacity() == SIZE);
    REQUIRE(fifo.size() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.size() == 2);

    fifo.clear();

    REQUIRE(fifo.size() == 0);
    REQUIRE(fifo.empty());

    fifo.push(3);
    fifo.push(4);
    fifo.push(5);
    fifo.push(6);

    REQUIRE(fifo.size() == 4);

    fifo.resize(2);

    REQUIRE(fifo.size() == 2);
    REQUIRE(fifo[-1] == 6);
    REQUIRE(fifo[0] == 5);

    fifo.resize(6);

    REQUIRE(fifo.size() == 6);
    REQUIRE(fifo[-1] == 6);
    REQUIRE(fifo[0] == 1);
}


TEST_CASE("Etl::FifoAccess<> basic test with Array<>", "[fifo][array][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Array<ItemType, SIZE> ArrayType;

    ArrayType array;
    Etl::FifoAccess<ItemType> fifo(array);

    REQUIRE(fifo.capacity() == SIZE);
    REQUIRE(fifo.size() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.size() == 2);
    REQUIRE(fifo[0] == 1);
    REQUIRE(fifo[-1] == 2);

    REQUIRE(fifo.pop() == 1);
    REQUIRE(fifo.pop() == 2);

    REQUIRE(fifo.size() == 0);
}
