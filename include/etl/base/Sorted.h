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


template<class C, class Comp = std::less<typename C::ValueType> >
class Sorted {

  public:   // types

    typedef typename C::ValueType ValueType;
    typedef typename C::Reference Reference;
    typedef typename C::ConstReference ConstReference;
    typedef typename C::Pointer Pointer;
    typedef typename C::ConstPointer ConstPointer;

    typedef typename C::Iterator Iterator;
    typedef typename C::ConstIterator ConstIterator;

    typedef typename C::Node Node;

  protected: // variables

    static Comp comp;

    mutable C container;

  public:   // functions

    Sorted() {};

#if ETL_USE_CPP11

    Sorted(std::initializer_list<ValueType> initList) :
        container(initList) {};

#endif

    ///\name Container<> forward
    /// @{
    uint32_t getSize() const {
        return container.getSize();
    }

    bool isEmpty() const {
        return (container.getSize() == 0);
    }

    Iterator begin() {
        return container.begin();
    }

    ConstIterator begin() const {
        return container.begin();
    }

    Iterator end() {
        return container.end();
    }

    ConstIterator end() const {
        return container.end();
    }

    void clear() {
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

    Iterator find(const Matcher<ValueType>& matchCall) {
        return container.find(begin(), end(), matchCall);
    }

    ConstIterator find(const Matcher<ValueType>& matchCall) const {
        return container.find(begin(), end(), matchCall);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<ValueType>& matchCall) {
        return container.find(startPos, endPos, matchCall);
    }

    ConstIterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<ValueType>& matchCall) const {
        return container.find(startPos, endPos, matchCall);
    }

#if ETL_USE_CPP11

    Iterator find(MatchFunc<ValueType>&& matchCall) {
        return container.find(begin(), end(), std::move(matchCall));
    }

    ConstIterator find(MatchFunc<ValueType>&& matchCall) const {
        return container.find(begin(), end(), std::move(matchCall));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, MatchFunc<ValueType>&& matchCall) {
        return container.find(startPos, endPos, std::move(matchCall));
    }

    ConstIterator find(ConstIterator startPos, ConstIterator endPos, MatchFunc<ValueType>&& matchCall) const {
        return container.find(startPos, endPos, std::move(matchCall));
    }

#endif

    void swap(Sorted& other) {
        container.swap(other.container);
    }
    /// @}

  protected:

    Iterator insert(ConstReference item);
    std::pair<Iterator, bool> insertUnique(ConstReference item);

    std::pair<ConstIterator, ConstIterator> findSortedRange(ConstReference item) const;

    std::pair<Iterator, Iterator> findSortedRange(ConstReference item) {
        std::pair<ConstIterator, ConstIterator> res = static_cast<const Sorted*>(this)->findSortedRange(item);
        return std::pair<Iterator, Iterator>(Iterator(res.first), Iterator(res.second));
    }

    template<typename F, typename V>
    std::pair<ConstIterator, ConstIterator> findSortedRange(F f, const V& v) const;

    template<typename F, typename V>
    std::pair<Iterator, Iterator> findSortedRange(F f, const V& v) {
        std::pair<ConstIterator, ConstIterator> res = static_cast<const Sorted*>(this)->findSortedRange<F, V>(f, v);
        return std::pair<Iterator, Iterator>(Iterator(res.first), Iterator(res.second));
    }

    std::pair<Iterator, bool> findSortedPosition(ConstReference item) {
        std::pair<Iterator, Iterator> res = findSortedRange(item);
        return std::pair<Iterator, bool>(res.second, (res.first != res.second));
    }

    std::pair<ConstIterator, bool> findSortedPosition(ConstReference item) const {
        std::pair<ConstIterator, ConstIterator> res = findSortedRange(item);
        return std::pair<ConstIterator, bool>(res.second, (res.first != res.second));
    }

    template<typename F, typename V>
    std::pair<Iterator, bool> findSortedPosition(F f, const V& v) {
        std::pair<Iterator, Iterator> res = findSortedRange<F, V>(f, v);
        return std::pair<Iterator, bool>(res.second, (res.first != res.second));
    }

    template<typename F, typename V>
    std::pair<ConstIterator, bool> findSortedPosition(F f, const V& v) const {
        std::pair<ConstIterator, ConstIterator> res = findSortedRange<F, V>(f, v);
        return std::pair<ConstIterator, bool>(res.second, (res.first != res.second));
    }

    Iterator insertTo(ConstIterator pos, ConstReference item) const {
        return container.insert(pos, item);
    }

#if ETL_USE_CPP11

    template<typename... Args>
    Iterator emplaceTo(ConstIterator pos, Args&& ... args) const {
        return container.emplace(pos, std::forward<Args>(args)...);
    }

#endif

};


template<class C, class Comp /* = std::less<typename C::ValueType> */>
Comp Sorted<C, Comp>::comp;


template<class C, class Comp /* = std::less<typename C::ValueType> */>
typename Sorted<C, Comp>::Iterator Sorted<C, Comp>::insert(ConstReference item) {

    std::pair<Iterator, bool> found = findSortedPosition(item);
    return container.insert(found.first, item);
}


template<class C, class Comp /* = std::less<typename C::ValueType> */>
std::pair<typename Sorted<C, Comp>::Iterator, bool> Sorted<C, Comp>::insertUnique(ConstReference item) {

    std::pair<Iterator, bool> found = findSortedPosition(item);

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


template<class C, class Comp /* = std::less<typename C::ValueType> */>
std::pair<typename Sorted<C, Comp>::ConstIterator, typename Sorted<C, Comp>::ConstIterator>
Sorted<C, Comp>::findSortedRange(ConstReference item) const {

    ConstIterator it = begin();
    ConstIterator endIt = end();

    std::pair<ConstIterator, ConstIterator> res(endIt, endIt);

    if (it != endIt) {

        bool notLessFound = false;

        while ((!notLessFound) && (it != endIt)) {

            if (comp(*it, item)) {
                ++it;
            } else {
                res.first = it;
                notLessFound = true;
            }
        }

        while (notLessFound && (it != endIt)) {

            if (comp(item, *it)) {
                res.second = it;
                notLessFound = false;
            } else {
                ++it;
            }
        }
    }

    return res;
}


template<class C, class Comp /* = std::less<typename C::ValueType> */>
template<typename F, typename V>
std::pair<typename Sorted<C, Comp>::ConstIterator, typename Sorted<C, Comp>::ConstIterator>
Sorted<C, Comp>::findSortedRange(F f, const V& v) const {

    ConstIterator it = begin();
    ConstIterator endIt = end();

    std::pair<ConstIterator, ConstIterator> res(endIt, endIt);

    if (it != endIt) {

        bool notLessFound = false;

        while ((!notLessFound) && (it != endIt)) {

            if ((*f)(*it) < v) {
                ++it;
            } else {
                res.first = it;
                notLessFound = true;
            }
        }

        while (notLessFound && (it != endIt)) {

            if (v < (*f)(*it)) {
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

