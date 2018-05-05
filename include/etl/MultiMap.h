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

/// MultiMap with std::allocator.
template<class K, class E>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, std::allocator> {

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E, std::allocator> Base;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

  public:   // functions

    MultiMap() {};

    MultiMap(const MultiMap& other) :
        Base(other) {};

    explicit MultiMap(const Base& other) :
        Base(other) {};

    MultiMap& operator=(const Base& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        Base(initList) {};

#endif

};

}

namespace Static {

/// MultiMap with unique pool allocator.
template<class K, class E, uint32_t N>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> Base;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

  public:   // functions

    MultiMap() {};

    MultiMap(const MultiMap& other) :
        Base(other) {};

    explicit MultiMap(const Base& other) :
        Base(other) {};

    MultiMap& operator=(const Base& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        Base(initList) {};

#endif

};

}

namespace Pooled {

/// MultiMap with common pool allocator.
template<class K, class E, uint32_t N>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> Base;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

  public:   // functions

    MultiMap() {};

    MultiMap(const MultiMap& other) :
        Base(other) {};

    explicit MultiMap(const Base& other) :
        Base(other) {};

    MultiMap& operator=(const Base& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    MultiMap(std::initializer_list<std::pair<K, E>> initList) :
        Base(initList) {};

#endif

};

}

}

#endif /* __ETL_MULTIMAP_H__ */

