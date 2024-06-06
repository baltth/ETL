/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2024 Balazs Toth.

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

#ifndef ETL_MULTIMAP_H_
#define ETL_MULTIMAP_H_

#include <etl/PoolAllocator.h>
#include <etl/base/AAllocator.h>
#include <etl/base/MultiMapTemplate.h>
#include <etl/etlSupport.h>

#include <memory>

namespace ETL_NAMESPACE {


namespace Custom {

/// MultiMap with custom allocator.
template<class K, class E, template<class> class A, class C = std::less<K>>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, C> {

  public:  // types

    using Base = ETL_NAMESPACE::MultiMap<K, E, C>;
    using Node = typename Base::Node;
    using AllocatorTraits = typename Detail::AllocatorTraits<Node, A>;
    using Allocator = typename AllocatorTraits::Type;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    MultiMap() noexcept :
        Base(allocator) {};

    MultiMap(const MultiMap& other) :
        MultiMap() {
        Base::operator=(other);
    }

    explicit MultiMap(const Base& other) :
        MultiMap() {
        Base::operator=(other);
    }

    MultiMap& operator=(const MultiMap& other) {
        Base::operator=(other);
        return *this;
    }

    MultiMap& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    MultiMap(MultiMap&& other) noexcept(noexcept(MultiMap().swap(other))) :
        MultiMap() {
        this->swap(other);
    }

    MultiMap& operator=(MultiMap&& other) noexcept(noexcept(MultiMap().swap(other))) {
        this->swap(other);
        return *this;
    }

    MultiMap(std::initializer_list<typename Base::value_type> initList) :
        MultiMap() {
        operator=(initList);
    }

    MultiMap& operator=(std::initializer_list<typename Base::value_type> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~MultiMap() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(MultiMap& other) noexcept {
        static_assert(noexcept(MultiMap().Base::swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!AllocatorTraits::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(MultiMap& lhs, MultiMap& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Custom


namespace Dynamic {

/// MultiMap with dynamic memory allocation using std::allocator.
template<class K, class E, class C = std::less<K>>
using MultiMap = ETL_NAMESPACE::Custom::MultiMap<K, E, std::allocator, C>;

}  // namespace Dynamic


namespace Static {

/// MultiMap with unique pool allocator.
template<class K, class E, uint32_t N, class C = std::less<K>>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, C> {

    static_assert(N > 0, "Invalid Etl::Static::MultiMap size");

  public:  // types

    using Base = ETL_NAMESPACE::MultiMap<K, E, C>;
    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template Allocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    MultiMap() noexcept :
        Base(allocator) {};

    MultiMap(const MultiMap& other) :
        MultiMap() {
        Base::operator=(other);
    }

    explicit MultiMap(const Base& other) :
        MultiMap() {
        Base::operator=(other);
    }

    MultiMap& operator=(const MultiMap& other) {
        Base::operator=(other);
        return *this;
    }

    MultiMap& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    MultiMap(MultiMap&& other) noexcept(noexcept(MultiMap().swap(other))) :
        MultiMap() {
        this->swap(other);
    }

    MultiMap& operator=(MultiMap&& other) noexcept(noexcept(MultiMap().swap(other))) {
        this->swap(other);
        return *this;
    }

    MultiMap(std::initializer_list<typename Base::value_type> initList) :
        MultiMap() {
        operator=(initList);
    }

    MultiMap& operator=(std::initializer_list<typename Base::value_type> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~MultiMap() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(MultiMap& other) noexcept(
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

    friend void swap(MultiMap& lhs, MultiMap& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Static


namespace Pooled {

/// MultiMap with common pool allocator.
template<class K, class E, uint32_t N, class C = std::less<K>>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, C> {

    static_assert(N > 0, "Invalid Etl::Pooled::MultiMap size");

  public:  // types

    using Base = ETL_NAMESPACE::MultiMap<K, E, C>;
    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template CommonAllocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    MultiMap() noexcept :
        Base(allocator) {};

    MultiMap(const MultiMap& other) :
        MultiMap() {
        Base::operator=(other);
    }

    explicit MultiMap(const Base& other) :
        MultiMap() {
        Base::operator=(other);
    }

    MultiMap& operator=(const MultiMap& other) {
        Base::operator=(other);
        return *this;
    }

    MultiMap& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    MultiMap(MultiMap&& other) noexcept(noexcept(MultiMap().swap(other))) :
        MultiMap() {
        this->swap(other);
    }

    MultiMap& operator=(MultiMap&& other) noexcept(noexcept(MultiMap().swap(other))) {
        this->swap(other);
        return *this;
    }

    MultiMap(std::initializer_list<typename Base::value_type> initList) :
        MultiMap() {
        operator=(initList);
    }

    MultiMap& operator=(std::initializer_list<typename Base::value_type> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~MultiMap() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(MultiMap& other) noexcept {
        static_assert(noexcept(MultiMap().Base::swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!Allocator::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(MultiMap& lhs, MultiMap& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Pooled
}  // namespace ETL_NAMESPACE

#endif  // ETL_MULTIMAP_H_
