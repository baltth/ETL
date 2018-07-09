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
    typedef Sorted<ContainerType, Compare> Base;

    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

    typedef Matcher<value_type> ItemMatcher;

  public:   // functions

    MultiMap() {};

    MultiMap(const MultiMap& other) {
        assign(other);
    }

    MultiMap& operator=(const MultiMap& other) {
        assign(other);
        return *this;
    }

#if ETL_USE_CPP11

    MultiMap(std::initializer_list<std::pair<K, E>> initList);

#endif

    using Base::find;
    using Base::erase;

    iterator insert(const value_type& item) {
        return Base::insert(item);
    }

    template<class InputIt>
    typename std::enable_if<!std::is_integral<InputIt>::value>::type    // *NOPAD*
    insert(InputIt first, InputIt last) {
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

    uint32_t erase(const K& k);

    iterator find(const K& k);
    const_iterator find(const K& k) const;

    std::pair<iterator, iterator> equal_range(const K& k) {
        return Base::findSortedRange(k);
    }

    std::pair<const_iterator, const_iterator> equal_range(const K& k) const {
        return Base::findSortedRange(k);
    }

#if ETL_USE_CPP11

    template<typename... Args>
    inline iterator emplace(const K& k, Args&& ... args);

#endif

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


template<typename K, class E, template<class> class A>
uint32_t MultiMap<K, E, A>::erase(const K& k) {

    std::pair<iterator, iterator> found = Base::findSortedRange(k);
    iterator it = found.first;
    uint32_t count = 0;

    while (it != found.second) {
        it = Base::erase(it);
        ++count;
    }

    return count;
}


template<typename K, class E, template<class> class A>
typename MultiMap<K, E, A>::iterator  MultiMap<K, E, A>::find(const K& k) {

    std::pair<iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<typename K, class E, template<class> class A>
typename MultiMap<K, E, A>::const_iterator  MultiMap<K, E, A>::find(const K& k) const {

    std::pair<const_iterator, bool> found = Base::findSortedPosition(k);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
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

    auto found = Base::findSortedPosition(k);
    found.first = Base::emplaceTo(found.first, k, std::forward<Args>(args)...);

    return found.first;
}

#endif

}

#endif /* __ETL_MULTIMAPTEMPLATE_H__ */

