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

#ifndef __ETL_MULTIMAPTEMPLATE_H__
#define __ETL_MULTIMAPTEMPLATE_H__

#include <etl/etlSupport.h>
#include <etl/base/Sorted.h>
#include <etl/base/KeyCompare.h>
#include <etl/base/ListTemplate.h>

#include <utility>

namespace ETL_NAMESPACE {


template<typename K, class E, template<class> class A>
class MultiMap : public Sorted<List<std::pair<const K, E>, A>, KeyCompare<K, E> > {

  public:   // types

    typedef K KeyType;
    typedef E MappedType;
    typedef std::pair<const K, E> value_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef List<value_type, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef KeyCompare<K, E> Compare;
    typedef Sorted<ContainerType, Compare> MapBase;

    typedef typename MapBase::iterator iterator;
    typedef typename MapBase::const_iterator const_iterator;

    typedef Matcher<value_type> ItemMatcher;

  public:   // functions

    MultiMap() {};

    MultiMap(const MultiMap& other) {
        copyElementsFrom(other);
    }

    MultiMap& operator=(const MultiMap& other) {
        MapBase::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    MultiMap(std::initializer_list<std::pair<K, E>> initList);

#endif

    using MapBase::find;
    using MapBase::erase;

    inline iterator insert(const K& k, const E& e) {
        return MapBase::insert(value_type(k, e));
    }

    inline std::pair<iterator, bool> insertUnique(const K& k, const E& e) {
        return MapBase::insertUnique(value_type(k, e));
    }

    uint32_t erase(const K& k);

    iterator find(const K& k);
    const_iterator find(const K& k) const;

    std::pair<iterator, iterator> equalRange(const K& k) {
        return MapBase::findSortedRange(k);
    }

    std::pair<const_iterator, const_iterator> equalRange(const K& k) const {
        return MapBase::findSortedRange(k);
    }

    void copyElementsFrom(const MultiMap<K, E, A>& other);

#if ETL_USE_CPP11

    template<typename... Args>
    inline iterator emplace(const K& k, Args&& ... args);

#endif

    static K getKey(const_reference item) {
        return item.first;
    }

};


template<typename K, class E, template<class> class A>
uint32_t MultiMap<K, E, A>::erase(const K& k) {

    std::pair<iterator, iterator> found = MapBase::findSortedRange(k);
    iterator it = found.first;
    uint32_t count = 0;

    while (it != found.second) {
        it = MapBase::erase(it);
        ++count;
    }

    return count;
}


template<typename K, class E, template<class> class A>
typename MultiMap<K, E, A>::iterator  MultiMap<K, E, A>::find(const K& k) {

    std::pair<iterator, bool> found = MapBase::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return MapBase::end();
    }
}


template<typename K, class E, template<class> class A>
typename MultiMap<K, E, A>::const_iterator  MultiMap<K, E, A>::find(const K& k) const {

    std::pair<const_iterator, bool> found = MapBase::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return MapBase::end();
    }
}


template<typename K, class E, template<class> class A>
void MultiMap<K, E, A>::copyElementsFrom(const MultiMap<K, E, A>& other) {

    const_iterator endIt = other.end();
    for (const_iterator it = other.begin(); it != endIt; ++it) {
        insert(it->first, it->second);
    }
}

#if ETL_USE_CPP11

template<typename K, class E, template<class> class A>
MultiMap<K, E, A>::MultiMap(std::initializer_list<std::pair<K, E>> initList) {

    for (auto& item : initList) {
        insert(item.first, item.second);
    }
}


template<typename K, class E, template<class> class A>
template<typename... Args>
typename MultiMap<K, E, A>::iterator MultiMap<K, E, A>::emplace(const K& k, Args&& ... args) {

    auto found = MapBase::findSortedPosition(k);
    found.first = MapBase::emplaceTo(found.first, k, std::forward<Args>(args)...);

    return found.first;
}

#endif

}

#endif /* __ETL_MULTIMAPTEMPLATE_H__ */

