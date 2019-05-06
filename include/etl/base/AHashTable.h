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

#ifndef __ETL_AHASHTABLE_H__
#define __ETL_AHASHTABLE_H__

#include <etl/etlSupport.h>
#include <etl/base/SingleChain.h>
#include <etl/Proxy.h>

#include <utility>

namespace ETL_NAMESPACE {
namespace Detail {


class AHashTable {

  public:  // types

    typedef std::uint32_t size_type;

    typedef std::size_t HashType;

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

        AHashTable::Node* node;

      public:  // functions

        bool operator==(const Iterator& other) const {
            return (node == other.node);
        }

        bool operator!=(const Iterator& other) const {
            return !(operator==(other));
        }

        Iterator& operator++() {
            node = static_cast<AHashTable::Node*>(node->next);
            return *this;
        }

      protected:

        explicit Iterator(AHashTable::Node* n) :
            node(n) {};
    };

    typedef SingleChain::Node* BucketItem;

  protected:  // variables

    SingleChain chain;
    size_type size_;

    MutableProxy<BucketItem> buckets;
    BucketItem lastItem;

  public:  // functions

    AHashTable(BucketItem* b, size_type s) :
        size_(0U),
        buckets(b, s),
        lastItem(&chain.getFrontNode()) {
        ETL_ASSERT(b != nullptr);
        ETL_ASSERT(s > 0U);
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

  protected:

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

    std::pair<SingleChain::Node*, size_type> findPrevious(Node& item) const;

    Node* find(HashType hash) {
        return const_cast<Node*>(static_cast<const AHashTable*>(this)->find(hash));
    }

    const Node* find(HashType hash) const;

    std::pair<Node*, Node*> equalRange(HashType hash) {
        auto res = static_cast<const AHashTable*>(this)->equalRange(hash);
        return std::pair<Node*, Node*>(const_cast<Node*>(res.first), const_cast<Node*>(res.second));
    }

    std::pair<const Node*, const Node*> equalRange(HashType hash) const;

    size_type count(HashType hash) const;

    /// \}

    size_type bucketOfHash(HashType h) const {
        return (h % buckets.size());
    }

  private:

    std::pair<SingleChain::Node*, bool> getPreviousInBucket(HashType hash, size_type ix);

};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_AHASHTABLE_H__
