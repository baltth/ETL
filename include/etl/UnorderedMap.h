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

#ifndef __ETL_UNORDEREDMAP_H__
#define __ETL_UNORDEREDMAP_H__

#include <etl/PoolAllocator.h>
#include <etl/Vector.h>
#include <etl/base/UnorderedMapTemplate.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {

namespace Custom {

/// UnorderedMap with custom allocator.
template<class K, class E, template<class> class A>
class UnorderedMap : public ETL_NAMESPACE::UnorderedMap<K, E> {

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMap<K, E>;
    using Node = typename Base::Node;

    using BucketImpl = Custom::Vector<typename Base::BucketItem, A>;
    using NodeAllocator = ETL_NAMESPACE::AllocatorWrapper<Node, A>;

  private:  // variables

    BucketImpl buckets;
    mutable NodeAllocator allocator;

  public:  // functions

    UnorderedMap() :
        Base(buckets, allocator),
        buckets(32) {
        ETL_ASSERT(buckets.size() == 32);
        this->bindOwnBuckets();
    };

    ~UnorderedMap() {
        this->clear();
    }
};

}  // namespace Custom


namespace Dynamic {

/// UnorderedMap with dynamic memory allocation using std::allocator.
template<class K, class E>
using UnorderedMap = ETL_NAMESPACE::Custom::UnorderedMap<K, E, std::allocator>;

}  // namespace Dynamic


namespace Static {

template<class K, class E, std::size_t NN, std::size_t NB>
class UnorderedMap : public ETL_NAMESPACE::UnorderedMap<K, E> {

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMap<K, E>;

    using NodeAllocator =
        typename ETL_NAMESPACE::PoolHelper<NN>::template Allocator<typename Base::Node>;
    using BucketImpl = Static::Vector<typename Base::BucketItem, NB>;

  private:  // variables

    BucketImpl buckets;
    mutable NodeAllocator allocator;

  public:  // functions

    UnorderedMap() :
        Base(buckets, allocator),
        buckets(NB) {
        ETL_ASSERT(buckets.size() == NB);
        this->bindOwnBuckets();
    }

    ~UnorderedMap() {
        this->clear();
    }
};

}  // namespace Static

}  // namespace ETL_NAMESPACE

#endif  // __ETL_UNORDEREDMAP_H__
