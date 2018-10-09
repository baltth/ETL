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

#include "catch.hpp"

#include <etl/mathSupport.h>


TEST_CASE("Math support tests", "[math][etl][basic]") {

    REQUIRE(NAN != NAN);
    REQUIRE(INFINITY == INFINITY);
    REQUIRE(-INFINITY == -INFINITY);
    REQUIRE(INFINITY != -INFINITY);

    REQUIRE(Etl::isnan(NAN));
    REQUIRE(Etl::isnan(0.0 / 0.0));

    REQUIRE_FALSE(Etl::isnan(0));
    REQUIRE_FALSE(Etl::isnan(0.0));
    REQUIRE_FALSE(Etl::isnan(1.0));
    REQUIRE_FALSE(Etl::isnan(-1.0));

    REQUIRE_FALSE(Etl::isnan(INFINITY));
    REQUIRE_FALSE(Etl::isnan(-INFINITY));

    REQUIRE(Etl::isinf(INFINITY));
    REQUIRE(Etl::isinf(-INFINITY));
    REQUIRE(Etl::isinf(1.0 / 0.0));
    REQUIRE(Etl::isinf(-1.0 / 0.0));

    REQUIRE_FALSE(Etl::isinf(0.0));
    REQUIRE_FALSE(Etl::isinf(0.0));
    REQUIRE_FALSE(Etl::isinf(1.0));
    REQUIRE_FALSE(Etl::isinf(-1.0));

    REQUIRE_FALSE(Etl::isinf(NAN));
}

