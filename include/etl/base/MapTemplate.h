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
#include <etl/base/tools.h>

#include <utility>
#include <functional>

namespace ETL_NAMESPACE {


template<class K, class E, class C = std::less<K>>
class Map : private Detail::SortedList<std::pair<const K, E>, Detail::KeyCompare<C>> {

  public:   // types

    typedef K key_type;
    typedef E mapped_type;
    typedef std::pair<const K, E> value_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef C key_compare;
    typedef Detail::SortedList<value_type, Detail::KeyCompare<C>> Base;
    typedef typename Base::Node Node;
    typedef typename Base::Cont ContainerType;
    typedef typename ContainerType::AllocatorBase AllocatorBase;

    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::reverse_iterator reverse_iterator;
    typedef typename Base::const_reverse_iterator const_reverse_iterator;

    typedef typename Base::size_type size_type;

  public:   // functions

    /// \name Construction, destruction, assignment
    /// \{

    Map(AllocatorBase& a) noexcept :
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
    /// \}

    /// \name Capacity
    /// \{
    using Base::size;
    using Base::max_size;
    using Base::empty;
    /// \}

    /// \name Element access
    /// \{

    E& operator[](const K& k) {
        return getItem(k)->second;
    }

    E& operator[](K&& k) {
        return getItem(k)->second;
    }
    /// \}

    /// \name Iterators
    /// \{
    using Base::begin;
    using Base::cbegin;
    using Base::end;
    using Base::cend;
    using Base::rbegin;
    using Base::crbegin;
    using Base::rend;
    using Base::crend;
    /// \}

    /// \name Modifiers
    /// \{
    using Base::clear;
    using Base::erase;

    void erase(const K& k);

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

    void insert(std::initializer_list<value_type> initList) {
        insert(initList.begin(), initList.end());
    }

    template<typename... Args>
    inline std::pair<iterator, bool> emplace(const K& k, Args&& ... args);

    std::pair<iterator, bool> insert_or_assign(const K& k, const E& e);

    void swap(Map& other) {
        Base::swap(other);
    }
    /// \}

    /// \name Lookup
    /// \{

    iterator find(const K& k);
    const_iterator find(const K& k) const;
    /// \}

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

    iterator getItem(const K& k);

};


template<class K, class E, class C>
auto Map<K, E, C>::insert_or_assign(const K& k, const E& e) -> std::pair<iterator, bool> {

    auto found = Base::findSortedPosition(k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, k, e);
    } else {
        --found.first;
        found.first->second = e;
    }

    found.second = !found.second;
    return found;
}


template<class K, class E, class C>
void Map<K, E, C>::erase(const K& k) {

    auto found = Base::findSortedPosition(k);

    if (found.second == true) {
        Base::erase(--found.first);
    }
}


template<class K, class E, class C>
auto Map<K, E, C>::find(const K& k) -> iterator {

    auto found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class K, class E, class C>
auto Map<K, E, C>::find(const K& k) const -> const_iterator {

    auto found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class K, class E, class C>
auto Map<K, E, C>::getItem(const K& k) -> iterator {

    auto found = Base::findSortedPosition(k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, std::move(value_type(k, E())));
    } else {
        --found.first;
    }

    return found.first;
}


template<class K, class E, class C>
template<typename... Args>
auto Map<K, E, C>::emplace(const K& k, Args&& ... args) -> std::pair<iterator, bool> {

    auto found = Base::findSortedPosition(k);

    if (found.second == false) {
        found.first = Base::emplaceTo(found.first, k, std::forward<Args>(args)...);
    }

    found.second = !found.second;
    return found;
}


template<class K, class E, class C>
bool operator==(const Map<K, E, C>& lhs, const Map<K, E, C>& rhs) {
    return Detail::isEqual(lhs, rhs);
}

template<class K, class E, class C>
bool operator!=(const Map<K, E, C>& lhs, const Map<K, E, C>& rhs) {
    return !(lhs == rhs);
}

template<class K, class E, class C>
bool operator<(const Map<K, E, C>& lhs, const Map<K, E, C>& rhs) {
    return Detail::isLess(lhs, rhs);
}

template<class K, class E, class C>
bool operator<=(const Map<K, E, C>& lhs, const Map<K, E, C>& rhs) {
    return !(rhs < lhs);
}

template<class K, class E, class C>
bool operator>(const Map<K, E, C>& lhs, const Map<K, E, C>& rhs) {
    return (rhs < lhs);
}

template<class K, class E, class C>
bool operator>=(const Map<K, E, C>& lhs, const Map<K, E, C>& rhs) {
    return !(lhs < rhs);
}


template<class K, class E, class C>
void swap(Map<K, E, C>& lhs, Map<K, E, C>& rhs) {
    lhs.swap(rhs);
}

}

#endif /* __ETL_MAPTEMPLATE_H__ */

