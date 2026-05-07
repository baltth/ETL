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

#ifndef ETL_BUFSTR_H_
#define ETL_BUFSTR_H_

#include <etl/Vector.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <utility>

namespace ETL_NAMESPACE {


class BufStr {

  public:  // types

    struct Char {
        char ch;
        explicit Char(char c) :
            ch(c) {};
    };

    static const struct EndlineT : Char {
        EndlineT() :
            Char('\n') {}
    } Endl;

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
            fill(f) {}
    };

    struct Fill {
        uint8_t fill;
        explicit Fill(uint8_t f) :
            fill(f) {}
    };

    struct Prec {
        uint8_t precision;
        explicit Prec(uint8_t p) :
            precision(p) {}
    };

    struct Pad {
        uint8_t padding;
        explicit Pad(uint8_t p) :
            padding(p) {}
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
        Radix radix {DEC};
        uint8_t fill {1U};
        uint8_t precision {3U};
        uint8_t padding {1U};
    };

    struct FormatSaver {
        BufStr& str;
        Format format;
        explicit FormatSaver(BufStr& s) :
            str(s),
            format(s.format) {}
        ~FormatSaver() {
            str.format = format;
        }
    };

  private:  // variables

    ETL_NAMESPACE::Vector<char>& data;
    Format format;

  public:  // functions

    BufStr() noexcept = delete;
    BufStr(const BufStr& other) = delete;
    BufStr(BufStr&& other) noexcept = delete;
    ~BufStr() = default;

    BufStr& operator=(const BufStr& other) & {
        data = other.data;
        format = other.format;
        closeStrOnDemand();
        return *this;
    }

