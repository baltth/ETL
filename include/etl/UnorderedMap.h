/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2022 Balazs Toth.

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
/// @tparam K key
/// @tparam E element
/// @tparam NA node allocator
/// @tparam BA bucket allocator
/// @tparam H hash type
/// @tparam KE 'key equal' type
template<class K,
         class E,
         template<class>
         class NA,
         template<class> class BA = NA,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedMap : public ETL_NAMESPACE::UnorderedMap<K, E, H, KE> {

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMap<K, E, H, KE>;
    using Node = typename Base::Node;

    using BucketImpl = Custom::Vector<typename Base::BucketItem, BA>;
    using NodeAllocator = ETL_NAMESPACE::AllocatorWrapper<Node, NA>;

    static constexpr size_t DEFAULT_BUCKETS {32U};

  private:  // variables

    BucketImpl buckets;
    mutable NodeAllocator allocator;

  public:  // functions

    UnorderedMap() :
        Base(buckets, allocator),
        buckets(DEFAULT_BUCKETS) {
        ETL_ASSERT(buckets.size() == DEFAULT_BUCKETS);
        this->bindOwnBuckets();
    }

    UnorderedMap(const UnorderedMap& other) :
        UnorderedMap() {
        Base::operator=(other);
    }

    explicit UnorderedMap(const Base& other) :
        UnorderedMap() {
        Base::operator=(other);
    }

    UnorderedMap& operator=(const UnorderedMap& other) {
        Base::operator=(other);
        return *this;
    }

    using Base::operator=;

    UnorderedMap(std::initializer_list<std::pair<K, E>> initList) :
        UnorderedMap() {
        Base::operator=(initList);
    }

    UnorderedMap(UnorderedMap&& other) :
        UnorderedMap() {
        operator=(std::move(other));
    }

    UnorderedMap& operator=(UnorderedMap&& other) {
        this->swap(other);
        return *this;
    }

    ~UnorderedMap() {
        this->clear();
    }

  private:

    friend void swap(UnorderedMap& lhs, Base& rhs) {
        lhs.swap(rhs);
    }
};

}  // namespace Custom


namespace Dynamic {

/// UnorderedMap with dynamic memory allocation using std::allocator.
/// @tparam K key
/// @tparam E element
/// @tparam H hash type
/// @tparam KE 'key equal' type
template<class K,
         class E,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
using UnorderedMap = ETL_NAMESPACE::Custom::UnorderedMap<K,
                                                         E,
                                                         std::allocator,
                                                         std::allocator,
                                                         H,
                                                         KE>;

}  // namespace Dynamic


namespace Static {

/// UnorderedMap with per instance storage.
/// @tparam K key
/// @tparam E element
/// @tparam NN maximum node count per instance
/// @tparam NB maximum bucket count per instance
/// @tparam H hash type
/// @tparam KE 'key equal' type
template<class K,
         class E,
         std::size_t NN,
         std::size_t NB = NN,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedMap : public ETL_NAMESPACE::UnorderedMap<K, E, H, KE> {

    static_assert(NN > 0, "Invalid Etl::Static::UnorderedMap size");
    static_assert(NB > 0, "Invalid Etl::Static::UnorderedMap size");

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMap<K, E, H, KE>;

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
        this->max_load_factor(static_cast<float>(NN) / static_cast<float>(NB));
    }

    UnorderedMap(const UnorderedMap& other) :
        UnorderedMap() {
        Base::operator=(other);
    }

    explicit UnorderedMap(const Base& other) :
        UnorderedMap() {
        Base::operator=(other);
    }

    UnorderedMap& operator=(const UnorderedMap& other) {
        Base::operator=(other);
        return *this;
    }

    using Base::operator=;

    UnorderedMap(std::initializer_list<std::pair<K, E>> initList) :
        UnorderedMap() {
        Base::operator=(initList);
    }

    UnorderedMap(UnorderedMap&& other) :
        UnorderedMap() {
        operator=(std::move(other));
    }

    UnorderedMap& operator=(UnorderedMap&& other) {
        this->swap(other);
        return *this;
    }

    ~UnorderedMap() {
        this->clear();
    }

  private:

    friend void swap(UnorderedMap& lhs, Base& rhs) {
        lhs.swap(rhs);
    }
};

}  // namespace Static


namespace Pooled {

/// UnorderedMap with per type node storage.
/// @tparam K key
/// @tparam E element
/// @tparam NN maximum node count per type
/// @tparam NB maximum bucket count per instance
/// @tparam H hash type
/// @tparam KE 'key equal' type
template<class K,
         class E,
         std::size_t NN,
         std::size_t NB = NN,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedMap : public ETL_NAMESPACE::UnorderedMap<K, E, H, KE> {

    static_assert(NN > 0, "Invalid Etl::Pooled::UnorderedMap size");
    static_assert(NB > 0, "Invalid Etl::Pooled::UnorderedMap size");

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMap<K, E, H, KE>;

    using NodeAllocator =
        typename ETL_NAMESPACE::PoolHelper<NN>::template CommonAllocator<typename Base::Node>;
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
        this->max_load_factor(static_cast<float>(NN) / static_cast<float>(NB));
    }

    UnorderedMap(const UnorderedMap& other) :
        UnorderedMap() {
        Base::operator=(other);
    }

    explicit UnorderedMap(const Base& other) :
        UnorderedMap() {
        Base::operator=(other);
    }

    UnorderedMap& operator=(const UnorderedMap& other) {
        Base::operator=(other);
        return *this;
    }

    using Base::operator=;

    UnorderedMap(std::initializer_list<std::pair<K, E>> initList) :
        UnorderedMap() {
        Base::operator=(initList);
    }

    UnorderedMap(UnorderedMap&& other) :
        UnorderedMap() {
        operator=(std::move(other));
    }

    UnorderedMap& operator=(UnorderedMap&& other) {
        this->swap(other);
        return *this;
    }

    ~UnorderedMap() {
        this->clear();
    }

  private:

    friend void swap(UnorderedMap& lhs, Base& rhs) {
        lhs.swap(rhs);
    }
};

}  // namespace Pooled

}  // namespace ETL_NAMESPACE

#endif  // __ETL_UNORDEREDMAP_H__
