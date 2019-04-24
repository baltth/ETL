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
#include <etl/traitSupport.h>
#include <etl/base/SortedList.h>
#include <etl/base/KeyCompare.h>

#include <utility>

namespace ETL_NAMESPACE {


template<typename K, class E>
class Map : public Detail::SortedList<std::pair<const K, E>, KeyCompare<K, E>> {

  public:   // types

    typedef K key_type;
    typedef E mapped_type;
    typedef std::pair<const K, E> value_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef KeyCompare<K, E> Compare;
    typedef Detail::SortedList<value_type, Compare> Base;
    typedef typename Base::Cont ContainerType;
    typedef typename ContainerType::AllocatorBase AllocatorBase;

    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

    typedef Matcher<value_type> ItemMatcher;

  public:   // functions

    Map(AllocatorBase& a) :
        Base(a) {};

    Map& operator=(const Map& other) {
        assign(other.begin(), other.end());
        return *this;
    }

    Map& operator=(Map&& other) {
        swap(other);
        return *this;
    }

    Map& operator=(std::initializer_list<std::pair<K, E>> initList) {
        assign(initList);
        return *this;
    }

    using Base::find;
    using Base::erase;

    std::pair<iterator, bool> insert(const value_type& item) {
        return Base::insertUnique(item);
    }

    template<class InputIt>
    enable_if_t<!is_integral<InputIt>::value>
    insert(InputIt first, InputIt last) {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    std::pair<iterator, bool> insert(const K& k, const E& e) {
        return insert(value_type(k, e));
    }

    std::pair<iterator, bool> insert_or_assign(const K& k, const E& e);

    void erase(const K& k);

    iterator find(const K& k);
    const_iterator find(const K& k) const;

    iterator getItem(const K& k);

    E& operator[](const K& k) {
        return getItem(k)->second;
    }

    void insert(std::initializer_list<value_type> initList) {
        insert(initList.begin(), initList.end());
    }

    template<typename... Args>
    inline std::pair<iterator, bool> emplace(const K& k, Args&& ... args);

    static K getKey(const_reference item) {
        return item.first;
    }

  protected:

    template<typename InputIt>
    void assign(InputIt first, InputIt last) {
        this->clear();
        insert(first, last);
    }

    template<class Cont>
    void assign(const Cont& other) {
        assign(other.begin(), other.end());
    }

};


template<typename K, class E>
std::pair<typename Map<K, E>::iterator, bool> Map<K, E>::insert_or_assign(const K& k, const E& e) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, k, e);
    } else {
        --found.first;
        found.first->second = e;
    }

    found.second = !found.second;
    return found;
}


template<typename K, class E>
void Map<K, E>::erase(const K& k) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        Base::erase(--found.first);
    }
}


template<typename K, class E>
typename Map<K, E>::iterator Map<K, E>::find(const K& k) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<typename K, class E>
typename Map<K, E>::const_iterator Map<K, E>::find(const K& k) const {

    std::pair<const_iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<typename K, class E>
typename Map<K, E>::iterator Map<K, E>::getItem(const K& k) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, std::move(value_type(k, E())));
    } else {
        --found.first;
    }

    return found.first;
}


template<typename K, class E>
template<typename... Args>
std::pair<typename Map<K, E>::iterator, bool> Map<K, E>::emplace(const K& k, Args&& ... args) {

    auto found = Base::findSortedPosition(k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, k, std::forward<Args>(args)...);
    }

    found.second = !found.second;
    return found;
}

}

#endif /* __ETL_MAPTEMPLATE_H__ */

