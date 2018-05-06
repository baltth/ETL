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

#ifndef __ETL_TYPEDLISTBASE_H__
#define __ETL_TYPEDLISTBASE_H__

#include <etl/etlSupport.h>
#include <etl/base/AListBase.h>

#undef min
#undef max

#include <cstddef>
#include <utility>

#if ETL_USE_CPP11
#include <functional>
#include <initializer_list>
#endif

namespace ETL_NAMESPACE {


template<class T>
class TypedListBase : protected AListBase {

  public:   // types

    typedef T ValueType;
    typedef T& Reference;
    typedef const T& ConstReference;
    typedef T* Pointer;
    typedef const T* ConstPointer;

    class Node : public AListBase::Node {

      public:   // variables

        T item;

      public:   // functions

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
        friend class TypedListBase<T>;

      public:

        ConstIterator() :
            AListBase::Iterator(NULLPTR) {};

        explicit ConstIterator(const AListBase::Iterator& it) :
            AListBase::Iterator(it) {};

        ConstReference operator*() const {
            return static_cast<TypedListBase<T>::Node*>(node)->item;
        }

        ConstPointer operator->() const {
            return &(static_cast<TypedListBase<T>::Node*>(node)->item);
        }

        bool operator==(const ConstIterator& other) const {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const ConstIterator& other) const {
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

      protected:

        explicit ConstIterator(TypedListBase<T>::Node* n) :
            AListBase::Iterator(n) {};

    };

    class Iterator : public ConstIterator {
        friend class TypedListBase<T>;

      public:

        Iterator() :
            ConstIterator(NULLPTR) {};

        explicit Iterator(const AListBase::Iterator& it) :
            ConstIterator(it) {};

        Reference operator*() const {
            return static_cast<TypedListBase<T>::Node*>(this->node)->item;
        }

        Pointer operator->() const {
            return &(static_cast<TypedListBase<T>::Node*>(this->node)->item);
        }

        bool operator==(const Iterator& other) const {
            return ConstIterator::operator==(other);
        }

        bool operator!=(const Iterator& other) const {
            return !(operator==(other));
        }

        bool operator==(const ConstIterator& other) const {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const ConstIterator& other) const {
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

      protected:

        explicit Iterator(TypedListBase<T>::Node* n) :
            ConstIterator(n) {};

    };

  public:   // functions

    TypedListBase() {};

    ///\name AListBase forward
    /// @{
    uint32_t getSize() const {
        return AListBase::getSize();
    }

    bool isEmpty() const {
        return AListBase::isEmpty();
    }

    Iterator begin() {
        return Iterator(AListBase::begin());
    }

    ConstIterator begin() const {
        return ConstIterator(AListBase::begin());
    }

    ConstIterator cbegin() const {
        return this->begin();
    }

    Iterator end() {
        return Iterator(AListBase::end());
    }

    ConstIterator end() const {
        return ConstIterator(AListBase::end());
    }

    ConstIterator cend() const {
        return this->end();
    }
    /// @}

    /// \name Search
    /// @{
    template<typename F, typename V>
    Iterator find(F f, const V& v) const {
        return find(begin(), end(), f, v);
    }

    template<typename F, typename V>
    Iterator find(ConstIterator startPos, ConstIterator endPos, F f, const V& v) const;

    Iterator find(const Matcher<T>& matchCall) const {
        return find(begin(), end(), matchCall);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<T>& matchCall) const;

#if ETL_USE_CPP11

    Iterator find(MatchFunc<T>&& matchCall) const {
        return find(begin(), end(), std::move(matchCall));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, MatchFunc<T>&& matchCall) const;

#endif
    /// @}

  protected:

#if ETL_USE_CPP11

    TypedListBase(TypedListBase&& other) :
        AListBase(std::move(other)) {};

    TypedListBase& operator=(TypedListBase&& other) {
        AListBase::operator=(other);
        return *this;
    }

#endif

    Iterator insert(ConstIterator pos, Node& node) {
        AListBase::insert(pos, &node);
        return Iterator(&node);
    }

    Node* replace(Iterator& it, Node* other) {
        Node* removed = static_cast<TypedListBase::Node*>(it.node);
        AListBase::replace(it.node, other);
        it.node = other;
        return removed;
    }

  private:

    // Non-copiable
    TypedListBase(const TypedListBase& other);
    TypedListBase& operator=(const TypedListBase& other);

};


template<class T>
template<typename F, typename V>
typename TypedListBase<T>::Iterator TypedListBase<T>::find(ConstIterator startPos,
                                                           ConstIterator endPos,
                                                           F f,
                                                           const V& v) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = (((*startPos).*f)() == v);

        if (!match) {
            ++startPos;
        }
    }

    return Iterator(startPos);
}


template<class T>
typename TypedListBase<T>::Iterator TypedListBase<T>::find(ConstIterator startPos,
                                                           ConstIterator endPos,
                                                           const Matcher<T>& matchCall) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = matchCall.call(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return Iterator(startPos);
}


#if ETL_USE_CPP11

template<class T>
typename TypedListBase<T>::Iterator TypedListBase<T>::find(ConstIterator startPos,
                                                           ConstIterator endPos,
                                                           MatchFunc<T>&& matchCall) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = matchCall(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return Iterator(startPos);
}

#endif

}

#endif /* __ETL_TYPEDLISTBASE_H__ */

