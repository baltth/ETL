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

#include <ETL/etlSupport.h>
#include <ETL/base/FifoIndexing.h>
#include <ETL/base/AFifoIterator.h>

namespace ETL_NAMESPACE {


template<class C>
class FifoTemplate : protected C, public FifoIndexing {

  public:   // types

    typedef typename C::ItemType ItemType;

    class Iterator : public FifoIterator<ItemType> {
        friend class FifoTemplate<C>;

      private:

        Iterator(const FifoTemplate<C>* fifo, uint32_t ix) :
            FifoIterator<ItemType>(const_cast<ItemType*>(fifo->getData()), fifo, ix) {};

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

    void push(const ItemType& item);
    ItemType pop();

    void drop() {
        FifoIndexing::pop();
    }

    ItemType getFromBack(uint32_t ix) const;
    ItemType getFromFront(uint32_t ix) const;

    ItemType& operator[](int32_t ix);
    const ItemType& operator[](int32_t ix) const;

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
void FifoTemplate<C>::push(const ItemType& item) {

    FifoIndexing::push();
    C::operator[](FifoIndexing::getWriteIx()) = item;
}


template<class C>
typename FifoTemplate<C>::ItemType FifoTemplate<C>::pop() {

    FifoIndexing::pop();
    return C::operator[](FifoIndexing::getReadIx());
}


template<class C>
typename FifoTemplate<C>::ItemType FifoTemplate<C>::getFromBack(uint32_t ix) const {

    return C::operator[](FifoIndexing::getIndexFromFront(ix));
}


template<class C>
typename FifoTemplate<C>::ItemType FifoTemplate<C>::getFromFront(uint32_t ix) const {

    return C::operator[](FifoIndexing::getIndexFromBack(ix));
}


template<class C>
typename FifoTemplate<C>::ItemType& FifoTemplate<C>::operator[](int32_t ix) {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return C::operator[](ix);
}


template<class C>
const typename FifoTemplate<C>::ItemType& FifoTemplate<C>::operator[](int32_t ix) const {

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

