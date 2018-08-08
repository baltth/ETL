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

#ifndef __ETL_SORTED_H__
#define __ETL_SORTED_H__

#include <etl/etlSupport.h>

#include <functional>

namespace ETL_NAMESPACE {


template<class C, class Comp = std::less<typename C::value_type> >
class Sorted {

  public:   // types

    typedef typename C::value_type value_type;
    typedef typename C::reference reference;
    typedef typename C::const_reference const_reference;
    typedef typename C::pointer pointer;
    typedef typename C::const_pointer const_pointer;

    typedef typename C::iterator iterator;
    typedef typename C::const_iterator const_iterator;

    typedef typename C::Node Node;

  protected: // variables

    static Comp comp;

    C container;

  public:   // functions

    Sorted(typename C::AllocatorBase& a) :
        container(a) {};

    ///\name Container<> forward
    /// \{
    uint32_t size() const {
        return container.size();
    }

    bool empty() const {
        return (container.size() == 0);
    }

    iterator begin() {
        return container.begin();
    }

    const_iterator begin() const {
        return container.begin();
    }

    const_iterator cbegin() const {
        return container.cbegin();
    }

    iterator end() {
        return container.end();
    }

    const_iterator end() const {
        return container.end();
    }

    const_iterator cend() const {
        return container.cend();
    }

    void clear() {
        container.clear();
    }

    iterator erase(iterator pos) {
        return container.erase(pos);
    }

    iterator find(const Matcher<value_type>& matchCall) {
        return container.find(begin(), end(), matchCall);
    }

    const_iterator find(const Matcher<value_type>& matchCall) const {
        return container.find(begin(), end(), matchCall);
    }

    iterator find(const_iterator startPos, const_iterator endPos, const Matcher<value_type>& matchCall) {
        return container.find(startPos, endPos, matchCall);
    }

    const_iterator find(const_iterator startPos, const_iterator endPos, const Matcher<value_type>& matchCall) const {
        return container.find(startPos, endPos, matchCall);
    }

#if ETL_USE_CPP11

    iterator find(MatchFunc<value_type>&& matchCall) {
        return container.find(begin(), end(), std::move(matchCall));
    }

    const_iterator find(MatchFunc<value_type>&& matchCall) const {
        return container.find(begin(), end(), std::move(matchCall));
    }

    iterator find(const_iterator startPos, const_iterator endPos, MatchFunc<value_type>&& matchCall) {
        return container.find(startPos, endPos, std::move(matchCall));
    }

    const_iterator find(const_iterator startPos, const_iterator endPos, MatchFunc<value_type>&& matchCall) const {
        return container.find(startPos, endPos, std::move(matchCall));
    }

#endif

    void swap(Sorted& other) {
        container.swap(other.container);
    }
    /// \}

  protected:

    template<typename It, typename CV, class CF>
    std::pair<It, It> findSortedRangeBase(It it, It endIt, const CV& val, CF& compare) const;

    template<typename CV>
    std::pair<iterator, iterator> findSortedRange(const CV& val) {
        return findSortedRangeBase(this->begin(), this->end(), val, comp);
    }

    template<typename CV>
    std::pair<const_iterator, const_iterator> findSortedRange(const CV& val) const {
        return findSortedRangeBase(this->cbegin(), this->cend(), val, comp);
    }

    template<typename CV, class CF>
    std::pair<iterator, iterator> findSortedRange(const CV& val, CF& compare) {
        return findSortedRangeBase(this->begin(), this->end(), val, compare);
    }

    template<typename CV, class CF>
    std::pair<const_iterator, const_iterator> findSortedRange(const CV& val, CF& compare) const {
        return findSortedRangeBase(this->cbegin(), this->cend(), val, compare);
    }

    template<typename CV>
    std::pair<iterator, bool> findSortedPosition(const CV& val) {
        std::pair<iterator, iterator> res = findSortedRange(val);
        return std::pair<iterator, bool>(res.second, (res.first != res.second));
    }

    template<typename CV>
    std::pair<const_iterator, bool> findSortedPosition(const CV& val) const {
        std::pair<const_iterator, const_iterator> res = findSortedRange(val);
        return std::pair<const_iterator, bool>(res.second, (res.first != res.second));
    }

    iterator insert(const_reference item);
    std::pair<iterator, bool> insertUnique(const_reference item);

    iterator insertTo(const_iterator pos, const_reference item) {
        return container.insert(pos, item);
    }

#if ETL_USE_CPP11

    template<typename... Args>
    iterator emplaceTo(const_iterator pos, Args&& ... args) {
        return container.emplace(pos, std::forward<Args>(args)...);
    }

#endif

};


template<class C, class Comp /* = std::less<typename C::value_type> */>
Comp Sorted<C, Comp>::comp;


template<class C, class Comp /* = std::less<typename C::value_type> */>
typename Sorted<C, Comp>::iterator Sorted<C, Comp>::insert(const_reference item) {

    std::pair<iterator, bool> found = findSortedPosition(item);
    return container.insert(found.first, item);
}


template<class C, class Comp /* = std::less<typename C::value_type> */>
std::pair<typename Sorted<C, Comp>::iterator, bool> Sorted<C, Comp>::insertUnique(const_reference item) {

    std::pair<iterator, bool> found = findSortedPosition(item);

    if (found.second == false) {

        found.first = container.insert(found.first, item);
        if (found.first != end()) {
            found.second = true;
        }

    } else {
        --found.first;
        found.second = false;
    }

    return found;
}


template<class C, class Comp /* = std::less<typename C::value_type> */>
template<typename It, typename CV, class CF>
std::pair<It, It> Sorted<C, Comp>::findSortedRangeBase(It it, It endIt, const CV& val, CF& compare) const {

    std::pair<It, It> res(endIt, endIt);

    if (it != endIt) {

        bool notLessFound = false;

        while ((!notLessFound) && (it != endIt)) {

            if (compare(*it, val)) {
                ++it;
            } else {
                res.first = it;
                notLessFound = true;
            }
        }

        while (notLessFound && (it != endIt)) {

            if (compare(val, *it)) {
                res.second = it;
                notLessFound = false;
            } else {
                ++it;
            }
        }
    }

    return res;
}

}

#endif /* __ETL_SORTED_H__ */

