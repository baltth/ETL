/**
\file
\date 2017.02.23.
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

#include "Fifo.h"
#include "FifoAccess.h"

#include "Array.h"
#include "Vector.h"


TEST_CASE("Etl::Fifo<> basic test with Array<>", "[fifo][array][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE> > FifoType;

    FifoType fifo;

    REQUIRE(fifo.getCapacity() == SIZE);
    REQUIRE(fifo.getLength() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.getLength() == 2);
    REQUIRE(fifo[0] == 1);
    REQUIRE(fifo[-1] == 2);

    REQUIRE(fifo.pop() == 1);
    REQUIRE(fifo.pop() == 2);

    REQUIRE(fifo.getLength() == 0);

}

TEST_CASE("Etl::Fifo<> basic test with Dynamic::Vector<>", "[fifo][vector][dynamic][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Dynamic::Vector<ItemType> > FifoType;

    FifoType fifo(SIZE);

    REQUIRE(fifo.getCapacity() == SIZE);
    REQUIRE(fifo.getLength() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.getLength() == 2);
    REQUIRE(fifo[0] == 1);
    REQUIRE(fifo[-1] == 2);

    REQUIRE(fifo.pop() == 1);
    REQUIRE(fifo.pop() == 2);

    REQUIRE(fifo.getLength() == 0);

}

TEST_CASE("Etl::Fifo<> basic test with Static::Vector<>", "[fifo][vector][static][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Static::Vector<ItemType, SIZE> > FifoType;

    FifoType fifo(SIZE);

    REQUIRE(fifo.getCapacity() == SIZE);
    REQUIRE(fifo.getLength() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.getLength() == 2);
    REQUIRE(fifo[0] == 1);
    REQUIRE(fifo[-1] == 2);

    REQUIRE(fifo.pop() == 1);
    REQUIRE(fifo.pop() == 2);

    REQUIRE(fifo.getLength() == 0);

}

TEST_CASE("Etl::Fifo<> element access", "[fifo][etl]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE> > FifoType;

    FifoType fifo;

    fifo.push(1);
    fifo.push(2);
    fifo.push(3);
    fifo.push(4);
    fifo.push(5);

    CHECK(fifo.getLength() == 5);

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

TEST_CASE("Etl::Fifo<> iteration", "[fifo][etl]") {

    typedef int ItemType;
    static const uint32_t SIZE = 4;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE> > FifoType;

    FifoType fifo;

    fifo.push(1);
    fifo.push(2);
    fifo.push(3);

    CHECK(fifo.getLength() == 3);

    int i = 0;
    for (FifoType::Iterator it = fifo.begin(); it != fifo.end(); ++it) {
        REQUIRE(*it == fifo[i]);
        ++i;
    }

    fifo.push(4);
    fifo.push(5);
    fifo.push(6);

    i = 0;
    for (FifoType::Iterator it = fifo.begin(); it != fifo.end(); ++it) {
        REQUIRE(*it == fifo[i]);
        ++i;
    }

    i = 0;
    FifoType::Iterator it = fifo.end();
    while (it != fifo.begin()) {
        --i;
        --it;
        REQUIRE(*it == fifo[i]);
    }

}

TEST_CASE("Etl::Fifo<> overflow", "[fifo][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 4;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE> > FifoType;

    FifoType fifo;

    fifo.push(1);
    fifo.push(2);
    fifo.push(3);
    fifo.push(4);

    CHECK(fifo.getLength() == SIZE);
    CHECK(fifo[0] == 1);
    CHECK(fifo[-1] == 4);

    fifo.push(5);

    REQUIRE(fifo.getLength() == SIZE);
    REQUIRE(fifo[0] == 2);
    REQUIRE(fifo[-1] == 5);

    fifo.push(6);

    REQUIRE(fifo.getLength() == SIZE);
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
    typedef Etl::Fifo<Etl::Dynamic::Vector<ItemType> > FifoType;

    FifoType fifo;

    REQUIRE(fifo.getCapacity() == 0);
    REQUIRE(fifo.getLength() == 0);

    fifo.setupFifo(SIZE1);

    REQUIRE(fifo.getCapacity() == SIZE1);
    REQUIRE(fifo.getLength() == 0);

    fifo.push(1);
    fifo.push(2);
    CHECK(fifo.getLength() == 2);

    fifo.setupFifo(SIZE2);

    REQUIRE(fifo.getCapacity() == SIZE2);
    REQUIRE(fifo.getLength() == 0);

}

TEST_CASE("Etl::Fifo<> length", "[fifo][etl]") {

    typedef int ItemType;
    static const uint32_t SIZE = 8;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE> > FifoType;

    FifoType fifo;

    CHECK(fifo.getCapacity() == SIZE);
    REQUIRE(fifo.getLength() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.getLength() == 2);

    fifo.setEmpty();

    REQUIRE(fifo.getLength() == 0);
    REQUIRE(fifo.isEmpty());

    fifo.push(3);
    fifo.push(4);
    fifo.push(5);
    fifo.push(6);

    REQUIRE(fifo.getLength() == 4);

    fifo.setLength(2);

    REQUIRE(fifo.getLength() == 2);
    REQUIRE(fifo[-1] == 6);
    REQUIRE(fifo[0] == 5);

    fifo.setLength(6);

    REQUIRE(fifo.getLength() == 6);
    REQUIRE(fifo[-1] == 6);
    REQUIRE(fifo[0] == 1);

}

TEST_CASE("Etl::FifoAccess<> basic test with Array<>", "[fifo][array][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Array<ItemType, SIZE> ArrayType;

    ArrayType array;
    Etl::FifoAccess<ItemType> fifo(array);

    REQUIRE(fifo.getCapacity() == SIZE);
    REQUIRE(fifo.getLength() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.getLength() == 2);
    REQUIRE(fifo[0] == 1);
    REQUIRE(fifo[-1] == 2);

    REQUIRE(fifo.pop() == 1);
    REQUIRE(fifo.pop() == 2);

    REQUIRE(fifo.getLength() == 0);

}


