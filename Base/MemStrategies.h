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
template<class C>
class SampleStrategy : public C {

// types
public:

    typedef C::ItemType ItemType;

// functions
public:

    SampleStrategy(void* initData, uint32_t initCapacity);
    void reserve(uint32_t length);
    void reserveAtLeast(uint32_t length);
    void shrinkToFit();
    void resize(uint32_t newLength);

};
*/


/** Mem strategy with static size, allocated externally */
template<class C>
class StaticSized : public C {

// types:
public:

    typedef typename C::ItemType ItemType;

// variables
private:

    void* const data;
    const uint32_t capacity;

// functions
public:

    // No StaticSized();

    StaticSized(void* initData, uint32_t initCapacity) :
        data(initData),
        capacity(initCapacity) {};

    ~StaticSized() {
        C::clear();
    }

    void reserve(uint32_t length) {
        setupData(length);
    }

    void reserveAtLeast(uint32_t length) {
        setupData(length);
    }

    void shrinkToFit() {
        setupData(capacity);
    }

    void resize(uint32_t newLength) {
        setupData(newLength);
    }

private:

    void setupData(uint32_t length);

};


template<class C>
void StaticSized<C>::setupData(uint32_t length) {

    if(length <= capacity) {
        C::proxy.setData(data);
        C::proxy.setCapacity(capacity);
    } else {
        // TODO throw;
    }
}



/** Mem strategy with dynamic size, allocated with Allocator */
template<class C, class A>
class DynamicSized : public C {

// types
public:

    typedef A Allocator;

    static const uint32_t RESIZE_STEP = 8;

// functions
public:

    // No DynamicSized(void* initData, uint32_t initCapacity);
    
    ~DynamicSized() {
        C::clear();
        deallocate();
    }

    void reserve(uint32_t length);

    void reserveAtLeast(uint32_t length) {
        reserve(getRoundedLength(length));
    }

    void shrinkToFit();
    void resize(uint32_t newLength);

private:

    void reallocateAndCopyFor(uint32_t len);

    void allocate(uint32_t len);

    void deallocate() {
        A::deallocate(C::getData());
    }

    static uint32_t getRoundedLength(uint32_t length) {
        return (length + (RESIZE_STEP - 1)) & ~(RESIZE_STEP - 1);
    }

};


template<class C, class A>
void DynamicSized<C, A>::reserve(uint32_t length) {

    if(length > C::getCapacity()) {
        reallocateAndCopyFor(length);
    }
}


template<class C, class A>
void DynamicSized<C, A>::shrinkToFit() {

    if(C::getCapacity() > C::getSize()) {
        reallocateAndCopyFor(C::getSize());
    }
}


template<class C, class A>
void DynamicSized<C, A>::resize(uint32_t newLength) {

    if(newLength > C::getSize()) {

        if(newLength > C::getCapacity()) {
            reallocateAndCopyFor(getRoundedLength(newLength));
        }

        typename C::Iterator newEnd = C::getData() + newLength;

        for(typename C::Iterator it = C::end(); it < newEnd; ++it) {
            C::placeDefaultTo(it);
        }

    } else if(newLength < C::getSize()) {

        typename C::Iterator newEnd = C::getData() + newLength;
        C::destruct(newEnd, C::end());
    }

    C::setSize(newLength);
}


template<class C, class A>
void DynamicSized<C, A>::reallocateAndCopyFor(uint32_t len) {

    typename C::ItemType* oldData = C::getData();

    allocate(len);

    if(oldData != NULLPTR) {

        uint32_t numToCopy = (len < C::getSize()) ? len : C::getSize();

        if((C::getData() != NULLPTR) && (numToCopy > 0)) {

            typename C::ItemType* dataAlias = C::getData();
            C::uninitializedCopy(oldData, dataAlias, numToCopy);
        }

        A::deallocate(oldData);
    }
}


template<class C, class A>
void DynamicSized<C, A>::allocate(uint32_t len) {

    if(len > 0) {
        C::proxy.setData(A::allocate(len * C::proxy.getItemSize()));
    } else {
        C::proxy.setData(NULLPTR);
    }

    if(C::getData() != NULLPTR) {
        C::proxy.setCapacity(len);
    } else {
        C::proxy.setCapacity(0);
    }
}


/** Helper for HeapUser<> */
class HeapAllocator {

// functions
public:

    static inline void* allocate(size_t size) {
        return operator new(size);
    }

    static inline void deallocate(void* ptr) {
        operator delete(ptr);
    }

};


/** Heap allocated mem strategy */
template<class C>
class HeapUser : public DynamicSized<C, HeapAllocator> {};

}


#endif /* __ETL_MEMSTARTEGIES_H__ */

