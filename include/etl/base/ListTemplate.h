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

#ifndef __ETL_LISTTEMPLATE_H__
#define __ETL_LISTTEMPLATE_H__

#include <etl/etlSupport.h>
#include <etl/base/TypedListBase.h>
#include <etl/base/tools.h>

#if ETL_USE_CPP11
#include <initializer_list>
#endif

namespace ETL_NAMESPACE {


template<class T, template<class> class A>
class ListTemplate : public TypedListBase<T> {

  public:   // types

    typedef T ItemType;
    typedef const T ConstItemType;
    typedef TypedListBase<T> ListBase;

    typedef typename ListBase::Node Node;
    typedef typename ListBase::Iterator Iterator;
    typedef typename ListBase::ConstIterator ConstIterator;

    typedef A<Node> Allocator;

    friend Node;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    ListTemplate() {};

    explicit ListTemplate(const Allocator& a) :
        allocator(a) {};

    ListTemplate(const ListTemplate& other) {
        copyElements(other);
    }

    ListTemplate& operator=(const ListTemplate& other);

    ListTemplate(const ListBase& other) {
        copyElements(other);
    }

    ListTemplate& operator=(const ListBase& other);

#if ETL_USE_CPP11

    ListTemplate(ListTemplate&& other) :
        ListBase(std::move(other)) {};

    ListTemplate& operator=(ListTemplate&& other) {
        ListBase::operator=(other);
        return *this;
    }

    ListTemplate(std::initializer_list<T> initList);

#endif

    ~ListTemplate() {
        clear();
    }

    void clear();

    /// \name Element operations
    /// @{
    inline void pushFront(const T& item);
    inline void pushBack(const T& item);

    void popFront() {
        deleteNode(static_cast<Node*>(AListBase::popBack()));
    }

    void popBack() {
        deleteNode(static_cast<Node*>(AListBase::popBack()));
    }

    inline Iterator insert(ConstIterator pos, const T& item);

    Iterator erase(Iterator pos) {
        Iterator next = pos;
        ++next;
        deleteNode(static_cast<Node*>(AListBase::remove(pos)));
        return next;
    }

#if ETL_USE_CPP11

    template<typename... Args >
    Iterator emplace(ConstIterator pos, Args&& ... args);

#endif

    void copyElements(const ListBase& other) {
        copyElements(other.begin(), other.end());
    }

    void copyElements(ConstIterator bIt, ConstIterator eIt);

    template<template<class> class B>
    void splice(ConstIterator pos, ListTemplate<T, B>& other) {
        splice(pos, other, other.begin(), other.end());
    }

    template<template<class> class B>
    void splice(ConstIterator pos, ListTemplate<T, B>& other, ConstIterator it) {
        ConstIterator it2 = it;
        ++it2;
        splice(pos, other, it, it2);
    }

    template<template<class> class B>
    void splice(ConstIterator pos,
                ListTemplate<T, B>& other,
                ConstIterator first,
                ConstIterator last);
    /// @}

    Allocator& getAllocator() const {
        return allocator;
    }

  private:

    Node* createNode(const T& item) {
        Node* p = allocator.allocate(1);
        if (p != NULLPTR) {
            new (p) Node(item);
        }
        return p;
    }

    void deleteNode(Node* ptr) {
        if (ptr) {
            ptr->~Node();
            allocator.deallocate(ptr, 1);
        }
    }

    Node* copyAndReplace(Node& item, Node& other);

    template<template<class> class B>
    void swapElements(ListTemplate<T, B>& other);

};


template<class T, template<class> class A>
ListTemplate<T, A>& ListTemplate<T, A>::operator=(const ListTemplate<T, A>& other) {

    if (&other != this) {
        clear();
        copyElements(other);
    }

    return *this;
}


template<class T, template<class> class A>
ListTemplate<T, A>& ListTemplate<T, A>::operator=(const TypedListBase<T>& other) {

    if (&other != static_cast<TypedListBase<T>*>(this)) {
        clear();
        copyElements(other);
    }

    return *this;
}


#if ETL_USE_CPP11

template<class T, template<class> class A>
ListTemplate<T, A>::ListTemplate(std::initializer_list<T> initList) {

    for (auto& it : initList) {
        pushBack(it);
    }
}

#endif


template<class T, template<class> class A>
void ListTemplate<T, A>::clear() {

    while (ListBase::getSize() > 0) {
        popBack();
    }
}


template<class T, template<class> class A>
void ListTemplate<T, A>::pushFront(const T& item) {

    Node* p = createNode(item);
    if (p != NULLPTR) {
        AListBase::pushFront(p);
    }
}


template<class T, template<class> class A>
void ListTemplate<T, A>::pushBack(const T& item) {

    Node* p = createNode(item);
    if (p != NULLPTR) {
        AListBase::pushBack(p);
    }
}


#if ETL_USE_CPP11


template<class T, template<class> class A>
typename ListTemplate<T, A>::Iterator ListTemplate<T, A>::insert(ConstIterator pos, const T& item) {
    return emplace(pos, item);
}


template<class T, template<class> class A>
template<typename... Args >
typename ListTemplate<T, A>::Iterator ListTemplate<T, A>::emplace(ConstIterator pos, Args&& ... args) {

    Iterator it = this->end();
    Node* inserted = allocator.allocate(1);
    if (inserted != NULLPTR) {
        new (inserted) Node(std::forward<Args>(args)...);
        it = ListBase::insert(pos, *inserted);
    }

    return it;
}


#else


template<class T, template<class> class A>
typename ListTemplate<T, A>::Iterator ListTemplate<T, A>::insert(ConstIterator pos, const T& item) {

    Iterator it = this->end();
    Node* inserted = createNode(item);
    if (inserted != NULLPTR) {
        it = ListBase::insert(pos, *inserted);
    }

    return it;
}


#endif


template<class T, template<class> class A>
void ListTemplate<T, A>::copyElements(ConstIterator bIt, ConstIterator eIt) {

    while (bIt != eIt) {
        pushBack(*bIt);
        ++bIt;
    }
}


template<class T, template<class> class A>
typename ListTemplate<T, A>::Node* ListTemplate<T, A>::copyAndReplace(Node& item, Node& other) {

    Node* removed = NULLPTR;
    Node* newItem = createNode(other);
    if (newItem != NULLPTR) {
        ListBase::replace(item, newItem);
        removed = &item;
    }

    return removed;
}


template<class T, template<class> class A>
template<template<class> class B>
void ListTemplate<T, A>::splice(ConstIterator pos,
                                ListTemplate<T, B>& other,
                                ConstIterator first,
                                ConstIterator last) {

    if (&other != this) {
        Iterator item(first);
        while (item != last) {
            insert(pos, *item);
            item = other.erase(item);
        }
    }
}


template<class T, template<class> class A>
template<template<class> class B>
void ListTemplate<T, A>::swapElements(ListTemplate<T, B>& other) {

    const SizeDiff diff(*this, other);

    Iterator ownIt = this->begin();
    Iterator otherIt = other.begin();

    for (uint32_t i = 0; i < diff.common; ++i) {

        Node* removed = copyAndReplace(*ownIt, *otherIt);
        if (removed != NULLPTR) {
            *otherIt = *removed;
            deleteNode(removed);
        }

        ++ownIt;
        ++otherIt;
    }

    if (diff.lGreaterWith > 0) {
        other.splice(other.end(), *this, ownIt, this->end());
    } else if (diff.rGreaterWith > 0) {
        this->splice(this->end(), other, otherIt, other.end());
    }
}

}

#endif /* __ETL_LISTTEMPLATE_H__ */

