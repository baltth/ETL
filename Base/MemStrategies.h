/**
\file
\date 2016.01.20. 19:44:12
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

#ifndef __ETL_MEMSTARTEGIES_H__
#define __ETL_MEMSTARTEGIES_H__

#undef min
#undef max

#include "langSupport.h"

#include "Base/AVectorBase.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {

/*
template<class T>
class SampleResizeStrategy {

// types
public:

    typedef T::ItemType ItemType;

// functions
protected:

    void reserve(uint32_t length);
    void reserveAtLeast(uint32_t length);
    void shrinkToFit();
    void resize(uint32_t newLength);
    void clear();

};
*/

template<class T>
class DynamicAllocator {

// functions
public:

    static inline void* allocate(uint32_t len) {
        return operator new(len * sizeof(T));
    }

    static inline void deallocate(void* ptr) {
        operator delete(ptr);
    }

};


template<class T, class A = DynamicAllocator<T> >
class DynamicResizeStrategy {

// types
public:

    typedef A Allocator;

    static const uint32_t RESIZE_STEP = 8;

// functions
public:

    ~DynamicResizeStrategy() {
       deallocate(); 
    }

    void reserve(uint32_t length);
    void reserveAtLeast(uint32_t length);
    void shrinkToFit();
    void resize(uint32_t newLength);
    void clear();

private:

    void reallocateAndCopyFor(uint32_t len);

    void allocate(uint32_t len);

    void deallocate() {
        A::deallocate(static_cast<T*>(this)->getData());
    }
    
    static uint32_t getRoundedLength(uint32_t length) {
        return (length + (RESIZE_STEP - 1)) & ~(RESIZE_STEP - 1);
    }

};


template<class T, class A>
void DynamicResizeStrategy<T, A>::reserve(uint32_t length) {

    T* alias = static_cast<T*>(this);
    
    if(length > alias->getCapacity()) {
        reallocateAndCopyFor(length);
    }
}


template<class T, class A>
void DynamicResizeStrategy<T, A>::reserveAtLeast(uint32_t length) {

    reserve(getRoundedLength(length));
}


template<class T, class A>
void DynamicResizeStrategy<T, A>::shrinkToFit() {

    T* alias = static_cast<T*>(this);
    
    if(alias->getCapacity() > alias->getSize()) {
        reallocateAndCopyFor(alias->getSize());
    }
}


template<class T, class A>
void DynamicResizeStrategy<T, A>::resize(uint32_t newLength) {

    T* alias = static_cast<T*>(this);
    
    if(newLength > alias->getSize()) {

        if(newLength > alias->getCapacity()) {
            reallocateAndCopyFor(getRoundedLength(newLength));
        }

        typename T::Iterator newEnd = alias->getData() + newLength;

        for(typename T::Iterator it = alias->end(); it < newEnd; ++it) {
            alias->placeDefaultTo(it);
        }

    } else if(newLength < alias->getSize()) {

        typename T::Iterator newEnd = alias->getData() + newLength;
        alias->destruct(newEnd, alias->end());
    }

    alias->numElements = newLength;
}


template<class T, class A>
void DynamicResizeStrategy<T, A>::reallocateAndCopyFor(uint32_t len) {

    T* alias = static_cast<T*>(this);
    typename T::ItemType* oldData = alias->getData();
    
    allocate(len);

    if(oldData != NULLPTR) {

        uint32_t numToCopy = (len < alias->numElements) ? len : alias->numElements;

        if((alias->getData() != NULLPTR) && (numToCopy > 0)) {

            typename T::ItemType* dataAlias = alias->getData();
            alias->uninitializedCopy(oldData, dataAlias, numToCopy);
        }

        A::deallocate(oldData);
    }
}


template<class T, class A>
void DynamicResizeStrategy<T, A>::allocate(uint32_t len) {

    T* alias = static_cast<T*>(this);
    
    if(len > 0) {
        alias->proxy.setData(A::allocate(len * alias->proxy.getItemSize()));
    } else {
        alias->proxy.setData(NULLPTR);
    }

    if(alias->proxy.getData() != NULLPTR) {
        alias->proxy.setCapacity(len);
    } else {
        alias->proxy.setCapacity(0);
    }
}

}

#endif /* __ETL_MEMSTARTEGIES_H__ */

