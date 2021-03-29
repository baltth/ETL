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

#include <utility>

namespace ETL_NAMESPACE {
namespace Detail {


class AHashTable {

  public:  // types

    using size_type = std::uint32_t;
    using HashType = std::size_t;

    class Node : public SingleChain::Node {
        friend class AHashTable;

      protected:  // variables

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

    using BucketItem = SingleChain::Node*;
    using Buckets = Span<BucketItem>;

  protected:  // variables

    SingleChain chain;
    size_type size_;

    Buckets buckets;
    BucketItem lastItem;

  public:  // functions

    AHashTable() :
        size_(0U),
        lastItem(&chain.getFrontNode()) {};

    explicit AHashTable(Buckets b) :
        AHashTable() {
        buckets = b;
    };

    AHashTable(const AHashTable& other) = delete;
    AHashTable& operator=(const AHashTable& other) = delete;

    AHashTable(AHashTable&& other) = default;
    AHashTable& operator=(AHashTable&& other) = default;

    ~AHashTable() = default;

    size_type size() const {
        return size_;
    }

    bool empty() const {
        return (size_ == 0U);
    }

    Iterator begin() const {
        return Iterator(static_cast<AHashTable::Node*>(chain.getFirst()));
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

    size_type bucketOfHash(HashType h) const {
        ETL_ASSERT(buckets.size() > 0U);
        return (h % buckets.size());
    }

    void bindBuckets(Buckets b) {
        ETL_ASSERT(empty());
        buckets = b;
    }

    Buckets rehash(Buckets newBuckets) {

        ETL_ASSERT(newBuckets.size() > 0U);

        auto oldBuckets = buckets;
        AHashTable rehashed(newBuckets);

        while (!chain.isEmpty()) {

            auto node = chain.removeAfter(&chain.getFrontNode());

            ETL_ASSERT(node != nullptr);
            rehashed.insert(static_cast<Node&>(*node));
        }

        *this = std::move(rehashed);

        return oldBuckets;
    }

  private:

    std::pair<SingleChain::Node*, bool> getPreviousInBucket(HashType hash, size_type ix);
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_AHASHTABLE_H__
