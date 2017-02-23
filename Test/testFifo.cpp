/**
\file
\date 2017.02.23. 12:03:21
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

#include <Fifo.h>
#include <Array.h>
#include <Test/ContainerTester.h>


TEST_CASE("Etl::Fifo<> basic test", "[fifo][etl][basic]") {

    typedef int ItemType;
    static const uint32_t SIZE = 16;
    typedef Etl::Fifo<Etl::Array<ItemType, SIZE> > FifoType;

    FifoType fifo;

    REQUIRE(fifo.getCapacity() == SIZE);
    REQUIRE(fifo.getLength() == 0);

    fifo.push(1);
    fifo.push(2);

    REQUIRE(fifo.getLength() == 2);

    REQUIRE(fifo.pop() == 1);
    REQUIRE(fifo.pop() == 2);

    REQUIRE(fifo.getLength() == 0);
    
}

/*
TEST_CASE("Etl::Fifo<> leak test", "[fifo][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Fifo<ItemType> FifoType;

    static const int PATTERN = 123;

    FifoType* fifo = new FifoType();

    fifo->pushBack(ContainerTester(PATTERN));
    fifo->pushBack(ContainerTester(PATTERN));
    fifo->pushBack(ContainerTester(PATTERN));
    fifo->pushBack(ContainerTester(PATTERN));
    fifo->pushBack(ContainerTester(PATTERN));
    fifo->pushBack(ContainerTester(PATTERN));
    fifo->pushBack(ContainerTester(PATTERN));
    fifo->pushBack(ContainerTester(PATTERN));

    CHECK(fifo->getSize() == ItemType::getObjectCount());

    fifo->popBack();
    REQUIRE(fifo->getSize() == ItemType::getObjectCount());

    fifo->erase(fifo->begin());
    REQUIRE(fifo->getSize() == ItemType::getObjectCount());

    delete fifo;
    REQUIRE(ItemType::getObjectCount() == 0);

}
*/

