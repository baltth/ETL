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
#include <cwchar>
#include <iomanip>

namespace {

bool equals(const Etl::BufStr& bs, const char* expected) {
    return strcmp(bs.cStr(), expected) == 0;
}

bool equals(const Etl::BasicBufStr<wchar_t>& bs, const wchar_t* expected) {
    return wcscmp(bs.cStr(), expected) == 0;
}


TEMPLATE_TEST_CASE("Etl::BufStr - basic stream operations",
                   "[bufstr][etl]",
                   (Etl::Static::BufStr<120>),
                   (Etl::Dynamic::BufStr)) {

    TestType bs;

    bs << 12 << " " << true << " " << std::boolalpha << false;
    bs << std::setw(5) << "0x" << std::hex << 45U << std::endl;

    CAPTURE(bs.cStr());
    REQUIRE(equals(bs, "12 1 false   0x2d\n"));
}


TEST_CASE("Etl::BasicBufStr<wchar_t>", "[bufstr][etl]") {

    Etl::Dynamic::BasicBufStr<wchar_t> bs;

    bs << -756 << " and some text";

    CAPTURE(bs.cStr());
    REQUIRE(equals(bs, L"-756 and some text"));
}


TEMPLATE_TEST_CASE("Etl::BufStr - move",
                   "[bufstr][etl]",
                   (Etl::Static::BufStr<120>),
                   (Etl::Dynamic::BufStr)) {

    TestType src;
    src << 12;
    CAPTURE(src.cStr());

    SECTION("Move constructor") {
        TestType dest {std::move(src)};
        CAPTURE(dest.cStr());
        REQUIRE(equals(dest, "12"));
    }

    SECTION("Move assignment") {
        TestType dest;
        dest << 22;
        dest = std::move(src);
        CAPTURE(dest.cStr());
        REQUIRE(equals(dest, "12"));
    }
}

}  // namespace
