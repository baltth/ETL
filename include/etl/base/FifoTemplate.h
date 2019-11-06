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
class FifoTemplate : protected C {

  public:  // types

    typedef typename C::value_type value_type;
    typedef typename C::reference reference;
    typedef typename C::const_reference const_reference;
    typedef typename C::pointer pointer;
    typedef typename C::const_pointer const_pointer;

    class iterator : public Detail::FifoIterator<value_type> {
        friend class FifoTemplate<C>;

      private:

        iterator(const FifoTemplate<C>* fifo, uint32_t ix) :
            Detail::FifoIterator<value_type>(const_cast<pointer>(fifo->data()),
                                             fifo->indexing,
                                             ix) {};
    };

  protected:  // variables

    Detail::FifoIndexing indexing;

  public:  // functions

    template<typename... Args>
    explicit FifoTemplate<C>(Args... args) :
        C(args...),
        indexing(C::size()) {};

    bool empty() const {
        return indexing.empty();
    }

    uint32_t size() const {
        return indexing.size();
    }

    uint32_t capacity() const {
        return indexing.capacity();
    }

    void clear() {
        indexing.clear();
    }

    void resize(uint32_t s) {
        indexing.setSize(s);
    }

    void push(const_reference item);
    value_type pop();

    void drop() {
        indexing.pop();
    }

    value_type getFromBack(uint32_t ix) const;
    value_type getFromFront(uint32_t ix) const;

    reference operator[](int32_t ix);
    const_reference operator[](int32_t ix) const;

    iterator begin() const {
        return iteratorFor(0);
    }

    iterator end() const {
        return iteratorFor(indexing.size());
    }

    iterator iteratorFor(uint32_t ix) const {
        return iterator(this, ix);
    }
};


template<class C>
void FifoTemplate<C>::push(const_reference item) {

    indexing.push();
    C::operator[](indexing.getWriteIx()) = item;
}


template<class C>
typename FifoTemplate<C>::value_type FifoTemplate<C>::pop() {

    indexing.pop();
    return C::operator[](indexing.getReadIx());
}


template<class C>
typename FifoTemplate<C>::value_type FifoTemplate<C>::getFromBack(uint32_t ix) const {

    return C::operator[](indexing.getIndexFromFront(ix));
}


template<class C>
typename FifoTemplate<C>::value_type FifoTemplate<C>::getFromFront(uint32_t ix) const {

    return C::operator[](indexing.getIndexFromBack(ix));
}


template<class C>
typename FifoTemplate<C>::reference FifoTemplate<C>::operator[](int32_t ix) {

    if (ix < 0) {
        ix = -1 - ix;
        ix = indexing.getIndexFromBack(ix);
    } else {
        ix = indexing.getIndexFromFront(ix);
    }

    return C::operator[](ix);
}


template<class C>
typename FifoTemplate<C>::const_reference FifoTemplate<C>::operator[](int32_t ix) const {

    if (ix < 0) {
        ix = -1 - ix;
        ix = indexing.getIndexFromBack(ix);
    } else {
        ix = indexing.getIndexFromFront(ix);
    }

    return C::operator[](ix);
}


}  // namespace ETL_NAMESPACE

#endif  // __ETL_FIFOTEMPLATE_H__
