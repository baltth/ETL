/**
\file
\date 2017.02.22. 13:01:20
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

#include <List.h>
#include <Test/UnalignedTester.h>


TEST_CASE("Etl::List<> basic test", "[list][etl][basic]") {

    typedef int ItemType;
    typedef Etl::List<ItemType> ListType;
    
    ListType list;

    REQUIRE(list.getSize() == 0);

    list.pushBack(2);
    list.pushFront(1);

    REQUIRE(list.getSize() == 2);

    ListType::Iterator it = list.begin();
    REQUIRE(*it == 1);
    ++it;
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(it == list.end());
   
    it = list.insert(list.begin(), 3);
    REQUIRE(list.getSize() == 3);
    REQUIRE(*it == 3);

    list.popFront();
    list.erase(list.begin());
    list.popBack();

    REQUIRE(list.getSize() == 0);

}

