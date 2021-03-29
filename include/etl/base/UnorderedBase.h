/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2021 Balazs Toth.

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

#include <etl/base/AAllocator.h>
#include <etl/base/AHashTable.h>
#include <etl/base/VectorTemplate.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {


template<class T, class Hash>
class UnorderedBase {

  public:  // types

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    using size_type = AHashTable::size_type;
    using BucketItem = AHashTable::BucketItem;
    using Buckets = AHashTable::Buckets;
    using HashType = AHashTable::HashType;

    class Node : public AHashTable::Node {
        friend class UnorderedBase<T, Hash>;

      public:  // variables

        T item;

      public:  // functions

        template<typename... Args>
        Node(Args&&... args) :
            item(std::forward<Args>(args)...) {};

      private:

        void setHash(AHashTable::HashType h) {
            hash = h;
        }
    };

    class const_iterator : public AHashTable::Iterator {
        friend class UnorderedBase<T, Hash>;

      public:

        using difference_type = int;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::forward_iterator_tag;

        const_iterator() :
            AHashTable::Iterator(nullptr) {};

        const_iterator(const const_iterator& it) :
            AHashTable::Iterator(it) {};

        explicit const_iterator(const AHashTable::Iterator& it) :
            AHashTable::Iterator(it) {};

        const_reference operator*() const {
            return static_cast<const UnorderedBase<T, Hash>::Node*>(node())->item;
        }

        const_pointer operator->() const {
            return &(static_cast<const UnorderedBase<T, Hash>::Node*>(node())->item);
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

        explicit const_iterator(UnorderedBase<T, Hash>::Node* n) :
            AHashTable::Iterator(n) {};
    };

    class iterator : public AHashTable::Iterator {
        friend class UnorderedBase<T, Hash>;

      public:

        using difference_type = int;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::forward_iterator_tag;

        iterator() :
            AHashTable::Iterator(nullptr) {};

        iterator(const iterator& it) :
            AHashTable::Iterator(it) {};

        operator const_iterator() const {
            return const_iterator(*this);
        }

        reference operator*() const {
            return static_cast<UnorderedBase<T, Hash>::Node*>(node())->item;
        }

        pointer operator->() const {
            return &(static_cast<UnorderedBase<T, Hash>::Node*>(node())->item);
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

        explicit iterator(UnorderedBase<T, Hash>::Node* n) :
            AHashTable::Iterator(n) {};

        explicit iterator(const AHashTable::Iterator& it) :
            AHashTable::Iterator(it) {};
    };

    using BucketImpl = ETL_NAMESPACE::Vector<AHashTable::BucketItem>;
    using NodeAllocator = ETL_NAMESPACE::AAllocator<Node>;

  protected:  // variables

    BucketImpl& buckets;
    NodeAllocator& allocator;
    AHashTable hashTable;

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{
    explicit UnorderedBase(BucketImpl& b, NodeAllocator& a) :
        buckets(b),
        allocator(a),
        hashTable(b) {};

    UnorderedBase(const UnorderedBase& other) = delete;
    UnorderedBase& operator=(const UnorderedBase& other) = delete;

    UnorderedBase(UnorderedBase&& other) = default;
    UnorderedBase& operator=(UnorderedBase&& other) = default;

    ~UnorderedBase() {
        clear();
    }
    /// \}

    /// \name Capacity
    /// \{
    size_type size() const {
        return hashTable.size();
    }

    bool empty() const {
        return hashTable.empty();
    }
    /// \}

    /// \name Iterators
    /// \{
    iterator begin() {
        return iterator(hashTable.begin());
    }

    const_iterator begin() const {
        return const_iterator(hashTable.begin());
    }

    const_iterator cbegin() const {
        return this->begin();
    }

    iterator end() {
        return iterator(hashTable.end());
    }

    const_iterator end() const {
        return const_iterator(hashTable.end());
    }

    const_iterator cend() const {
        return this->end();
    }
    /// \}

    /// \name Modifiers
    /// \{
    void clear() noexcept(NodeAllocator::NoexceptDestroy);
    iterator erase(iterator pos) noexcept(NodeAllocator::NoexceptDestroy);
    /// \}

  protected:

    void bindBuckets(Buckets b) {
        hashTable.bindBuckets(b);
    }

    /// \name Lookup
    /// \{
    std::pair<iterator, iterator> equalHashRange(HashType hash) {
        auto res = hashTable.equalHashRange(hash);
        return {makeIt(res.first), makeIt(res.second)};
    }

    std::pair<const_iterator, const_iterator> equalHashRange(HashType hash) const {
        auto res = hashTable.equalHashRange(hash);
        return {makeConstIt(res.first), makeConstIt(res.second)};
    }

    template<typename P>
    iterator findExact(HashType hash, P predicate) {
        auto range = this->equalHashRange(hash);
        auto it = findExactInRange(range.first, range.second, std::move(predicate));
        return makeIt(it.node());
    }

    template<typename P>
    const_iterator findExact(HashType hash, P predicate) const {
        auto range = this->equalHashRange(hash);
        return findExactInRange(range.first, range.second, std::move(predicate));
    }

    size_type count(HashType hash) const {
        return hashTable.count(hash);
    }
    /// \}

    template<typename H>
    iterator insert(H hasher, const_reference item) {
        return emplace(std::move(hasher), item);
    }

    template<typename H, typename... Args>
    iterator emplace(H hasher, Args&&... args);

    /// \name Utils
    /// \{
    static iterator makeIt(AHashTable::Node* n) {
        return iterator(static_cast<Node*>(n));
    }

    static const_iterator makeConstIt(AHashTable::Node* n) {
        return const_iterator(static_cast<Node*>(n));
    }
    /// \}

  private:

    template<typename It, typename P>
    const_iterator findExactInRange(It first, It end, P predicate) const;
};


template<class T, class Hash>
void UnorderedBase<T, Hash>::clear() noexcept(NodeAllocator::NoexceptDestroy) {

    auto it = begin();
    while (it != end()) {
        it = erase(it);
    }
}


template<class T, class Hash>
auto UnorderedBase<T, Hash>::erase(iterator pos) noexcept(NodeAllocator::NoexceptDestroy)
    -> iterator {

    ETL_ASSERT(pos != end());

    auto next = pos;
    ++next;

    auto item = hashTable.remove(*pos.node());
    if (item != nullptr) {
        NodeAllocator::destroy(static_cast<Node*>(item));
        allocator.deallocate(static_cast<Node*>(item), 1U);
    }

    return next;
}


template<class T, class Hash>
template<typename It, typename P>
auto UnorderedBase<T, Hash>::findExactInRange(It first, It last, P predicate) const
    -> const_iterator {

    bool found = false;
    while ((!found) && (first != last)) {
        if (predicate(*first)) {
            found = true;
        } else {
            ++first;
        }
    }

    if (found) {
        return first;
    } else {
        return end();
    }
}


template<class T, class Hash>
template<typename H, typename... Args>
auto UnorderedBase<T, Hash>::emplace(H hasher, Args&&... args) -> iterator {

    auto inserted = allocator.allocate(1);
    if (inserted != nullptr) {
        NodeAllocator::construct(inserted, std::forward<Args>(args)...);
        inserted->setHash(hasher(inserted->item));
        hashTable.insert(*inserted);
        return iterator {inserted};
    } else {
        return this->end();
    }
}

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif /* __ETL_UNORDEREDBASE_H__ */