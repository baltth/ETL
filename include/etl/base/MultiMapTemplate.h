/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2024 Balazs Toth.

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

#ifndef ETL_MULTIMAPTEMPLATE_H_
#define ETL_MULTIMAPTEMPLATE_H_

#include <etl/base/KeyCompare.h>
#include <etl/base/SortedList.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <functional>
#include <utility>

namespace ETL_NAMESPACE {


template<class K, class E, class C = std::less<K>>
class MultiMap : private Detail::SortedList<std::pair<const K, E>, Detail::KeyCompare<C>> {

  public:  // types

    using key_type = K;
    using mapped_type = E;
    using value_type = std::pair<const K, E>;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using key_compare = C;
    using Base = Detail::SortedList<value_type, Detail::KeyCompare<C>>;
    using Node = typename Base::Node;
    using ContainerType = typename Base::Cont;
    using AllocatorBase = typename ContainerType::AllocatorBase;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = typename Base::reverse_iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;

    using size_type = typename Base::size_type;

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{

    MultiMap(AllocatorBase& a) noexcept :
        Base(a) {};

    MultiMap& operator=(const MultiMap& other) {
        assign(other.begin(), other.end());
        return *this;
    }

    MultiMap& operator=(MultiMap&& other) {
        swap(other);
        return *this;
    }

    MultiMap& operator=(std::initializer_list<value_type> initList) {
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

    size_type erase(const K& k);

    iterator insert(const value_type& item) {
        return Base::insert(item);
    }

    template<class InputIt>
    enable_if_t<!is_integral<InputIt>::value> insert(InputIt first, InputIt last) {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    std::pair<iterator, bool> insert_unique(const K& k, const E& e) {
        return Base::insertUnique(value_type(k, e));
    }

    iterator insert(const K& k, const E& e) {
        return insert(value_type(k, e));
    }

    template<typename... Args>
    inline iterator emplace(const K& k, Args&&... args);

    void swap(MultiMap& other) {
        Base::swap(other);
    }
    /// \}

    /// \name Lookup
    /// \{

    iterator find(const K& k);
    const_iterator find(const K& k) const;

    std::pair<iterator, iterator> equal_range(const K& k) {
        return Base::findSortedRange(k);
    }

    std::pair<const_iterator, const_iterator> equal_range(const K& k) const {
        return Base::findSortedRange(k);
    }

    iterator lower_bound(const K& k) {
        return equal_range(k).first;
    }

    const_iterator lower_bound(const K& k) const {
        return equal_range(k).first;
    }

    iterator upper_bound(const K& k) {
        return equal_range(k).second;
    }

    const_iterator upper_bound(const K& k) const {
        return equal_range(k).second;
    }
    /// \}

  protected:

    void
    swapNodeList(MultiMap& other) noexcept(noexcept(std::declval<Base>().swapNodeList(other))) {
        Base::swapNodeList(other);
    }

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


template<class K, class E, class C>
auto MultiMap<K, E, C>::erase(const K& k) -> size_type {

    auto found = Base::findSortedRange(k);
    iterator it = found.first;
    size_type count = 0;

    while (it != found.second) {
        it = Base::erase(it);
        ++count;
    }

    return count;
}


template<class K, class E, class C>
auto MultiMap<K, E, C>::find(const K& k) -> iterator {

    auto found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class K, class E, class C>
auto MultiMap<K, E, C>::find(const K& k) const -> const_iterator {

    auto found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class K, class E, class C>
template<typename... Args>
auto MultiMap<K, E, C>::emplace(const K& k, Args&&... args) -> iterator {

    auto found = Base::findSortedPosition(k);
    found.first = Base::emplaceTo(found.first, k, std::forward<Args>(args)...);

    return found.first;
}


template<class K, class E, class C>
bool operator==(const MultiMap<K, E, C>& lhs, const MultiMap<K, E, C>& rhs) {
    return Detail::isEqual(lhs, rhs);
}

template<class K, class E, class C>
bool operator!=(const MultiMap<K, E, C>& lhs, const MultiMap<K, E, C>& rhs) {
    return !(lhs == rhs);
}

template<class K, class E, class C>
bool operator<(const MultiMap<K, E, C>& lhs, const MultiMap<K, E, C>& rhs) {
    return Detail::isLess(lhs, rhs);
}

template<class K, class E, class C>
bool operator<=(const MultiMap<K, E, C>& lhs, const MultiMap<K, E, C>& rhs) {
    return !(rhs < lhs);
}

template<class K, class E, class C>
bool operator>(const MultiMap<K, E, C>& lhs, const MultiMap<K, E, C>& rhs) {
    return (rhs < lhs);
}

template<class K, class E, class C>
bool operator>=(const MultiMap<K, E, C>& lhs, const MultiMap<K, E, C>& rhs) {
    return !(lhs < rhs);
}


template<class K, class E, class C>
void swap(MultiMap<K, E, C>& lhs, MultiMap<K, E, C>& rhs) {
    lhs.swap(rhs);
}

}  // namespace ETL_NAMESPACE

#endif  // ETL_MULTIMAPTEMPLATE_H_
