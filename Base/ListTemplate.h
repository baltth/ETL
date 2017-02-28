/**
\file
\date 2016.01.20. 19:39:42
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

#ifndef __ETL_LISTTEMPLATE_H__
#define __ETL_LISTTEMPLATE_H__

#include "etlSupport.h"

#undef min
#undef max

#include <utility>

#if ETL_USE_CPP11
#include <functional>
#include <initializer_list>
#endif

#include "Base/AListBase.h"


namespace ETL_NAMESPACE {


template<class T>
class ListTemplate : protected AListBase {

// types
public:

    typedef T ItemType;
    typedef const T ConstItemType;

    class Node : public AListBase::Node {

    public:

        T item;

#if ETL_USE_CPP11
        template<typename... Args>
        Node(Args&& ... args) :
            item(std::forward<Args>(args)...) {};
#else
        Node() {};
        explicit Node(const T& value) :
            item(value) {};
#endif

    };


    class ConstIterator : public AListBase::Iterator {
        friend class ListTemplate<T>;

    public:

        ConstItemType& operator*() const {
            return static_cast<ListTemplate<T>::Node*>(node)->item;
        }

        ConstItemType* operator->() const {
            return &(static_cast<ListTemplate<T>::Node*>(node)->item);
        }

        bool operator==(const ConstIterator& other) {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const ConstIterator& other) {
            return !(operator==(other));
        }

        ConstIterator& operator++() {
            AListBase::Iterator::operator++();
            return *this;
        }

        ConstIterator& operator--() {
            AListBase::Iterator::operator--();
            return *this;
        }

        const ConstIterator operator++(int) {
            ConstIterator old = *this;
            this->operator++();
            return old;
        }

        const ConstIterator operator--(int) {
            ConstIterator old = *this;
            this->operator--();
            return old;
        }

        ConstIterator(const AListBase::Iterator& it) :
            AListBase::Iterator(it) {};

    protected:

        ConstIterator(ListTemplate<T>::Node* n) :
            AListBase::Iterator(n) {};

    };
    
    class Iterator : public ConstIterator {
        friend class ListTemplate<T>;

    public:

        ItemType& operator*() const {
            return static_cast<ListTemplate<T>::Node*>(this->node)->item;
        }

        ItemType* operator->() const {
            return &(static_cast<ListTemplate<T>::Node*>(this->node)->item);
        }

        bool operator==(const Iterator& other) {
            return ConstIterator::operator==(other);
        }

        bool operator!=(const Iterator& other) {
            return !(operator==(other));
        }

        Iterator& operator++() {
            AListBase::Iterator::operator++();
            return *this;
        }

        Iterator& operator--() {
            AListBase::Iterator::operator--();
            return *this;
        }

        const Iterator operator++(int) {
            Iterator old = *this;
            this->operator++();
            return old;
        }

        const Iterator operator--(int) {
            Iterator old = *this;
            this->operator--();
            return old;
        }

        Iterator(const AListBase::Iterator& it) :
            ConstIterator(it) {};

    protected:

        Iterator(ListTemplate<T>::Node* n) :
            ConstIterator(n) {};

    };

// functions
public:

    ListTemplate() {};

#if ETL_USE_CPP11

    ListTemplate(ListTemplate&& other) :
        AListBase(std::move(other)) {};

    ListTemplate& operator=(ListTemplate&& other) {
        AListBase::operator=(other);
        return *this;
    }

    ListTemplate(const std::initializer_list<T>& initList);

#endif

    ~ListTemplate() {
        clear();
    }

    void clear();

    ///\name AListBase forward
    /// @{
    inline uint32_t getSize() const {
        return AListBase::getSize();
    }

    inline Iterator begin() const {
        return Iterator(AListBase::begin());
    }

    inline Iterator end() const {
        return Iterator(AListBase::end());
    }
    /// @}

    virtual T* createItem() {
        return NULLPTR;
    }

    /// \name Element operations
    /// @{
    inline void pushFront(const T& item) {
        return AListBase::pushFront(new Node(item));
    }

    inline void pushBack(const T& item) {
        return AListBase::pushBack(new Node(item));
    }

    inline void popFront() {
        delete AListBase::popBack();
    }

    inline void popBack() {
        delete AListBase::popBack();
    }

    inline Iterator insert(ConstIterator pos, const T& item);

    inline void erase(Iterator pos) {
        delete AListBase::remove(pos);
    }

#if ETL_USE_CPP11

    template<typename... Args >
    Iterator emplace(ConstIterator pos, Args&& ... args);

#endif

    /// @}

    template<typename F, typename V>
    Iterator find(F f, const V& v) const {
        return find(begin(), end(), f, v);
    }

    template<typename F, typename V>
    Iterator find(ConstIterator startPos, ConstIterator endPos, F f, const V& v) const;

#if ETL_USE_CPP11

    Iterator find(Matcher<T>&& matchCall) const {
        return find(begin(), end(), std::move(matchCall));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, Matcher<T>&& matchCall) const;

#else

    Iterator find(const Matcher<T>& matchCall) const {
        return find(begin(), end(), matchCall);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<T>& matchCall) const;

#endif



};


#if ETL_USE_CPP11

template<class T>
ListTemplate<T>::ListTemplate(const std::initializer_list<T>& initList) {

    for(auto& it : initList) {
        pushBack(it);
    }
}

#endif


template<class T>
void ListTemplate<T>::clear() {

    while(getSize() > 0) {
        popBack();
    }
}


template<class T>
template<typename F, typename V>
typename ListTemplate<T>::Iterator ListTemplate<T>::find(ConstIterator startPos,
                                                         ConstIterator endPos,
                                                         F f,
                                                         const V& v) const {

    bool match = false;

    while(!match && (startPos != endPos)) {

        match = (((*startPos).*f)() == v);

        if(!match) {
            ++startPos;
        }
    }

    return startPos;
}


#if ETL_USE_CPP11


template<class T>
typename ListTemplate<T>::Iterator ListTemplate<T>::insert(ConstIterator pos, const T& item) {
    return emplace(pos, item);
}


template<class T>
template<typename... Args >
typename ListTemplate<T>::Iterator ListTemplate<T>::emplace(ConstIterator pos, Args&& ... args) {

    Node* inserted = new Node(std::forward<Args>(args)...);
    AListBase::insert(pos, inserted);
    return Iterator(inserted);
}


template<class T>
typename ListTemplate<T>::Iterator ListTemplate<T>::find(ConstIterator startPos,
                                                         ConstIterator endPos,
                                                         Matcher<T>&& matchCall) const {

    bool match = false;

    while(!match && (startPos != endPos)) {

        match = matchCall(*startPos);

        if(!match) {
            ++startPos;
        }
    }

    return Iterator(startPos);
}


#else


template<class T>
typename ListTemplate<T>::Iterator ListTemplate<T>::insert(ConstIterator pos, const T& item) {

    Node* inserted = new Node(item);
    AListBase::insert(pos, inserted);
    return Iterator(inserted);
}


template<class T>
typename ListTemplate<T>::Iterator ListTemplate<T>::find(ConstIterator startPos,
                                                         ConstIterator endPos,
                                                         const Matcher<T>& matchCall) const {

    bool match = false;

    while(!match && (startPos != endPos)) {

        match = matchCall.call(*startPos);

        if(!match) {
            ++startPos;
        }
    }

    return Iterator(startPos);
}

#endif

}

#endif /* __ETL_LISTTEMPLATE_H__ */

