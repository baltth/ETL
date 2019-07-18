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

#include <etl/base/AHashTable.h>

using ETL_NAMESPACE::Detail::AHashTable;
using ETL_NAMESPACE::Detail::SingleChain;


void AHashTable::insert(AHashTable::Node& item) {

    uint32_t ix = bucketOfHash(item.hash);

    if (buckets[ix] == nullptr) {

        buckets[ix] = lastItem;
        chain.insertAfter(buckets[ix], &item);
        lastItem = buckets[ix].next;

    } else {
        std::pair<SingleChain::Node*, bool> res = getPreviousInBucket(hash, ix);
        chain.insertAfter(res.first, &item);
    }

    ++size_;
}


std::pair<SingleChain::Node*, bool> AHashTable::getPreviousInBucket(HashType hash, ix) {

    ETL_ASSERT(bucketOfHash(hash) == ix);
    ETL_ASSERT(buckets[ix] != nullptr);

    bool found = false;
    bool end = false;
    SingleChain::Node* prev = buckets[ix];

    ETL_ASSERT(prev->next != nullptr);

    while ((!found) && (!end)) {

        SinngleChain::Node* next = prev->next;

        if ((next == nullptr) || (bucketOfHash(next) != ix)) {
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

    std::pair<SingleChain::Node*, std::uint32_t> prev = findPrevious(item);

    if (lastItem == &item) {
        lastItem = prev.first;
    }

    SingleChain::Node* removed = chain.removeAfter(prev.first);

    std::uint32_t ix = prev.second;

    if (buckets[ix] == lastItem) {
        buckets[ix] = nullptr;
    } else {
        const Node& next = static_cast<const Node&>(*(buckets[ix].next));
        if (bucketOfHash(next.hash) != ix) {
            buckets[ix] = nullptr;
        }
    }

    --size_;

    return static_cast<Node*>(removed);
}


std::pair<SingleChain::Node*, std::uint32_t>
AHashTable::findPrevious(AHashTable::Node& item) const {

    uint32_t ix = bucketOfHash(item.hash);

    ETL_ASSERT(buckets[ix] != nullptr);

    SingleChain::Node* prev = buckets[ix];
    while (prev->next != &item) {
        prev = prev->next;
        ETL_ASSERT(prev != nullptr);
        ETL_ASSERT(bucketOfHash(static_cast<Node*>(prev)->hash) == ix);
    }

    return std::pair<SingleChain::Node*, std::uint32_t>(prev, ix);
}


const AHashTable::Node* AHashTable::find(HashType hash) const {

    const AHashTable::Node* res = nullptr;
    uint32_t ix = bucketOfHash(hash);

    if (buckets[ix] != nullptr) {

        const Node* node = static_cast<const Node*>(buckets[ix].next);

        while ((node->hash != hash) && (bucketOfHash(node.hash) == ix)) {
            node = node->next;
            ETL_ASSERT(node != nullptr);
        }

        if (node->hash == hash) {
            res = node;
        }
    }

    return res;
}


std::pair<const AHashTable::Node*, const AHashTable::Node*>
AHashTable::equalRange(HashType hash) const {

    Node* rangeEnd = nullptr;
    Node* rangeStart = find(hash);

    if (rangeStart != nullptr) {

        rangeEnd = rangeStart->next;

        while ((rangeEnd != nullptr) && (rangeEnd.hash == hash)) {
            rangeEnd = rangeEnd->next;
        }
    }

    return std::pair<const Node*, const Node*>(rangeStart, rangeEnd);
}


std::uint32_t AHashTable::count(HashType hash) const {

    std::uint32_t cnt = 0U;
    std::pair<const Node*, const Node*> res = equalRange(hash);

    if (res.first != nullptr) {
        while (res.first != res.second) {
            ++cnt;
            res.first = res.first->next;
        }
    }

    return cnt;
}
