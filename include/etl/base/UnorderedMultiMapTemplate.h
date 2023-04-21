/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2022-2023 Balazs Toth.

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

#ifndef __ETL_UNORDEREDMULTIMAPTEMPLATE_H__
#define __ETL_UNORDEREDMULTIMAPTEMPLATE_H__

#include <etl/base/AAllocator.h>
#include <etl/base/UnorderedBase.h>
#include <etl/base/VectorTemplate.h>
#include <etl/etlSupport.h>

#include <algorithm>

namespace ETL_NAMESPACE {


template<class K,
         class E,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedMultiMap : public Detail::UnorderedBase<std::pair<const K, E>> {

  public:  // types

    using key_type = K;
    using mapped_type = E;
    using value_type = std::pair<const K, E>;

    using hasher = H;
    using key_equal = KE;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using Base = Detail::UnorderedBase<value_type>;
    using Node = typename Base::Node;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;

    using size_type = typename Base::size_type;

  private:

    struct KeyHasher {
        typename Base::HashType operator()(const_reference val) {
            return hasher()(val.first);
        }
    };

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{
    UnorderedMultiMap(typename Base::BucketImpl& b,
                      typename Base::NodeAllocator& a) noexcept(noexcept(Base(b, a))) :
        Base(b, a) {}

    UnorderedMultiMap& operator=(const UnorderedMultiMap& other) {
        this->clear();  // @todo this could be optimized to reuse existing elements
        this->max_load_factor(other.max_load_factor());
        this->rehash(other.bucket_count());
        ETL_ASSERT(this->empty());
        insert(other.begin(), other.end());
        return *this;
    }

    UnorderedMultiMap& operator=(UnorderedMultiMap&& other) {
        swap(other);
        return *this;
    }

    UnorderedMultiMap& operator=(std::initializer_list<value_type> initList) {
        assign(initList);
        return *this;
    }

    UnorderedMultiMap(const UnorderedMultiMap& other) = delete;
    UnorderedMultiMap(UnorderedMultiMap&& other) = delete;

    ~UnorderedMultiMap() = default;
    /// \}

    /// \name Capacity
    /// \{
    using Base::size;
    using Base::empty;
    using Base::max_size;
    /// \}

    /// \name Iterators
    /// \{
    using Base::begin;
    using Base::cbegin;
    using Base::end;
    using Base::cend;
    /// \}

    /// \name Bucket interface
    /// \{
    size_type bucket(const key_type& key) const {
        return this->bucketIx(hasher()(key));
    }
    /// \}

    /// \name Lookup
    /// \{
    using Base::count;

    iterator find(const key_type& key) {
        return this->findExact(
            hasher()(key),
            [&key](const value_type& item) { return key_equal()(key, item.first); });
    }

    const_iterator find(const key_type& key) const {
        return this->findExact(
            hasher()(key),
            [&key](const value_type& item) { return key_equal()(key, item.first); });
    }

    std::pair<iterator, iterator> equal_range(const K& key) {
        return this->findRange(
            hasher()(key),
            [&key](const value_type& item) { return key_equal()(key, item.first); });
    }

    std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
        return this->findRange(
            hasher()(key),
            [&key](const value_type& item) { return key_equal()(key, item.first); });
    }

    /// \}

    /// \name Modifiers
    /// \{
    using Base::clear;
    using Base::erase;

    size_type erase(const key_type& k) {
        auto found = find(k);
        if (found != end()) {
            erase(found);
            return 1U;
        } else {
            return 0U;
        }
    }

    iterator insert(const value_type& val) {
        return emplace(val);
    }

    iterator insert(const K& k, const E& e) {
        return emplace(k, e);
    }

    template<typename InputIt>
    enable_if_t<!is_integral<InputIt>::value, void> insert(InputIt first, InputIt last) {
        while (first != last) {
            emplace(*first);
            ++first;
        }
    }

    template<typename... Args>
    iterator emplace(Args&&... args) {
        return Base::emplace(KeyHasher(), std::forward<Args>(args)...);
    }

    void swap(UnorderedMultiMap& other) {
        Base::swap(KeyHasher(), other);
    }
    /// \}

    /// \name Observers
    /// \{

    hasher hash_function() const {
        return hasher();
    }

    key_equal key_eq() const {
        return key_equal();
    }
    /// \}

  protected:

    template<typename InputIt>
    void assign(InputIt first, InputIt last) {
        this->clear();
        insert(first, last);
    }

    template<class Cont>
    void assign(const Cont& other) {
        assign(other.begin(), other.end());
    }

  private:

    friend bool operator==(const UnorderedMultiMap& lhs, const UnorderedMultiMap& rhs) {

        if (lhs.size() != rhs.size()) {
            return false;
        }
        if (lhs.empty()) {
            return true;
        }

        bool notEqual = false;
        auto lIt = lhs.begin();
        while (lIt != lhs.end() && (!notEqual)) {
            auto lEr = lhs.equal_range(lIt->first);
            auto rEr = rhs.equal_range(lIt->first);
            if (std::distance(lEr.first, lEr.second) != std::distance(rEr.first, rEr.second)) {
                notEqual = true;
            } else {
                notEqual = !std::is_permutation(lEr.first, lEr.second, rEr.first);
            }
            lIt = lEr.second;
        }

        return !notEqual;
    }

    friend bool operator!=(const UnorderedMultiMap& lhs, const UnorderedMultiMap& rhs) {
        return !(lhs == rhs);
    }

    friend void swap(UnorderedMultiMap& lhs, UnorderedMultiMap& rhs) {
        lhs.swap(rhs);
    }
};


}  // namespace ETL_NAMESPACE

#endif /* __ETL_UNORDEREDMULTIMAPTEMPLATE_H__ */
