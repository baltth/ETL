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

#include <utility>

#include "Base/SortedTemplate.h"
#include "Base/MapItem.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<typename K, class E>
using MapBase = SortedTemplate<MapItem<K, E>>;


template<typename K, class E>
class Map : public MapBase<K, E> {

// types
public:

    typedef MapItem<K, E> ItemType;
    typedef typename MapBase<K, E>::Iterator Iterator;

// functions
public:

    Map() {};
    Map(const std::initializer_list<std::pair<K, E>> &initList);

    inline std::pair<Iterator, bool> insert(const K &k, const E &e) {
        return MapBase<K, E>::insertUnique(ItemType(k, e));
    }

    std::pair<Iterator, bool> insertOrAssign(const K &k, const E &e);

    template<typename... Args>
    inline std::pair<Iterator, bool> emplace(const K &k, Args &&... args);

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

};


template<typename K, class E>
Map<K, E>::Map(const std::initializer_list<std::pair<K, E>> &initList) {

    for(auto &item : initList) {
        insert(item.first, item.second);
    }
}


template<typename K, class E>
std::pair<typename Map<K, E>::Iterator, bool> Map<K, E>::insertOrAssign(const K &k, const E &e) {

    auto found = MapBase<K, E>::findSortedPosition(&ItemType::getKey, k);

    if(found.second == false) {
        found.first = MapBase<K, E>::emplaceTo(found.first, k, e);
    } else {
        --found.first;
        found.first->setElement(e);
    }

    found.second = !found.second;
    return found;
}


template<typename K, class E>
template<typename... Args>
std::pair<typename Map<K, E>::Iterator, bool> Map<K, E>::emplace(const K &k, Args &&... args) {

    auto found = MapBase<K, E>::findSortedPosition(&ItemType::getKey, k);

    if(found.second == false) {
        found.first = MapBase<K, E>::emplaceTo(found.first, k, std::forward<Args>(args)...);
    }

    found.second = !found.second;
    return found;
}


template<typename K, class E>
void Map<K, E>::erase(const K &k) {

    auto found = MapBase<K, E>::findSortedPosition(&ItemType::getKey, k);

    if(found.second == true) {
        MapBase<K, E>::erase(--found.first);
    }
}


template<typename K, class E>
typename Map<K, E>::Iterator  Map<K, E>::find(const K &k) const {

    auto found = MapBase<K, E>::findSortedPosition(&ItemType::getKey, k);

    if(found.second == true) {
        return --found.first;
    } else {
        return MapBase<K, E>::end();
    }
}


template<typename K, class E>
E &Map<K, E>::getElement(const K &k) const {

    auto found = MapBase<K, E>::findSortedPosition(&ItemType::getKey, k);

    if(found.second == false) {
        found.first = MapBase<K, E>::emplaceTo(found.first, std::move(ItemType(k)));
    } else {
        --found.first;
    }

    return found.first->getElement();
}


template<typename K, class E>
void Map<K, E>::copyElementsFrom(const Map<K, E> &other) {

    for(ItemType &item : other) {
        insertOrAssign(item.getKey(), item.getElement());
    }
}

}

#endif /* __ETL_MAP_H__ */
