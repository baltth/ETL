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

#include <ETL/etlSupport.h>
#include <ETL/base/ListTemplate.h>
#include <ETL/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {


template<class T, template<class> class A = std::allocator>
class List : public ListTemplate<T, A> {

  public:   // types

    typedef ListTemplate<T, A> ListBase;
    typedef typename ListBase::Allocator Allocator;
    typedef typename ListBase::Iterator Iterator;
    typedef typename ListBase::ConstIterator ConstIterator;
    typedef typename ListBase::Node Node;

  public:   // functions

    List() {};

    explicit List(const Allocator& a) :
        ListBase(a) {};

#if ETL_USE_CPP11

    List(const std::initializer_list<T>& initList) :
        ListBase(initList) {};

#endif

};


namespace Pooled {

template<class E, uint32_t N>
class List : public ETL_NAMESPACE::List<E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::List<E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> ListBase;
    typedef typename ListBase::Iterator Iterator;
    typedef typename ListBase::ConstIterator ConstIterator;

  public:   // functions

    List() {};

#if ETL_USE_CPP11

    List(const std::initializer_list<T>& initList) :
        ListBase(initList) {};

#endif

};

}

}

#endif /* __ETL_LIST_H__ */