    BufStr& operator=(BufStr&& other) & {
        data = std::move(other.data);
        format = std::move(other.format);
        closeStrOnDemand();
        return *this;
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

    const ETL_NAMESPACE::Vector<char>& getBuff() const noexcept {
        return data;
    }

    const char* cStr() const noexcept {
        return data.begin();
    }

    uint32_t size() const noexcept {
        return data.size() ? (data.size() - 1) : 0;
    }

    bool empty() const noexcept {
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

    void setFormat(Format f) {
        format = f;
    }

    Format getFormat() const noexcept {
        return format;
    }

    Radix getRadix() const noexcept {
        return format.radix;
    }

    uint8_t getFill() const noexcept {
        return format.fill;
    }

    uint8_t getPrecision() const noexcept {
        return format.precision;
    }

    uint8_t getPadding() const noexcept {
        return format.padding;
    }
    /// \}

  private:

    /// \name Stream interface
    /// \{

    friend BufStr& operator<<(BufStr& self, bool data) {
        if (data) {
            return self.write("true", sizeof("true") - 1);
        } else {
            return self.write("false", sizeof("false") - 1);
        }
    }

    friend BufStr& operator<<(BufStr& self, Char data) {
        return self.put(data.ch);
    }

    template<typename T>
    friend enable_if_t<is_integral<T>::value && is_unsigned<T>::value, BufStr&>
    operator<<(BufStr& self, T data) {
        return self.putUNumber(data);
    }

    template<typename T>
    friend enable_if_t<is_integral<T>::value && is_signed<T>::value, BufStr&>
    operator<<(BufStr& self, T data) {
        return self.putSNumber(data);
    }

    template<typename T>
    friend enable_if_t<is_floating_point<T>::value, BufStr&> operator<<(BufStr& self, T data) {
        return self.putFloat(data);
    }

    template<typename T>
    friend enable_if_t<is_enum<T>::value, BufStr&> operator<<(BufStr& self, T data) {
        return self << static_cast<typename std::underlying_type<T>::type>(data);
    }

    friend BufStr& operator<<(BufStr& self, const char* data) {
        return self.write(data);
    }

    friend BufStr& operator<<(BufStr& self, const void* data) {
        return self.putPointer(data);
    }

    friend BufStr& operator<<(BufStr& self, const BufStr& other) {
        if (!other.empty()) {
            self.write(other.cStr(), other.size());
        }
        return self;
    }

    template<typename T>
    friend BufStr& operator<<(BufStr& self, IntFormatSpec<T> data) {
        FormatSaver fs {self};
        self.format.radix = data.radix;
        self << Fill(data.fill) << data.val;
        return self;
    }

    friend BufStr& operator<<(BufStr& self, DecModT) {
        self.format.radix = DEC;
        return self;
    }

    friend BufStr& operator<<(BufStr& self, HexModT) {
        self.format.radix = HEX;
        return self;
    }

    friend BufStr& operator<<(BufStr& self, BinModT) {
        self.format.radix = BIN;
        return self;
    }

    friend BufStr& operator<<(BufStr& self, Fill mod) {
        if (mod.fill) {
            self.format.fill = mod.fill;
        }
        return self;
    }

    friend BufStr& operator<<(BufStr& self, Prec mod) {
        if (mod.precision) {
            self.format.precision = mod.precision;
        }
        return self;
    }

    friend BufStr& operator<<(BufStr& self, Pad mod) {
        if (mod.padding) {
            self.format.padding = mod.padding;
        }
        return self;
    }

    friend BufStr& operator<<(BufStr& self, DefaultModT) {
        self.resetFormat();
        return self;
    }
    /// \}

  protected:

    explicit BufStr(ETL_NAMESPACE::Vector<char>& d) noexcept :
        data(d) {}

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

    void closeStrOnDemand() {
        if (data.empty() || (data.back() != '\0')) {
            data.push_back('\0');
        }
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

    static_assert(N > 0U, "Invalid size for Static::BufStr");

    using Base = ETL_NAMESPACE::BufStr;
    using Data = ETL_NAMESPACE::Static::Vector<char, N>;

  private:  // variables

    Data data;

  public:  // functions

    BufStr() noexcept :
        Base(data) {
        closeStr();
    }

    BufStr(const BufStr& other) :
        BufStr() {
        this->operator=(other);
    }

    BufStr& operator=(const BufStr& other) & {
        Base::operator=(other);
        return *this;
    }

    BufStr(BufStr&& other) noexcept(noexcept(BufStr().operator=(std::move(other)))) :
        Base(data) {
        this->operator=(std::move(other));
    }

    BufStr& operator=(BufStr&& other) noexcept(std::is_nothrow_move_assignable<Data>::value) {
        // Direct move of members allow propagation of
        // noexcept properties of the data container type
        data = std::move(other.data);
        setFormat(other.getFormat());
        return *this;
    }

    explicit BufStr(const Base& other) :
        BufStr() {
        this->operator=(other);
    }

    BufStr& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    explicit BufStr(Base&& other) :
        BufStr() {
        this->operator=(std::move(other));
    }

    BufStr& operator=(Base&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    explicit BufStr(const char* str) :
        BufStr() {
        write(str);
    }
};

}  // namespace Static


namespace Dynamic {

class BufStr : public ETL_NAMESPACE::BufStr {

  public:  // types

    using Base = ETL_NAMESPACE::BufStr;
    using Data = ETL_NAMESPACE::Dynamic::Vector<char>;

  private:  // variables

    Data data;

  public:  // functions

    BufStr() :
        Base(data) {
        closeStr();
    }

    BufStr(const BufStr& other) :
        BufStr() {
        this->operator=(other);
    }

    BufStr& operator=(const BufStr& other) & {
        Base::operator=(other);
        return *this;
    }

    BufStr(BufStr&& other) noexcept(std::is_nothrow_move_assignable<BufStr::Data>::value) :
        Base(data) {
        // Direct move of members allow propagation of
        // noexcept properties of the data container type
        data = std::move(other.data);
        setFormat(other.getFormat());
    }

    BufStr& operator=(BufStr&& other) noexcept(
        std::is_nothrow_move_assignable<BufStr::Data>::value) {
        // Direct move of members allow propagation of
        // noexcept properties of the data container type
        data = std::move(other.data);
        setFormat(other.getFormat());
        return *this;
    }

    explicit BufStr(const Base& other) :
        BufStr() {
        this->operator=(other);
    }

    BufStr& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    explicit BufStr(Base&& other) :
        BufStr() {
        this->operator=(std::move(other));
    }

    BufStr& operator=(Base&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    explicit BufStr(const char* str) :
        BufStr() {
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

#endif  // ETL_BUFSTR_H_
