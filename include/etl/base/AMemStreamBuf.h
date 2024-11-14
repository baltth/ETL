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

#ifndef ETL_DETAIL_AMEMSTREAMBUF_H_
#define ETL_DETAIL_AMEMSTREAMBUF_H_

#include <etl/etlSupport.h>

#include <streambuf>
#include <utility>

namespace ETL_NAMESPACE {
namespace Detail {

template<typename CharType>
using StreamBuf = std::basic_streambuf<CharType, std::char_traits<CharType>>;


template<typename CharType>
class AMemStreamBuf : private StreamBuf<CharType> {

  public:  // types

    using Base = StreamBuf<CharType>;

    using char_type = typename Base::char_type;
    using traits_type = typename Base::traits_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;

  public:  // functions

    virtual ~AMemStreamBuf() = default;

    virtual const char_type* data() const = 0;

    Base& streambuf() {
        return *static_cast<Base*>(this);
    }

  protected:

    void setPutArea(CharType* buff, std::streamsize size) {
        ETL_ASSERT(buff != nullptr);
        ETL_ASSERT(size > 0U);
        this->setp(buff, buff + size);
    }

    virtual int_type onOverflow(int_type ch) = 0;

  private:

    int_type overflow(int_type ch) override {
        return onOverflow(ch);
    }
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_DETAIL_AMEMSTREAMBUF_H_
