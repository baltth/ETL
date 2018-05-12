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
#include <iterator>

#if ETL_USE_CPP11
#include <functional>
#include <initializer_list>
#endif

namespace ETL_NAMESPACE {


template<class T>
class TypedListBase : protected AListBase {

  public:   // types

    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

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

    class const_iterator : public AListBase::Iterator {
        friend class TypedListBase<T>;

      public:

        typedef void difference_type;
        typedef const T value_type;
        typedef const T* pointer;
        typedef const T& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        const_iterator() :
            AListBase::Iterator(NULLPTR) {};

        explicit const_iterator(const AListBase::Iterator& it) :
            AListBase::Iterator(it) {};

        const_reference operator*() const {
            return static_cast<TypedListBase<T>::Node*>(node)->item;
        }

        const_pointer operator->() const {
            return &(static_cast<TypedListBase<T>::Node*>(node)->item);
        }

        bool operator==(const const_iterator& other) const {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const const_iterator& other) const {
            return !(operator==(other));
        }

        const_iterator& operator++() {
            AListBase::Iterator::operator++();
            return *this;
        }

        const_iterator& operator--() {
            AListBase::Iterator::operator--();
            return *this;
        }

        const const_iterator operator++(int) {
            const_iterator old = *this;
            this->operator++();
            return old;
        }

        const const_iterator operator--(int) {
            const_iterator old = *this;
            this->operator--();
            return old;
        }

      protected:

        explicit const_iterator(TypedListBase<T>::Node* n) :
            AListBase::Iterator(n) {};

    };

    class iterator : public const_iterator {
        friend class TypedListBase<T>;

      public:

        typedef void difference_type;
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        iterator() :
            const_iterator(NULLPTR) {};

        explicit iterator(const AListBase::Iterator& it) :
            const_iterator(it) {};

        reference operator*() const {
            return static_cast<TypedListBase<T>::Node*>(this->node)->item;
        }

        pointer operator->() const {
            return &(static_cast<TypedListBase<T>::Node*>(this->node)->item);
        }

        bool operator==(const iterator& other) const {
            return const_iterator::operator==(other);
        }

        bool operator!=(const iterator& other) const {
            return !(operator==(other));
        }

        bool operator==(const const_iterator& other) const {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const const_iterator& other) const {
            return !(operator==(other));
        }

        iterator& operator++() {
            AListBase::Iterator::operator++();
            return *this;
        }

        iterator& operator--() {
            AListBase::Iterator::operator--();
            return *this;
        }

        const iterator operator++(int) {
            iterator old = *this;
            this->operator++();
            return old;
        }

        const iterator operator--(int) {
            iterator old = *this;
            this->operator--();
            return old;
        }

      protected:

        explicit iterator(TypedListBase<T>::Node* n) :
            const_iterator(n) {};

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

    iterator begin() {
        return iterator(AListBase::begin());
    }

    const_iterator begin() const {
        return const_iterator(AListBase::begin());
    }

    const_iterator cbegin() const {
        return this->begin();
    }

    iterator end() {
        return iterator(AListBase::end());
    }

    const_iterator end() const {
        return const_iterator(AListBase::end());
    }

    const_iterator cend() const {
        return this->end();
    }
    /// @}

    /// \name Search
    /// @{
    template<typename F, typename V>
    iterator find(F f, const V& v) const {
        return find(begin(), end(), f, v);
    }

    template<typename F, typename V>
    iterator find(const_iterator startPos, const_iterator endPos, F f, const V& v) const;

    iterator find(const Matcher<T>& matchCall) const {
        return find(begin(), end(), matchCall);
    }

    iterator find(const_iterator startPos, const_iterator endPos, const Matcher<T>& matchCall) const;

#if ETL_USE_CPP11

    iterator find(MatchFunc<T>&& matchCall) const {
        return find(begin(), end(), std::move(matchCall));
    }

    iterator find(const_iterator startPos, const_iterator endPos, MatchFunc<T>&& matchCall) const;

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

    iterator insert(const_iterator pos, Node& node) {
        AListBase::insert(pos, &node);
        return iterator(&node);
    }

    Node* replace(iterator& it, Node* other) {
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
typename TypedListBase<T>::iterator TypedListBase<T>::find(const_iterator startPos,
                                                           const_iterator endPos,
                                                           F f,
                                                           const V& v) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = (((*startPos).*f)() == v);

        if (!match) {
            ++startPos;
        }
    }

    return iterator(startPos);
}


template<class T>
typename TypedListBase<T>::iterator TypedListBase<T>::find(const_iterator startPos,
                                                           const_iterator endPos,
                                                           const Matcher<T>& matchCall) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = matchCall.call(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return iterator(startPos);
}


#if ETL_USE_CPP11

template<class T>
typename TypedListBase<T>::iterator TypedListBase<T>::find(const_iterator startPos,
                                                           const_iterator endPos,
                                                           MatchFunc<T>&& matchCall) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = matchCall(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return iterator(startPos);
}

#endif

}

#endif /* __ETL_TYPEDLISTBASE_H__ */

