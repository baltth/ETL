/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Balazs Toth.

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

#include <etl/Array.h>
#include <etl/Proxy.h>

#include "UnalignedTester.h"

using Etl::Test::UnalignedTester;


TEST_CASE("Etl::Array<> basic test", "[array][etl][basic]") {

    typedef int ItemType;
    typedef Etl::Array<ItemType, 16> ArrayType;

    ArrayType array;

    uint32_t size = array.size();
    REQUIRE(size == 16);

    ArrayType::iterator it = array.begin();
    *it = 1;
    CAPTURE(*it);
    REQUIRE(array[0] == *it);
}


TEST_CASE("Etl::GenericProxy - Array<> test", "[array][etl][basic]") {

    typedef int ItemType;

    Etl::Array<ItemType, 16> array;
    Etl::GenericProxy proxy(array);

    REQUIRE(proxy.size() == array.size());
    REQUIRE(proxy.getItemSize() == sizeof(ItemType));
    REQUIRE(proxy.data() == array.data());
    REQUIRE(proxy.getItemPointer(0) == array.begin());
    REQUIRE(proxy.getItemPointer(proxy.size()) == array.end());
}


TEST_CASE("Etl::Proxy - Array<> test", "[array][etl][basic]") {

    typedef int ItemType;

    Etl::Array<ItemType, 16> array;
    Etl::Proxy<ItemType> proxy(array);

    REQUIRE(proxy.size() == array.size());
    REQUIRE(proxy.data() == array.data());
    REQUIRE(proxy.getItemPointer(0) == array.begin());
    REQUIRE(proxy.getItemPointer(proxy.size()) == array.end());
}


TEST_CASE("Etl::MutableProxy - Array<> test", "[array][etl][basic]") {

    typedef int ItemType;

    Etl::Array<ItemType, 16> array;
    Etl::MutableProxy<ItemType> proxy(array);

    REQUIRE(proxy.size() == array.size());
    REQUIRE(proxy.data() == array.data());
    REQUIRE(proxy.getItemPointer(0) == array.begin());
    REQUIRE(proxy.getItemPointer(proxy.size()) == array.end());
}
