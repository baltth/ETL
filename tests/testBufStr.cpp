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

    static constexpr char EXPECTED[] = "12 1 false   0x2d -123.45\n";

    TestType bs;
    CHECK(bs.good());

    bs << 12 << " " << true << " " << std::boolalpha << false;
    bs << std::setw(5) << "0x" << std::hex << 45U;
    bs << " " << -123.45 << std::endl;

    CAPTURE(bs.cStr());
    REQUIRE(equals(bs, EXPECTED));
    REQUIRE(bs.size() == std::strlen(EXPECTED));
    REQUIRE(bs.good());
}


TEST_CASE("Etl::BasicBufStr<wchar_t>", "[bufstr][etl]") {

    Etl::Dynamic::BasicBufStr<wchar_t> bs;
    CHECK(bs.good());

    bs << -756 << " and some text";

    CAPTURE(bs.cStr());
    REQUIRE(equals(bs, L"-756 and some text"));
    REQUIRE(bs.good());
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
        REQUIRE(dest.good());
    }

    SECTION("Move assignment") {
        TestType dest;
        dest << 22;
        dest = std::move(src);
        CAPTURE(dest.cStr());
        REQUIRE(equals(dest, "12"));
        REQUIRE(dest.good());
    }
}


TEST_CASE("Etl::StaticBufStr<> - overflow", "[bufstr][etl]") {

    static constexpr char INPUT[] = "12345678901234567890";
    static constexpr std::size_t SIZE {16U};
    static constexpr char TRUNCATED[] = "123456789012345";

    Etl::Static::BufStr<SIZE> bs;

    CHECK(bs.buffer().size() == SIZE);

    bs << INPUT;

    REQUIRE(bs.buffer().size() == SIZE);
    REQUIRE(bs.size() == std::strlen(TRUNCATED));
    CAPTURE(bs.cStr());
    REQUIRE(equals(bs, TRUNCATED));
    REQUIRE_FALSE(bs.good());
}


TEST_CASE("Etl::DynamicBufStr - overflow", "[bufstr][etl]") {

    static constexpr char INPUT[] = "12345678901234567890";

    Etl::Dynamic::BufStr bs;

    CHECK(bs.buffer().size() == 0U);

    bs << INPUT;

    REQUIRE(bs.buffer().size() == Etl::Detail::DynamicStreamBuf<char>::SIZE_INCREMENT);
    REQUIRE(bs.size() == std::strlen(INPUT));
    CAPTURE(bs.cStr());
    REQUIRE(equals(bs, INPUT));
    REQUIRE(bs.good());
}


TEST_CASE("Etl::BufStr - std formatters", "[bufstr][etl]") {

    Etl::Static::BufStr<128U> bs;

    SECTION("std::setprecision()") {
        bs << std::setprecision(4) << -12.7777;

        CAPTURE(bs.cStr());
        REQUIRE(equals(bs, "-12.78"));
        REQUIRE(bs.good());
    }

    SECTION("std::setw()") {
        bs << std::setw(4) << 14;

        CAPTURE(bs.cStr());
        REQUIRE(equals(bs, "  14"));
        REQUIRE(bs.good());
    }

    SECTION("std::setfill()") {
        bs << std::setfill('0') << std::setw(4) << 14;

        CAPTURE(bs.cStr());
        REQUIRE(equals(bs, "0014"));
        REQUIRE(bs.good());
    }
}


TEST_CASE("Etl::BufStr - non-sticky formatters", "[bufstr][etl]") {

    static constexpr std::uint16_t VAL = 17U;
    Etl::Static::BufStr<120U> bs;

    SECTION("Basic formatting") {
        SECTION("Dec(v)") {
            bs << Etl::BufStr::Dec(VAL) << " " << Etl::BufStr::Dec(VAL, 5U);

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "17 00017"));
        }

        SECTION("Hex(v)") {
            bs << Etl::BufStr::Hex(VAL) << " " << Etl::BufStr::Hex(VAL, 5U);

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "11 00011"));
        }

        SECTION("Oct(v)") {
            bs << Etl::BufStr::Oct(VAL) << " " << Etl::BufStr::Oct(VAL, 5U);

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "21 00021"));
        }

        SECTION("Bin(v)") {
            bs << Etl::BufStr::Bin(VAL) << " " << Etl::BufStr::Bin(VAL, 8U);

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "10001 00010001"));
        }
    }

    SECTION("Reset format") {
        SECTION("Dec(v)") {
            bs << std::hex << VAL << " " << Etl::BufStr::Dec(VAL, 5U) << " " << VAL;

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "11 00017 11"));
        }

        SECTION("Hex(v)") {
            bs << VAL << " " << Etl::BufStr::Hex(VAL, 5U) << " " << VAL;

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "17 00011 17"));
        }

        SECTION("Oct(v)") {
            bs << VAL << " " << Etl::BufStr::Oct(VAL, 5U) << " " << VAL;

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "17 00021 17"));
        }

        SECTION("Bin(v)") {
            bs << VAL << " " << Etl::BufStr::Bin(VAL, 8U) << " " << VAL;

            CAPTURE(bs.cStr());
            REQUIRE(equals(bs, "17 00010001 17"));
        }
    }
}

}  // namespace
