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

#ifndef ETL_BUFSTR_H_
#define ETL_BUFSTR_H_

#include <etl/Vector.h>
#include <etl/base/AMemStreamBuf.h>
#include <etl/base/OutStream.h>
#include <etl/base/StreamBufImpl.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <array>
#include <climits>
#include <iomanip>
#include <limits>
#include <ostream>
#include <streambuf>
#include <string>

namespace ETL_NAMESPACE {

template<typename CharType>
class BasicBufStr {

  public:  // types

    using char_type = CharType;

    using StreamBuf = Detail::AMemStreamBuf<char_type>;
    using OutStream = Detail::OutStream<char_type>;

    using BasicOutStream = Detail::BasicOutStream<char_type>;
    using BasicIos = std::basic_ios<char_type, std::char_traits<char_type>>;

    /// \name Format manipulation
    /// \{

    class FormatSaver {

      private:  // variables

        BasicBufStr& bs;
        std::ios_base::fmtflags flags;
        char_type fill;
        std::streamsize precision;

      public:

        explicit FormatSaver(BasicBufStr& bs) :
            bs {bs},
            flags {bs.stream.flags()},
            fill {bs.stream.fill()},
            precision {bs.stream.precision()} {}

        ~FormatSaver() {
            bs.stream.precision(precision);
            bs.stream.fill(fill);
            bs.stream.flags(flags);
        }
    };

    enum class Radix : std::uint8_t {
        Bin,
        Oct,
        Dec,
        Hex
    };

    template<typename T>
    struct FormattedInt {
        const T val;
        const Radix radix;
        const std::uint8_t width;
    };
    /// \}

  private:  // variables

    StreamBuf* sb;
    OutStream stream;

  public:  // functions

    BasicBufStr() = delete;
    ~BasicBufStr() = default;

    BasicBufStr(const BasicBufStr& other) = delete;
    BasicBufStr& operator=(const BasicBufStr& other) & = delete;
    BasicBufStr(BasicBufStr&& other) = delete;
    BasicBufStr& operator=(BasicBufStr&& other) & = delete;

    /// \name Access interface
    /// \{

    const char_type* cStr() const {
        ETL_ASSERT(sb != nullptr);
        return sb->data();
    }

    std::string str() const {
        return std::string {cStr()};
    }

    std::size_t size() const {
        return sb->size();
    }

    bool good() const {
        return stream.good();
    }
    /// \}

    /// \name Stream interface
    /// \{

    template<typename T>
    BasicBufStr& operator<<(T v) {
        stream << v;
        return *this;
    }

    BasicBufStr& operator<<(std::ios_base& (*func)(std::ios_base&)) {
        stream << func;
        return *this;
    }

    BasicBufStr& operator<<(BasicIos& (*func)(BasicIos&)) {
        stream << func;
        return *this;
    }

    BasicBufStr& operator<<(BasicOutStream& (*func)(BasicOutStream&)) {
        stream << func;
        return *this;
    }
    /// \}

    /// \name Format manipulation
    /// \{

    template<typename T>
    BasicBufStr& operator<<(const FormattedInt<T>& f) {
        add(f);
        return *this;
    }

    template<typename T>
    static FormattedInt<T> Dec(T val, std::uint8_t w = 0U) {
        return FormattedInt<T> {val, Radix::Dec, w};
    }

    template<typename T>
    static FormattedInt<T> Hex(T val, std::uint8_t w = 0U) {
        return FormattedInt<T> {val, Radix::Hex, w};
    }

    template<typename T>
    static FormattedInt<T> Oct(T val, std::uint8_t w = 0U) {
        return FormattedInt<T> {val, Radix::Oct, w};
    }

    template<typename T>
    static FormattedInt<T> Bin(T val, std::uint8_t w = 0U) {
        return FormattedInt<T> {val, Radix::Bin, w};
    }
    /// \}

  protected:

    explicit BasicBufStr(StreamBuf& s) :
        sb {&s},
        stream {s} {
        stream.exceptions(std::ios_base::goodbit);  // no exceptions
    }

  private:

