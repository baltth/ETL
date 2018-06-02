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

    typedef K key_type;
    typedef E mapped_type;
    typedef std::pair<const K, E> value_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef List<value_type, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef KeyCompare<K, E> Compare;
    typedef Sorted<ContainerType, Compare> Base;

    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

    typedef Matcher<value_type> ItemMatcher;

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

    std::pair<iterator, bool> insert(const K& k, const E& e) {
        return Base::insertUnique(value_type(k, e));
    }

    std::pair<iterator, bool> insertOrAssign(const K& k, const E& e);

    void erase(const K& k);

    iterator find(const K& k);
    const_iterator find(const K& k) const;

    iterator getItem(const K& k);

    E& operator[](const K& k) {
        return getItem(k)->second;
    }

    void copyElementsFrom(const Map<K, E, A>& other);

#if ETL_USE_CPP11

    template<typename... Args>
    inline std::pair<iterator, bool> emplace(const K& k, Args&& ... args);

#endif

    static K getKey(const_reference item) {
        return item.first;
    }

};


template<typename K, class E, template<class> class A>
std::pair<typename Map<K, E, A>::iterator, bool> Map<K, E, A>::insertOrAssign(const K& k, const E& e) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == false) {
#if ETL_USE_CPP11
        found.first = Base::emplaceTo(found.first, k, e);
#else
        found.first = Base::insertTo(found.first, value_type(k, e));
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

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        Base::erase(--found.first);
    }
}


template<typename K, class E, template<class> class A>
typename Map<K, E, A>::iterator  Map<K, E, A>::find(const K& k) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<typename K, class E, template<class> class A>
typename Map<K, E, A>::const_iterator  Map<K, E, A>::find(const K& k) const {

    std::pair<const_iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<typename K, class E, template<class> class A>
typename Map<K, E, A>::iterator Map<K, E, A>::getItem(const K& k) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == false) {
#if ETL_USE_CPP11
        found.first = Base::emplaceTo(found.first, std::move(value_type(k, E())));
#else
        found.first = Base::insertTo(found.first, value_type(k, E()));
#endif
    } else {
        --found.first;
    }

    return found.first;
}


template<typename K, class E, template<class> class A>
void Map<K, E, A>::copyElementsFrom(const Map<K, E, A>& other) {

    const_iterator endIt = other.end();
    for (const_iterator it = other.begin(); it != endIt; ++it) {
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
std::pair<typename Map<K, E, A>::iterator, bool> Map<K, E, A>::emplace(const K& k, Args&& ... args) {

    auto found = Base::findSortedPosition(k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, k, std::forward<Args>(args)...);
    }

    found.second = !found.second;
    return found;
}

#endif

}

#endif /* __ETL_MAPTEMPLATE_H__ */

