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

#include <ETL/etlSupport.h>
#include <ETL/base/FifoTemplate.h>
#include <ETL/Array.h>

namespace ETL_NAMESPACE {


template<class C>
class Fifo : public FifoTemplate<C> {

  public:   // types

    typedef typename FifoTemplate<C>::ItemType ItemType;
    typedef typename FifoTemplate<C>::Iterator Iterator;

  public:   // functions

#if ETL_USE_CPP11

    template<typename... Args>
    explicit Fifo<C>(Args&& ... args) :
        FifoTemplate<C>(args...) {};

#else

    Fifo<C>() :
        FifoTemplate<C>() {};

    Fifo<C>(uint32_t len) :
        FifoTemplate<C>(len) {};

#endif

    void setupFifo(uint32_t len);

};


template<class C>
void Fifo<C>::setupFifo(uint32_t len) {

    C::resize(len);
    FifoIndexing::setCapacity(C::getSize());
    FifoIndexing::resetIndexes();
}


template<>
template<typename T, uint32_t N>
class Fifo<Array<T, N> > : public FifoTemplate<Array<T, N> > {

  public:   // types

    typedef typename FifoTemplate<Array<T, N> >::ItemType ItemType;
    typedef typename FifoTemplate<Array<T, N> >::Iterator Iterator;

  public:   // functions

#if ETL_USE_CPP11

    template<typename... Args>
    explicit Fifo<Array<T, N> >(Args&& ... args) :
        FifoTemplate<Array<T, N> >(args...) {};

#else

    Fifo<Array<T, N> >() :
        FifoTemplate<Array<T, N> >() {};

#endif

    void setupFifo(uint32_t len) {};

};

}

#endif /* __ETL_FIFO_H__ */

