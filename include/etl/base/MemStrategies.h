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

#ifndef __ETL_MEMSTARTEGIES_H__
#define __ETL_MEMSTARTEGIES_H__

#include <etl/etlSupport.h>
#include <etl/base/AVectorBase.h>

#include <memory>

namespace ETL_NAMESPACE {


template<class C>
class AMemStrategy {

  public:   // functions

    virtual uint32_t getMaxCapacity() const = 0;
    virtual void reserve(C& cont, uint32_t length) = 0;
    virtual void reserveAtLeast(C& cont, uint32_t length) = 0;
    virtual void shrinkToFit(C& cont) = 0;
    virtual void resize(C& cont, uint32_t length) = 0;
    virtual void cleanup(C& cont) = 0;

};



/// Mem strategy with static size, allocated externally.
template<class C>
class StaticSized : public AMemStrategy<C> {

  private:  // variables

    void* const data;
    const uint32_t capacity;

  public:   // functions

    // No defult constructor;

    StaticSized(void* d, uint32_t c) :
        data(d),
        capacity(c) {};

    virtual uint32_t getMaxCapacity() const FINAL OVERRIDE {
        return capacity;
    }

    virtual void reserve(C& cont, uint32_t length) FINAL OVERRIDE {
        setupData(cont, length);
    }

    virtual void reserveAtLeast(C& cont, uint32_t length) FINAL OVERRIDE {
        setupData(cont, length);
    }

    virtual void shrinkToFit(C& cont) FINAL OVERRIDE {
        setupData(cont, capacity);
    }

    virtual void cleanup(C& cont) {
        cont.clear();
    }

    virtual void resize(C& cont, uint32_t length) FINAL OVERRIDE;

  private:

    void setupData(C& cont, uint32_t length);

};


template<class C>
void StaticSized<C>::resize(C& cont, uint32_t length) {

    if (length <= capacity) {

        setupData(cont, length);

        if (length > cont.getSize()) {

            typename C::Iterator newEnd = cont.getData() + length;

            for (typename C::Iterator it = cont.end(); it < newEnd; ++it) {
                C::placeDefaultTo(it);
            }

        } else if (length < cont.getSize()) {

            typename C::Iterator newEnd = cont.getData() + length;
            C::destruct(newEnd, cont.end());
        }

        cont.proxy.setSize(length);
    }
}

template<class C>
void StaticSized<C>::setupData(C& cont, uint32_t length) {

    if (length <= capacity) {
        cont.proxy.setData(data);
        cont.proxy.setCapacity(capacity);
    } else {
        // TODO throw;
    }
}



/// Mem strategy with dynamic size, allocated with Allocator
template<class C, class A>
class DynamicSized : public AMemStrategy<C> {

  public:   // types

    typedef A Allocator;

    static const uint32_t RESIZE_STEP = 8;

  private:

    A allocator;

  public:   // functions

    virtual uint32_t getMaxCapacity() const FINAL OVERRIDE {
        return UINT32_MAX;
    }

    virtual void reserve(C& cont, uint32_t length) FINAL OVERRIDE;

    virtual void reserveAtLeast(C& cont, uint32_t length) FINAL OVERRIDE {
        reserve(cont, getRoundedLength(length));
    }

    virtual void shrinkToFit(C& cont) FINAL OVERRIDE;
    virtual void resize(C& cont, uint32_t length) FINAL OVERRIDE;

    virtual void cleanup(C& cont) {
        cont.clear();
        deallocate(cont);
    }

  private:

    void reallocateAndCopyFor(C& cont, uint32_t len);
    void allocate(C& cont, uint32_t len);

    void deallocate(C& cont) {
        allocator.deallocate(cont.getData(), cont.getCapacity());
    }

    static uint32_t getRoundedLength(uint32_t length) {
        return (length + (RESIZE_STEP - 1)) & ~(RESIZE_STEP - 1);
    }

};


template<class C, class A>
void DynamicSized<C, A>::reserve(C& cont, uint32_t length) {

    if (length > cont.getCapacity()) {
        reallocateAndCopyFor(cont, length);
    }
}


template<class C, class A>
void DynamicSized<C, A>::shrinkToFit(C& cont) {

    if (cont.getCapacity() > cont.getSize()) {
        reallocateAndCopyFor(cont, cont.getSize());
    }
}


template<class C, class A>
void DynamicSized<C, A>::resize(C& cont, uint32_t length) {

    if (length > cont.getSize()) {

        if (length > cont.getCapacity()) {
            reallocateAndCopyFor(cont, getRoundedLength(length));
        }

        typename C::Iterator newEnd = cont.getData() + length;

        for (typename C::Iterator it = cont.end(); it < newEnd; ++it) {
            C::placeDefaultTo(it);
        }

    } else if (length < cont.getSize()) {

        typename C::Iterator newEnd = cont.getData() + length;
        C::destruct(newEnd, cont.end());
    }

    cont.proxy.setSize(length);
}


template<class C, class A>
void DynamicSized<C, A>::reallocateAndCopyFor(C& cont, uint32_t len) {

    typename C::Pointer oldData = cont.getData();
    typename C::Iterator oldEnd = cont.end();
    uint32_t oldCapacity = cont.getCapacity();

    allocate(cont, len);

    if (oldData != NULLPTR) {

        uint32_t numToCopy = (len < cont.getSize()) ? len : cont.getSize();

        if ((cont.getData() != NULLPTR) && (numToCopy > 0)) {

            typename C::Pointer dataAlias = cont.getData();
            C::uninitializedCopy(oldData, dataAlias, numToCopy);
        }

        C::destruct(oldData, oldEnd);
        allocator.deallocate(oldData, oldCapacity);
    }
}


template<class C, class A>
void DynamicSized<C, A>::allocate(C& cont, uint32_t len) {

    if (len > 0) {
        cont.proxy.setData(allocator.allocate(len));
    } else {
        cont.proxy.setData(NULLPTR);
    }

    if (cont.getData() != NULLPTR) {
        cont.proxy.setCapacity(len);
    } else {
        cont.proxy.setCapacity(0);
    }
}

}


#endif /* __ETL_MEMSTARTEGIES_H__ */

