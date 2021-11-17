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

#ifndef __ETL_BUFSTR_H__
#define __ETL_BUFSTR_H__

#include <etl/Vector.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

namespace ETL_NAMESPACE {


class BufStr {

  public:  // types

    struct Char {
        char ch;
        explicit Char(char c) :
            ch(c) {};
    };

    static const struct EndlineT : Char { EndlineT() : Char('\n') {}; } Endl;

    enum Radix {
        BIN = 2,
        DEC = 10,
        HEX = 16
    };

    template<typename V>
    struct IntFormatSpec {
        const V val;
        const Radix radix;
        const uint8_t fill;
        explicit IntFormatSpec(V v, Radix r, uint8_t f = 0) :
            val(v),
            radix(r),
            fill(f) {};
    };

    struct Fill {
        uint8_t fill;
        explicit Fill(uint8_t f) :
            fill(f) {};
    };

    struct Prec {
        uint8_t precision;
        explicit Prec(uint8_t p) :
            precision(p) {};
    };

    struct Pad {
        uint8_t padding;
        explicit Pad(uint8_t p) :
            padding(p) {};
    };

    struct DecModT {};
    static const DecModT SetDec;
    struct HexModT {};
    static const HexModT SetHex;
    struct BinModT {};
    static const BinModT SetBin;
    struct DefaultModT {};
    static const DefaultModT Default;

    template<uint32_t S>
    struct SizeTypeTrait {};

    template<typename T>
    struct LengthTrait {
        static const uint8_t VALUE = SizeTypeTrait<sizeof(T)>::VALUE;
    };

    struct Format {
        Radix radix;
        uint8_t fill;
        uint8_t precision;
        uint8_t padding;
        Format() :
            radix(DEC),
            fill(1),
            precision(3),
            padding(1) {};
    };

    struct FormatSaver {
        BufStr& str;
        Format format;
        explicit FormatSaver(BufStr& s) :
            str(s),
            format(s.format) {};
        ~FormatSaver() {
            str.format = format;
        }
    };

  private:  // variables

    ETL_NAMESPACE::Vector<char>& data;
    Format format;

  public:  // functions

    BufStr& operator=(const BufStr& other) {
        clear();
        return operator<<(other);
    }

    /// \name Data interface
    /// \{
    BufStr& put(char c) {
        data.back() = c;
        closeStr();
        return *this;
    }

    BufStr& write(const char* str, size_t len);
    BufStr& write(const char* str);

    BufStr& operator<<(bool data) {
        if (data) {
            return write("true", sizeof("true") - 1);
        } else {
            return write("false", sizeof("false") - 1);
        }
    }

    BufStr& operator<<(Char data) {
        return put(data.ch);
    }

    template<typename T>
    enable_if_t<is_integral<T>::value && is_unsigned<T>::value, BufStr&> operator<<(T data) {
        return putUNumber(data);
    }

    template<typename T>
    enable_if_t<is_integral<T>::value && is_signed<T>::value, BufStr&> operator<<(T data) {
        return putSNumber(data);
    }

    template<typename T>
    enable_if_t<is_floating_point<T>::value, BufStr&> operator<<(T data) {
        return putFloat(data);
    }

    template<typename T>
    enable_if_t<is_enum<T>::value, BufStr&> operator<<(T data) {
        return operator<<(static_cast<typename std::underlying_type<T>::type>(data));
    }

    BufStr& operator<<(const void* data) {
        return putPointer(data);
    }

    BufStr& operator<<(const BufStr& other) {
        if (!other.empty()) {
            write(other.cStr(), other.size());
        }
        return *this;
    }

    const ETL_NAMESPACE::Vector<char>& getBuff() const {
        return data;
    }

    const char* cStr() const {
        return data.begin();
    }

    uint32_t size() const {
        return data.size() ? (data.size() - 1) : 0;
    }

    bool empty() const {
        return (size() == 0);
    }

    void clear() {
        data.clear();
        closeStr();
    }
    /// \}

    /// \name Format interface
    /// \{
    template<typename T>
    BufStr& operator<<(IntFormatSpec<T> data) {
        Format f = format;
        format.radix = data.radix;
        *this << Fill(data.fill) << data.val;
        format = f;
        return *this;
    }

    BufStr& operator<<(DecModT) {
        format.radix = DEC;
        return *this;
    }

    BufStr& operator<<(HexModT) {
        format.radix = HEX;
        return *this;
    }

    BufStr& operator<<(BinModT) {
        format.radix = BIN;
        return *this;
    }

    BufStr& operator<<(Fill mod) {
        if (mod.fill) {
            format.fill = mod.fill;
        }
        return *this;
    }

    BufStr& operator<<(Prec mod) {
        if (mod.precision) {
            format.precision = mod.precision;
        }
        return *this;
    }

    BufStr& operator<<(Pad mod) {
        if (mod.padding) {
            format.padding = mod.padding;
        }
        return *this;
    }

    BufStr& operator<<(DefaultModT) {
        resetFormat();
        return *this;
    }

    template<typename T>
    static IntFormatSpec<T> Dec(T val, uint8_t f = 0) {
        return IntFormatSpec<T>(val, DEC, f);
    }

    template<typename T>
    static IntFormatSpec<T> Hex(T val, uint8_t f = 0) {
        return IntFormatSpec<T>(val, HEX, f);
    }

