/**
\file
\date 2015.05.26. 23:11:34
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

#ifndef __ETL_MAP_H__
#define __ETL_MAP_H__

#include "etlSupport.h"

#include <utility>

#include "Base/Sorted.h"
#include "Base/MapItem.h"


namespace ETL_NAMESPACE {


template<typename K, class E>
class Map : public Sorted<ListTemplate<MapItem<K, E> > > {

// types
public:

    typedef MapItem<K, E> ItemType;
    typedef Sorted<ListTemplate<ItemType> > MapBase;
    typedef typename MapBase::Iterator Iterator;
    typedef typename MapBase::ConstIterator ConstIterator;

// functions
public:

    Map() {};

#if ETL_USE_CPP11

    Map(const std::initializer_list<std::pair<K, E>> &initList);

#endif

    inline std::pair<Iterator, bool> insert(const K &k, const E &e) {
        return MapBase::insertUnique(ItemType(k, e));
    }

    std::pair<Iterator, bool> insertOrAssign(const K &k, const E &e);

    void erase(const K &k);

    Iterator find(const K &k) const;

    E &getElement(const K &k) const;

    void copyElementsFrom(const Map<K, E> &other);

    E &operator[](const K &k) {
        return Map<K, E>::getElement(k);
    }

    const E &operator[](const K &k) const {
        return Map<K, E>::getElement(k);
    }

#if ETL_USE_CPP11

    template<typename... Args>
    inline std::pair<Iterator, bool> emplace(const K &k, Args &&... args);

#endif

};


template<typename K, class E>
std::pair<typename Map<K, E>::Iterator, bool> Map<K, E>::insertOrAssign(const K &k, const E &e) {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if(found.second == false) {
#if ETL_USE_CPP11
        found.first = MapBase::emplaceTo(found.first, k, e);
#else
        found.first = MapBase::insertTo(found.first, ItemType(k, e));
#endif
    } else {
        --found.first;
        found.first->setElement(e);
    }

    found.second = !found.second;
    return found;
}


template<typename K, class E>
void Map<K, E>::erase(const K &k) {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if(found.second == true) {
        MapBase::erase(--found.first);
    }
}


template<typename K, class E>
typename Map<K, E>::Iterator  Map<K, E>::find(const K &k) const {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if(found.second == true) {
        return --found.first;
    } else {
        return MapBase::end();
    }
}


template<typename K, class E>
E &Map<K, E>::getElement(const K &k) const {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if(found.second == false) {
#if ETL_USE_CPP11
        found.first = MapBase::emplaceTo(found.first, std::move(ItemType(k)));
#else
        found.first = MapBase::insertTo(found.first, ItemType(k));
#endif
    } else {
        --found.first;
    }

    return found.first->getElement();
}


template<typename K, class E>
void Map<K, E>::copyElementsFrom(const Map<K, E> &other) {

    Iterator endIt = other.end();
    for(Iterator it = other.begin(); it != endIt; ++it) {
        insertOrAssign(it->getKey(), it->getElement());
    }
}

#if ETL_USE_CPP11

template<typename K, class E>
Map<K, E>::Map(const std::initializer_list<std::pair<K, E>> &initList) {

    for(auto &item : initList) {
        insert(item.first, item.second);
    }
}


template<typename K, class E>
template<typename... Args>
std::pair<typename Map<K, E>::Iterator, bool> Map<K, E>::emplace(const K &k, Args &&... args) {

    auto found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if(found.second == false) {
        found.first = MapBase::emplaceTo(found.first, k, std::forward<Args>(args)...);
    }

    found.second = !found.second;
    return found;
}

#endif

}

#endif /* __ETL_MAP_H__ */

