/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Balazs Toth.

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

#include <etl/BufStr.h>

#include <cstring>

#if 0
#define PRINT_DATA      { std::cout << std::string(data.begin()); }
#define PRINT_DATA_NL   { std::cout << std::string(data.begin()) << std::endl; }
#else
#define PRINT_DATA
#define PRINT_DATA_NL
#endif


TEST_CASE("Etl::BufStr() test", "[bufstr][etl]") {

    using Etl::BufStr;

    Etl::Static::BufStr<120> bs;
    const Etl::Vector<char>& data = bs.getBuff();

    SECTION("Default state") {

        REQUIRE(data.begin() == bs.cStr());
        REQUIRE(data.size() == 1);
        REQUIRE(data.back() == '\0');

        REQUIRE(bs.getFill() == 1);
        REQUIRE(bs.getRadix() == 10);
        REQUIRE(bs.getPrecision() == 3);
    }

    SECTION("Char serialization") {

        bs << BufStr::Char('a') << "bcd" << BufStr::Endl;
        PRINT_DATA;
        REQUIRE(strcmp(data.begin(), "abcd\n") == 0);
    }

    SECTION("Bool serialization") {

        bs << true << ", " << false << BufStr::Endl;
        PRINT_DATA;
        REQUIRE(strcmp(data.begin(), "true, false\n") == 0);
    }

    SECTION("Integer serialization") {

        bs << 132UL << ", " << -132L << BufStr::Endl;
        PRINT_DATA;
        REQUIRE(strcmp(data.begin(), "132, -132\n") == 0);

        bs.clear();
        bs << INT64_MIN << ", " << -1 << BufStr::Endl;
        PRINT_DATA;
        REQUIRE(strcmp(data.begin(), "-9223372036854775808, -1\n") == 0);

        bs.clear();
        bs << UINT64_MAX << BufStr::Endl;
        PRINT_DATA;
        REQUIRE(strcmp(data.begin(), "18446744073709551615\n") == 0);
    }

    SECTION("Float serialization") {

        bs << 0.0;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "0.0") == 0);

        bs.clear();
        bs << 132.0 << ", " << -132.102f;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "132.0, -132.102") == 0);

        bs.clear();
        bs << 132.10222 << ", " << 132.10255;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "132.102, 132.103") == 0);

        SECTION("Float specials") {

            bs.clear();
            bs << INFINITY << ", " << -INFINITY;
            PRINT_DATA_NL;
            REQUIRE(strcmp(data.begin(), "inf, -inf") == 0);

            bs.clear();
            bs << NAN;
            PRINT_DATA_NL;
            REQUIRE(strcmp(data.begin(), "NaN") == 0);
        }
    }

    SECTION("Pointer serialization") {

        static const size_t PTR_TETRADES = sizeof(void*) * 2;

        bs << &data;
        PRINT_DATA_NL;
        REQUIRE(strlen(data.begin()) == (PTR_TETRADES + 2));
        REQUIRE(data[0] == '0');
        REQUIRE(data[1] == 'x');

        bs.clear();
        bs << BufStr::Pad(PTR_TETRADES + 6) << &data;
        PRINT_DATA_NL;
        REQUIRE(strlen(data.begin()) == (PTR_TETRADES + 6));
        REQUIRE(data[0] == ' ');
        REQUIRE(data[3] == ' ');
        REQUIRE(data[4] == '0');
        REQUIRE(data[5] == 'x');
    }
}


TEST_CASE("Etl::BufStr() - Formats", "[bufstr][etl]") {

    using Etl::BufStr;

    Etl::Static::BufStr<120> bs;
    const Etl::Vector<char>& data = bs.getBuff();

    SECTION("Fill") {

        bs << BufStr::Fill(5) << 112;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "00112") == 0);

        bs.clear();
        bs << BufStr::Fill(5) << -112;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "-00112") == 0);
    }

    SECTION("Precision") {

        bs << BufStr::Prec(5) << 1.1234567;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "1.12346") == 0);

        bs.clear();
        bs << BufStr::Prec(1) << 1.1234567;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "1.1") == 0);

        bs.clear();
        bs << BufStr::Prec(1) << 1.99 << ", " << -1.99;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "2.0, -2.0") == 0);
    }

    SECTION("Padding") {

        bs << BufStr::Pad(6) << 13;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "    13") == 0);

        bs.clear();
        bs << BufStr::Pad(6) << -13;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "   -13") == 0);

        bs.clear();
        bs << BufStr::Pad(6) << 13.02;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "    13.02") == 0);

        bs.clear();
        bs << BufStr::Pad(6) << -1356.7;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), " -1356.7") == 0);

        bs.clear();
        bs << BufStr::Pad(12) << INT8_C(-13);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "         -13") == 0);
    }

    SECTION("Format persistency") {

        bs << 33 << ", " << BufStr::SetHex << 33;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "33, 21") == 0);

        bs.clear();
        bs << 33 << ", " << BufStr::Fill(4) << 33;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "21, 0021") == 0);

        bs.clear();
        bs << 33 << ", " << BufStr::SetDec << 33;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "0021, 0033") == 0);

        bs.clear();
        bs << BufStr::Default << 33;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "33") == 0);
    }

    SECTION("On-the-fly ints") {

        bs << 11 << ", " << BufStr::Hex(11) << ", " << 11;
        PRINT_DATA_NL;
        CHECK(strcmp(data.begin(), "11, b, 11") == 0);

        bs.clear();
        bs << BufStr::Fill(4);
        bs << 11 << ", " << BufStr::Hex(11, 2) << ", " << 11;
        PRINT_DATA_NL;
        CHECK(strcmp(data.begin(), "0011, 0b, 0011") == 0);

        bs.clear();
        bs << BufStr::Fill(3);
        bs << 11 << ", " << BufStr::Bin(11) << ", " << 11;
        PRINT_DATA_NL;
        CHECK(strcmp(data.begin(), "011, 1011, 011") == 0);
    }
}


