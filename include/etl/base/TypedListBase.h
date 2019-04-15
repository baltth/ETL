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
namespace Detail {


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

        typedef int difference_type;
        typedef const T value_type;
        typedef const T* pointer;
        typedef const T& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        const_iterator() :
            AListBase::Iterator(nullptr) {};

        const_iterator(const const_iterator& it) :
            AListBase::Iterator(it) {};

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

      private:

        explicit const_iterator(TypedListBase<T>::Node* n) :
            AListBase::Iterator(n) {};

    };

    class iterator : public AListBase::Iterator {
        friend class TypedListBase<T>;

      public:

        typedef int difference_type;
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        iterator() :
            AListBase::Iterator(nullptr) {};

        iterator(const iterator& it) :
            AListBase::Iterator(it) {};

        operator const_iterator() const {
            return const_iterator(*this);
        }

        reference operator*() const {
            return static_cast<TypedListBase<T>::Node*>(this->node)->item;
        }

        pointer operator->() const {
            return &(static_cast<TypedListBase<T>::Node*>(this->node)->item);
        }

        bool operator==(const iterator& other) const {
            return AListBase::Iterator::operator==(other);
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

      private:

        explicit iterator(TypedListBase<T>::Node* n) :
            AListBase::Iterator(n) {};

        explicit iterator(const AListBase::Iterator& it) :
            AListBase::Iterator(it) {};

    };

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  public:   // functions

    TypedListBase() {};

    /// \name Capacity
    /// \{
    uint32_t size() const {
        return AListBase::size();
    }

    bool empty() const {
        return AListBase::empty();
    }
    /// \}

    /// \name Iterators
    /// \{
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

    reverse_iterator rbegin() {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(this->end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(this->cend());
    }

    reverse_iterator rend() {
        return reverse_iterator(this->begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(this->begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(this->cbegin());
    }
    /// \}

    /// \name Element access
    /// \{
    reference front() {
        return *begin();
    }

    const_reference front() const {
        return *begin();
    }

    reference back() {
        return *(--end());
    }

    const_reference back() const {
        return *(--end());
    }
    /// \}

    /// \name Lookup
    /// \{
    iterator find(const Matcher<T>& matchCall) {
        return find(begin(), end(), matchCall);
    }

    const_iterator find(const Matcher<T>& matchCall) const {
        return find(begin(), end(), matchCall);
    }

    iterator find(const_iterator startPos, const_iterator endPos, const Matcher<T>& matchCall) {
        return findBase<iterator>(startPos, endPos, matchCall);
    }

    const_iterator find(const_iterator startPos, const_iterator endPos, const Matcher<T>& matchCall) const {
        return findBase<const_iterator>(startPos, endPos, matchCall);
    }

#if ETL_USE_CPP11

    iterator find(MatchFunc<T>&& matchCall) {
        return find(begin(), end(), std::move(matchCall));
    }

    const_iterator find(MatchFunc<T>&& matchCall) const {
        return find(begin(), end(), std::move(matchCall));
    }

    iterator find(const_iterator startPos, const_iterator endPos, MatchFunc<T>&& matchCall) {
        return findBase<iterator>(startPos, endPos, std::move(matchCall));
    }

    const_iterator find(const_iterator startPos, const_iterator endPos, MatchFunc<T>&& matchCall) const {
        return findBase<const_iterator>(startPos, endPos, std::move(matchCall));
    }

#endif
    /// \}

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

    static iterator convert(const_iterator it) {
        return iterator(it);
    }

  private:

    // Non-copyable
    TypedListBase(const TypedListBase& other);
    TypedListBase& operator=(const TypedListBase& other);

    template<typename It>
    It findBase(const_iterator startPos, const_iterator endPos, const Matcher<T>& matchCall) const;

#if ETL_USE_CPP11
    template<typename It>
    It findBase(const_iterator startPos, const_iterator endPos, MatchFunc<T>&& matchCall) const;
#endif

};


template<class T>
template<class It>
It TypedListBase<T>::findBase(const_iterator startPos,
                              const_iterator endPos,
                              const Matcher<T>& matchCall) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = matchCall.call(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return It(startPos);
}


#if ETL_USE_CPP11

template<class T>
template<class It>
It TypedListBase<T>::findBase(const_iterator startPos,
                              const_iterator endPos,
                              MatchFunc<T>&& matchCall) const {

    bool match = false;

    while (!match && (startPos != endPos)) {

        match = matchCall(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return It(startPos);
}

#endif

}
}

#endif /* __ETL_TYPEDLISTBASE_H__ */

