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
#include <etl/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {


namespace Dynamic {

/// MultiMap dynamic memory allocation, defaults to std::allocator.
template<class K, class E, template<class> class A = std::allocator>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E> {

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E> Base;
    typedef ETL_NAMESPACE::DynamicAllocator<typename Base::Node, A> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    MultiMap() :
        Base(allocator) {};

    MultiMap(const MultiMap& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit MultiMap(const Base& other) :
        Base(allocator) {
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

#if ETL_USE_CPP11

    MultiMap(MultiMap&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        Base(allocator) {
        operator=(initList);
    }

    MultiMap& operator=(MultiMap&& other) {
        this->swap(other);
        return *this;
    }

    MultiMap& operator=(std::initializer_list<std::pair<K, E>> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    Allocator& getAllocator() const {
        return allocator;
    }

};

}

namespace Static {

/// MultiMap with unique pool allocator.
template<class K, class E, uint32_t N>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E> Base;
    typedef typename ETL_NAMESPACE::PoolHelper<N>::template Allocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    MultiMap() :
        Base(allocator) {};

    MultiMap(const MultiMap& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit MultiMap(const Base& other) :
        Base(allocator) {
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

#if ETL_USE_CPP11

    MultiMap(MultiMap&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        Base(allocator) {
        operator=(initList);
    }

    MultiMap& operator=(MultiMap&& other) {
        this->swap(other);
        return *this;
    }

    MultiMap& operator=(std::initializer_list<std::pair<K, E>> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    Allocator& getAllocator() const {
        return allocator;
    }

};

}

namespace Pooled {

/// MultiMap with common pool allocator.
template<class K, class E, uint32_t N>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E> Base;
    typedef typename ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    MultiMap() :
        Base(allocator) {};

    MultiMap(const MultiMap& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit MultiMap(const Base& other) :
        Base(allocator) {
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

#if ETL_USE_CPP11

    MultiMap(MultiMap&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        Base(allocator) {
        operator=(initList);
    }

    MultiMap& operator=(MultiMap&& other) {
        this->swap(other);
        return *this;
    }

    MultiMap& operator=(std::initializer_list<std::pair<K, E>> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    Allocator& getAllocator() const {
        return allocator;
    }

};

}

}

#endif /* __ETL_MULTIMAP_H__ */

