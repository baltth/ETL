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

#ifndef __ETL_FIFO_H__
#define __ETL_FIFO_H__

#include <etl/Array.h>
#include <etl/base/FifoTemplate.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


template<class C>
class Fifo : public FifoTemplate<C> {

  public:  // functions

    template<typename... Args>
    explicit Fifo<C>(Args&&... args) :
        FifoTemplate<C>(args...) {};

    void setupFifo(uint32_t len);
};


template<class C>
void Fifo<C>::setupFifo(uint32_t len) {

    C::resize(len);
    FifoIndexing::setCapacity(C::size());
    FifoIndexing::resetIndexes();
}


template<typename T, uint32_t N>
class Fifo<Array<T, N>> : public FifoTemplate<Array<T, N>> {

  public:  // functions

    template<typename... Args>
    explicit Fifo<Array<T, N>>(Args&&... args) :
        FifoTemplate<Array<T, N>>(args...) {};

    void setupFifo(uint32_t len) {};
};

}  // namespace ETL_NAMESPACE

#endif  // __ETL_FIFO_H__
