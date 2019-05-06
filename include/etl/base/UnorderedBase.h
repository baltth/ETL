/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019 Balazs Toth.

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

#ifndef __ETL_UNORDEREDBASE_H__
#define __ETL_UNORDEREDBASE_H__

#include <etl/etlSupport.h>
#include <etl/base/AHashTable.h>

namespace ETL_NAMESPACE {
namespace Detail {


template<class T>
class UnorderedBase : protected AHashTable {

  public:   // types

    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

    typedef size_type AHashTable::size_type;

    class Node : public AHashTable::Node {

      public:   // variables

        T item;

      public:   // functions

        template<typename... Args>
        Node(Args&& ... args) :
            item(std::forward<Args>(args)...) {};

    };

    class const_iterator : public AHashTable::Iterator {
        friend class UnorderedBase<T>;

      public:

        typedef int difference_type;
        typedef const T value_type;
        typedef const T* pointer;
        typedef const T& reference;
        typedef std::forward_iterator_tag iterator_category;

        const_iterator() :
            AHashTable::Iterator(nullptr) {};

        const_iterator(const const_iterator& it) :
            AHashTable::Iterator(it) {};

        explicit const_iterator(const AHashTable::Iterator& it) :
            AHashTable::Iterator(it) {};

        const_reference operator*() const {
            return static_cast<UnorderedBase<T>::Node*>(node)->item;
        }

        const_pointer operator->() const {
            return &(static_cast<UnorderedBase<T>::Node*>(node)->item);
        }

        bool operator==(const const_iterator& other) const {
            return AHashTable::Iterator::operator==(other);
        }

        bool operator!=(const const_iterator& other) const {
            return !(operator==(other));
        }

        const_iterator& operator++() {
            AHashTable::Iterator::operator++();
            return *this;
        }

        const const_iterator operator++(int) {
            const_iterator old = *this;
            this->operator++();
            return old;
        }

      private:

        explicit const_iterator(UnorderedBase<T>::Node* n) :
            AHashTable::Iterator(n) {};

    };

    class iterator : public AHashTable::Iterator {
        friend class UnorderedBase<T>;

      public:

        typedef int difference_type;
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef std::forward_iterator_tag iterator_category;

        iterator() :
            AHashTable::Iterator(nullptr) {};

        iterator(const iterator& it) :
            AHashTable::Iterator(it) {};

        operator const_iterator() const {
            return const_iterator(*this);
        }

        reference operator*() const {
            return static_cast<UnorderedBase<T>::Node*>(this->node)->item;
        }

        pointer operator->() const {
            return &(static_cast<UnorderedBase<T>::Node*>(this->node)->item);
        }

        bool operator==(const iterator& other) const {
            return AHashTable::Iterator::operator==(other);
        }

        bool operator!=(const iterator& other) const {
            return !(operator==(other));
        }

        bool operator==(const const_iterator& other) const {
            return AHashTable::Iterator::operator==(other);
        }

        bool operator!=(const const_iterator& other) const {
            return !(operator==(other));
        }

        iterator& operator++() {
            AHashTable::Iterator::operator++();
            return *this;
        }

        const iterator operator++(int) {
            iterator old = *this;
            this->operator++();
            return old;
        }

      private:

        explicit iterator(UnorderedBase<T>::Node* n) :
            AHashTable::Iterator(n) {};

        explicit iterator(const AHashTable::Iterator& it) :
            AHashTable::Iterator(it) {};

    };

  public:   // functions

    UnorderedBase() = default;

    UnorderedBase(const UnorderedBase& other) = delete;
    UnorderedBase& operator=(const UnorderedBase& other) = delete;

    UnorderedBase(UnorderedBase&& other) = default;
    UnorderedBase& operator=(UnorderedBase&& other) = default;

    /// \name Capacity
    /// \{
    using AHashTable::size;
    using AHashTable::empty;
    /// \}

    /// \name Iterators
    /// \{
    iterator begin() {
        return iterator(AHashTable::begin());
    }

    const_iterator begin() const {
        return const_iterator(AHashTable::begin());
    }

    const_iterator cbegin() const {
        return this->begin();
    }

    iterator end() {
        return iterator(AHashTable::end());
    }

    const_iterator end() const {
        return const_iterator(AHashTable::end());
    }

    const_iterator cend() const {
        return this->end();
    }
    /// \}

  protected:

  private:

};

}
}

#endif /* __ETL_UNORDEREDBASE_H__ */
