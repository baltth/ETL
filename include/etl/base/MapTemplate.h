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

#ifndef __ETL_MAPTEMPLATE_H__
#define __ETL_MAPTEMPLATE_H__

#include <etl/etlSupport.h>
#include <etl/base/Sorted.h>
#include <etl/base/KeyCompare.h>
#include <etl/base/ListTemplate.h>

#include <utility>

namespace ETL_NAMESPACE {


template<typename K, class E, template<class> class A>
class Map : public Sorted<List<std::pair<const K, E>, A>, KeyCompare<K, E> > {

  public:   // types

    typedef K KeyType;
    typedef E MappedType;
    typedef std::pair<const K, E> ValueType;

    typedef ValueType& Reference;
    typedef const ValueType& ConstReference;
    typedef ValueType* Pointer;
    typedef const ValueType* ConstPointer;

    typedef List<ValueType, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef KeyCompare<K, E> Compare;
    typedef Sorted<ContainerType, Compare> Base;

    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

    typedef Matcher<ValueType> ItemMatcher;

  public:   // functions

    Map() {};

    Map(const Map& other) {
        copyElementsFrom(other);
    }

    Map& operator=(const Map& other) {
        Base::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Map(std::initializer_list<std::pair<K, E>> initList);

#endif

    using Base::find;
    using Base::erase;

    inline std::pair<Iterator, bool> insert(const K& k, const E& e) {
        return Base::insertUnique(ValueType(k, e));
    }

    std::pair<Iterator, bool> insertOrAssign(const K& k, const E& e);

    void erase(const K& k);

    ConstIterator find(const K& k) const;

    Iterator find(const K& k) {
        return Iterator(static_cast<const Map*>(this)->find(k));
    }

    Iterator getItem(const K& k) const;

    E& getElement(const K& k) const {
        return getItem(k)->second;
    }

    void copyElementsFrom(const Map<K, E, A>& other);

    E& operator[](const K& k) {
        return Map<K, E, A>::getElement(k);
    }

    const E& operator[](const K& k) const {
        return Map<K, E, A>::getElement(k);
    }

#if ETL_USE_CPP11

    template<typename... Args>
    inline std::pair<Iterator, bool> emplace(const K& k, Args&& ... args);

#endif

    static K getKey(ConstReference item) {
        return item.first;
    }

};


template<typename K, class E, template<class> class A>
std::pair<typename Map<K, E, A>::Iterator, bool> Map<K, E, A>::insertOrAssign(const K& k, const E& e) {

    std::pair<Iterator, bool> found = Base::findSortedPosition(getKey, k);

    if (found.second == false) {
#if ETL_USE_CPP11
        found.first = Base::emplaceTo(found.first, k, e);
#else
        found.first = Base::insertTo(found.first, ValueType(k, e));
#endif
    } else {
        --found.first;
        found.first->second = e;
    }

    found.second = !found.second;
    return found;
}


template<typename K, class E, template<class> class A>
void Map<K, E, A>::erase(const K& k) {

    std::pair<Iterator, bool> found = Base::findSortedPosition(getKey, k);

    if (found.second == true) {
        Base::erase(--found.first);
    }
}


template<typename K, class E, template<class> class A>
typename Map<K, E, A>::ConstIterator  Map<K, E, A>::find(const K& k) const {

    std::pair<ConstIterator, bool> found = Base::findSortedPosition(getKey, k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<typename K, class E, template<class> class A>
typename Map<K, E, A>::Iterator Map<K, E, A>::getItem(const K& k) const {

    std::pair<ConstIterator, bool> found = Base::findSortedPosition(getKey, k);

    if (found.second == false) {
#if ETL_USE_CPP11
        found.first = Base::emplaceTo(found.first, std::move(ValueType(k, E())));
#else
        found.first = Base::insertTo(found.first, ValueType(k, E()));
#endif
    } else {
        --found.first;
    }

    return Iterator(found.first);
}


template<typename K, class E, template<class> class A>
void Map<K, E, A>::copyElementsFrom(const Map<K, E, A>& other) {

    ConstIterator endIt = other.end();
    for (ConstIterator it = other.begin(); it != endIt; ++it) {
        insertOrAssign(it->first, it->second);
    }
}

#if ETL_USE_CPP11

template<typename K, class E, template<class> class A>
Map<K, E, A>::Map(std::initializer_list<std::pair<K, E>> initList) {

    for (auto& item : initList) {
        insert(item.first, item.second);
    }
}


template<typename K, class E, template<class> class A>
template<typename... Args>
std::pair<typename Map<K, E, A>::Iterator, bool> Map<K, E, A>::emplace(const K& k, Args&& ... args) {

    auto found = Base::findSortedPosition(getKey, k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, k, std::forward<Args>(args)...);
    }

    found.second = !found.second;
    return found;
}

#endif

}

#endif /* __ETL_MAPTEMPLATE_H__ */

