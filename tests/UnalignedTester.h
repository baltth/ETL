/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Balazs Toth.

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

#ifndef __ETL_TEST_UNALIGNEDTESTER_H__
#define __ETL_TEST_UNALIGNEDTESTER_H__

#include <stdint.h>

namespace Etl {
namespace Test {


class __attribute__((packed)) UnalignedTester {

  public:  // variables

    uint32_t a;
    uint8_t b;
};

}  // namespace Test
}  // namespace Etl

#endif  // __ETL_TEST_UNALIGNEDTESTER_H__
