/**
\file
\date 2016.01.20. 20:27:49
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

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

#include "Base/FifoIndexing.h"
#include "Base/AFifoIterator.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<class T>
class FifoTemplate : protected T, protected FifoIndexing {

// types
public:

    typedef typename T::ItemType ItemType;

    class Iterator : public AFifoIterator<ItemType> {
        friend class FifoTemplate<T>;

    private:

        Iterator(const FifoTemplate<T>* fifoArray, uint32_t ix) :
            AFifoIterator<ItemType>(const_cast<ItemType*>(fifoArray->getData()), fifoArray, ix) {};

    };

// functions
public:

    template<typename... Args>
    explicit FifoTemplate(Args... args) :
        T(args...),
        FifoIndexing(T::getSize(), 0) {};

    void createFifo(uint32_t len);

    uint32_t getCapacity() const {
        return T::getCapacity();
    }

    void push(const ItemType &item);
    ItemType pop();

    ItemType getFromBack(uint32_t ix) const;
    ItemType getFromFront(uint32_t ix) const;

    uint32_t getLength() const {
        return numItems;
    }

    void setLength(uint32_t len);

    inline void setEmpty() {
        setLength(0);
    }

    ItemType &operator[](int32_t ix);
    const ItemType &operator[](int32_t ix) const;

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


template<class T>
void FifoTemplate<T>::push(const ItemType &item) {

    FifoIndexing::push();
    T::operator[](FifoIndexing::getWriteIx()) = item;
}


template<class T>
typename FifoTemplate<T>::ItemType FifoTemplate<T>::pop() {

    FifoIndexing::pop();
    return T::operator[](FifoIndexing::getReadIx());
}


template<class T>
typename FifoTemplate<T>::ItemType FifoTemplate<T>::getFromBack(uint32_t ix) const {

    return T::operator[](FifoIndexing::getIndexFromFront(ix));
}


template<class T>
typename FifoTemplate<T>::ItemType FifoTemplate<T>::getFromFront(uint32_t ix) const {

    return T::operator[](FifoIndexing::getIndexFromBack(ix));
}


template<class T>
typename FifoTemplate<T>::ItemType &FifoTemplate<T>::operator[](int32_t ix) {

    if(ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return T::operator[](ix);
}


template<class T>
const typename FifoTemplate<T>::ItemType &FifoTemplate<T>::operator[](int32_t ix) const {

    if(ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return T::operator[](ix);
}


template<class T>
void FifoTemplate<T>::setLength(uint32_t len) {

    if(len >= FifoIndexing::getCapacity()) {
        len = FifoIndexing::getCapacity() - 1;
    }

    if(len > writeIx) {
        readIx = writeIx + FifoIndexing::getCapacity();
    } else {
        readIx = writeIx;
    }

    readIx -= len;
    numItems = len;
}

}

#endif /* __ETL_FIFOTEMPLATE_H__ */
