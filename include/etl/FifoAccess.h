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

#ifndef __ETL_FIFOACCESS_H__
#define __ETL_FIFOACCESS_H__

#include <etl/Span.h>
#include <etl/base/AFifoIterator.h>
#include <etl/base/FifoIndexing.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


template<class T>
class FifoAccess {

  public:  // types

    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

    class iterator : public Detail::FifoIterator<value_type> {
        friend class FifoAccess<value_type>;

      private:

        iterator(const FifoAccess<value_type>* fifo, uint32_t ix) :
            Detail::FifoIterator<value_type>(const_cast<pointer>(fifo->data()),
                                             fifo->indexing,
                                             ix) {};
    };

  private:  // variables

    Detail::FifoIndexing indexing;
    Span<T> span;

  public:  // functions

    explicit FifoAccess(Span<T> s) :
        indexing(s.size()),
        span(s) {};

    template<class C>
    explicit FifoAccess(C& container) :
        indexing(container.size()),
        span(container) {};

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

  protected:

    void* data() const {
        return span.data();
    }

  private:

    FifoAccess<T>();
};


template<class T>
void FifoAccess<T>::push(const_reference item) {

    indexing.push();
    span[indexing.getWriteIx()] = item;
}


template<class T>
typename FifoAccess<T>::value_type FifoAccess<T>::pop() {

    indexing.pop();
    return span[indexing.getReadIx()];
}


template<class T>
typename FifoAccess<T>::value_type FifoAccess<T>::getFromBack(uint32_t ix) const {

    return span[indexing.getIndexFromFront(ix)];
}


template<class T>
typename FifoAccess<T>::value_type FifoAccess<T>::getFromFront(uint32_t ix) const {

    return span[indexing.getIndexFromBack(ix)];
}


template<class T>
typename FifoAccess<T>::reference FifoAccess<T>::operator[](int32_t ix) {

    if (ix < 0) {
        ix = -1 - ix;
        ix = indexing.getIndexFromBack(ix);
    } else {
        ix = indexing.getIndexFromFront(ix);
    }

    return span[ix];
}


template<class T>
typename FifoAccess<T>::const_reference FifoAccess<T>::operator[](int32_t ix) const {

    if (ix < 0) {
        ix = -1 - ix;
        ix = indexing.getIndexFromBack(ix);
    } else {
        ix = indexing.getIndexFromFront(ix);
    }

    return span[ix];
}

}  // namespace ETL_NAMESPACE

#endif  // __ETL_FIFOACCESS_H__
