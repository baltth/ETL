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

#ifndef __ETL_SET_H__
#define __ETL_SET_H__

#include <etl/etlSupport.h>
#include <etl/base/SetTemplate.h>
#include <etl/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {

namespace Dynamic {

/// Set with std::allocator.
template<class E>
class Set : public ETL_NAMESPACE::Set<E, std::allocator> {

  public:   // types

    typedef ETL_NAMESPACE::Set<E, std::allocator> Base;

  public:   // functions

    Set() {};

    Set(const Set& other) :
        Base(other) {};

    explicit Set(const Base& other) :
        Base(other) {};

    Set& operator=(const Base& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Set(std::initializer_list<E> initList) :
        Base(initList) {};

#endif

};

}

namespace Static {

/// Set with unique pool allocator.
template<class E, uint32_t N>
class Set : public ETL_NAMESPACE::Set<E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::Set<E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> Base;

  public:   // functions

    Set() {};

    Set(const Set& other) :
        Base(other) {};

    explicit Set(const Base& other) :
        Base(other) {};

    Set& operator=(const Base& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Set(std::initializer_list<E> initList) :
        Base(initList) {};

#endif

};

}

namespace Pooled {

/// Set with common pool allocator.
template<class E, uint32_t N>
class Set : public ETL_NAMESPACE::Set<E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::Set<E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> Base;

  public:   // functions

    Set() {};

    Set(const Set& other) :
        Base(other) {};

    explicit Set(const Base& other) :
        Base(other) {};

    Set& operator=(const Base& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Set(std::initializer_list<E> initList) :
        Base(initList) {};

#endif

};

}

}

#endif /* __ETL_SET_H__ */
