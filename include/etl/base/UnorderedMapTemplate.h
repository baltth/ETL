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

#ifndef __ETL_UNORDEREDMAPTEMPLATE_H__
#define __ETL_UNORDEREDMAPTEMPLATE_H__

#include <etl/base/AAllocator.h>
#include <etl/base/UnorderedBase.h>
#include <etl/base/VectorTemplate.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


template<class K,
         class E,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedMap : public Detail::UnorderedBase<std::pair<const K, E>> {

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
    UnorderedMap(typename Base::BucketImpl& b, typename Base::NodeAllocator& a) :
        Base(b, a) {};

    UnorderedMap& operator=(const UnorderedMap& other) {
        this->clear();  // @todo this could be optimized to reuse existing elements
        this->max_load_factor(other.max_load_factor());
        this->rehash(other.bucket_count());
        insert(other.begin(), other.end());
        return *this;
    }

    UnorderedMap& operator=(UnorderedMap&& other) {
        swap(other);
        return *this;
    }

    UnorderedMap& operator=(std::initializer_list<std::pair<K, E>> initList) {
        assign(initList);
        return *this;
    }

    UnorderedMap(const UnorderedMap& other) = delete;
    UnorderedMap(UnorderedMap&& other) = delete;

    ~UnorderedMap() = default;
    /// \}

    /// \name Capacity
    /// \{
    using Base::size;
    using Base::empty;
    /// \}

    /// \name Element access
    /// \{
    E& operator[](const K& k) {
        return getItem(k)->second;
    }

    E& operator[](K&& k) {
        return getItem(k)->second;
    }
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
        return bucketIx(hasher()(key));
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

    std::pair<iterator, bool> insert(const value_type& val) {
        return emplace(val);
    }

    std::pair<iterator, bool> insert(const K& k, const E& e) {
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
    std::pair<iterator, bool> emplace(Args&&... args);

    std::pair<iterator, bool> insert_or_assign(const K& k, const E& e);

    void swap(UnorderedMap& other) {
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

    iterator getItem(const K& k) {
        return emplace(k, E()).first;
    }

    friend bool operator==(const UnorderedMap& lhs, const UnorderedMap& rhs) {

        if (lhs.size() != rhs.size()) {
            return false;
        }
        if (lhs.empty()) {
            return true;
        }

        auto lIt = lhs.begin();
        while (lIt != lhs.end()) {
            auto rIt = rhs.find(lIt->first);
            if ((rIt != rhs.end()) && (lIt->second == rIt->second)) {
                ++lIt;
            } else {
                return false;
            }
        }

        return true;
    }

    friend bool operator!=(const UnorderedMap& lhs, const UnorderedMap& rhs) {
        return !(lhs == rhs);
    }
};


template<class K,
         class E,
         class H,
         class KE>
template<typename... Args>
auto UnorderedMap<K, E, H, KE>::emplace(Args&&... args) -> std::pair<iterator, bool> {

    // note: this construction is suboptimal, to be corrected.
    auto val = value_type(std::forward<Args>(args)...);

    auto found = find(val.first);
    if (found == end()) {
        auto it = Base::emplace(KeyHasher(), std::move(val));
        return std::make_pair(it, (it != end()));
    } else {
        return std::make_pair(found, false);
    }
}


template<class K,
         class E,
         class H,
         class KE>
auto UnorderedMap<K, E, H, KE>::insert_or_assign(const K& k, const E& e)
    -> std::pair<iterator, bool> {

    auto found = find(k);
    if (found == end()) {
        auto it = Base::emplace(KeyHasher(), value_type(k, e));
        return std::make_pair(it, (it != end()));
    } else {
        found->second = e;
        return std::make_pair(found, false);
    }
}

}  // namespace ETL_NAMESPACE

#endif /* __ETL_UNORDEREDMAPTEMPLATE_H__ */
