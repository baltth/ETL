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

#include <etl/base/AFifoIterator.h>
#include <etl/base/FifoIndexing.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


template<class C>
class FifoTemplate : protected C, public FifoIndexing {

  public:  // types

    typedef typename C::value_type value_type;
    typedef typename C::reference reference;
    typedef typename C::const_reference const_reference;
    typedef typename C::pointer pointer;
    typedef typename C::const_pointer const_pointer;

    class iterator : public FifoIterator<value_type> {
        friend class FifoTemplate<C>;

      private:

        iterator(const FifoTemplate<C>* fifo, uint32_t ix) :
            FifoIterator<value_type>(const_cast<pointer>(fifo->data()), fifo, ix) {};
    };

  public:  // functions

    template<typename... Args>
    explicit FifoTemplate<C>(Args... args) :
        C(args...),
        FifoIndexing(C::size()) {};

    bool empty() const {
        return FifoIndexing::empty();
    }

    uint32_t capacity() const {
        return FifoIndexing::capacity();
    }

    void push(const_reference item);
    value_type pop();

    void drop() {
        FifoIndexing::pop();
    }

    value_type getFromBack(uint32_t ix) const;
    value_type getFromFront(uint32_t ix) const;

    reference operator[](int32_t ix);
    const_reference operator[](int32_t ix) const;

    iterator begin() const {
        return iteratorFor(0);
    }

    iterator end() const {
        return iteratorFor(getLength());
    }

    iterator iteratorFor(uint32_t ix) const {
        return iterator(this, ix);
    }
};


template<class C>
void FifoTemplate<C>::push(const_reference item) {

    FifoIndexing::push();
    C::operator[](FifoIndexing::getWriteIx()) = item;
}


template<class C>
typename FifoTemplate<C>::value_type FifoTemplate<C>::pop() {

    FifoIndexing::pop();
    return C::operator[](FifoIndexing::getReadIx());
}


template<class C>
typename FifoTemplate<C>::value_type FifoTemplate<C>::getFromBack(uint32_t ix) const {

    return C::operator[](FifoIndexing::getIndexFromFront(ix));
}


template<class C>
typename FifoTemplate<C>::value_type FifoTemplate<C>::getFromFront(uint32_t ix) const {

    return C::operator[](FifoIndexing::getIndexFromBack(ix));
}


template<class C>
typename FifoTemplate<C>::reference FifoTemplate<C>::operator[](int32_t ix) {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return C::operator[](ix);
}


template<class C>
typename FifoTemplate<C>::const_reference FifoTemplate<C>::operator[](int32_t ix) const {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return C::operator[](ix);
}


}  // namespace ETL_NAMESPACE

#endif  // __ETL_FIFOTEMPLATE_H__
