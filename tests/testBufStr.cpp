/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2024 Balazs Toth.

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

#include <etl/BufStr.h>

#include <cstring>
#include <iomanip>

namespace {

TEMPLATE_TEST_CASE("Etl::BufStr() - basic stream operations",
                   "[bufstr][etl]",
                   (Etl::Static::BufStr<120>),
                   (Etl::Dynamic::BufStr<>)) {

    TestType bs;

    bs << 12 << " " << true << " " << std::boolalpha << false;
    bs << std::setw(5) << "0x" << std::hex << 45U << std::endl;

    CAPTURE(bs.cStr());
    REQUIRE(strcmp(bs.cStr(), "12 1 false   0x2d\n") == 0);
}

}  // namespace
