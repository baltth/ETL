/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2022 Balazs Toth.

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

#ifndef __ETL_SET_H__
#define __ETL_SET_H__

#include <etl/PoolAllocator.h>
#include <etl/base/AAllocator.h>
#include <etl/base/SetTemplate.h>
#include <etl/etlSupport.h>

#include <memory>

namespace ETL_NAMESPACE {


namespace Custom {

/// Set with custom allocator.
template<class E, template<class> class A, class C = std::less<E>>
class Set : public ETL_NAMESPACE::Set<E, C> {

  public:  // types

    using Base = ETL_NAMESPACE::Set<E, C>;
    using Node = typename Base::Node;
    using AllocatorTraits = typename Detail::AllocatorTraits<Node, A>;
    using Allocator = typename AllocatorTraits::Type;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    Set() noexcept :
        Base(allocator) {};

    Set(const Set& other) :
        Set() {
        Base::operator=(other);
    }

    explicit Set(const Base& other) :
        Set() {
        Base::operator=(other);
    }

    Set& operator=(const Set& other) {
        Base::operator=(other);
        return *this;
    }

    Set& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    Set(Set&& other) noexcept(noexcept(Set().swap(other))) :
        Set() {
        this->swap(other);
    }

    Set& operator=(Set&& other) noexcept(noexcept(Set().swap(other))) {
        this->swap(other);
        return *this;
    }

    Set(std::initializer_list<E> initList) :
        Set() {
        operator=(initList);
    }

    Set& operator=(std::initializer_list<E> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Set() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(Set& other) noexcept {
        static_assert(noexcept(Set().Base::swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!AllocatorTraits::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(Set& lhs, Set& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Custom


namespace Dynamic {

/// Set with dynamic memory allocation using std::allocator.
template<class E, class C = std::less<E>>
using Set = ETL_NAMESPACE::Custom::Set<E, std::allocator, C>;

}  // namespace Dynamic


namespace Static {

/// Set with unique pool allocator.
template<class E, uint32_t N, class C = std::less<E>>
class Set : public ETL_NAMESPACE::Set<E, C> {

    static_assert(N > 0, "Invalid Etl::Static::Set size");

  public:  // types

    using Base = ETL_NAMESPACE::Set<E, C>;
    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template Allocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    Set() noexcept :
        Base(allocator) {};

    Set(const Set& other) :
        Set() {
        Base::operator=(other);
    }

    explicit Set(const Base& other) :
        Set() {
        Base::operator=(other);
    }

    Set& operator=(const Set& other) {
        Base::operator=(other);
        return *this;
    }

    Set& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    Set(Set&& other) noexcept(noexcept(Set().swap(other))) :
        Set() {
        this->swap(other);
    }

    Set& operator=(Set&& other) noexcept(noexcept(Set().swap(other))) {
        this->swap(other);
        return *this;
    }

    Set(std::initializer_list<E> initList) :
        Set() {
        operator=(initList);
    }

    Set& operator=(std::initializer_list<E> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Set() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(Set& other) noexcept(
        noexcept(Detail::NothrowContract<typename Base::value_type>::nothrowIfMovable)) {
        // Note: this operation is noexcept when T can be moved 'noexceptly',
        // however lower level functions are not annotated with noexcept qualifier.
        static_assert(Allocator::uniqueAllocator,
                      "Allocator should use uniqueAllocator == true");
        if (&other != this) {
            Base::swap(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(Set& lhs, Set& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Static


namespace Pooled {

/// Set with common pool allocator.
template<class E, uint32_t N, class C = std::less<E>>
class Set : public ETL_NAMESPACE::Set<E, C> {

    static_assert(N > 0, "Invalid Etl::Pooled::Set size");

  public:  // types

    using Base = ETL_NAMESPACE::Set<E, C>;
    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template CommonAllocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    Set() noexcept :
        Base(allocator) {};

    Set(const Set& other) :
        Set() {
        Base::operator=(other);
    }

    explicit Set(const Base& other) :
        Set() {
        Base::operator=(other);
    }

    Set& operator=(const Set& other) {
        Base::operator=(other);
        return *this;
    }

    Set& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    Set(Set&& other) noexcept(noexcept(Set().swap(other))) :
        Set() {
        this->swap(other);
    }

    Set& operator=(Set&& other) noexcept(noexcept(Set().swap(other))) {
        this->swap(other);
        return *this;
    }

    Set(std::initializer_list<E> initList) :
        Set() {
        operator=(initList);
    }

    Set& operator=(std::initializer_list<E> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Set() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(Set& other) noexcept {
        static_assert(noexcept(Set().Base::swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!Allocator::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(Set& lhs, Set& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Pooled

}  // namespace ETL_NAMESPACE

#endif  // __ETL_SET_H__
