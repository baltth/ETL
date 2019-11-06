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

#ifndef __ETL_FIFOTEMPLATE_H__
#define __ETL_FIFOTEMPLATE_H__

#include <etl/FifoAccess.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


template<class C>
class FifoTemplate : public FifoAccess<typename C::value_type> {

  public:  // types

    using value_type = typename C::value_type;
    using reference = typename C::reference;
    using const_reference = typename C::const_reference;
    using pointer = typename C::pointer;
    using const_pointer = typename C::const_pointer;

    using Base = FifoAccess<value_type>;

  protected:  // variables

    C cont;

  public:  // functions

    template<typename... Args>
    explicit FifoTemplate<C>(Args... args) :
        Base(),
        cont(args...) {
        Base::setupFor(cont);
    };

  protected:

    void setupFifo(uint32_t len) {

        cont.resize(len);
        Base::setupFor(cont);
    }
};


}  // namespace ETL_NAMESPACE

#endif  // __ETL_FIFOTEMPLATE_H__
