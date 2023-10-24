/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2023 Balazs Toth.

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

#ifndef __ETL_UNORDEREDMULTIMAP_H__
#define __ETL_UNORDEREDMULTIMAP_H__

#include <etl/PoolAllocator.h>
#include <etl/Vector.h>
#include <etl/base/UnorderedMultiMapTemplate.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {

namespace Custom {

/// UnorderedMultiMap with custom allocator.
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
class UnorderedMultiMap : public ETL_NAMESPACE::UnorderedMultiMap<K, E, H, KE> {

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMultiMap<K, E, H, KE>;
    using Node = typename Base::Node;

    using BucketImpl = Custom::Vector<typename Base::BucketItem, BA>;
    using NodeAllocatorTraits = typename Detail::AllocatorTraits<Node, NA>;
    using NodeAllocator = typename NodeAllocatorTraits::Type;

    static constexpr size_t DEFAULT_BUCKETS {32U};

  private:  // variables

    BucketImpl buckets;
    mutable NodeAllocator allocator;

  public:  // functions

    UnorderedMultiMap() :
        Base(buckets, allocator),
        buckets(DEFAULT_BUCKETS) {
        ETL_ASSERT(buckets.size() == DEFAULT_BUCKETS);
        this->bindOwnBuckets();
    }

    UnorderedMultiMap(const UnorderedMultiMap& other) :
        UnorderedMultiMap() {
        Base::operator=(other);
    }

    explicit UnorderedMultiMap(const Base& other) :
        UnorderedMultiMap() {
        Base::operator=(other);
    }

    UnorderedMultiMap& operator=(const UnorderedMultiMap& other) {
        Base::operator=(other);
        return *this;
    }

    using Base::operator=;

    UnorderedMultiMap(std::initializer_list<typename Base::value_type> initList) :
        UnorderedMultiMap() {
        Base::operator=(initList);
    }

    UnorderedMultiMap(UnorderedMultiMap&& other) noexcept(
        noexcept(Base(buckets, allocator)) && noexcept(BucketImpl())
        && noexcept(std::declval<UnorderedMultiMap>().operator=(std::move(other)))) :
        // No default constructor delegation as it's not noexcept
        Base(buckets, allocator),
        buckets() {
        this->bindOwnBuckets();
        operator=(std::move(other));
    }

    UnorderedMultiMap& operator=(UnorderedMultiMap&& other) noexcept(
        noexcept(std::declval<UnorderedMultiMap>().swap(other))) {
        this->swap(other);
        return *this;
    }

    ~UnorderedMultiMap() {
        this->clear();
    }

    void swap(UnorderedMultiMap& other) noexcept(
        noexcept(std::declval<UnorderedMultiMap>().swapSameType(other))) {
        if (&other != this) {
            swapSameType(other);
        }
    }

    using Base::swap;

  private:

