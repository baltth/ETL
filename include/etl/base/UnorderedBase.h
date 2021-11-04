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


template<class T>
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
        friend class UnorderedBase<T>;

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
        friend class UnorderedBase<T>;

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
            return static_cast<const UnorderedBase<T>::Node*>(node())->item;
        }

        const_pointer operator->() const {
            return &(static_cast<const UnorderedBase<T>::Node*>(node())->item);
        }

        using AHashTable::Iterator::operator==;
        using AHashTable::Iterator::operator!=;

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
            return static_cast<UnorderedBase<T>::Node*>(node())->item;
        }

        pointer operator->() const {
            return &(static_cast<UnorderedBase<T>::Node*>(node())->item);
        }

        using AHashTable::Iterator::operator==;
        using AHashTable::Iterator::operator!=;

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

    class const_local_iterator : public AHashTable::LocalIterator {
        friend class UnorderedBase<T>;

      public:

        using difference_type = int;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::forward_iterator_tag;

        const_local_iterator() = default;

        const_local_iterator(const const_local_iterator& it) :
            AHashTable::LocalIterator(it) {};

        explicit const_local_iterator(const AHashTable::LocalIterator& it) :
            AHashTable::LocalIterator(it) {};

        const_reference operator*() const {
            return static_cast<const UnorderedBase<T>::Node*>(node())->item;
        }

        const_pointer operator->() const {
            return &(static_cast<const UnorderedBase<T>::Node*>(node())->item);
        }

        using AHashTable::LocalIterator::operator==;
        using AHashTable::LocalIterator::operator!=;

        const_local_iterator& operator++() {
            AHashTable::LocalIterator::operator++();
            return *this;
        }

        const const_local_iterator operator++(int) {
            const_local_iterator old = *this;
            this->operator++();
            return old;
        }

      private:

        explicit const_local_iterator(UnorderedBase<T>::Node* n) :
            AHashTable::LocalIterator(n) {};
    };

    class local_iterator : public AHashTable::LocalIterator {
        friend class UnorderedBase<T>;

      public:

        using difference_type = int;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::forward_iterator_tag;

        local_iterator() = default;

        local_iterator(const local_iterator& it) :
            AHashTable::LocalIterator(it) {};

        explicit local_iterator(const AHashTable::LocalIterator& it) :
            AHashTable::LocalIterator(it) {};

        const_reference operator*() const {
            return static_cast<const UnorderedBase<T>::Node*>(node())->item;
        }

        const_pointer operator->() const {
            return &(static_cast<const UnorderedBase<T>::Node*>(node())->item);
        }

        using AHashTable::LocalIterator::operator==;
        using AHashTable::LocalIterator::operator!=;

        local_iterator& operator++() {
            AHashTable::LocalIterator::operator++();
            return *this;
        }

        const local_iterator operator++(int) {
            local_iterator old = *this;
            this->operator++();
            return old;
        }

      private:

        explicit local_iterator(UnorderedBase<T>::Node* n) :
            AHashTable::LocalIterator(n) {};
    };

    using BucketImpl = ETL_NAMESPACE::Vector<AHashTable::BucketItem>;
    using NodeAllocator = ETL_NAMESPACE::AAllocator<Node>;

  protected:  // variables

    BucketImpl& buckets;
    NodeAllocator& allocator;
    AHashTable hashTable;
    float mlf;

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{
    UnorderedBase(BucketImpl& b, NodeAllocator& a) :
        buckets {b},  // Note: reference of buckets are stored here but...
        allocator {a},
        hashTable {},  // ...bucket binding for the hashTable is skipped intentionally as creating a
                       // Span<> from a container would access uninitialized data of the container.
        mlf {1.0f} {};

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

    template<typename H>
    void swap(H hasher, UnorderedBase& other) {
        if (this != &other) {
            if (allocator.handle() == other.allocator.handle()) {
                hashTable.swapWithSources(buckets,
                                          other.hashTable,
                                          other.buckets);
            } else {
                swapElements(std::move(hasher), other);
            }
        }
    }
    /// \}

    /// \name Bucket interface
    /// \{
    local_iterator begin(size_type ix) {
        return local_iterator(hashTable.begin(ix));
    }

    const_local_iterator begin(size_type ix) const {
        return const_local_iterator(hashTable.begin(ix));
    }

    const_local_iterator cbegin(size_type ix) const {
        return this->begin(ix);
    }

    local_iterator end(size_type ix) {
        return local_iterator(hashTable.end(ix));
    }

    const_local_iterator end(size_type ix) const {
        return const_local_iterator(hashTable.end(ix));
    }

    const_local_iterator cend(size_type ix) const {
        return this->end(ix);
    }

    size_type bucket_size(size_type ix) const {
        return hashTable.bucketSize(ix);
    }

    size_type bucket_count() const {
        return buckets.size();
    }

    size_type max_bucket_count() const {
        return buckets.max_size();
    }
    /// \}

    /// \name Hash policy
    /// \{
    float load_factor() const {
        return static_cast<float>(size()) / bucket_count();
    }

    float max_load_factor() const {
        return mlf;
    }

    void max_load_factor(float m) {
        static constexpr float LF_MIN = 0.01f;
        mlf = (m > LF_MIN) ? m : LF_MIN;
    }

    void rehash(size_type count);
    /// \}

    const AHashTable& ht() const {
        return hashTable;
    }

  protected:

    void bindOwnBuckets() {
        hashTable.bindBuckets(buckets);
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

    size_type bucketIx(HashType h) const {
        return hashTable.bucketIxOfHash(h);
    }

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

    template<typename H>
    void swapElements(H hasher, UnorderedBase& other);

    /// Helper to perform non-trivial swap on two elements of different containers.
    /// This overload is used when `T` conforms the contract of a `swap` function.
    template<typename H, typename U = T>
    enable_if_t<Detail::UseSwapInCont<U>::value, std::pair<Node*, Node*>>
    swapTwo(H hasher, Node* own, UnorderedBase& other, Node* toSwap) {
        using std::swap;
        (void)other;
        (void)hasher;
        Node* nextOwn = static_cast<Node*>(own->next);
        Node* nextToSwap = static_cast<Node*>(toSwap->next);
        swap(own->item, toSwap->item);
        hashTable.insert(own);
        other.hashTable.insert(toSwap);
        return std::make_pair(nextOwn, nextToSwap);
    }

    /// Helper to perform non-trivial swap on two elements of different containers.
    /// This overload is used when `T` does not conforms the contract of a `swap` function.
    /// The function uses no assignment, but expects capacity for one extra element on `this`.
    template<typename H, class U = T>
    enable_if_t<!Detail::UseSwapInCont<U>::value, std::pair<Node*, Node*>>
    swapTwo(H hasher, Node* own, UnorderedBase& other, Node* toSwap) {
        auto nextOther = stealElement(hasher, other, toSwap);
        auto nextOwn = other.stealElement(hasher, *this, own);
        return std::make_pair(nextOwn, nextOther);
    }

    template<typename H>
    Node* stealElement(H hasher,
                       UnorderedBase& other,
                       Node* toSteal) {
        Node* next = static_cast<Node*>(toSteal->next);
        emplace(std::move(hasher), std::move(toSteal->item));
        other.destroy(toSteal);
        return next;
    }

    void destroy(Node* node) {
        NodeAllocator::destroy(node);
        allocator.deallocate(node, 1U);
    }

    void rehashForNextInsertOnDemand() {
        auto rehashLimit = static_cast<size_t>(max_load_factor() * bucket_count());
        if ((size() + 1U) > rehashLimit) {
            // @todo add a better scaling logic
            rehash(rehashLimit * 2U);
        }
    }
};


template<class T>
void UnorderedBase<T>::clear() noexcept(NodeAllocator::NoexceptDestroy) {

    auto it = begin();
    while (it != end()) {
        it = erase(it);
    }
}


template<class T>
auto UnorderedBase<T>::erase(iterator pos) noexcept(NodeAllocator::NoexceptDestroy)
    -> iterator {

    ETL_ASSERT(pos != end());

    auto next = pos;
    ++next;

    auto item = hashTable.remove(*pos.node());
    if (item != nullptr) {
        destroy(static_cast<Node*>(item));
    }

    return next;
}


template<class T>
template<typename It, typename P>
auto UnorderedBase<T>::findExactInRange(It first, It last, P predicate) const -> const_iterator {

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


template<class T>
template<typename H, typename... Args>
auto UnorderedBase<T>::emplace(H hasher, Args&&... args) -> iterator {

    rehashForNextInsertOnDemand();

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


template<class T>
template<typename H>
void UnorderedBase<T>::swapElements(H hasher, UnorderedBase& other) {

    auto origOwnSize = size();
    auto origOwnBucketsSize = buckets.size();
    auto origOtherSize = other.size();
    auto origOtherBucketsSize = other.buckets.size();

    if ((allocator.max_size() >= (origOtherSize + 1U))
        && (other.allocator.max_size() >= (origOwnSize + 1U))) {

        // Steal the chains to stack variables
        SingleChain origOwnChain = std::move(hashTable.chain());
        SingleChain origOtherChain = std::move(other.hashTable.chain());

        ETL_ASSERT(hashTable.chain().isEmpty());
        ETL_ASSERT(other.hashTable.chain().isEmpty());

        // Resize and reset the buckets
        auto setupBuckets = [](BucketImpl& buckets, size_type targetSize) {
            if ((buckets.capacity() >= targetSize) && (buckets.size() < targetSize)) {
                buckets.resize(targetSize);
            }
            for (auto& item : buckets) {
                item = nullptr;
            }
        };

        setupBuckets(buckets, origOtherBucketsSize);
        setupBuckets(other.buckets, origOwnBucketsSize);

        ETL_ASSERT((buckets.size() >= origOtherBucketsSize)
                   || (buckets.size() == origOwnBucketsSize));
        ETL_ASSERT((other.buckets.size() >= origOwnBucketsSize)
                   || (other.buckets.size() == origOtherBucketsSize));

        // Reset the hashTables
        hashTable = AHashTable {buckets};
        other.hashTable = AHashTable {other.buckets};

        // Realloc and insert elements
        Node* ownNode = static_cast<Node*>(origOwnChain.getFirst());
        Node* otherNode = static_cast<Node*>(origOtherChain.getFirst());

        const auto diff = sizeDiff(origOwnSize, origOtherSize);
        for (uint32_t i = 0; i < diff.common; ++i) {

            ETL_ASSERT(ownNode != nullptr);
            ETL_ASSERT(otherNode != nullptr);

            auto res = swapTwo(hasher, ownNode, other, otherNode);
            ownNode = res.first;
            otherNode = res.second;
        }

        auto stealRemaining = [&hasher](UnorderedBase& dest,
                                        UnorderedBase& src,
                                        Node* remaining) {
            while (remaining != nullptr) {
                remaining = dest.stealElement(hasher, src, remaining);
            }
        };

        if (diff.lGreaterWith > 0) {
            stealRemaining(other, *this, ownNode);
        } else if (diff.rGreaterWith > 0) {
            stealRemaining(*this, other, otherNode);
        } else {
            // NOP
        }
    }
}


template<class T>
void UnorderedBase<T>::rehash(size_type count) {

    // Static::Vector<> buckets are not rehashed. This is detected by
    // checking size() vs capacity() vs max_size()
    if ((buckets.size() == buckets.capacity()) && (buckets.size() == buckets.max_size())) {
        return;
    }

    // Checking and correcting the count according to
    // https://en.cppreference.com/w/cpp/container/unordered_map/rehash
    ETL_ASSERT(max_load_factor() > 0.0);
    auto countLimit = static_cast<size_type>(size() / max_load_factor());
    if (count < countLimit) {
        count = countLimit;
    }

    buckets.reserve(count);
    if (buckets.capacity() >= count) {
        buckets.clear();
        buckets.insert(buckets.begin(), count, nullptr);
        hashTable = rehashTable(hashTable, buckets);
    }
}

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif /* __ETL_UNORDEREDBASE_H__ */
