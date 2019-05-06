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

#include <etl/Span.h>

#include "ContainerTester.h"

using ETL_NAMESPACE::Test::ContainerTester;


TEST_CASE("Etl::Span basic test", "[span][etl][basic]") {

    const char data[] = "0123456789";

    Etl::Span<const char> span(data);

    static_assert(span.extent == Etl::dynamic_extent, "dynamic_extent expected");

    REQUIRE_FALSE(span.empty());
    REQUIRE(span.size() == sizeof(data));

    REQUIRE(span.front() == '0');
    REQUIRE(span.back() == '\0');
    REQUIRE(span[4] == '4');
}
