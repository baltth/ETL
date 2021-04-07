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

#ifndef __ETL_AHASHTABLE_H__
#define __ETL_AHASHTABLE_H__

#include <etl/Span.h>
#include <etl/base/SingleChain.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>

#include <limits>
#include <utility>

namespace ETL_NAMESPACE {
namespace Detail {


class AHashTable {

  public:  // types

    using size_type = std::uint32_t;
    using HashType = std::size_t;

    class Node : public SingleChain::Node {
        friend class AHashTable;

      public:  // variables

        HashType hash;

      protected:  // functions

        Node() :
            SingleChain::Node(),
            hash(0U) {};

        Node(const SingleChain::Node& n, HashType h) :
            SingleChain::Node(n),
            hash(h) {};
    };

    class Iterator {
        friend class AHashTable;

      protected:  // variables

        AHashTable::Node* node_;

      public:  // functions

        bool operator==(const Iterator& other) const {
            return (node_ == other.node_);
        }

        bool operator!=(const Iterator& other) const {
            return !(operator==(other));
        }

        Iterator& operator++() {
            node_ = static_cast<AHashTable::Node*>(node_->next);
            return *this;
        }

      protected:

        explicit Iterator(AHashTable::Node* n) :
            node_(n) {};

        Node* node() {
            return node_;
        }

        Node* node() const {
            return node_;
        }
    };

    class LocalIterator : protected Iterator {
        friend class AHashTable;

      public:  // types

        static constexpr size_type INVALID_IX {std::numeric_limits<size_type>::max()};

      protected:  // variables

        size_type origIx;
        mutable size_type ix;
        size_type div;

      public:  // functions

        LocalIterator& operator++() {
            Iterator::operator++();
            if (node() != nullptr) {
                ix = AHashTable::bucketIx(node()->hash, div);
            } else {
                ix = INVALID_IX;
            }
            return *this;
        }

        bool operator==(const LocalIterator& other) const;

        bool operator!=(const LocalIterator& other) const {
            return !this->operator==(other);
        }

      protected:

        LocalIterator(AHashTable::Node* n, size_type ix, size_type d) :
            Iterator(n),
            origIx(ix),
            ix((n != nullptr) ? ix : INVALID_IX),
            div(d) {};

        LocalIterator() :
            LocalIterator(nullptr, 0, 1) {};

      private:

        bool inOrigBucket() const {
            return ix == origIx;
        }

        bool isConstructedEnd() const {
            return ix == INVALID_IX;
        }

        bool isEndOfIx(size_type ix) const {
            return (ix == this->origIx) && isConstructedEnd();
        }
    };

    using BucketItem = SingleChain::Node*;
    using Buckets = Span<BucketItem>;

  private:  // variables

    SingleChain chain_;
    size_type size_;

    Buckets buckets;
    BucketItem lastItem;

  public:  // functions

    AHashTable() :
        size_(0U),
        lastItem(&chain_.getFrontNode()) {};

    explicit AHashTable(Buckets b) :
        AHashTable() {
        buckets = b;
    };

    AHashTable(const AHashTable& other) = delete;
    AHashTable& operator=(const AHashTable& other) = delete;

    AHashTable(AHashTable&& other) noexcept :
        AHashTable() {
        *this = std::move(other);
    }

    AHashTable& operator=(AHashTable&& other) noexcept {
        steal(other);
        other.reset();
        return *this;
    }

    ~AHashTable() = default;

    size_type size() const {
        return size_;
    }

    bool empty() const {
        return (size_ == 0U);
    }

    Iterator begin() const {
        return Iterator(static_cast<AHashTable::Node*>(chain_.getFirst()));
    }

    Iterator end() const {
        return Iterator(static_cast<AHashTable::Node*>(lastItem->next));
    }

    /// \name Element operations
    /// \{
    void insert(Node& item);
    Node* remove(Node& item);

    std::pair<SingleChain::Node*, size_type> findPreviousOfNode(Node& item) const;

    Node* findNode(HashType hash) {
        return const_cast<Node*>(static_cast<const AHashTable*>(this)->findNode(hash));
    }

    const Node* findNode(HashType hash) const;

    std::pair<Node*, Node*> equalHashRange(HashType hash) {
        auto res = Detail::asConst(this)->equalHashRange(hash);
        return std::pair<Node*, Node*>(const_cast<Node*>(res.first), const_cast<Node*>(res.second));
    }

    std::pair<const Node*, const Node*> equalHashRange(HashType hash) const;

    size_type count(HashType hash) const;

    /// \}

    /// \name Bucket interface
    /// \{

    LocalIterator begin(size_type ix) const;

