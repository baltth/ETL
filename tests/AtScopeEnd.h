/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2022-2024 Balazs Toth.

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

#ifndef ETL_TEST_ATSCOPEEND_H_
#define ETL_TEST_ATSCOPEEND_H_

#include <functional>

namespace Etl {
namespace Test {

class AtScopeEnd {

  private:  // variables

    std::function<void()> func;

  public:  // functions

    template<typename F>
    explicit AtScopeEnd(F f) :
        func {f} {}

    ~AtScopeEnd() {
        func();
    }
};


}  // namespace Test
}  // namespace Etl

#endif  // ETL_TEST_ATSCOPEEND_H_
