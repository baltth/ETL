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

#include <etl/base/AHashTable.h>

using ETL_NAMESPACE::Detail::AHashTable;
using ETL_NAMESPACE::Detail::SingleChain;


void AHashTable::insert(AHashTable::Node& item) {

    ETL_ASSERT(buckets.data() != nullptr);
    ETL_ASSERT(buckets.size() > 0U);

    uint32_t ix = bucketIxOfHash(item.hash);

    if (buckets[ix] == nullptr) {

        buckets[ix] = lastItem;
        chain_.insertAfter(buckets[ix], &item);
        lastItem = buckets[ix]->next;

    } else {
        std::pair<SingleChain::Node*, bool> res = getPreviousInBucket(item.hash, ix);
        chain_.insertAfter(res.first, &item);
        if (res.first == lastItem) {
            lastItem = res.first->next;
        }
    }

    ++size_;
}


std::pair<SingleChain::Node*, bool> AHashTable::getPreviousInBucket(HashType hash, size_type ix) {

    ETL_ASSERT(bucketIxOfHash(hash) == ix);
    ETL_ASSERT(buckets[ix] != nullptr);

    bool found = false;
    bool end = false;
    SingleChain::Node* prev = buckets[ix];

    ETL_ASSERT(prev->next != nullptr);

    while ((!found) && (!end)) {

        auto next = static_cast<Node*>(prev->next);

        if ((next == nullptr) || (bucketIxOfHash(next->hash) != ix)) {
            end = true;
        } else if (next->hash > hash) {
            end = true;
        } else if (next->hash == hash) {
            found = true;
        } else {
            prev = next;
        }
    }

    return std::pair<SingleChain::Node*, bool>(prev, found);
}


AHashTable::Node* AHashTable::remove(AHashTable::Node& item) {

    auto* next = static_cast<Node*>(item.next);
    std::pair<SingleChain::Node*, std::uint32_t> prev = findPreviousOfNode(item);

    if (lastItem == &item) {
        lastItem = prev.first;
    }

    SingleChain::Node* removed = chain_.removeAfter(prev.first);

    std::uint32_t ix = prev.second;

    if (buckets[ix] == lastItem) {
        buckets[ix] = nullptr;
    } else {
        const Node& nextOfBucket = static_cast<const Node&>(*(buckets[ix]->next));
        if (bucketIxOfHash(nextOfBucket.hash) != ix) {
            buckets[ix] = nullptr;
        }
    }

    if (next != nullptr) {
        auto nextIx = bucketIxOfHash(next->hash);
        if (nextIx != ix) {
            // The next element of removed belongs to another bucket,
            // this means the next bucket pointer has to be corrected.
            buckets[nextIx] = prev.first;
        }
    }

    --size_;

    return static_cast<Node*>(removed);
}


std::pair<SingleChain::Node*, std::uint32_t>
AHashTable::findPreviousOfNode(AHashTable::Node& item) const {

    auto ix = bucketIxOfHash(item.hash);

    ETL_ASSERT(buckets[ix] != nullptr);

    SingleChain::Node* prev = buckets[ix];
    ETL_ASSERT(prev != nullptr);

    while (prev->next != &item) {
        prev = prev->next;
        ETL_ASSERT(prev != nullptr);
        ETL_ASSERT(bucketIxOfHash(static_cast<Node*>(prev)->hash) == ix);
    }

    return std::pair<SingleChain::Node*, std::uint32_t>(prev, ix);
}


const AHashTable::Node* AHashTable::findNode(HashType hash) const {

    const Node* res = nullptr;
    auto ix = bucketIxOfHash(hash);

    if (buckets[ix] != nullptr) {

        auto node = static_cast<const Node*>(buckets[ix]->next);

        ETL_ASSERT(node != nullptr);

        while ((node != nullptr) && (node->hash != hash) && (bucketIxOfHash(node->hash) == ix)) {
            node = static_cast<const Node*>(node->next);
        }

        if ((node != nullptr) && (node->hash == hash)) {
            res = node;
        }
    }

    return res;
}


std::pair<const AHashTable::Node*, const AHashTable::Node*>
AHashTable::equalHashRange(HashType hash) const {


    const Node* rangeEnd = nullptr;
    auto rangeStart = findNode(hash);

    if (rangeStart != nullptr) {

        rangeEnd = static_cast<const Node*>(rangeStart->next);

        while ((rangeEnd != nullptr) && (rangeEnd->hash == hash)) {
            rangeEnd = static_cast<const Node*>(rangeEnd->next);
        }
    }

    return std::pair<const Node*, const Node*>(rangeStart, rangeEnd);
}


std::uint32_t AHashTable::count(HashType hash) const {

    std::uint32_t cnt = 0U;
    std::pair<const Node*, const Node*> res = equalHashRange(hash);

    if (res.first != nullptr) {
        while (res.first != res.second) {
            ++cnt;
            res.first = static_cast<const Node*>(res.first->next);
        }
    }

    return cnt;
}
