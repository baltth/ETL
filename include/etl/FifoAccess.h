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

#include <etl/etlSupport.h>
#include <etl/Proxy.h>
#include <etl/base/FifoIndexing.h>
#include <etl/base/AFifoIterator.h>

namespace ETL_NAMESPACE {


template<class T>
class FifoAccess : public FifoIndexing {

  public:   // types

    typedef T ValueType;
    typedef T& Reference;
    typedef const T& ConstReference;
    typedef T* Pointer;
    typedef const T* ConstPointer;

    class Iterator : public FifoIterator<ValueType> {
        friend class FifoAccess<ValueType>;

      private:

        Iterator(const FifoAccess<ValueType>* fifo, uint32_t ix) :
            FifoIterator<ValueType>(const_cast<Pointer>(fifo->getData()), fifo, ix) {};

    };

  private:  // variables

    MutableProxy<T> proxy;

  public:   // functions

    explicit FifoAccess(const MutableProxy<T>& p) :
        FifoIndexing(p.getSize()),
        proxy(p) {};

    template<class C>
    explicit FifoAccess(C& container) :
        FifoIndexing(container.getSize()),
        proxy(container) {};

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

  protected:

    void* getData() const {
        return proxy.getData();
    }

  private:

    FifoAccess<T>();

};


template<class T>
void FifoAccess<T>::push(ConstReference item) {

    FifoIndexing::push();
    proxy.operator[](FifoIndexing::getWriteIx()) = item;
}


template<class T>
typename FifoAccess<T>::ValueType FifoAccess<T>::pop() {

    FifoIndexing::pop();
    return proxy.operator[](FifoIndexing::getReadIx());
}


template<class T>
typename FifoAccess<T>::ValueType FifoAccess<T>::getFromBack(uint32_t ix) const {

    return proxy.operator[](FifoIndexing::getIndexFromFront(ix));
}


template<class T>
typename FifoAccess<T>::ValueType FifoAccess<T>::getFromFront(uint32_t ix) const {

    return proxy.operator[](FifoIndexing::getIndexFromBack(ix));
}


template<class T>
typename FifoAccess<T>::Reference FifoAccess<T>::operator[](int32_t ix) {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return proxy.operator[](ix);
}


template<class T>
typename FifoAccess<T>::ConstReference FifoAccess<T>::operator[](int32_t ix) const {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return proxy.operator[](ix);
}

}

#endif /* __ETL_FIFOACCESS_H__ */