TEST_CASE("Etl::BufStr() - Decimal representations", "[bufstr][etl]") {

    using Etl::BufStr;

    Etl::Static::BufStr<120> bs;
    const Etl::Vector<char>& data = bs.getBuff();

    SECTION("Hex") {

        bs << BufStr::SetHex << 132UL;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "84") == 0);

        bs.clear();
        bs << UINT64_MAX;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "ffffffffffffffff") == 0);

        bs.clear();
        bs << INT64_C(-1);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "ffffffffffffffff") == 0);

        bs.clear();
        bs << -1;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "ffffffff") == 0);

        bs.clear();
        bs << -2;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "fffffffe") == 0);

        bs.clear();
        bs << static_cast<int8_t>(INT8_MAX);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "7f") == 0);
    }

    SECTION("Hex formats") {

        bs << BufStr::SetHex;
        bs << BufStr::Fill(7) << 0x33AAF;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "0033aaf") == 0);

        bs.clear();
        bs << BufStr::Pad(9) << 0x33AAF;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "  0033aaf") == 0);
    }

    SECTION("Bin") {

        bs << BufStr::SetBin << 132UL;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "10000100") == 0);

        bs.clear();
        bs << UINT16_MAX;
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "1111111111111111") == 0);

        bs.clear();
        bs << static_cast<int16_t>(-1);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "1111111111111111") == 0);

        bs.clear();
        bs << static_cast<int16_t>(-2);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "1111111111111110") == 0);

        bs.clear();
        bs << static_cast<int8_t>(INT8_MAX);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "1111111") == 0);
    }

    SECTION("Bin formats") {

        bs << BufStr::SetBin;
        bs << BufStr::Fill(8) << static_cast<int8_t>(47);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "00101111") == 0);

        bs.clear();
        bs << BufStr::Pad(11) << static_cast<int8_t>(47);
        PRINT_DATA_NL;
        REQUIRE(strcmp(data.begin(), "   00101111") == 0);
    }
}


TEST_CASE("Etl::BufStr() - copy/assignment", "[bufstr][etl]") {

    using Etl::BufStr;

    Etl::Static::BufStr<43> bs1("testString");
    Etl::Static::BufStr<43> bs2(bs1);
    Etl::Static::BufStr<44> bs3(bs1);
    Etl::Static::BufStr<45> bs4("2nd testString");
    Etl::Static::BufStr<45> bs5 = bs4;
    Etl::Static::BufStr<46> bs6;
    Etl::Dynamic::BufStr bs7;

    REQUIRE(bs2.cStr() != bs1.cStr());
    CAPTURE(bs2.cStr())
    CAPTURE(bs1.cStr())
    REQUIRE(strcmp(bs2.cStr(), bs1.cStr()) == 0);

    REQUIRE(bs3.cStr() != bs1.cStr());
    CAPTURE(bs3.cStr())
    CAPTURE(bs1.cStr())
    REQUIRE(strcmp(bs3.cStr(), bs1.cStr()) == 0);

    bs2 = bs4;

    REQUIRE(bs2.cStr() != bs4.cStr());
    CAPTURE(bs2.cStr())
    CAPTURE(bs4.cStr())
    REQUIRE(strcmp(bs2.cStr(), bs4.cStr()) == 0);

    REQUIRE(bs5.cStr() != bs4.cStr());
    CAPTURE(bs5.cStr())
    CAPTURE(bs4.cStr())
    REQUIRE(strcmp(bs5.cStr(), bs4.cStr()) == 0);

    bs6 = bs4;

    REQUIRE(bs6.cStr() != bs4.cStr());
    CAPTURE(bs6.cStr())
    CAPTURE(bs4.cStr())
    REQUIRE(strcmp(bs6.cStr(), bs4.cStr()) == 0);

    bs7 = bs4;

    REQUIRE(bs7.cStr() != bs4.cStr());
    CAPTURE(bs7.cStr())
    CAPTURE(bs4.cStr())
    REQUIRE(strcmp(bs7.cStr(), bs4.cStr()) == 0);
}


TEST_CASE("Etl::BufStr() - fill", "[bufstr][etl]") {

    Etl::Static::BufStr<17> bs;
    bs << "1234567890";
    bs << "1234567890";

    REQUIRE(bs.size() == 16);
    REQUIRE(strcmp(bs.cStr(), "1234567890123456") == 0);
}

