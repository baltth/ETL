/**
\file
\date 2017.02.20. 11:49:49
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

#include <FixedVector.h>
#include <Test/UnalignedTester.h>


TEST_CASE("Etl::FixedVector<> basic test", "[fixedvector][etl][basic]") {

    typedef int ItemType;
    typedef Etl::FixedVector<ItemType> VectorType;
    
    VectorType vector;

    REQUIRE(vector.getSize() == 0);

    vector.reserve(16);
    REQUIRE(vector.getCapacity() >= 16);
    REQUIRE(vector.getSize() == 0);

    vector.pushBack(1);
    vector.pushBack(2);

    REQUIRE(vector.getSize() == 2);

    VectorType::Iterator it = vector.begin();
    REQUIRE(*it == 1);
    REQUIRE(vector[0] == *it);

    ++it;
    *it = 3;

    vector.erase(vector.begin());

    REQUIRE(*vector.begin() == 3);
    REQUIRE(vector.getSize() == 1);

}