    template<typename T>
    static IntFormatSpec<T> Bin(T val, uint8_t f = 0) {
        return IntFormatSpec<T>(val, BIN, f);
    }

    void resetFormat() {
        format = Format();
    }

    Radix getRadix() const {
        return format.radix;
    }

    uint8_t getFill() const {
        return format.fill;
    }

    uint8_t getPrecision() const {
        return format.precision;
    }

    uint8_t getPadding() const {
        return format.padding;
    }
    /// \}

  protected:

    explicit BufStr(ETL_NAMESPACE::Vector<char>& d) :
        data(d) {};

    template<typename T>
    BufStr& putUNumber(T val) {

        if (getRadix() == HEX) {
            toHexString(static_cast<uint64_t>(val), sizeof(T));
        } else if (getRadix() == BIN) {
            toBinString(static_cast<uint64_t>(val), sizeof(T));
        } else {
            toString(static_cast<uint64_t>(val), LengthTrait<T>::VALUE);
        }
        return *this;
    }

    template<typename T>
    BufStr& putSNumber(T val) {

        if (getRadix() == HEX) {
            toHexString(static_cast<uint64_t>(val), sizeof(T));
        } else if (getRadix() == BIN) {
            toBinString(static_cast<uint64_t>(val), sizeof(T));
        } else {
            toString(static_cast<int64_t>(val), LengthTrait<T>::VALUE);
        }
        return *this;
    }

    BufStr& putFloat(double val) {
        if (!handleFloatSpecials(val)) {
            toString(val);
        }
        return *this;
    }

    BufStr& putPointer(const void* val);

    void toString(uint64_t val, uint8_t lenType, char prefix = 0);
    void toString(int64_t val, uint8_t lenType);
    void toHexString(uint64_t val, uint8_t size);
    void toBinString(uint64_t val, uint8_t size);

    bool handleFloatSpecials(double val);
    void toString(double val);

    void pad(uint8_t num);
    void putDigits(uint64_t val, uint64_t decades, bool forceAll = false);

    void writeWithPadding(const char* str, uint32_t len, uint8_t padding);

    void putChar(char c) {
        data.push_back(c);
    }

    void closeStr() {
        data.push_back('\0');
    }

    void openStr() {
        data.pop_back();
    }

    void insertOp(const char* str, uint32_t len) {
        data.insert(data.end(), str, (str + len));
    }

    static char tetradeToChar(uint8_t val);
};


namespace Static {

template<uint32_t N>
class BufStr : public ETL_NAMESPACE::BufStr {

  public:  // types

    typedef ETL_NAMESPACE::BufStr Base;

  private:  // variables

    ETL_NAMESPACE::Static::Vector<char, N> data;

  public:  // functions

    BufStr() :
        Base(data) {
        closeStr();
    }

    BufStr(const BufStr& other) :
        Base(data) {
        closeStr();
        operator<<(other);
    };

    explicit BufStr(const ETL_NAMESPACE::BufStr& other) :
        Base(data) {
        closeStr();
        operator<<(other);
    };

    BufStr& operator=(const BufStr& other) {
        ETL_NAMESPACE::BufStr::operator=(other);
        return *this;
    }

    BufStr& operator=(const ETL_NAMESPACE::BufStr& other) {
        ETL_NAMESPACE::BufStr::operator=(other);
        return *this;
    }

    explicit BufStr(const char* str) :
        Base(data) {
        closeStr();
        write(str);
    }
};

}  // namespace Static


namespace Dynamic {

class BufStr : public ETL_NAMESPACE::BufStr {

  public:  // types

    typedef ETL_NAMESPACE::BufStr Base;

  private:  // variables

    ETL_NAMESPACE::Dynamic::Vector<char> data;

  public:  // functions

    BufStr() :
        Base(data) {
        closeStr();
    }

    BufStr(const BufStr& other) :
        Base(data) {
        closeStr();
        operator<<(other);
    };

    explicit BufStr(const ETL_NAMESPACE::BufStr& other) :
        Base(data) {
        closeStr();
        operator<<(other);
    };

    BufStr& operator=(const BufStr& other) {
        ETL_NAMESPACE::BufStr::operator=(other);
        return *this;
    }

    BufStr& operator=(const ETL_NAMESPACE::BufStr& other) {
        ETL_NAMESPACE::BufStr::operator=(other);
        return *this;
    }

    explicit BufStr(const char* str) :
        Base(data) {
        closeStr();
        write(str);
    }
};

}  // namespace Dynamic


template<>
struct BufStr::SizeTypeTrait<sizeof(uint8_t)> {
    static const uint8_t VALUE = 0;
};

template<>
struct BufStr::SizeTypeTrait<sizeof(uint16_t)> {
    static const uint8_t VALUE = 1;
};

template<>
struct BufStr::SizeTypeTrait<sizeof(uint32_t)> {
    static const uint8_t VALUE = 2;
};

template<>
struct BufStr::SizeTypeTrait<sizeof(uint64_t)> {
    static const uint8_t VALUE = 3;
};

}  // namespace ETL_NAMESPACE


inline ETL_NAMESPACE::BufStr& operator<<(ETL_NAMESPACE::BufStr& bs, const char* data) {

    return bs.write(data);
}

#endif  // __ETL_BUFSTR_H__
