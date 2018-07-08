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
#include <etl/base/Sorted.h>
#include <etl/base/ListTemplate.h>

#include <utility>

namespace ETL_NAMESPACE {


template<class E, template<class> class A>
class Set : public Sorted<List<E, A> > {

  public:   // types

    typedef E key_type;
    typedef E value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef List<E, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef Sorted<ContainerType> Base;

    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

  public:   // functions

    Set() {};

    Set(const Set& other) {
        assign(other);
    }

    Set& operator=(const Set& other) {
        assign(other);
        return *this;
    }

#if ETL_USE_CPP11

    Set(std::initializer_list<E> initList);

#endif

    using Base::find;
    using Base::erase;

    std::pair<iterator, bool> insert(const E& e) {
        return Base::insertUnique(e);
    }

    template<class InputIt>
    typename std::enable_if<!std::is_integral<InputIt>::value>::type    // *NOPAD*
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


#if ETL_USE_CPP11

template<class E, template<class> class A>
Set<E, A>::Set(std::initializer_list<E> initList) {

    for (auto& item : initList) {
        Base::insertUnique(item);
    }
}

#endif


template<class E, template<class> class A>
void Set<E, A>::erase(const E& e) {

    std::pair<iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        Base::erase(--found.first);
    }
}


template<class E, template<class> class A>
typename Set<E, A>::iterator  Set<E, A>::find(const E& e) {

    std::pair<iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}


template<class E, template<class> class A>
typename Set<E, A>::const_iterator  Set<E, A>::find(const E& e) const {

    std::pair<const_iterator, bool> found = Base::findSortedPosition(e);

    if (found.second == true) {
        return --found.first;
    } else {
        return Base::end();
    }
}

}

#endif /* __ETL_SETTEMPLATE_H__ */

