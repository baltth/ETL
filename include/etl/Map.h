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

#ifndef ETL_MAP_H_
#define ETL_MAP_H_

#include <etl/PoolAllocator.h>
#include <etl/base/AAllocator.h>
#include <etl/base/MapTemplate.h>
#include <etl/etlSupport.h>

#include <memory>

namespace ETL_NAMESPACE {

namespace Custom {

/// Map with custom allocator.
template<class K, class E, template<class> class A, class C = std::less<K>>
class Map : public ETL_NAMESPACE::Map<K, E, C> {

  public:  // types

    using Base = ETL_NAMESPACE::Map<K, E, C>;
    using Node = typename Base::Node;
    using AllocatorTraits = typename Detail::AllocatorTraits<Node, A>;
    using Allocator = typename AllocatorTraits::Type;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    Map() noexcept :
        Base(allocator) {};

    Map(const Map& other) :
        Map() {
        Base::operator=(other);
    }

    explicit Map(const Base& other) :
        Map() {
        Base::operator=(other);
    }

    Map& operator=(const Map& other) {
        Base::operator=(other);
        return *this;
    }

    Map& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    Map(Map&& other) noexcept(noexcept(Map().swap(other))) :
        Map() {
        this->swap(other);
    }

    Map& operator=(Map&& other) noexcept(noexcept(Map().swap(other))) {
        this->swap(other);
        return *this;
    }

    Map(std::initializer_list<typename Base::value_type> initList) :
        Map() {
        operator=(initList);
    }

    Map& operator=(std::initializer_list<typename Base::value_type> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Map() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(Map& other) noexcept {
        static_assert(noexcept(Map().Base::swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!AllocatorTraits::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(Map& lhs, Map& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Custom


namespace Dynamic {

/// Map with dynamic memory allocation using std::allocator.
template<class K, class E, class C = std::less<K>>
using Map = ETL_NAMESPACE::Custom::Map<K, E, std::allocator, C>;

}  // namespace Dynamic


namespace Static {

/// Map with unique pool allocator.
template<class K, class E, uint32_t N, class C = std::less<K>>
class Map : public ETL_NAMESPACE::Map<K, E, C> {

    static_assert(N > 0, "Invalid Etl::Static::Map size");

  public:  // types

    using Base = ETL_NAMESPACE::Map<K, E, C>;
    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template Allocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    Map() noexcept :
        Base(allocator) {};

    Map(const Map& other) :
        Map() {
        Base::operator=(other);
    }

    explicit Map(const Base& other) :
        Map() {
        Base::operator=(other);
    }

    Map& operator=(const Map& other) {
        Base::operator=(other);
        return *this;
    }

    Map& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    Map(Map&& other) noexcept(noexcept(Map().swap(other))) :
        Map() {
        this->swap(other);
    }

    Map& operator=(Map&& other) noexcept(noexcept(Map().swap(other))) {
        this->swap(other);
        return *this;
    }

    Map(std::initializer_list<typename Base::value_type> initList) :
        Map() {
        operator=(initList);
    }

    Map& operator=(std::initializer_list<typename Base::value_type> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Map() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(Map& other) noexcept(
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

    friend void swap(Map& lhs, Map& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Static


namespace Pooled {

/// Map with common pool allocator.
template<class K, class E, uint32_t N, class C = std::less<K>>
class Map : public ETL_NAMESPACE::Map<K, E, C> {

    static_assert(N > 0, "Invalid Etl::Pooled::Map size");

  public:  // types

    using Base = ETL_NAMESPACE::Map<K, E, C>;
    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template CommonAllocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    Map() noexcept :
        Base(allocator) {};

    Map(const Map& other) :
        Map() {
        Base::operator=(other);
    }

    explicit Map(const Base& other) :
        Map() {
        Base::operator=(other);
    }

    Map& operator=(const Map& other) {
        Base::operator=(other);
        return *this;
    }

    Map& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

    Map(Map&& other) noexcept(noexcept(Map().swap(other))) :
        Map() {
        this->swap(other);
    }

    Map& operator=(Map&& other) noexcept(noexcept(Map().swap(other))) {
        this->swap(other);
        return *this;
    }

    Map(std::initializer_list<typename Base::value_type> initList) :
        Map() {
        operator=(initList);
    }

    Map& operator=(std::initializer_list<typename Base::value_type> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Map() noexcept(Allocator::noexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

    void swap(Map& other) noexcept {
        static_assert(noexcept(Map().Base::swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!Allocator::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(Map& lhs, Map& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Pooled

}  // namespace ETL_NAMESPACE

#endif  // ETL_MAP_H_
