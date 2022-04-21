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

#ifndef __ETL_MAP_H__
#define __ETL_MAP_H__

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

    typedef ETL_NAMESPACE::Map<K, E, C> Base;
    typedef typename Base::Node Node;
    typedef typename AllocatorTraits<Node, A>::Type Allocator;

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

    Map(Map&& other) :
        Map() {
        operator=(std::move(other));
    }

    Map& operator=(Map&& other) {
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

    ~Map() noexcept(Allocator::NoexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
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

    typedef ETL_NAMESPACE::Map<K, E, C> Base;
    typedef typename ETL_NAMESPACE::PoolHelperForSize<N>::template Allocator<typename Base::Node>
        Allocator;

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

    Map(Map&& other) :
        Map() {
        operator=(std::move(other));
    }

    Map& operator=(Map&& other) {
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

    ~Map() noexcept(Allocator::NoexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }
};

}  // namespace Static


namespace Pooled {

/// Map with common pool allocator.
template<class K, class E, uint32_t N, class C = std::less<K>>
class Map : public ETL_NAMESPACE::Map<K, E, C> {

    static_assert(N > 0, "Invalid Etl::Pooled::Map size");

  public:  // types

    typedef ETL_NAMESPACE::Map<K, E, C> Base;
    typedef
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template CommonAllocator<typename Base::Node>
            Allocator;

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

    Map(Map&& other) :
        Map() {
        operator=(std::move(other));
    }

    Map& operator=(Map&& other) {
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

    ~Map() noexcept(Allocator::NoexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }
};

}  // namespace Pooled

}  // namespace ETL_NAMESPACE

#endif  // __ETL_MAP_H__
