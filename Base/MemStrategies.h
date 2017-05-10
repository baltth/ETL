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

#include "etlSupport.h"

#include "Base/AVectorBase.h"

#include <memory>

namespace ETL_NAMESPACE {

class AMemStrategy {

  public:   // functions

    virtual void reserve(uint32_t length) = 0;
    virtual void reserveAtLeast(uint32_t length) = 0;
    virtual void shrinkToFit() = 0;
    virtual void resize(uint32_t newLength) = 0;

};



/** Mem strategy with static size, allocated externally */
template<class C>
class StaticSized : public AMemStrategy {

  private:  // variables

    void* const data;
    const uint32_t capacity;
    C& container;

  public:   // functions

    // No StaticSized();

    StaticSized(void* initData, uint32_t initCapacity, C& cont) :
        data(initData),
        capacity(initCapacity),
        container(cont) {};

    ~StaticSized() {
        container.clear();
    }

    virtual void reserve(uint32_t length) FINAL OVERRIDE {
        setupData(length);
    }

    virtual void reserveAtLeast(uint32_t length) FINAL OVERRIDE {
        setupData(length);
    }

    virtual void shrinkToFit() FINAL OVERRIDE {
        setupData(capacity);
    }

    virtual void resize(uint32_t newLength) FINAL OVERRIDE {
        setupData(newLength);
    }

  private:

    void setupData(uint32_t length);

};


template<class C>
void StaticSized<C>::setupData(uint32_t length) {

    if (length <= capacity) {
        container.proxy.setData(data);
        container.proxy.setCapacity(capacity);
    } else {
        // TODO throw;
    }
}



/** Mem strategy with dynamic size, allocated with Allocator */
template<class C, class A>
class DynamicSized : public AMemStrategy {

  public:   // types

    typedef A Allocator;

    static const uint32_t RESIZE_STEP = 8;

  private:

    C& container;
    A allocator;

  public:   // functions

    DynamicSized(C& cont) :
        container(cont) {};
    // No DynamicSized(void* initData, uint32_t initCapacity);

    ~DynamicSized() {
        container.clear();
        deallocate();
    }

    virtual void reserve(uint32_t length) FINAL OVERRIDE;

    virtual void reserveAtLeast(uint32_t length) FINAL OVERRIDE {
        reserve(getRoundedLength(length));
    }

    virtual void shrinkToFit() FINAL OVERRIDE;
    virtual void resize(uint32_t newLength) FINAL OVERRIDE;

  private:

    void reallocateAndCopyFor(uint32_t len);

    void allocate(uint32_t len);

    void deallocate() {
        allocator.deallocate(container.getData(), container.getCapacity());
    }

    static uint32_t getRoundedLength(uint32_t length) {
        return (length + (RESIZE_STEP - 1)) & ~(RESIZE_STEP - 1);
    }

};


template<class C, class A>
void DynamicSized<C, A>::reserve(uint32_t length) {

    if (length > container.getCapacity()) {
        reallocateAndCopyFor(length);
    }
}


template<class C, class A>
void DynamicSized<C, A>::shrinkToFit() {

    if (container.getCapacity() > container.getSize()) {
        reallocateAndCopyFor(container.getSize());
    }
}


template<class C, class A>
void DynamicSized<C, A>::resize(uint32_t newLength) {

    if (newLength > container.getSize()) {

        if (newLength > container.getCapacity()) {
            reallocateAndCopyFor(getRoundedLength(newLength));
        }

        typename C::Iterator newEnd = container.getData() + newLength;

        for (typename C::Iterator it = container.end(); it < newEnd; ++it) {
            C::placeDefaultTo(it);
        }

    } else if (newLength < container.getSize()) {

        typename C::Iterator newEnd = container.getData() + newLength;
        C::destruct(newEnd, container.end());
    }

    container.proxy.setSize(newLength);
}


template<class C, class A>
void DynamicSized<C, A>::reallocateAndCopyFor(uint32_t len) {

    typename C::ItemType* oldData = container.getData();
    uint32_t oldCapacity = container.getCapacity();

    allocate(len);

    if (oldData != NULLPTR) {

        uint32_t numToCopy = (len < container.getSize()) ? len : container.getSize();

        if ((container.getData() != NULLPTR) && (numToCopy > 0)) {

            typename C::ItemType* dataAlias = container.getData();
            C::uninitializedCopy(oldData, dataAlias, numToCopy);
        }

        allocator.deallocate(oldData, oldCapacity);
    }
}


template<class C, class A>
void DynamicSized<C, A>::allocate(uint32_t len) {

    if (len > 0) {
        container.proxy.setData(allocator.allocate(len));
    } else {
        container.proxy.setData(NULLPTR);
    }

    if (container.getData() != NULLPTR) {
        container.proxy.setCapacity(len);
    } else {
        container.proxy.setCapacity(0);
    }
}


/** Heap allocated mem strategy */
template<class C>
class HeapUser : public DynamicSized<C, std::allocator<typename C::ItemType> > {

    typedef DynamicSized<C, std::allocator<typename C::ItemType> > Base;

  public:   // functions

    HeapUser(C& cont) :
        Base(cont) {};

};

}


#endif /* __ETL_MEMSTARTEGIES_H__ */