    LocalIterator end(size_type ix) const {
        return LocalIterator {nullptr, ix, divisorForHash()};
    }

    size_type bucketSize(size_type ix) const {
        size_type cnt = 0U;
        auto it = begin(ix);
        auto endIt = end(ix);
        while (it != endIt) {
            ++cnt;
            ++it;
        }
        return cnt;
    }

    static size_type bucketIx(HashType h, size_type divisor) {
        ETL_ASSERT(divisor > 0U);
        return (h % divisor);
    }

    size_type divisorForHash() const {
        return buckets.size();
    }

    size_type bucketIxOfHash(HashType h) const {
        return bucketIx(h, divisorForHash());
    }

    BucketItem& bucketOfHash(HashType h) const {
        return buckets[bucketIxOfHash(h)];
    }

    void bindBuckets(Buckets b) {
        ETL_ASSERT(empty());
        buckets = b;
    }

    Buckets getBuckets() const {
        return buckets;
    }
    /// \}

    SingleChain& chain() {
        return chain_;
    }

    template<typename B>
    void swapWithSources(B& ownBucketSource,
                         AHashTable& other,
                         B& otherBucketSource);

    void consume(SingleChain& chain);

    template<typename I>
    void inspect(I inspector) const {
        auto* node = static_cast<const Node*>(chain_.getFirst());
        while (node != nullptr) {
            inspector(bucketIxOfHash(node->hash), node);
            node = static_cast<const Node*>(node->next);
        }
    }

  private:

    void steal(AHashTable& other) noexcept {
        chain_ = std::move(other.chain_);
        size_ = other.size_;
        buckets = other.buckets;
        if (size_ == 0) {
            lastItem = &chain_.getFrontNode();
        } else {
            lastItem = other.lastItem;
        }
    }

    void reset() noexcept {
        chain_ = SingleChain {};
        size_ = 0;
        buckets = Buckets {};
        lastItem = &chain_.getFrontNode();
    }

    std::pair<SingleChain::Node*, bool> getPreviousInBucket(HashType hash, size_type ix);
};


inline bool AHashTable::LocalIterator::operator==(const LocalIterator& other) const {

    if (origIx == other.origIx) {
        if (inOrigBucket() && other.inOrigBucket()) {
            return Iterator::operator==(other);
        } else if ((!inOrigBucket()) && (other.isEndOfIx(origIx))) {
            return true;
        } else if (isEndOfIx(other.origIx) && (!other.inOrigBucket())) {
            return true;
        } else if ((!inOrigBucket()) && (!other.inOrigBucket())) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


inline AHashTable::LocalIterator AHashTable::begin(size_type ix) const {

    if ((ix >= buckets.size()) || (buckets[ix] == nullptr)) {
        return end(ix);
    }

    auto* first = buckets[ix]->next;
    return LocalIterator {static_cast<Node*>(first), ix, divisorForHash()};
}


inline void swap(AHashTable& lhs, AHashTable& rhs) {
    AHashTable tmp {std::move(lhs)};
    lhs = std::move(rhs);
    rhs = std::move(tmp);
}


template<typename B>
void AHashTable::swapWithSources(B& ownBucketSource,
                                 AHashTable& other,
                                 B& otherBucketSource) {

    // swap the base
    swap(*this, other);

    // swap the bucket contents and rebind the buckets
    ownBucketSource.swap(otherBucketSource);
    buckets = Buckets {ownBucketSource};
    other.buckets = Buckets {otherBucketSource};

    // fix the incorrect 'front node' references in buckets
    auto fixBucketOfFirst = [](AHashTable& table) {
        if (!table.chain_.isEmpty()) {
            auto* first = static_cast<const Node*>(table.chain_.getFirst());
            ETL_ASSERT(first != nullptr);
            auto& b = table.bucketOfHash(first->hash);
            ETL_ASSERT(b != nullptr);
            b = &table.chain_.getFrontNode();
        }
    };
    fixBucketOfFirst(*this);
    fixBucketOfFirst(other);
}


inline void AHashTable::consume(SingleChain& chain) {

    while (!chain.isEmpty()) {

        auto* node = chain.removeAfter(&chain.getFrontNode());

        ETL_ASSERT(node != nullptr);
        insert(static_cast<AHashTable::Node&>(*node));
    }
}


inline AHashTable rehash(AHashTable& hashTable, AHashTable::Buckets newBuckets) {

    ETL_ASSERT(newBuckets.size() > 0U);

    AHashTable rehashed(newBuckets);
    rehashed.consume(hashTable.chain());

    return rehashed;
}


}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_AHASHTABLE_H__
