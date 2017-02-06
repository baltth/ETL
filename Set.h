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

#include <utility>

#include "Base/SortedTemplate.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {

    
template<class E>
using SetBase = SortedTemplate<E>;


template<class E>
class Set : public SetBase<E> {

// types
public:

    typedef E ItemType;
    typedef typename SetBase<E>::Iterator Iterator;

// functions
public:

    Set() {};
    Set(const std::initializer_list<E> &initList);

    std::pair<Iterator, bool> insert(const E &e) {
        return SetBase<E>::insertUnique(e);
    }

    std::pair<Iterator, bool> insertOrAssign(const E &e);

    void erase(const E &e);
    Iterator find(const E &e) const;

    void copyElementsFrom(const Set<E> &other);

};


template<class E>
Set<E>::Set(const std::initializer_list<E> &initList) {

    for(auto &item : initList) {
        SetBase<E>::insertUnique(item);
    }
}


template<class E>
void Set<E>::erase(const E &e) {

    auto found = SetBase<E>::findSortedPosition(e);

    if(found.second == true) {
        SetBase<E>::erase(--found.first);
    }
}


template<class E>
typename Set<E>::Iterator  Set<E>::find(const E &e) const {

    auto found = SetBase<E>::findSortedPosition(e);

    if(found.second == true) {
        return --found.first;
    } else {
        return SetBase<E>::end();
    }
}


template<class E>
void Set<E>::copyElementsFrom(const Set<E> &other) {

    for(ItemType &item : other) {
        insertOrAssign(item.getKey(), item.getElement());
    }
}

}

#endif /* __ETL_SET_H__ */
