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

    typedef T valueType;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

    typedef TypedListBase<T> Base;
    typedef typename Base::Node Node;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

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
    inline void push_front(const T& item);
    inline void push_back(const T& item);

    void pop_front() {
        deleteNode(static_cast<Node*>(AListBase::popBack()));
    }

    void pop_back() {
        deleteNode(static_cast<Node*>(AListBase::popBack()));
    }

    void pushFront(const T& item) {
        push_front(item);
    }

    void pushBack(const T& item) {
        push_back(item);
    }

    void popFront() {
        pop_front();
    }

    void popBack() {
        pop_back();
    }

    inline iterator insert(const_iterator pos, const T& item);

    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        deleteNode(static_cast<Node*>(AListBase::remove(pos)));
        return next;
    }

#if ETL_USE_CPP11

    template<typename... Args >
    iterator emplace(const_iterator pos, Args&& ... args);

#endif

    void copyElements(const Base& other) {
        copyElements(other.begin(), other.end());
    }

    void copyElements(const_iterator bIt, const_iterator eIt);

    template<template<class> class B>
    void splice(const_iterator pos, List<T, B>& other) {
        splice(pos, other, other.begin(), other.end());
    }

    template<template<class> class B>
    void splice(const_iterator pos, List<T, B>& other, const_iterator it) {
        const_iterator it2 = it;
        ++it2;
        splice(pos, other, it, it2);
    }

    template<template<class> class B>
    void splice(const_iterator pos,
                List<T, B>& other,
                const_iterator first,
                const_iterator last);

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

    Node* copyAndReplace(iterator& item, const T& value);

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
        push_back(it);
    }
}

#endif


template<class T, template<class> class A>
void List<T, A>::clear() {

    while (Base::getSize() > 0) {
        pop_back();
    }
}


template<class T, template<class> class A>
void List<T, A>::push_front(const T& item) {

    Node* p = createNode(item);
    if (p != NULLPTR) {
        AListBase::pushFront(p);
    }
}


template<class T, template<class> class A>
void List<T, A>::push_back(const T& item) {

    Node* p = createNode(item);
    if (p != NULLPTR) {
        AListBase::pushBack(p);
    }
}


#if ETL_USE_CPP11


template<class T, template<class> class A>
typename List<T, A>::iterator List<T, A>::insert(const_iterator pos, const T& item) {
    return emplace(pos, item);
}


template<class T, template<class> class A>
template<typename... Args >
typename List<T, A>::iterator List<T, A>::emplace(const_iterator pos, Args&& ... args) {

    iterator it = this->end();
    Node* inserted = allocator.allocate(1);
    if (inserted != NULLPTR) {
        new (inserted) Node(std::forward<Args>(args)...);
        it = Base::insert(pos, *inserted);
    }

    return it;
}


#else


template<class T, template<class> class A>
typename List<T, A>::iterator List<T, A>::insert(const_iterator pos, const T& item) {

    iterator it = this->end();
    Node* inserted = createNode(item);
    if (inserted != NULLPTR) {
        it = Base::insert(pos, *inserted);
    }

    return it;
}


#endif


template<class T, template<class> class A>
void List<T, A>::copyElements(const_iterator bIt, const_iterator eIt) {

    while (bIt != eIt) {
        pushBack(*bIt);
        ++bIt;
    }
}


template<class T, template<class> class A>
typename List<T, A>::Node* List<T, A>::copyAndReplace(iterator& item, const T& value) {

    Node* removed = NULLPTR;
    Node* newItem = createNode(value);
    if (newItem != NULLPTR) {
        removed = Base::replace(item, newItem);
    }

    return removed;
}


template<class T, template<class> class A>
template<template<class> class B>
void List<T, A>::splice(const_iterator pos,
                        List<T, B>& other,
                        const_iterator first,
                        const_iterator last) {

    if (static_cast<Base*>(&other) != static_cast<Base*>(this)) {
        iterator item(first);
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

    iterator ownIt = this->begin();
    iterator otherIt = other.begin();

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

