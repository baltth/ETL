/**
\file
\date 2016.01.22. 9:59:14
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

#ifndef __ETL_SORTED_H__
#define __ETL_SORTED_H__

#include "etlSupport.h"

#include "Base/ListTemplate.h"

#include <functional>

namespace ETL_NAMESPACE {


template<class C, class Comp = std::less<typename C::ItemType> >
class Sorted {

  public:   // types

    typedef typename C::ItemType ItemType;
    typedef typename C::Iterator Iterator;
    typedef typename C::ConstIterator ConstIterator;
    typedef typename C::Node Node;

  protected: // variables

    static Comp comp;

    mutable C container;

  public:   // functions

    Sorted() {};

#if ETL_USE_CPP11

    Sorted(const std::initializer_list<ItemType>& initList) :
        container(initList) {};

#endif

    ///\name Container<> forward
    /// @{
    inline uint32_t getSize() const {
        return container.getSize();
    }

    inline Iterator begin() const {
        return Iterator(container.begin());
    }

    inline Iterator end() const {
        return Iterator(container.end());
    }

    inline void clear() {
        container.clear();
    }

    Iterator erase(Iterator pos) {
        return container.erase(pos);
    }

    template<typename F, typename V>
    Iterator find(F f, const V& v) const {
        return container.find(begin(), end(), f, v);
    }

    template<typename F, typename V>
    Iterator find(ConstIterator startPos, ConstIterator endPos, F f, const V& v) const {
        return container.find(startPos, endPos, f, v);
    }

#if ETL_USE_CPP11

    Iterator find(Matcher<ItemType>&& matchCall) const {
        return container.find(begin(), end(), std::move(matchCall));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, Matcher<ItemType>&& matchCall) const {
        return container.find(startPos, endPos, std::move(matchCall));
    }
#else

    Iterator find(const Matcher<ItemType>& matchCall) const {
        return container.find(begin(), end(), matchCall);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<ItemType>& matchCall) const {
        return container.find(startPos, endPos, matchCall);
    }

#endif
    /// @}

  protected:

    Iterator insert(const ItemType& item);
    std::pair<Iterator, bool> insertUnique(const ItemType& item);

    std::pair<Iterator, bool> findSortedPosition(const ItemType& item) const;

    template<typename F, typename V>
    std::pair<Iterator, bool> findSortedPosition(F f, const V& v) const;

    Iterator insertTo(ConstIterator pos, const ItemType& item) const {
        return container.insert(pos, item);
    }

#if ETL_USE_CPP11

    template<typename... Args>
    Iterator emplaceTo(ConstIterator pos, Args&& ... args) const {
        return container.emplace(pos, std::forward<Args>(args)...);
    }

#endif

};


template<class C, class Comp /* = std::less<typename C::ItemType> */>
Comp Sorted<C, Comp>::comp;


template<class C, class Comp /* = std::less<typename C::ItemType> */>
typename Sorted<C, Comp>::Iterator Sorted<C, Comp>::insert(const ItemType& item) {

    std::pair<Iterator, bool> found = findSortedPosition(item);
    return container.insert(found.first, item);
}


template<class C, class Comp /* = std::less<typename C::ItemType> */>
std::pair<typename Sorted<C, Comp>::Iterator, bool> Sorted<C, Comp>::insertUnique(const ItemType& item) {

    std::pair<Iterator, bool> found = findSortedPosition(item);

    if (found.second == false) {
        found.first = container.insert(found.first, item);
    } else {
        --found.first;
    }

    found.second = !found.second;

    return found;
}


template<class C, class Comp /* = std::less<typename C::ItemType> */>
std::pair<typename Sorted<C, Comp>::Iterator, bool> Sorted<C, Comp>::findSortedPosition(const ItemType& item) const {

    Iterator startIt = begin();
    Iterator endIt = end();
    Iterator it = startIt;

    if (it == endIt) {
        return std::pair<typename Sorted<C, Comp>::Iterator, bool>(it, false);
    }

    bool notLessFound = false;
    bool equalFound = false;

    while ((!notLessFound) && (it != endIt)) {

        if (comp(*it, item)) {
            ++it;
        } else {
            notLessFound = true;
        }
    }

    while (notLessFound && (it != endIt)) {

        if (comp(item, *it)) {
            notLessFound = false;
        } else {
            equalFound = true;
            ++it;
        }
    }

    return std::pair<typename Sorted<C, Comp>::Iterator, bool>(it, equalFound);
}


template<class C, class Comp /* = std::less<typename C::ItemType> */>
template<typename F, typename V>
std::pair<typename Sorted<C, Comp>::Iterator, bool>
Sorted<C, Comp>::findSortedPosition(F f, const V& v) const {


    Iterator startIt = begin();
    Iterator endIt = end();
    Iterator it = startIt;

    if (it == endIt) {
        return std::pair<typename Sorted<C, Comp>::Iterator, bool>(it, false);
    }

    bool notLessFound = false;
    bool equalFound = false;

    while ((!notLessFound) && (it != endIt)) {

        if (((*it).*f)() < v) {
            ++it;
        } else {
            notLessFound = true;
        }
    }

    while (notLessFound && (it != endIt)) {

        if (v < ((*it).*f)()) {
            notLessFound = false;
        } else {
            equalFound = true;
            ++it;
        }
    }

    return std::pair<typename Sorted<C, Comp>::Iterator, bool>(it, equalFound);
}

}

#endif /* __ETL_SORTED_H__ */

