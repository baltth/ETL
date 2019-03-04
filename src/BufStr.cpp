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

#include <etl/BufStr.h>
#include <etl/mathSupport.h>

#include <cstring>

using ETL_NAMESPACE::BufStr;

const BufStr::EndlineT BufStr::Endl;

const uint8_t BufStr::DIGITS[] = {     /// \todo do something more elegant
    3,
    5,
    10,
    20
};

const uint64_t BufStr::DECADES[] = {   /// \todo do something more elegant
    UINT64_C(100),
    UINT64_C(10000),
    UINT64_C(1000000000),
    UINT64_C(10000000000000000000)
};


BufStr& BufStr::write(const char* str, size_t len) {

    if (str) {
        data.reserve(data.size() + len);
        uint32_t freeSpace = data.capacity() - data.size();
        uint32_t lenToCopy = (len <= freeSpace) ? len : freeSpace;
        if (lenToCopy > 0) {
            Etl::Vector<char>::iterator it = data.end();
            data.insert(--it, str, &str[lenToCopy]);
        }
    }

    return *this;
}


BufStr& BufStr::write(const char* data) {

    return write(data, std::strlen(data));
}


void BufStr::toString(int64_t val, uint8_t lenType) {

    if (val == INT64_MIN) {

        uint64_t alias = static_cast<uint64_t>(INT64_MAX) + 1ULL;
        toString(alias, lenType, '-');

    } else if (val < 0) {

        uint64_t alias = static_cast<uint64_t>(0 - val);
        toString(alias, lenType, '-');

    } else {

        toString(static_cast<uint64_t>(val), lenType);
    }
}


void BufStr::toString(uint64_t val, uint8_t lenType, char prefix /* = 0 */) {

    uint64_t decade = DECADES[lenType];
    uint8_t fillLimit = DIGITS[lenType];
    uint8_t padding = getPadding();
    bool startNum = false;
    bool prefixWritten = (prefix == 0);

    openStr();

    if (padding > fillLimit) {
        pad(padding - fillLimit);
        padding = fillLimit;
    }

    if (prefix) {
        --padding;
    }

    while ((decade > 1) && !startNum) {

        if (val >= decade) {

            startNum = true;
            break;

        } else if (getFill() >= fillLimit) {

            if (!prefixWritten) {
                putChar(prefix);
                prefixWritten = true;
            }
            putChar('0');

        } else if (padding >= fillLimit) {
            putChar(' ');
        }

        val %= decade;
        decade /= 10;
        --fillLimit;
    }

    if (!prefixWritten) {
        putChar(prefix);
        prefixWritten = true;
    }

    putDigits(val, decade);
    closeStr();
}


void BufStr::toHexString(uint64_t val, uint8_t size) {

    bool fillStarted = false;
    uint8_t digits = size * 2;
    uint8_t padding = getPadding();

    openStr();

    if (padding > digits) {
        pad(padding - digits);
        padding = digits;
    }

    for (int i = (digits - 1); i >= 0; --i) {

        uint8_t b = (val >> (i << 2)) & 0x0f;

        if (b == 0) {
            if ((fillStarted) || (getFill() > i)) {
                putChar(tetradeToChar(b));
                fillStarted = true;
            } else if (padding > i) {
                putChar(' ');
            }
        } else {
            putChar(tetradeToChar(b));
            fillStarted = true;
        }
    }

    closeStr();
}


void BufStr::toBinString(uint64_t val, uint8_t size) {

    bool fillStarted = false;
    uint8_t digits = size * 8;
    uint8_t padding = getPadding();

    openStr();

    if (padding > digits) {
        pad(padding - digits);
        padding = digits;
    }

    for (int i = (digits - 1); i >= 0; --i) {

        uint8_t b = (val >> i) & 0x01;

        if (b == 0) {
            if ((fillStarted) || (getFill() > i)) {
                putChar('0');
                fillStarted = true;
            } else if (padding > i) {
                putChar(' ');
            }
        } else {
            putChar('1');
            fillStarted = true;
        }
    }

    closeStr();
}


bool BufStr::handleFloatSpecials(double val) {

    bool special = true;

    if (isnan(val)) {
        writeWithPadding("NaN", (sizeof("NaN") - 1), getPadding());
    } else if (isinf(val)) {
        if (val > 0) {
            writeWithPadding("inf", (sizeof("inf") - 1), getPadding());
        } else {
            writeWithPadding("-inf", (sizeof("-inf") - 1), getPadding());
        }
    } else {
        special = false;
    }

    return special;
}


void BufStr::toString(double val) {

    char intPrefix = 0;
    uint32_t fractScaling = 1;
    for (int i = 0; i < getPrecision(); ++i) {
        fractScaling *= 10;
    }

    if (val < 0) {
        val = 0 - val;
        intPrefix = '-';
    }

    int64_t upscaled = round(val * static_cast<double>(fractScaling));
    double rounded = static_cast<double>(upscaled) / fractScaling;
    int64_t integerPart = static_cast<int64_t>(rounded);
    toString(integerPart, LengthTrait<int64_t>::VALUE, intPrefix);

    openStr();
    putChar('.');

    rounded -= integerPart;
    integerPart = round(rounded * static_cast<float>(fractScaling));
    fractScaling /= 10;

    while (((integerPart % 10) == 0) && (fractScaling >= 10)) {
        integerPart /= 10;
        fractScaling /= 10;
    }

    putDigits(integerPart, fractScaling, true);
    closeStr();
}


void BufStr::writeWithPadding(const char* str, uint32_t len, uint8_t padding) {

    int32_t actualPadding = static_cast<int32_t>(padding - len);
    if (actualPadding > 0) {
        openStr();
        pad(actualPadding);
        closeStr();
    }

    write(str, len);
}


void BufStr::pad(uint8_t num) {

    for (uint8_t i = 0; i < num ; ++i) {
        putChar(' ');
    }
}


void BufStr::putDigits(uint64_t val, uint64_t decades, bool forceAll /* = false */) {

    bool started = forceAll;

    while (decades > 1) {

        if (val >= decades) {

            putChar((val / decades) + '0');
            started = true;

        } else if (started) {

            putChar('0');
        }

        val %= decades;
        decades /= 10;
    }

    putChar(val + '0');
}


char BufStr::tetradeToChar(uint8_t val) {

    val &= 0x0f;

    if (val >= 10) {
        return (val + 'a' - 10);
    } else {
        return (val + '0');
    }
}


BufStr& BufStr::putPointer(const void* val) {

    static const size_t PTR_LEN = sizeof(void*) * 2;
    static const size_t PTR_STR_LEN = PTR_LEN + 2;

    FormatSaver fs(*this);

    openStr();

    if (getPadding() > PTR_STR_LEN) {
        pad(getPadding() - PTR_STR_LEN);
    }

    putChar('0');
    putChar('x');
    closeStr();

    format.fill = PTR_LEN;
    format.padding = 1;

    toHexString(reinterpret_cast<unsigned long long>(val), sizeof(void*));

    return *this;
}

