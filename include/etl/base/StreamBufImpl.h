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

#ifndef ETL_DETAIL_STREAMBUFIMPL_H_
#define ETL_DETAIL_STREAMBUFIMPL_H_

#include <etl/Vector.h>
#include <etl/base/AMemStreamBuf.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <array>
#include <limits>
#include <streambuf>

namespace ETL_NAMESPACE {
namespace Detail {

template<typename CharType, std::size_t N>
class StaticStreamBuf : public AMemStreamBuf<CharType> {

    static_assert(N > 1, "Invalid Etl::Static::BufStr size");
    static_assert(N <= std::numeric_limits<std::streamsize>::max(),
                  "Invalid Etl::Static::BufStr size");

  public:  // types

    using Base = AMemStreamBuf<CharType>;

    using char_type = typename Base::char_type;
    using traits_type = typename Base::traits_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;

  private:  // variables

    std::array<char_type, N> buff {};

  public:  // functions

    StaticStreamBuf() {
        this->setPutArea(buff.data(), buff.size() - 1U);
    }

    const char_type* data() const override {
        return buff.data();
    }

  private:

    int_type onOverflow(int_type) override {
        return traits_type::eof();
    }
};


template<typename CharType>
class DynamicStreamBuf : public AMemStreamBuf<CharType> {

  public:  // types

    using Base = AMemStreamBuf<CharType>;

    using char_type = typename Base::char_type;
    using traits_type = typename Base::traits_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;

    static constexpr std::size_t SIZE_INCREMENT {128U};

  private:  // variables

    Dynamic::Vector<char_type> buff {};

  public:  // functions

    const char_type* data() const override {
        return buff.data();
    }

  private:

    int_type onOverflow(int_type ch) override {
        if (traits_type::not_eof(ch) == ch) {
            ETL_ASSERT(buff.size() == buff.capacity());
            auto origSize = buff.size();
            if (incrementBufferSize()) {
                ETL_ASSERT(buff.size() > origSize);
                buff[origSize] = static_cast<char_type>(ch);
                setOffsetBuffer(origSize + 1U);
                return traits_type::not_eof(ch);
            }
        }
        return traits_type::eof();
    }

    bool incrementBufferSize() {
        auto origSize = buff.size();
        buff.reserve(buff.size() + SIZE_INCREMENT);
        buff.resize(buff.capacity());
        return (buff.size() > origSize);
    }

    void setOffsetBuffer(std::size_t offset) {
        this->setPutArea((buff.data() + offset), (buff.size() - offset));
    }
};

template<typename CharType>
constexpr std::size_t DynamicStreamBuf<CharType>::SIZE_INCREMENT;

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_DETAIL_STREAMBUFIMPL_H_