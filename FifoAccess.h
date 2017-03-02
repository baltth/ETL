/** \file
\copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.

\author Balazs Toth \<balazs.toth@commsignia.com\>
\date 2017

\internal Forked from http://github.com/baltth/ETL.git
*/

#ifndef __ETL_FIFOACCESS_H__
#define __ETL_FIFOACCESS_H__

#include "etlSupport.h"

#include "ContainerProxy.h"
#include "Base/FifoIndexing.h"
#include "Base/AFifoIterator.h"

namespace ETL_NAMESPACE {


template<class T>
class FifoAccess : public FifoIndexing {

  public:   // types

    typedef T ItemType;

    class Iterator : public FifoIterator<ItemType> {
        friend class FifoAccess<ItemType>;

      private:

        Iterator(const FifoAccess<ItemType>* fifo, uint32_t ix) :
            FifoIterator<ItemType>(const_cast<ItemType*>(fifo->getData()), fifo, ix) {};

    };

  private:  // variables

    TypedContainerProxy<T> proxy;

  public:   // functions


    explicit FifoAccess(const TypedContainerProxy<T>& p) :
        FifoIndexing(p.getSize()),
        proxy(p) {};

    template<class C>
    explicit FifoAccess(C& container) :
        FifoIndexing(container.getSize()),
        proxy(container) {};

    uint32_t getCapacity() const {
        return FifoIndexing::getCapacity();
    }

    void push(const ItemType& item);
    ItemType pop();

    ItemType getFromBack(uint32_t ix) const;
    ItemType getFromFront(uint32_t ix) const;

    /*void setLength(uint32_t len) {
    FifoIndexing::setLength(len);
    }*/

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

  protected:

    void* getData() const {
        return proxy.getData();
    }

  private:

    FifoAccess<T>();

};


template<class T>
void FifoAccess<T>::push(const ItemType& item) {

    FifoIndexing::push();
    proxy.operator[](FifoIndexing::getWriteIx()) = item;
}


template<class T>
typename FifoAccess<T>::ItemType FifoAccess<T>::pop() {

    FifoIndexing::pop();
    return proxy.operator[](FifoIndexing::getReadIx());
}


template<class T>
typename FifoAccess<T>::ItemType FifoAccess<T>::getFromBack(uint32_t ix) const {

    return proxy.operator[](FifoIndexing::getIndexFromFront(ix));
}


template<class T>
typename FifoAccess<T>::ItemType FifoAccess<T>::getFromFront(uint32_t ix) const {

    return proxy.operator[](FifoIndexing::getIndexFromBack(ix));
}


template<class T>
typename FifoAccess<T>::ItemType& FifoAccess<T>::operator[](int32_t ix) {

    if (ix < 0) {
        ix = -1 - ix;
        ix = FifoIndexing::getIndexFromBack(ix);
    } else {
        ix = FifoIndexing::getIndexFromFront(ix);
    }

    return proxy.operator[](ix);
}


template<class T>
const typename FifoAccess<T>::ItemType& FifoAccess<T>::operator[](int32_t ix) const {

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

