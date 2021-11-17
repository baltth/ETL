/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2021 Balazs Toth.

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

#include <etl/base/AListBase.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>

#undef min
#undef max

#include <cstddef>
#include <functional>
#include <iterator>
#include <utility>

namespace ETL_NAMESPACE {
namespace Detail {


template<class T>
class TypedListBase : protected AListBase {

  public:  // types

    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

    typedef AListBase::size_type size_type;

    class Node : public AListBase::Node {

      public:  // variables

        T item;

      public:  // functions

        template<typename... Args>
        Node(Args&&... args) :
            item(std::forward<Args>(args)...) {}

        Node& operator=(const Node& other) = delete;
        Node& operator=(Node&& other) = delete;
        ~Node() = default;
    };

    class const_iterator : public AListBase::Iterator {
        friend class TypedListBase<T>;

      public:

        typedef int difference_type;
        typedef const T value_type;
        typedef const T* pointer;
        typedef const value_type& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        const_iterator() noexcept :
            AListBase::Iterator(nullptr) {}

        explicit const_iterator(const AListBase::Iterator& it) noexcept :
            AListBase::Iterator(it) {}

        const_iterator(const const_iterator& other) noexcept = default;
        const_iterator& operator=(const const_iterator& other) & noexcept = default;
        const_iterator(const_iterator&& other) noexcept = default;
        const_iterator& operator=(const_iterator&& other) & noexcept = default;
        ~const_iterator() noexcept = default;

        const_reference operator*() const noexcept {
            return static_cast<TypedListBase<T>::Node*>(node)->item;
        }

        const_pointer operator->() const noexcept {
            return &(static_cast<TypedListBase<T>::Node*>(node)->item);
        }

        bool operator==(const const_iterator& other) const noexcept {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const const_iterator& other) const noexcept {
            return !(operator==(other));
        }

        const_iterator& operator++() noexcept {
            AListBase::Iterator::operator++();
            return *this;
        }

        const_iterator& operator--() noexcept {
            AListBase::Iterator::operator--();
            return *this;
        }

        const const_iterator operator++(int) noexcept {
            const_iterator old = *this;
            this->operator++();
            return old;
        }

        const const_iterator operator--(int) noexcept {
            const_iterator old = *this;
            this->operator--();
            return old;
        }

      private:

        explicit const_iterator(TypedListBase<T>::Node* n) noexcept :
            AListBase::Iterator(n) {}
    };

    class iterator : public AListBase::Iterator {
        friend class TypedListBase<T>;

      public:

        typedef int difference_type;
        typedef T value_type;
        typedef T* pointer;
        typedef value_type& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        iterator() noexcept :
            AListBase::Iterator(nullptr) {}

        iterator(const iterator& other) noexcept = default;
        iterator& operator=(const iterator& other) & noexcept = default;
        iterator(iterator&& other) noexcept = default;
        iterator& operator=(iterator&& other) & noexcept = default;
        ~iterator() noexcept = default;

        operator const_iterator() const noexcept {
            return const_iterator(*this);
        }

        reference operator*() const noexcept {
            return static_cast<TypedListBase<T>::Node*>(this->node)->item;
        }

        pointer operator->() const noexcept {
            return &(static_cast<TypedListBase<T>::Node*>(this->node)->item);
        }

        bool operator==(const iterator& other) const noexcept {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const iterator& other) const noexcept {
            return !(operator==(other));
        }

        bool operator==(const const_iterator& other) const noexcept {
            return AListBase::Iterator::operator==(other);
        }

        bool operator!=(const const_iterator& other) const noexcept {
            return !(operator==(other));
        }

        iterator& operator++() noexcept {
            AListBase::Iterator::operator++();
            return *this;
        }

        iterator& operator--() noexcept {
            AListBase::Iterator::operator--();
            return *this;
        }

        const iterator operator++(int) noexcept {
            iterator old = *this;
            this->operator++();
            return old;
        }

        const iterator operator--(int) noexcept {
            iterator old = *this;
            this->operator--();
            return old;
        }

      private:

        explicit iterator(TypedListBase<T>::Node* n) noexcept :
            AListBase::Iterator(n) {}

        explicit iterator(const AListBase::Iterator& it) noexcept :
            AListBase::Iterator(it) {}
    };

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  public:  // functions

    TypedListBase() = default;

    TypedListBase(const TypedListBase& other) = delete;
    TypedListBase& operator=(const TypedListBase& other) = delete;

    /// \name Capacity
    /// \{
    size_type size() const noexcept {
        return AListBase::size();
    }

    bool empty() const noexcept {
        return AListBase::empty();
    }
    /// \}

    /// \name Iterators
    /// \{
    iterator begin() noexcept {
        return iterator(AListBase::begin());
    }

    const_iterator begin() const noexcept {
        return const_iterator(AListBase::begin());
    }

    const_iterator cbegin() const noexcept {
        return this->begin();
    }

    iterator end() noexcept {
        return iterator(AListBase::end());
    }

    const_iterator end() const noexcept {
        return const_iterator(AListBase::end());
    }

    const_iterator cend() const noexcept {
        return this->end();
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(this->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(this->cend());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(this->begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(this->begin());
    }

    const_reverse_iterator crend() const noexcept {
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

  protected:

    TypedListBase(TypedListBase&& other) = default;
    TypedListBase& operator=(TypedListBase&& other) = default;

    iterator insert(const_iterator pos, Node& node) noexcept {
        AListBase::insert(pos, node);
        return iterator(&node);
    }

    static iterator convert(const_iterator it) noexcept {
        return iterator(it);
    }

    static_assert(NothrowContract<const_iterator>::value,
                  "const_iterator violates nothrow contract");
    static_assert(NothrowContract<iterator>::value,
                  "iterator violates nothrow contract");
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_TYPEDLISTBASE_H__
