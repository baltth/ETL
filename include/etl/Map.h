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

#ifndef __ETL_MAP_H__
#define __ETL_MAP_H__

#include <etl/etlSupport.h>
#include <etl/base/MapTemplate.h>
#include <etl/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {

namespace Dynamic {

/// Map with dynamic memory allocation, defaults to std::allocator.
template<class K, class E, template<class> class A = std::allocator>
class Map : public ETL_NAMESPACE::Map<K, E> {

  public:   // types

    typedef ETL_NAMESPACE::Map<K, E> Base;
    typedef ETL_NAMESPACE::DynamicAllocator<typename Base::Node, A> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    Map() :
        Base(allocator) {};

    Map(const Map& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit Map(const Base& other) :
        Base(allocator) {
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

#if ETL_USE_CPP11

    Map(Map&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    Map(std::initializer_list<std::pair<K, E>> initList) :
        Base(allocator) {
        operator=(initList);
    }

    Map& operator=(Map&& other) {
        this->swap(other);
        return *this;
    }

    Map& operator=(std::initializer_list<std::pair<K, E>> initList) {
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

/// Map with unique pool allocator.
template<class K, class E, uint32_t N>
class Map : public ETL_NAMESPACE::Map<K, E> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::Map<K, E> Base;
    typedef typename ETL_NAMESPACE::PoolHelper<N>::template Allocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    Map() :
        Base(allocator) {};

    Map(const Map& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit Map(const Base& other) :
        Base(allocator) {
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

#if ETL_USE_CPP11

    Map(Map&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    Map(std::initializer_list<std::pair<K, E>> initList) :
        Base(allocator) {
        operator=(initList);
    }

    Map& operator=(Map&& other) {
        this->swap(other);
        return *this;
    }

    Map& operator=(std::initializer_list<std::pair<K, E>> initList) {
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

/// Map with common pool allocator.
template<class K, class E, uint32_t N>
class Map : public ETL_NAMESPACE::Map<K, E> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::Map<K, E> Base;
    typedef typename ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    Map() :
        Base(allocator) {};

    Map(const Map& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit Map(const Base& other) :
        Base(allocator) {
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

#if ETL_USE_CPP11

    Map(Map&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    Map(std::initializer_list<std::pair<K, E>> initList) :
        Base(allocator) {
        operator=(initList);
    }

    Map& operator=(Map&& other) {
        this->swap(other);
        return *this;
    }

    Map& operator=(std::initializer_list<std::pair<K, E>> initList) {
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

#endif /* __ETL_MAP_H__ */