    template<typename T>
    void add(const FormattedInt<T>& f) {
        auto fs = FormatSaver {*this};
        if (f.radix == Radix::Bin) {
            addBin(f.val, f.width);
        } else {
            std::ios_base::fmtflags radix {};
            switch (f.radix) {
                case Radix::Dec:
                    radix = std::ios_base::dec;
                    break;
                case Radix::Hex:
                    radix = std::ios_base::hex;
                    break;
                case Radix::Oct:
                    radix = std::ios_base::oct;
                    break;
                default:
                    ETL_ASSERT(false);  // Invalid radix
                    break;
            }
            stream.setf(radix, std::ios_base::basefield);
            if (f.width > 0) {
                stream.fill(char_type {'0'});
                stream << std::setw(f.width);
            }
            stream << f.val;
        }
    }

    template<typename T>
    void addBin(T val, std::uint8_t width) {
        static constexpr std::size_t DIGITS = CHAR_BIT * sizeof(T);

        stream.fill(char_type {'0'});
        bool fillStarted = false;

        if (width > DIGITS) {
            for (int i = (width - DIGITS); i >= 0; --i) {
                stream.put('0');
            }
            fillStarted = true;
        }

        for (int i = (DIGITS - 1); i >= 0; --i) {
            std::uint8_t b = (val >> i) & 0x01U;
            if (b == 0) {
                if (fillStarted || (width > i)) {
                    stream.put('0');
                    fillStarted = true;
                }
            } else {
                stream.put('1');
                fillStarted = true;
            }
        }
    }
};


using BufStr = BasicBufStr<char>;


namespace Static {

template<std::size_t N, typename CharType>
class BasicBufStr : public ETL_NAMESPACE::BasicBufStr<CharType> {

    static_assert(N > 1, "Invalid Etl::Static::BasicBufStr size");
    static_assert(N <= std::numeric_limits<std::streamsize>::max(),
                  "Invalid Etl::Static::BasicBufStr size");

  public:  // types

    using char_type = CharType;
    using Base = ETL_NAMESPACE::BasicBufStr<char_type>;
    using StreamBuf = Detail::StaticStreamBuf<char_type, N>;

  private:  // variables

    StreamBuf sb;

  public:  // functions

    BasicBufStr() :
        Base {sb},
        sb {} {}

    BasicBufStr(BasicBufStr&& other) :
        Base {sb},
        sb {std::move(other.sb)} {};

    BasicBufStr& operator=(BasicBufStr&& other) & {
        sb = std::move(other.sb);
        return *this;
    }

    BasicBufStr(const BasicBufStr& other) = delete;
    BasicBufStr& operator=(const BasicBufStr& other) & = delete;

    ~BasicBufStr() = default;

    const typename StreamBuf::Buffer& buffer() const {
        return sb.buffer();
    }
};


template<std::size_t N>
using BufStr = BasicBufStr<N, char>;

}  // namespace Static


namespace Dynamic {

template<typename CharType>
class BasicBufStr : public ETL_NAMESPACE::BasicBufStr<CharType> {

  public:  // types

    using char_type = CharType;
    using Base = ETL_NAMESPACE::BasicBufStr<char_type>;
    using StreamBuf = Detail::DynamicStreamBuf<char_type>;

  private:  // variables

    StreamBuf sb {};

  public:  // functions

    BasicBufStr() :
        Base {sb} {}

    BasicBufStr(BasicBufStr&& other) :
        Base {sb},
        sb {std::move(other.sb)} {};

    BasicBufStr& operator=(BasicBufStr&& other) & {
        sb = std::move(other.sb);
        return *this;
    }
    BasicBufStr(const BasicBufStr& other) = delete;
    BasicBufStr& operator=(const BasicBufStr& other) & = delete;

    ~BasicBufStr() = default;

    const typename StreamBuf::Buffer& buffer() const {
        return sb.buffer();
    }
};


using BufStr = BasicBufStr<char>;

}  // namespace Dynamic

}  // namespace ETL_NAMESPACE

#endif  // ETL_BUFSTR_H_
