/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017-2026 Balazs Toth.

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

namespace {

static_assert(Etl::Detail::NothrowContract<Etl::Static::BufStr<32U>>::value,
              "Etl::Static::BufStr<N> violates nothrow contract");
static_assert(Etl::Detail::NothrowContract<Etl::Dynamic::BufStr>::nothrowIfMovable,
              "Etl::Dynamic::BufStr<N> violates nothrow contract");


void checkContent(const Etl::BufStr& bs, const char* expected) {

    CAPTURE(bs.getBuff().begin(), expected);
    REQUIRE(strcmp(bs.getBuff().begin(), expected) == 0);
}


TEST_CASE("Etl::BufStr() test", "[bufstr][etl]") {

    using Etl::BufStr;

    Etl::Static::BufStr<120> bs;

    SECTION("Default state") {

        const Etl::Vector<char>& data = bs.getBuff();

        REQUIRE(data.begin() == bs.cStr());
        REQUIRE(data.size() == 1);
        REQUIRE(data.back() == '\0');

        REQUIRE(bs.getFill() == 1);
        REQUIRE(bs.getRadix() == 10);
        REQUIRE(bs.getPrecision() == 3);
    }

    SECTION("Endline serialization") {

        bs << BufStr::Endl;
        checkContent(bs, "\n");
    }

    SECTION("Char serialization") {

        bs << BufStr::Char('a') << "bcd";
        checkContent(bs, "abcd");
    }

    SECTION("Bool serialization") {

        bs << true << ", " << false;
        checkContent(bs, "true, false");
    }

    SECTION("Integer serialization") {

        bs << 132UL << ", " << -132L;
        checkContent(bs, "132, -132");

        bs.clear();
        bs << INT64_MIN << ", " << -1;
        checkContent(bs, "-9223372036854775808, -1");

        bs.clear();
        bs << UINT64_MAX;
        checkContent(bs, "18446744073709551615");
    }

    SECTION("Float serialization") {

        bs << 0.0;
        checkContent(bs, "0.0");

        bs.clear();
        bs << 132.0 << ", " << -132.102f;
        checkContent(bs, "132.0, -132.102");

        bs.clear();
        bs << 132.10222 << ", " << 132.10255;
        checkContent(bs, "132.102, 132.103");

        SECTION("Float specials") {

            bs.clear();
            bs << INFINITY << ", " << -INFINITY;
            checkContent(bs, "inf, -inf");

            bs.clear();
            bs << NAN;
            checkContent(bs, "NaN");
        }
    }

    SECTION("Enum serialization") {

        bs << BufStr::Radix::HEX;
        checkContent(bs, "16");

        bs.clear();
        bs << BufStr::Radix::BIN;
        checkContent(bs, "2");
    }

    SECTION("Pointer serialization") {

        static const size_t PTR_TETRADES = sizeof(void*) * 2;

        bs << &bs;
        REQUIRE(strlen(bs.getBuff().begin()) == (PTR_TETRADES + 2));
        REQUIRE(bs.getBuff()[0] == '0');
        REQUIRE(bs.getBuff()[1] == 'x');

        bs.clear();
        bs << BufStr::Pad(PTR_TETRADES + 6) << &bs;
        REQUIRE(strlen(bs.getBuff().begin()) == (PTR_TETRADES + 6));
        REQUIRE(bs.getBuff()[0] == ' ');
        REQUIRE(bs.getBuff()[3] == ' ');
        REQUIRE(bs.getBuff()[4] == '0');
        REQUIRE(bs.getBuff()[5] == 'x');
    }
}


TEST_CASE("Etl::BufStr() - Formats", "[bufstr][etl]") {

    using Etl::BufStr;

    Etl::Static::BufStr<120> bs;

    SECTION("Fill") {

        bs << BufStr::Fill(5) << 112;
        checkContent(bs, "00112");

        bs.clear();
        bs << BufStr::Fill(5) << -112;
        checkContent(bs, "-00112");
    }

    SECTION("Precision") {

        bs << BufStr::Prec(5) << 1.1234567;
        checkContent(bs, "1.12346");

        bs.clear();
        bs << BufStr::Prec(1) << 1.1234567;
        checkContent(bs, "1.1");

        bs.clear();
        bs << BufStr::Prec(1) << 1.99 << ", " << -1.99;
        checkContent(bs, "2.0, -2.0");
    }

    SECTION("Padding") {

        bs << BufStr::Pad(6) << 13;
        checkContent(bs, "    13");

        bs.clear();
        bs << BufStr::Pad(6) << -13;
        checkContent(bs, "   -13");

        bs.clear();
        bs << BufStr::Pad(6) << 13.02;
        checkContent(bs, "    13.02");

        bs.clear();
        bs << BufStr::Pad(6) << -1356.7;
        checkContent(bs, " -1356.7");

        bs.clear();
        bs << BufStr::Pad(12) << INT8_C(-13);
        checkContent(bs, "         -13");
    }

    SECTION("Format persistency") {

        bs << 33 << ", " << BufStr::SetHex << 33;
        checkContent(bs, "33, 21");

        bs.clear();
        bs << 33 << ", " << BufStr::Fill(4) << 33;
        checkContent(bs, "21, 0021");

        bs.clear();
        bs << 33 << ", " << BufStr::SetDec << 33;
        checkContent(bs, "0021, 0033");

        bs.clear();
        bs << BufStr::Default << 33;
        checkContent(bs, "33");
    }

    SECTION("On-the-fly ints") {

        bs << 11 << ", " << BufStr::Hex(11) << ", " << 11;
        checkContent(bs, "11, b, 11");

        bs.clear();
        bs << BufStr::Fill(4);
        bs << 11 << ", " << BufStr::Hex(11, 2) << ", " << 11;
        checkContent(bs, "0011, 0b, 0011");

        bs.clear();
        bs << BufStr::Fill(3);
        bs << 11 << ", " << BufStr::Bin(11) << ", " << 11;
        checkContent(bs, "011, 1011, 011");
    }
}


TEST_CASE("Etl::BufStr() - Decimal representations", "[bufstr][etl]") {

    using Etl::BufStr;

    Etl::Static::BufStr<120> bs;

    SECTION("Hex") {

        bs << BufStr::SetHex << 132UL;
        checkContent(bs, "84");

        bs.clear();
        bs << UINT64_MAX;
        checkContent(bs, "ffffffffffffffff");

        bs.clear();
        bs << INT64_C(-1);
        checkContent(bs, "ffffffffffffffff");

        bs.clear();
        bs << -1;
        checkContent(bs, "ffffffff");

        bs.clear();
        bs << -2;
        checkContent(bs, "fffffffe");

        bs.clear();
        bs << static_cast<int8_t>(INT8_MAX);
        checkContent(bs, "7f");
    }

    SECTION("Hex formats") {

        bs << BufStr::SetHex;
        bs << BufStr::Fill(7) << 0x33AAF;
        checkContent(bs, "0033aaf");

        bs.clear();
        bs << BufStr::Pad(9) << 0x33AAF;
        checkContent(bs, "  0033aaf");
    }

    SECTION("Bin") {

        bs << BufStr::SetBin << 132UL;
        checkContent(bs, "10000100");

        bs.clear();
        bs << UINT16_MAX;
        checkContent(bs, "1111111111111111");

        bs.clear();
        bs << static_cast<int16_t>(-1);
        checkContent(bs, "1111111111111111");

        bs.clear();
        bs << static_cast<int16_t>(-2);
        checkContent(bs, "1111111111111110");

        bs.clear();
        bs << static_cast<int8_t>(INT8_MAX);
        checkContent(bs, "1111111");
    }

    SECTION("Bin formats") {

        bs << BufStr::SetBin;
        bs << BufStr::Fill(8) << static_cast<int8_t>(47);
        checkContent(bs, "00101111");

        bs.clear();
        bs << BufStr::Pad(11) << static_cast<int8_t>(47);
        checkContent(bs, "   00101111");
    }
}

TEMPLATE_TEST_CASE("Etl::BufStr() - copy/move",
                   "[bufstr][etl]",
                   Etl::Static::BufStr<43>,
                   Etl::Static::BufStr<45>,
                   Etl::Dynamic::BufStr) {

    using BS = Etl::Static::BufStr<43>;
    using BD = Etl::Dynamic::BufStr;

    const char TEST_STR[] = "testString";

    auto checkCopy = [](const Etl::BufStr& dst, const Etl::BufStr& src) {
        CAPTURE(dst.cStr());
        CAPTURE(src.cStr());
        REQUIRE(dst.cStr() != src.cStr());
        REQUIRE(strcmp(dst.cStr(), src.cStr()) == 0);
    };

    SECTION("B(const B&)") {
        SECTION("from Static") {
            BS src(TEST_STR);
            TestType dst(src);
            checkCopy(dst, src);
        }

        SECTION("from Dynamic") {
            BD src(TEST_STR);
            TestType dst(src);
            checkCopy(dst, src);
        }
    }

    SECTION("B=(const B&)") {
        SECTION("from Static") {
            BS src(TEST_STR);
            TestType dst;
            CHECK(dst.empty());
            dst = src;
            checkCopy(dst, src);
        }

        SECTION("from Dynamic") {
            BD src(TEST_STR);
            TestType dst;
            CHECK(dst.empty());
            dst = src;
            checkCopy(dst, src);
        }
    }

    auto checkMove = [&TEST_STR](const Etl::BufStr& dst, const Etl::BufStr& src) {
        CAPTURE(dst.cStr());
        CAPTURE(src.cStr());
        REQUIRE(dst.cStr() != src.cStr());
        REQUIRE(strcmp(dst.cStr(), TEST_STR) == 0);
    };

    SECTION("B(B&&)") {
        SECTION("from Static") {
            BS src(TEST_STR);
            CHECK_FALSE(src.empty());
            TestType dst(std::move(src));
            checkMove(dst, src);
        }

        SECTION("from Dynamic") {
            BD src(TEST_STR);
            CHECK_FALSE(src.empty());
            TestType dst(std::move(src));
            checkMove(dst, src);
        }
    }

    SECTION("B=(B&&)") {
        SECTION("from Static") {
            BS src(TEST_STR);
            CHECK_FALSE(src.empty());

            TestType dst("DD");
            CHECK_FALSE(dst.empty());
            dst = std::move(src);
            checkMove(dst, src);
        }

        SECTION("from Dynamic") {
            BD src(TEST_STR);
            CHECK_FALSE(src.empty());

            TestType dst("DD");
            CHECK_FALSE(dst.empty());
            dst = std::move(src);
            checkMove(dst, src);
        }
    }
}


TEST_CASE("Etl::BufStr() - fill", "[bufstr][etl]") {

    Etl::Static::BufStr<17> bs;
    bs << "1234567890";
    bs << "1234567890";

    REQUIRE(bs.size() == 16);
    checkContent(bs, "1234567890123456");
}

}  // namespace
