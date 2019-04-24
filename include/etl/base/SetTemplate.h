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
class Set : public Detail::SortedList<E> {

  public:   // types

    typedef E key_type;
    typedef E value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef Detail::SortedList<E> Base;
    typedef typename Base::Cont ContainerType;
    typedef typename ContainerType::AllocatorBase AllocatorBase;

    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

  public:   // functions

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

    using Base::erase;

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

    void erase(const E& e);

    iterator find(const E& e);
    const_iterator find(const E& e) const;

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
typename Set<E>::iterator  Set<E>::find(const E& e) {

    std::pair<iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class E>
typename Set<E>::const_iterator  Set<E>::find(const E& e) const {

    std::pair<const_iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}

}

#endif /* __ETL_SETTEMPLATE_H__ */

