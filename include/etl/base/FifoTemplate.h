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

#include <etl/etlSupport.h>
#include <etl/base/FifoIndexing.h>
#include <etl/base/AFifoIterator.h>

namespace ETL_NAMESPACE {


template<class C>
class FifoTemplate : protected C, public FifoIndexing {

  public:   // types

    typedef typename C::ValueType ValueType;
    typedef typename C::Reference Reference;
    typedef typename C::ConstReference ConstReference;
    typedef typename C::Pointer Pointer;
    typedef typename C::ConstPointer ConstPointer;

    class Iterator : public FifoIterator<ValueType> {
        friend class FifoTemplate<C>;

      private:

        Iterator(const FifoTemplate<C>* fifo, uint32_t ix) :
            FifoIterator<ValueType>(const_cast<Pointer>(fifo->getData()), fifo, ix) {};

    };

  public:   // functions

#if ETL_USE_CPP11

    template<typename... Args>
    explicit FifoTemplate<C>(Args... args) :
        C(args...),
        FifoIndexing(C::getSize()) {};

#else

    FifoTemplate<C>() :
        C(),
        FifoIndexing(C::getSize()) {};

    FifoTemplate<C>(uint32_t len) :
        C(len),
        FifoIndexing(C::getSize()) {};

#endif

    uint32_t getCapacity() const {
        return FifoIndexing::getCapacity();
    }

    void push(ConstReference item);
    ValueType pop();

    void drop() {
        FifoIndexing::pop();
    }

    ValueType getFromBack(uint32_t ix) const;
    ValueType getFromFront(uint32_t ix) const;

    Reference operator[](int32_t ix);
    ConstReference operator[](int32_t ix) const;

    Iterator begin() const {
        return iteratorFor(0);
    }

    Iterator end() const {
        return iteratorFor(getLength());
    }

    Iterator iteratorFor(uint32_t ix) const {
        return Iterator(this, ix);
    }

};


template<class C>
void FifoTemplate<C>::push(ConstReference item) {

    FifoIndexing::push();
    C::operator[](FifoIndexing::getWriteIx()) = item;
}


template<class C>
typename FifoTemplate<C>::ValueType FifoTemplate<C>::pop() {

    FifoIndexing::pop();
    return C::operator[](FifoIndexing::getReadIx());
}


template<class C>
typename FifoTemplate<C>::ValueType FifoTemplate<C>::getFromBack(uint32_t ix) const {

    return C::operator[](FifoIndexing::getIndexFromFront(ix));
}


template<class C>
typename FifoTemplate<C>::ValueType FifoTemplate<C>::getFromFront(uint32_t ix) const {

    return C::operator[](FifoIndexing::getIndexFromBack(ix));
}


template<class C>
typename FifoTemplate<C>::Reference FifoTemplate<C>::operator[](int32_t ix) {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return C::operator[](ix);
}


template<class C>
typename FifoTemplate<C>::ConstReference FifoTemplate<C>::operator[](int32_t ix) const {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return C::operator[](ix);
}


}

#endif /* __ETL_FIFOTEMPLATE_H__ */

