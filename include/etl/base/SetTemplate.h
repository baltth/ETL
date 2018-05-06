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

#ifndef __ETL_SETTEMPLATE_H__
#define __ETL_SETTEMPLATE_H__

#include <etl/etlSupport.h>
#include <etl/base/Sorted.h>
#include <etl/base/ListTemplate.h>

#include <utility>

namespace ETL_NAMESPACE {


template<class E, template<class> class A>
class Set : public Sorted<List<E, A> > {

  public:   // types

    typedef E KeyType;
    typedef E ValueType;
    typedef ValueType& Reference;
    typedef const ValueType& ConstReference;
    typedef ValueType* Pointer;
    typedef const ValueType* ConstPointer;

    typedef List<E, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef Sorted<ContainerType> Base;

    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

  public:   // functions

    Set() {};

    Set(const Set& other) {
        copyElementsFrom(other);
    }

    Set& operator=(const Set& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Set(std::initializer_list<E> initList);

#endif

    using Base::find;
    using Base::erase;

    std::pair<Iterator, bool> insert(const E& e) {
        return Base::insertUnique(e);
    }

    void erase(const E& e);

    ConstIterator find(const E& e) const;

    Iterator find(const E& e) {
        return Iterator(static_cast<const Set*>(this)->find(e));
    }

    void copyElementsFrom(const Set<E, A>& other);

};


#if ETL_USE_CPP11

template<class E, template<class> class A>
Set<E, A>::Set(std::initializer_list<E> initList) {

    for (auto& item : initList) {
        Base::insertUnique(item);
    }
}

#endif


template<class E, template<class> class A>
void Set<E, A>::erase(const E& e) {

    std::pair<Iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        Base::erase(--found.first);
    }
}


template<class E, template<class> class A>
typename Set<E, A>::ConstIterator  Set<E, A>::find(const E& e) const {

    std::pair<ConstIterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class E, template<class> class A>
void Set<E, A>::copyElementsFrom(const Set<E, A>& other) {

    ConstIterator endIt = other.end();
    for (ConstIterator it = other.begin(); it != endIt; ++it) {
        insert(*it);
    }
}

}

#endif /* __ETL_SETTEMPLATE_H__ */

