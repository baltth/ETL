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

#include <memory>

namespace ETL_NAMESPACE {


template<class C>
class AMemStrategy {

  public:   // functions

    virtual uint32_t getMaxCapacity() const noexcept = 0;
    virtual void reserveExactly(C& cont, uint32_t length) = 0;
    virtual void reserve(C& cont, uint32_t length) = 0;
    virtual void shrinkToFit(C& cont) noexcept = 0;
    virtual void resize(C& cont, uint32_t length) = 0;
    virtual void cleanup(C& cont) noexcept = 0;

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

    uint32_t getMaxCapacity() const noexcept final {
        return capacity;
    }

    void reserveExactly(C& cont, uint32_t length) final {
        setupData(cont, length);
    }

    void reserve(C& cont, uint32_t length) final {
        setupData(cont, length);
    }

    void shrinkToFit(C& cont) noexcept final {
        setupData(cont, capacity);
    }

    void cleanup(C& cont) noexcept final {
        cont.clear();
    }

    void resize(C& cont, uint32_t length) final;

  private:

    void setupData(C& cont, uint32_t length);

};


template<class C>
void StaticSized<C>::resize(C& cont, uint32_t length) {

    if (length <= capacity) {

        setupData(cont, length);

        if (length > cont.size()) {

            typename C::iterator newEnd = cont.data() + length;

            for (typename C::iterator it = cont.end(); it < newEnd; ++it) {
                C::placeDefaultTo(it);
            }

        } else if (length < cont.size()) {

            typename C::iterator newEnd = cont.data() + length;
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

    uint32_t getMaxCapacity() const noexcept final {
        return UINT32_MAX;
    }

    void reserveExactly(C& cont, uint32_t length) final;

    void reserve(C& cont, uint32_t length) final {
        reserveExactly(cont, getRoundedLength(length));
    }

    void shrinkToFit(C& cont) noexcept final;
    void resize(C& cont, uint32_t length) final;

    void cleanup(C& cont) noexcept final {
        cont.clear();
        deallocate(cont);
    }

  private:

    void reallocateAndCopyFor(C& cont, uint32_t len);
    void allocate(C& cont, uint32_t len);

    void deallocate(C& cont) noexcept {
        allocator.deallocate(cont.data(), cont.capacity());
    }

    static uint32_t getRoundedLength(uint32_t length) noexcept {
        return (length + (RESIZE_STEP - 1)) & ~(RESIZE_STEP - 1);
    }

};


template<class C, class A>
void DynamicSized<C, A>::reserveExactly(C& cont, uint32_t length) {

    if (length > cont.capacity()) {
        reallocateAndCopyFor(cont, length);
    }
}


template<class C, class A>
void DynamicSized<C, A>::shrinkToFit(C& cont) noexcept {

    if (cont.capacity() > cont.size()) {
        reallocateAndCopyFor(cont, cont.size());
    }
}


template<class C, class A>
void DynamicSized<C, A>::resize(C& cont, uint32_t length) {

    if (length > cont.size()) {

        if (length > cont.capacity()) {
            reallocateAndCopyFor(cont, getRoundedLength(length));
        }

        typename C::iterator newEnd = cont.data() + length;

        for (typename C::iterator it = cont.end(); it < newEnd; ++it) {
            C::placeDefaultTo(it);
        }

    } else if (length < cont.size()) {

        typename C::iterator newEnd = cont.data() + length;
        C::destruct(newEnd, cont.end());
    }

    cont.proxy.setSize(length);
}


template<class C, class A>
void DynamicSized<C, A>::reallocateAndCopyFor(C& cont, uint32_t len) {

    typename C::pointer oldData = cont.data();
    typename C::iterator oldEnd = cont.end();
    uint32_t oldCapacity = cont.capacity();

    allocate(cont, len);

    if (oldData != nullptr) {

        uint32_t numToCopy = (len < cont.size()) ? len : cont.size();

        if ((cont.data() != nullptr) && (numToCopy > 0)) {

            typename C::pointer dataAlias = cont.data();
            C::uninitializedMove(oldData, dataAlias, numToCopy);
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
        cont.proxy.setData(nullptr);
    }

    if (cont.data() != nullptr) {
        cont.proxy.setCapacity(len);
    } else {
        cont.proxy.setCapacity(0);
    }
}

}

#endif /* __ETL_MEMSTARTEGIES_H__ */

