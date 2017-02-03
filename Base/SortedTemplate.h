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

#ifndef __ETL_SORTEDTEMPLATE_H__
#define __ETL_SORTEDTEMPLATE_H__

#include "ETL/Base/ListTemplate.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<class T, class Comp = std::less<T>>
class SortedTemplate {

// types
protected:

    class Node : public ListTemplate<T>::Node {

    public:

        template<typename... Args>
        Node(Args &&... args) :
            ListTemplate<T>::Node(args...) {};

    };

public:

    typedef typename ListTemplate<T>::Iterator Iterator;

// variables
protected:

    static Comp comp;

    mutable ListTemplate<T> list;

// functions
public:

    SortedTemplate() = default;
    SortedTemplate(const std::initializer_list<T> &initList) :
        list(initList) {};

    ///\name ListTemplate forward
    /// @{
    inline uint32_t getSize() const {
        return list.getSize();
    }

    inline Iterator begin() const {
        return Iterator(list.begin());
    }

    inline Iterator end() const {
        return Iterator(list.end());
    }

    inline void clear() {
        list.clear();
    }
    /// @}

protected:

    Iterator insert(const T &item);
    std::pair<Iterator, bool> insertUnique(const T &item);

    std::pair<Iterator, bool> findSortedPosition(const T &item) const;

    template<typename F, typename V, class ValComp = std::less<V>>
    std::pair<Iterator, bool> findSortedPosition(F f, const V &v) const;

    Iterator insertTo(Iterator pos, const T &item) const {
        return list.insert(pos, item);
    }

    template<typename... Args>
    Iterator emplaceTo(Iterator pos, Args &&... args) const {
        return list.emplace(pos, std::forward<Args>(args)...);
    }

    void erase(Iterator pos) {
        list.erase(pos);
    }

};


template<class T, class Comp /*= std::less<T>*/>
Comp SortedTemplate<T, Comp>::comp;


template<class T, class Comp /*= std::less<T>*/>
typename SortedTemplate<T, Comp>::Iterator SortedTemplate<T, Comp>::insert(const T &item) {

    std::pair<Iterator, bool> found = findSortedPosition(item);
    return list.insert(found.first, item);
}


template<class T, class Comp /*= std::less<T>*/>
std::pair<typename SortedTemplate<T, Comp>::Iterator, bool> SortedTemplate<T, Comp>::insertUnique(const T &item) {

    std::pair<Iterator, bool> found = findSortedPosition(item);

    if(found.second == false) {
        found.first = list.insert(found.first, item);
    }

    found.second = !found.second;

    return found;
}


template<class T, class Comp /*= std::less<T>*/>
std::pair<typename SortedTemplate<T, Comp>::Iterator, bool> SortedTemplate<T, Comp>::findSortedPosition(const T &item) const {

    Iterator startIt = begin();
    Iterator endIt = end();
    Iterator it = startIt;

    if(it == endIt) {
        return {it, false};
    }

    bool notLessFound = false;
    bool equalFound = false;

    while((!notLessFound) && (it != endIt)) {

        if(comp(*it, item)) {
            ++it;
        } else {
            notLessFound = true;
        }
    }

    while(notLessFound && (it != endIt)) {

        if(comp(item, *it)) {
            notLessFound = false;
        } else {
            equalFound = true;
            ++it;
        }
    }

    return {it, equalFound};
}


template<class T, class Comp /*= std::less<T>*/>
template<typename F, typename V, class ValComp /*= std::less<V>*/>
std::pair<typename SortedTemplate<T, Comp>::Iterator, bool> SortedTemplate<T, Comp>::findSortedPosition(F f, const V &v) const {

    static ValComp valComp;

    Iterator startIt = begin();
    Iterator endIt = end();
    Iterator it = startIt;

    if(it == endIt) {
        return {it, false};
    }

    bool notLessFound = false;
    bool equalFound = false;

    while((!notLessFound) && (it != endIt)) {

        if(valComp(((*it).*f)(), v)) {
            ++it;
        } else {
            notLessFound = true;
        }
    }

    while(notLessFound && (it != endIt)) {

        if(valComp(v, ((*it).*f)())) {
            notLessFound = false;
        } else {
            equalFound = true;
            ++it;
        }
    }

    return {it, equalFound};
}

}

#endif /* __ETL_SORTEDTEMPLATE_H__ */