    void swapSameType(UnorderedMultiMap& other) noexcept {
        static_assert(noexcept(Detail::AHashTable::swapWithSources(this->Base::hashTable,
                                                                   this->buckets,
                                                                   other.Base::hashTable,
                                                                   other.buckets)),
                      "noexcept contract violation");
        static_assert(!NodeAllocatorTraits::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        ETL_ASSERT(allocator.handle() == other.allocator.handle());
        Detail::AHashTable::swapWithSources(this->Base::hashTable,
                                            this->buckets,
                                            other.Base::hashTable,
                                            other.buckets);
    }

    friend void swap(UnorderedMultiMap& lhs,
                     UnorderedMultiMap& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Custom


namespace Dynamic {

/// UnorderedMultiMap with dynamic memory allocation using std::allocator.
/// @tparam K key
/// @tparam E element
/// @tparam H hash type
/// @tparam KE 'key equal' type
template<class K,
         class E,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
using UnorderedMultiMap = ETL_NAMESPACE::Custom::UnorderedMultiMap<K,
                                                                   E,
                                                                   std::allocator,
                                                                   std::allocator,
                                                                   H,
                                                                   KE>;

}  // namespace Dynamic


namespace Static {

/// UnorderedMultiMap with per instance storage.
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
class UnorderedMultiMap : public ETL_NAMESPACE::UnorderedMultiMap<K, E, H, KE> {

    static_assert(NN > 0, "Invalid Etl::Static::UnorderedMultiMap size");
    static_assert(NB > 0, "Invalid Etl::Static::UnorderedMultiMap size");

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMultiMap<K, E, H, KE>;

    using NodeAllocator =
        typename ETL_NAMESPACE::PoolHelperForSize<NN>::template Allocator<typename Base::Node>;
    using BucketImpl = Static::Vector<typename Base::BucketItem, NB>;

  private:  // variables

    BucketImpl buckets;
    mutable NodeAllocator allocator;

  public:  // functions

    UnorderedMultiMap() noexcept(noexcept(Base(buckets, allocator))) :
        Base(buckets, allocator),
        buckets(NB) {
        ETL_ASSERT(buckets.size() == NB);
        this->bindOwnBuckets();
        this->max_load_factor(static_cast<float>(NN) / static_cast<float>(NB));
    }

    UnorderedMultiMap(const UnorderedMultiMap& other) :
        UnorderedMultiMap() {
        Base::operator=(other);
    }

    explicit UnorderedMultiMap(const Base& other) :
        UnorderedMultiMap() {
        Base::operator=(other);
    }

    UnorderedMultiMap& operator=(const UnorderedMultiMap& other) {
        Base::operator=(other);
        return *this;
    }

    using Base::operator=;

    UnorderedMultiMap(std::initializer_list<typename Base::value_type> initList) :
        UnorderedMultiMap() {
        Base::operator=(initList);
    }

    UnorderedMultiMap(UnorderedMultiMap&& other) noexcept(
        noexcept(UnorderedMultiMap()) && noexcept(UnorderedMultiMap().swap(other))) :
        UnorderedMultiMap() {
        operator=(std::move(other));
    }

    UnorderedMultiMap&
    operator=(UnorderedMultiMap&& other) noexcept(noexcept(UnorderedMultiMap().swap(other))) {
        this->swap(other);
        return *this;
    }

    ~UnorderedMultiMap() {
        this->clear();
    }

    void
    swap(UnorderedMultiMap& other) noexcept(noexcept(UnorderedMultiMap().swapSameType(other))) {
        if (&other != this) {
            swapSameType(other);
        }
    }

    using Base::swap;

  private:

    void swapSameType(UnorderedMultiMap& other) noexcept(
        (Detail::NothrowContract<typename Base::Node>::nothrowIfMovable)
        && (Detail::NothrowContract<typename Base::Node>::nothrowIfDestructible)) {
        static_assert(NodeAllocator::uniqueAllocator,
                      "Allocator should use uniqueAllocator == true");
        // While Base::swap() has no noexcept guarantee, being noexcept here depends only on the
        // noexcept properties of Node.
        Base::swap(other);
    }

    friend void swap(UnorderedMultiMap& lhs,
                     UnorderedMultiMap& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Static


namespace Pooled {

/// UnorderedMultiMap with per type node storage.
/// @tparam K key
/// @tparam E element
/// @tparam NN maximum node count per type
/// @tparam NB maximum bucket count per instance
/// @tparam H hash type
/// @tparam KE 'key equal' type
template<class K,
         class E,
         std::size_t NN,
         std::size_t NB,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedMultiMap : public ETL_NAMESPACE::UnorderedMultiMap<K, E, H, KE> {

    static_assert(NN > 0, "Invalid Etl::Pooled::UnorderedMultiMap size");
    static_assert(NB > 0, "Invalid Etl::Pooled::UnorderedMultiMap size");

  public:  // types

    using Base = ETL_NAMESPACE::UnorderedMultiMap<K, E, H, KE>;

    using NodeAllocator = typename ETL_NAMESPACE::PoolHelperForSize<NN>::template CommonAllocator<
        typename Base::Node>;
    using BucketImpl = Static::Vector<typename Base::BucketItem, NB>;

  private:  // variables

    BucketImpl buckets;
    mutable NodeAllocator allocator;

  public:  // functions

    UnorderedMultiMap() noexcept(noexcept(Base(buckets, allocator))) :
        Base(buckets, allocator),
        buckets(NB) {
        ETL_ASSERT(buckets.size() == NB);
        this->bindOwnBuckets();
        this->max_load_factor(static_cast<float>(NN) / static_cast<float>(NB));
    }

    UnorderedMultiMap(const UnorderedMultiMap& other) :
        UnorderedMultiMap() {
        Base::operator=(other);
    }

    explicit UnorderedMultiMap(const Base& other) :
        UnorderedMultiMap() {
        Base::operator=(other);
    }

    UnorderedMultiMap& operator=(const UnorderedMultiMap& other) {
        Base::operator=(other);
        return *this;
    }

    using Base::operator=;

    UnorderedMultiMap(std::initializer_list<typename Base::value_type> initList) :
        UnorderedMultiMap() {
        Base::operator=(initList);
    }

    UnorderedMultiMap(UnorderedMultiMap&& other) noexcept(
        noexcept(Base(buckets, allocator)) && noexcept(BucketImpl())
        && noexcept(UnorderedMultiMap().operator=(std::move(other)))) :
        Base(buckets, allocator),
        buckets() {
        this->bindOwnBuckets();
        this->max_load_factor(static_cast<float>(NN) / static_cast<float>(NB));
        operator=(std::move(other));
    }

    UnorderedMultiMap&
    operator=(UnorderedMultiMap&& other) noexcept(noexcept(UnorderedMultiMap().swap(other))) {
        this->swap(other);
        return *this;
    }

    ~UnorderedMultiMap() {
        this->clear();
    }

    void
    swap(UnorderedMultiMap& other) noexcept(noexcept(UnorderedMultiMap().swapSameType(other))) {
        if (&other != this) {
            swapSameType(other);
        }
    }

    using Base::swap;

  private:

    void swapSameType(UnorderedMultiMap& other) noexcept {
        static_assert(noexcept(Detail::AHashTable::swapWithSources(this->Base::hashTable,
                                                                   this->buckets,
                                                                   other.Base::hashTable,
                                                                   other.buckets)),
                      "noexcept contract violation");
        static_assert(!NodeAllocator::uniqueAllocator,
                      "Allocator should use uniqueAllocator == false");
        Detail::AHashTable::swapWithSources(this->Base::hashTable,
                                            this->buckets,
                                            other.Base::hashTable,
                                            other.buckets);
    }

    friend void swap(UnorderedMultiMap& lhs,
                     UnorderedMultiMap& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Pooled

}  // namespace ETL_NAMESPACE

#endif  // __ETL_UNORDEREDMULTIMAP_H__
