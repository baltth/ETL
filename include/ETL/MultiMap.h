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

#ifndef __ETL_MULTIMAP_H__
#define __ETL_MULTIMAP_H__

#include <ETL/etlSupport.h>
#include <ETL/base/Sorted.h>
#include <ETL/base/MapItem.h>
#include <ETL/PoolAllocator.h>

#include <memory>
#include <utility>

namespace ETL_NAMESPACE {


template<typename K, class E, template<class> class A = std::allocator>
class MultiMap : public Sorted<ListTemplate<MapItem<K, E>, A> > {

  public:   // types

    typedef MapItem<K, E> ItemType;
    typedef ListTemplate<ItemType, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef Sorted<ContainerType> MapBase;
    typedef typename MapBase::Iterator Iterator;
    typedef typename MapBase::ConstIterator ConstIterator;
    typedef Matcher<ItemType> ItemMatcher;

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

    MultiMap(const std::initializer_list<std::pair<K, E>>& initList);

#endif

    using MapBase::find;
    using MapBase::erase;

    inline Iterator insert(const K& k, const E& e) {
        return MapBase::insert(ItemType(k, e));
    }

    inline std::pair<Iterator, bool> insertUnique(const K& k, const E& e) {
        return MapBase::insertUnique(ItemType(k, e));
    }

    uint32_t erase(const K& k);

    ConstIterator find(const K& k) const;

    Iterator find(const K& k) {
        return Iterator(static_cast<const MultiMap*>(this)->find(k));
    }

    std::pair<ConstIterator, ConstIterator> equalRange(const K& k) const {
        return MapBase::findSortedRange(&ItemType::getKey, k);
    }

    std::pair<Iterator, Iterator> equalRange(const K& k) {
        return MapBase::findSortedRange(&ItemType::getKey, k);
    }

    void copyElementsFrom(const MultiMap<K, E, A>& other);

#if ETL_USE_CPP11

    template<typename... Args>
    inline Iterator emplace(const K& k, Args&& ... args);

#endif

};


template<typename K, class E, template<class> class A>
uint32_t MultiMap<K, E, A>::erase(const K& k) {

    std::pair<Iterator, Iterator> found = MapBase::findSortedRange(&ItemType::getKey, k);
    Iterator it = found.first;
    uint32_t count = 0;

    while (it != found.second) {
        it = MapBase::erase(it);
        ++count;
    }

    return count;
}


template<typename K, class E, template<class> class A>
typename MultiMap<K, E, A>::ConstIterator  MultiMap<K, E, A>::find(const K& k) const {

    std::pair<ConstIterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if (found.second == true) {
        return --found.first;
    } else {
        return MapBase::end();
    }
}


template<typename K, class E, template<class> class A>
void MultiMap<K, E, A>::copyElementsFrom(const MultiMap<K, E, A>& other) {

    ConstIterator endIt = other.end();
    for (ConstIterator it = other.begin(); it != endIt; ++it) {
        insert(it->getKey(), it->getElement());
    }
}

#if ETL_USE_CPP11

template<typename K, class E, template<class> class A>
MultiMap<K, E, A>::MultiMap(const std::initializer_list<std::pair<K, E>>& initList) {

    for (auto& item : initList) {
        insert(item.first, item.second);
    }
}


template<typename K, class E, template<class> class A>
template<typename... Args>
typename MultiMap<K, E, A>::Iterator MultiMap<K, E, A>::emplace(const K& k, Args&& ... args) {

    auto found = MapBase::findSortedPosition(&ItemType::getKey, k);
    found.first = MapBase::emplaceTo(found.first, k, std::forward<Args>(args)...);

    return found.first;
}

#endif


namespace Pooled {


template<class K, class E, uint32_t N>
class MultiMap : public ETL_NAMESPACE::MultiMap<K, E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::MultiMap<K, E, ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator> MapBase;
    typedef typename MapBase::Iterator Iterator;
    typedef typename MapBase::ConstIterator ConstIterator;

  public:   // functions

    MultiMap() {};

    MultiMap(const MapBase& other) :
        MapBase(other) {};

    MultiMap& operator=(const MapBase& other) {
        MapBase::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    MultiMap(const std::initializer_list<std::pair<K, E>>& initList) :
        MapBase(initList) {};

#endif

};

}

}

#endif /* __ETL_MULTIMAP_H__ */

