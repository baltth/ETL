/**
\file
\date 2016.01.19.
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

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

#include "etlSupport.h"

#include "Base/Sorted.h"
#include "PoolAllocator.h"

#include <memory>
#include <utility>

namespace ETL_NAMESPACE {


template<class E, template<class> class A = std::allocator>
class Set : public Sorted<ListTemplate<E, A> > {

  public:   // types

    typedef E ItemType;
    typedef ListTemplate<ItemType, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef Sorted<ContainerType> SetBase;
    typedef typename SetBase::Iterator Iterator;
    typedef typename SetBase::ConstIterator ConstIterator;

  public:   // functions

    Set() {};

    Set(const Set& other) {
        copyElementsFrom(other);
    }

    Set& operator=(const Set& other) {
        SetBase::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Set(const std::initializer_list<E>& initList);

#endif

    std::pair<Iterator, bool> insert(const E& e) {
        return SetBase::insertUnique(e);
    }

    void erase(const E& e);

    Iterator erase(Iterator pos) {
        return SetBase::erase(pos);
    }

    Iterator find(const E& e) const;

    void copyElementsFrom(const Set<E, A>& other);

};


#if ETL_USE_CPP11

template<class E, template<class> class A>
Set<E, A>::Set(const std::initializer_list<E>& initList) {

    for (auto& item : initList) {
        SetBase::insertUnique(item);
    }
}

#endif


template<class E, template<class> class A>
void Set<E, A>::erase(const E& e) {

    std::pair<Iterator, bool> found = SetBase::findSortedPosition(e);

    if (found.second == true) {
        SetBase::erase(--found.first);
    }
}


template<class E, template<class> class A>
typename Set<E, A>::Iterator  Set<E, A>::find(const E& e) const {

    std::pair<Iterator, bool> found = SetBase::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return SetBase::end();
    }
}


template<class E, template<class> class A>
void Set<E, A>::copyElementsFrom(const Set<E, A>& other) {

    Iterator endIt = other.end();
    for (Iterator it = other.begin(); it != endIt; ++it) {
        insert(*it);
    }
}


namespace Pooled {


template<class E, uint32_t N>
class Set : public ETL_NAMESPACE::Set<E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> {

  public:   // types

    typedef ETL_NAMESPACE::Set<E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> SetBase;
    typedef typename SetBase::Iterator Iterator;
    typedef typename SetBase::ConstIterator ConstIterator;

  public:   // functions

    Set() {};

    Set(const SetBase& other) :
        SetBase(other) {};

    Set& operator=(const SetBase& other) {
        SetBase::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Set(const std::initializer_list<E>& initList) :
        SetBase(initList) {};

#endif

};

}

}

#endif /* __ETL_SET_H__ */

