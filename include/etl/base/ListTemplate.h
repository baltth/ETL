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
class List : public TypedListBase<T> {

  public:   // types

    typedef T ItemType;
    typedef const T ConstItemType;
    typedef TypedListBase<T> Base;

    typedef typename Base::Node Node;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

    typedef A<Node> Allocator;

    friend Node;

    template<class T2, template<class> class B>
    friend class List;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    List() {};

    explicit List(const Allocator& a) :
        allocator(a) {};

    List(const List& other) {
        copyElements(other);
    }

    List& operator=(const List& other);

    List(const Base& other) {
        copyElements(other);
    }

    List& operator=(const Base& other);

#if ETL_USE_CPP11

    List(List&& other) :
        Base(std::move(other)) {};

    List& operator=(List&& other) {
        Base::operator=(other);
        return *this;
    }

    List(std::initializer_list<T> initList);

#endif

    ~List() {
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

    void copyElements(const Base& other) {
        copyElements(other.begin(), other.end());
    }

    void copyElements(ConstIterator bIt, ConstIterator eIt);

    template<template<class> class B>
    void splice(ConstIterator pos, List<T, B>& other) {
        splice(pos, other, other.begin(), other.end());
    }

    template<template<class> class B>
    void splice(ConstIterator pos, List<T, B>& other, ConstIterator it) {
        ConstIterator it2 = it;
        ++it2;
        splice(pos, other, it, it2);
    }

    template<template<class> class B>
    void splice(ConstIterator pos,
                List<T, B>& other,
                ConstIterator first,
                ConstIterator last);

    template<template<class> class B>
    void swap(List<T, B>& other) {
        swapElements(other);
    }
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

    Node* copyAndReplace(Iterator& item, const T& value);

    template<template<class> class B>
    void swapElements(List<T, B>& other);

};


template<class T, template<class> class A>
List<T, A>& List<T, A>::operator=(const List<T, A>& other) {

    if (&other != this) {
        clear();
        copyElements(other);
    }

    return *this;
}


template<class T, template<class> class A>
List<T, A>& List<T, A>::operator=(const TypedListBase<T>& other) {

    if (&other != static_cast<TypedListBase<T>*>(this)) {
        clear();
        copyElements(other);
    }

    return *this;
}


#if ETL_USE_CPP11

template<class T, template<class> class A>
List<T, A>::List(std::initializer_list<T> initList) {

    for (auto& it : initList) {
        pushBack(it);
    }
}

#endif


template<class T, template<class> class A>
void List<T, A>::clear() {

    while (Base::getSize() > 0) {
        popBack();
    }
}


template<class T, template<class> class A>
void List<T, A>::pushFront(const T& item) {

    Node* p = createNode(item);
    if (p != NULLPTR) {
        AListBase::pushFront(p);
    }
}


template<class T, template<class> class A>
void List<T, A>::pushBack(const T& item) {

    Node* p = createNode(item);
    if (p != NULLPTR) {
        AListBase::pushBack(p);
    }
}


#if ETL_USE_CPP11


template<class T, template<class> class A>
typename List<T, A>::Iterator List<T, A>::insert(ConstIterator pos, const T& item) {
    return emplace(pos, item);
}


template<class T, template<class> class A>
template<typename... Args >
typename List<T, A>::Iterator List<T, A>::emplace(ConstIterator pos, Args&& ... args) {

    Iterator it = this->end();
    Node* inserted = allocator.allocate(1);
    if (inserted != NULLPTR) {
        new (inserted) Node(std::forward<Args>(args)...);
        it = Base::insert(pos, *inserted);
    }

    return it;
}


#else


template<class T, template<class> class A>
typename List<T, A>::Iterator List<T, A>::insert(ConstIterator pos, const T& item) {

    Iterator it = this->end();
    Node* inserted = createNode(item);
    if (inserted != NULLPTR) {
        it = Base::insert(pos, *inserted);
    }

    return it;
}


#endif


template<class T, template<class> class A>
void List<T, A>::copyElements(ConstIterator bIt, ConstIterator eIt) {

    while (bIt != eIt) {
        pushBack(*bIt);
        ++bIt;
    }
}


template<class T, template<class> class A>
typename List<T, A>::Node* List<T, A>::copyAndReplace(Iterator& item, const T& value) {

    Node* removed = NULLPTR;
    Node* newItem = createNode(value);
    if (newItem != NULLPTR) {
        removed = Base::replace(item, newItem);
    }

    return removed;
}


template<class T, template<class> class A>
template<template<class> class B>
void List<T, A>::splice(ConstIterator pos,
                        List<T, B>& other,
                        ConstIterator first,
                        ConstIterator last) {

    if (static_cast<Base*>(&other) != static_cast<Base*>(this)) {
        Iterator item(first);
        while (item != last) {
            insert(pos, *item);
            item = other.erase(item);
        }
    }
}


template<class T, template<class> class A>
template<template<class> class B>
void List<T, A>::swapElements(List<T, B>& other) {

    const SizeDiff diff(*this, other);

    Iterator ownIt = this->begin();
    Iterator otherIt = other.begin();

    for (uint32_t i = 0; i < diff.common; ++i) {

        Node* removed = copyAndReplace(ownIt, *otherIt);
        if (removed != NULLPTR) {
            Node* otherRemoved = other.copyAndReplace(otherIt, removed->item);
            deleteNode(removed);
            other.deleteNode(otherRemoved);
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

