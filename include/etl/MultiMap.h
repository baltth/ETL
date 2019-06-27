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

#ifndef __ETL_MULTIMAP_H__
#define __ETL_MULTIMAP_H__

#include <etl/etlSupport.h>
#include <etl/base/MultiMapTemplate.h>
#include <etl/base/AAllocator.h>
#include <etl/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {


namespace Custom {

/// MultiMap with custom allocator.
template<class K, class E, template<class> class A, class C = std::less<K>>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, C> {

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E, C> Base;
    typedef typename Base::Node Node;
    typedef typename AllocatorType<Node, A>::Type Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

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

    MultiMap(MultiMap&& other) :
        MultiMap() {
        operator=(std::move(other));
    }

    MultiMap& operator=(MultiMap&& other) {
        this->swap(other);
        return *this;
    }

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        MultiMap() {
        operator=(initList);
    }

    MultiMap& operator=(std::initializer_list<std::pair<K, E>> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~MultiMap() noexcept(Allocator::NoexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

};

}


namespace Dynamic {

/// MultiMap with dynamic memory allocation using std::allocator.
template<class K, class E, class C = std::less<K>>
using MultiMap = ETL_NAMESPACE::Custom::MultiMap<K, E, std::allocator, C>;

}


namespace Static {

/// MultiMap with unique pool allocator.
template<class K, class E, uint32_t N, class C = std::less<K>>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, C> {

    static_assert(N > 0, "Invalid Etl::Static::MultiMap size");

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E, C> Base;
    typedef typename ETL_NAMESPACE::PoolHelper<N>::template Allocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

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

    MultiMap(MultiMap&& other) :
        MultiMap() {
        operator=(std::move(other));
    }

    MultiMap& operator=(MultiMap&& other) {
        this->swap(other);
        return *this;
    }

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        MultiMap() {
        operator=(initList);
    }

    MultiMap& operator=(std::initializer_list<std::pair<K, E>> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~MultiMap() noexcept(Allocator::NoexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

};

}


namespace Pooled {

/// MultiMap with common pool allocator.
template<class K, class E, uint32_t N, class C = std::less<K>>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, C> {

    static_assert(N > 0, "Invalid Etl::Pooled::MultiMap size");

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E, C> Base;
    typedef typename ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

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

    MultiMap(MultiMap&& other) :
        MultiMap() {
        operator=(std::move(other));
    }

    MultiMap& operator=(MultiMap&& other) {
        this->swap(other);
        return *this;
    }

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        MultiMap() {
        operator=(initList);
    }

    MultiMap& operator=(std::initializer_list<std::pair<K, E>> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~MultiMap() noexcept(Allocator::NoexceptDestroy) {
        this->clear();
    }

    Allocator& getAllocator() const noexcept {
        return allocator;
    }

};

}
}

#endif /* __ETL_MULTIMAP_H__ */

