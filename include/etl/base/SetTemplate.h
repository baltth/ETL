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

#ifndef __ETL_SETTEMPLATE_H__
#define __ETL_SETTEMPLATE_H__

#include <etl/etlSupport.h>
#include <etl/traitSupport.h>
#include <etl/base/SortedList.h>

namespace ETL_NAMESPACE {


template<class E>
class Set : private Detail::SortedList<E> {

  public:   // types

    typedef E key_type;
    typedef E value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef Detail::SortedList<E> Base;
    typedef typename Base::Cont ContainerType;
    typedef typename Base::Node Node;
    typedef typename ContainerType::AllocatorBase AllocatorBase;

    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::reverse_iterator reverse_iterator;
    typedef typename Base::const_reverse_iterator const_reverse_iterator;

    typedef typename Base::size_type size_type;

  public:   // functions

    /// \name Construction, destruction, assignment
    /// \{

    Set(AllocatorBase& a) :
        Base(a) {};

    Set& operator=(const Set& other) {
        assign(other.begin(), other.end());
        return *this;
    }

    Set& operator=(Set&& other) {
        swap(other);
        return *this;
    }

    Set& operator=(std::initializer_list<E> initList) {
        assign(initList);
        return this;
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

    void erase(const E& e);

    std::pair<iterator, bool> insert(const E& e) {
        return Base::insertUnique(e);
    }

    template<class InputIt>
    enable_if_t<!is_integral<InputIt>::value>
    insert(InputIt first, InputIt last) {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    void swap(Set& other) {
        Base::swap(other);
    }
    /// \}

    /// \name Lookup
    /// \{

    iterator find(const E& e);
    const_iterator find(const E& e) const;
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

};


template<class E>
void Set<E>::erase(const E& e) {

    std::pair<iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        Base::erase(--found.first);
    }
}


template<class E>
auto Set<E>::find(const E& e) -> iterator {

    std::pair<iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class E>
auto Set<E>::find(const E& e) const -> const_iterator {

    std::pair<const_iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}

}

#endif /* __ETL_SETTEMPLATE_H__ */

