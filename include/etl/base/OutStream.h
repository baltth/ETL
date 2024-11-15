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

#ifndef ETL_DETAIL_OUTSTREAM_H_
#define ETL_DETAIL_OUTSTREAM_H_

#include <etl/base/AMemStreamBuf.h>
#include <etl/etlSupport.h>

#include <ostream>

namespace ETL_NAMESPACE {
namespace Detail {

template<typename CharType>
using BasicOutStream = std::basic_ostream<CharType, std::char_traits<CharType>>;


template<typename CharType>
class OutStream : public BasicOutStream<CharType> {

  public:  // types

    using Base = BasicOutStream<CharType>;

    using char_type = typename Base::char_type;
    using traits_type = typename Base::traits_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;

  public:  // functions

    explicit OutStream(AMemStreamBuf<CharType>& sb) :
        Base {&sb.streambuf()} {}

    OutStream() = delete;
    OutStream(const OutStream& other) = delete;
    OutStream& operator=(const OutStream& other) & = delete;

    OutStream(OutStream&& other) :
        Base {std::move(other)} {}

    OutStream& operator=(OutStream&& other) & {
        Base::operator=(std::move(other));
    }

    ~OutStream() = default;
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_DETAIL_OUTSTREAM_H_
