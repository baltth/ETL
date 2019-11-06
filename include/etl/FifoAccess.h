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

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    class iterator : public Detail::FifoIterator<value_type> {
        friend class FifoAccess<value_type>;

      private:

        iterator(FifoAccess<value_type>& fifo, uint32_t ix) :
            Detail::FifoIterator<value_type>(fifo.data(), fifo.indexing, ix) {};
    };

    class const_iterator : public Detail::FifoIterator<const value_type> {
        friend class FifoAccess<value_type>;

      private:

        const_iterator(const FifoAccess<value_type>& fifo, uint32_t ix) :
            Detail::FifoIterator<const value_type>(fifo.data(), fifo.indexing, ix) {};
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
    void push(value_type&& item);
    value_type pop();
    void drop();

    value_type getFromBack(uint32_t ix) const;
    value_type getFromFront(uint32_t ix) const;

    reference operator[](int32_t ix);
    const_reference operator[](int32_t ix) const;

    iterator begin() {
        return iteratorFor(0);
    }

    const_iterator cbegin() {
        return constIteratorFor(0);
    }

    const_iterator begin() const {
        return constIteratorFor(0);
    }

    iterator end() {
        return iteratorFor(indexing.size());
    }

    const_iterator cend() {
        return constIteratorFor(indexing.size());
    }

    const_iterator end() const {
        return constIteratorFor(indexing.size());
    }

    iterator iteratorFor(uint32_t ix) {
        return iterator(*this, ix);
    }

    const_iterator constIteratorFor(uint32_t ix) const {
        return const_iterator(*this, ix);
    }

  protected:

    FifoAccess<T>() :
        indexing(0) {};

    template<class C>
    void setupFor(C& container) {
        span = Span<T>(container);
        indexing = Detail::FifoIndexing(container.size());
    }

    pointer data() {
        return span.data();
    }

    const_pointer data() const {
        return span.data();
    }
};


template<class T>
void FifoAccess<T>::push(const_reference item) {

    indexing.push();
    span[indexing.getWriteIx()] = item;
}


template<class T>
void FifoAccess<T>::push(value_type&& item) {

    indexing.push();
    span[indexing.getWriteIx()] = std::move(item);
}


template<class T>
typename FifoAccess<T>::value_type FifoAccess<T>::pop() {

    indexing.pop();
    return value_type {std::move(span[indexing.getReadIx()])};
}


template<class T>
void FifoAccess<T>::drop() {

    indexing.pop();
    span[indexing.getReadIx()] = value_type {};
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
