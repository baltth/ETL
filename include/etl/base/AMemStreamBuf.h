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
using BasicStreamBuf = std::basic_streambuf<CharType, std::char_traits<CharType>>;


template<typename CharType>
class AMemStreamBuf : private BasicStreamBuf<CharType> {

  public:  // types

    using Base = BasicStreamBuf<CharType>;

    using char_type = typename Base::char_type;
    using traits_type = typename Base::traits_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;

  public:  // functions

    virtual ~AMemStreamBuf() = default;

    virtual const char_type* data() const = 0;
    virtual std::size_t size() const = 0;

    Base& streambuf() {
        return *static_cast<Base*>(this);
    }

  protected:

    void setPutArea(char_type* buff, char_type* end) {
        ETL_ASSERT(buff != nullptr);
        ETL_ASSERT(end != nullptr);
        ETL_ASSERT(std::distance(buff, end) >= 0U);
        this->setp(buff, end);
    }

    const char_type* base() const {
        return this->pbase();
    }

    void deactivate() {
        this->setp(nullptr, nullptr);
    }

    // Validate put area and calculate pptr offset relative to the range.
    std::ptrdiff_t offsetInRange(const char_type* buffBegin, const char_type* buffEnd) const {
        ETL_ASSERT(buffBegin != nullptr);
        ETL_ASSERT(buffEnd != nullptr);
        auto size = std::distance(buffBegin, buffEnd);
        ETL_ASSERT(size > 0);
        (void)size;

        // 1) check base pointer, it has to point somewhere in the range.
        auto offsetOfBase = std::distance(buffBegin, this->base());
        ETL_ASSERT(offsetOfBase >= 0);
        ETL_ASSERT(offsetOfBase < size);
        // 2) offset of put pointer and base pointer is calculated and asserted to point in range.
        auto offsetOfPtr = std::distance(this->base(), static_cast<const char_type*>(this->pptr()));
        ETL_ASSERT(offsetOfPtr >= 0);
        ETL_ASSERT((offsetOfBase + offsetOfPtr) < size);
        return offsetOfBase + offsetOfPtr;
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
