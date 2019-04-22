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
#include <limits>

namespace ETL_NAMESPACE {


template<class C>
class AMemStrategy {

  public:   // types

    using value_type = typename C::value_type;
    using size_type = typename C::size_type;

  public:   // functions

    virtual size_type getMaxCapacity() const noexcept = 0;
    virtual void reserveExactly(C& cont, size_type length) = 0;
    virtual void reserve(C& cont, size_type length) = 0;
    virtual void shrinkToFit(C& cont) noexcept = 0;
    virtual void resize(C& cont, size_type length) = 0;
    virtual void resize(C& cont, size_type length, const value_type& ref) = 0;
    virtual void cleanup(C& cont) noexcept = 0;

};



/// Mem strategy with static size, allocated externally.
template<class C>
class StaticSized : public AMemStrategy<C> {

  public:   // types

    using value_type = typename C::value_type;
    using size_type = typename C::size_type;

  private:  // variables

    void* const data;
    const size_type capacity;

  public:   // functions

    // No defult constructor;

    StaticSized(void* d, size_type c) :
        data(d),
        capacity(c) {};

    size_type getMaxCapacity() const noexcept final {
        return capacity;
    }

    void reserveExactly(C& cont, size_type length) final {
        setupData(cont, length);
    }

    void reserve(C& cont, size_type length) final {
        setupData(cont, length);
    }

    void shrinkToFit(C& cont) noexcept final {
        setupData(cont, capacity);
    }

    void cleanup(C& cont) noexcept final {
        cont.clear();
    }

    void resize(C& cont, size_type length) final {
        resizeWithInserter(cont, length, [](typename C::iterator pos) {
            C::placeDefaultTo(pos);
        });
    }

    void resize(C& cont, size_type length, const value_type& ref) final {
        resizeWithInserter(cont, length, [&ref](typename C::iterator pos) {
            C::placeValueTo(pos, ref);
        });
    }

  private:

    template<class INS>
    void resizeWithInserter(C& cont, size_type length, INS inserter);
    void setupData(C& cont, size_type length);

};


template<class C>
template<class INS>
void StaticSized<C>::resizeWithInserter(C& cont, size_type length, INS inserter) {

    using iterator = typename C::iterator;

    if (length <= capacity) {

        setupData(cont, length);

        if (length > cont.size()) {

            iterator newEnd = cont.data() + length;

            for (iterator it = cont.end(); it < newEnd; ++it) {
                inserter(it);
            }

        } else if (length < cont.size()) {

            iterator newEnd = cont.data() + length;
            C::destruct(newEnd, cont.end());
        }

        cont.proxy.setSize(length);
    }
}

template<class C>
void StaticSized<C>::setupData(C& cont, size_type length) {

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

    using value_type = typename C::value_type;
    using size_type = typename C::size_type;

    using Allocator = A;

    static const size_type RESIZE_STEP = 8;

  private:

    A allocator;

  public:   // functions

    size_type getMaxCapacity() const noexcept final {
        return std::numeric_limits<size_type>::max();
    }

    void reserveExactly(C& cont, size_type length) final;

    void reserve(C& cont, size_type length) final {
        reserveExactly(cont, getRoundedLength(length));
    }

    void shrinkToFit(C& cont) noexcept final;

    void resize(C& cont, size_type length) final {
        resizeWithInserter(cont, length, [](typename C::iterator pos) {
            C::placeDefaultTo(pos);
        });
    }

    void resize(C& cont, size_type length, const value_type& ref) final {
        resizeWithInserter(cont, length, [&ref](typename C::iterator pos) {
            C::placeValueTo(pos, ref);
        });
    }

    void cleanup(C& cont) noexcept final {
        cont.clear();
        deallocate(cont);
    }

  private:

    template<class INS>
    void resizeWithInserter(C& cont, size_type length, INS inserter);

    void reallocateAndCopyFor(C& cont, size_type len);
    void allocate(C& cont, size_type len);

    void deallocate(C& cont) noexcept {
        allocator.deallocate(cont.data(), cont.capacity());
    }

    static size_type getRoundedLength(size_type length) noexcept {
        return (length + (RESIZE_STEP - 1)) & ~(RESIZE_STEP - 1);
    }

};


template<class C, class A>
void DynamicSized<C, A>::reserveExactly(C& cont, size_type length) {

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
template<class INS>
void DynamicSized<C, A>::resizeWithInserter(C& cont, size_type length, INS inserter) {

    using iterator = typename C::iterator;

    if (length > cont.size()) {

        if (length > cont.capacity()) {
            reallocateAndCopyFor(cont, getRoundedLength(length));
        }

        iterator newEnd = cont.data() + length;

        for (iterator it = cont.end(); it < newEnd; ++it) {
            inserter(it);
        }

    } else if (length < cont.size()) {

        iterator newEnd = cont.data() + length;
        C::destruct(newEnd, cont.end());
    }

    cont.proxy.setSize(length);
}


template<class C, class A>
void DynamicSized<C, A>::reallocateAndCopyFor(C& cont, size_type len) {

    auto oldData = cont.data();
    auto oldEnd = cont.end();
    auto oldCapacity = cont.capacity();

    allocate(cont, len);

    if (oldData != nullptr) {

        size_type numToCopy = (len < cont.size()) ? len : cont.size();

        if ((cont.data() != nullptr) && (numToCopy > 0)) {

            auto dataAlias = cont.data();
            C::uninitializedMove(oldData, dataAlias, numToCopy);
        }

        C::destruct(oldData, oldEnd);
        allocator.deallocate(oldData, oldCapacity);
    }
}


template<class C, class A>
void DynamicSized<C, A>::allocate(C& cont, size_type len) {

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

