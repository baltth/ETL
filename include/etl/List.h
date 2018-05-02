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

#ifndef __ETL_LIST_H__
#define __ETL_LIST_H__

#include <etl/etlSupport.h>
#include <etl/base/ListTemplate.h>
#include <etl/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {

namespace Dynamic {

template<class T>
class List : public ETL_NAMESPACE::List<T, std::allocator> {

  public:   // types

    typedef ETL_NAMESPACE::List<T, std::allocator> Base;
    typedef typename Base::Allocator Allocator;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;
    typedef typename Base::Node Node;

  public:   // functions

    List() {};

    explicit List(const Allocator& a) :
        Base(a) {};

#if ETL_USE_CPP11

    List(std::initializer_list<T> initList) :
        Base(initList) {};

#endif

    void swap(List& other) {
        AListBase::swapNodeList(other);
    }

    template<template<class> class B>
    void swap(ETL_NAMESPACE::List<T, B>& other) {
        Base::swap(other);
    }

};

}


namespace Static {

template<class T, uint32_t N>
class List : public ETL_NAMESPACE::List<T, ETL_NAMESPACE::PoolHelper<N>::template Allocator> {

    STATIC_ASSERT(N > 0);

    public:   // types

    typedef ETL_NAMESPACE::List<T, ETL_NAMESPACE::PoolHelper<N>::template Allocator> Base;
typedef typename Base::Iterator Iterator;
typedef typename Base::ConstIterator ConstIterator;

public:   // functions

List() {};

#if ETL_USE_CPP11

List(std::initializer_list<T> initList) :
Base(initList) {};

#endif

template<template<class> class B>
void swap(ETL_NAMESPACE::List<T, B>& other) {
    Base::swap(other);
}

                 };
}


namespace Pooled {

template<class T, uint32_t N>
class List : public ETL_NAMESPACE::List<T, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::List<T, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> Base;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

  public:   // functions

    List() {};

#if ETL_USE_CPP11

    List(std::initializer_list<T> initList) :
        Base(initList) {};

#endif

    void swap(List& other) {
        AListBase::swapNodeList(other);
    }

    template<template<class> class B>
    void swap(ETL_NAMESPACE::List<T, B>& other) {
        Base::swap(other);
    }

};

}

}

#endif /* __ETL_LIST_H__ */

