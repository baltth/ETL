/**
\file
\date 2016.01.19. 10:02:21
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

#include <utility>

#include "Base/Sorted.h"


namespace ETL_NAMESPACE {


template<class E>
class Set : public Sorted<ListTemplate<E> > {

  public:   // types

    typedef E ItemType;
    typedef Sorted<ListTemplate<E > > SetBase;
    typedef typename SetBase::Iterator Iterator;
    typedef typename SetBase::ConstIterator ConstIterator;

  public:   // functions

    Set() {};

#if ETL_USE_CPP11

    Set(const std::initializer_list<E>& initList);

#endif

    std::pair<Iterator, bool> insert(const E& e) {
        return SetBase::insertUnique(e);
    }

    std::pair<Iterator, bool> insertOrAssign(const E& e);

    void erase(const E& e);
    Iterator find(const E& e) const;

    void copyElementsFrom(const Set<E>& other);

};


#if ETL_USE_CPP11

template<class E>
Set<E>::Set(const std::initializer_list<E>& initList) {

    for (auto& item : initList) {
        SetBase::insertUnique(item);
    }
}

#endif

template<class E>
std::pair<typename Set<E>::Iterator, bool> Set<E>::insertOrAssign(const E& e) {

    std::pair<Iterator, bool> found = SetBase::findSortedPosition(e);

    if (found.second == false) {
#if ETL_USE_CPP11
        found.first = SetBase::emplaceTo(found.first, e);
#else
        found.first = SetBase::insertTo(found.first, e);
#endif
    } else {
        --found.first;
        *found.first = e;
    }

    found.second = !found.second;
    return found;
}

template<class E>
void Set<E>::erase(const E& e) {

    std::pair<Iterator, bool> found = SetBase::findSortedPosition(e);

    if (found.second == true) {
        SetBase::erase(--found.first);
    }
}


template<class E>
typename Set<E>::Iterator  Set<E>::find(const E& e) const {

    std::pair<Iterator, bool> found = SetBase::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return SetBase::end();
    }
}


template<class E>
void Set<E>::copyElementsFrom(const Set<E>& other) {

    Iterator endIt = other.end();
    for (Iterator it = other.begin(); it != endIt; ++it) {
        insertOrAssign(*it);
    }
}

}

#endif /* __ETL_SET_H__ */

