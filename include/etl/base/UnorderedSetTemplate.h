/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2022-2024 Balazs Toth.

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

#ifndef ETL_UNORDEREDSETTEMPLATE_H_
#define ETL_UNORDEREDSETTEMPLATE_H_

#include <etl/base/AAllocator.h>
#include <etl/base/UnorderedBase.h>
#include <etl/base/VectorTemplate.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


template<class K,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedSet : public Detail::UnorderedBase<K> {

  public:  // types

    using key_type = K;
    using value_type = K;

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

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{
    UnorderedSet(typename Base::BucketImpl& b,
                 typename Base::NodeAllocator& a) noexcept(noexcept(Base(b, a))) :
        Base(b, a) {}

    UnorderedSet& operator=(const UnorderedSet& other) {
        this->clear();  // @todo this could be optimized to reuse existing elements
        this->max_load_factor(other.max_load_factor());
        this->rehash(other.bucket_count());
        insert(other.begin(), other.end());
        return *this;
    }

    UnorderedSet& operator=(UnorderedSet&& other) {
        swap(other);
        return *this;
    }

    UnorderedSet& operator=(std::initializer_list<K> initList) {
        assign(initList);
        return *this;
    }

    UnorderedSet(const UnorderedSet& other) = delete;
    UnorderedSet(UnorderedSet&& other) = delete;

    ~UnorderedSet() = default;
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
        return this->findExact(hasher()(key),
                               [&key](const value_type& item) { return key_equal()(key, item); });
    }

    const_iterator find(const key_type& key) const {
        return this->findExact(hasher()(key),
                               [&key](const value_type& item) { return key_equal()(key, item); });
    }

    std::pair<iterator, iterator> equal_range(const K& key) {
        return this->findRange(hasher()(key),
                               [&key](const value_type& item) { return key_equal()(key, item); });
    }

    std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
        return this->findRange(hasher()(key),
                               [&key](const value_type& item) { return key_equal()(key, item); });
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

    template<typename InputIt>
    enable_if_t<!is_integral<InputIt>::value, void> insert(InputIt first, InputIt last) {
        while (first != last) {
            emplace(*first);
            ++first;
        }
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args);

    void swap(UnorderedSet& other) {
        Base::swap(hasher(), other);
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

    friend bool operator==(const UnorderedSet& lhs, const UnorderedSet& rhs) {

        if (lhs.size() != rhs.size()) {
            return false;
        }
        if (lhs.empty()) {
            return true;
        }

        auto lIt = lhs.begin();
        while (lIt != lhs.end()) {
            auto rIt = rhs.find(*lIt);
            if (rIt != rhs.end()) {
                ++lIt;
            } else {
                return false;
            }
        }

        return true;
    }

    friend bool operator!=(const UnorderedSet& lhs, const UnorderedSet& rhs) {
        return !(lhs == rhs);
    }

    friend void swap(UnorderedSet& lhs, UnorderedSet& rhs) {
        lhs.swap(rhs);
    }
};


template<class K,
         class H,
         class KE>
template<typename... Args>
auto UnorderedSet<K, H, KE>::emplace(Args&&... args) -> std::pair<iterator, bool> {

    // note: this construction is suboptimal, to be corrected.
    auto val = value_type(std::forward<Args>(args)...);

    auto found = find(val);
    if (found == end()) {
        auto it = Base::emplace(hasher(), std::move(val));
        return std::make_pair(it, (it != end()));
    } else {
        return std::make_pair(found, false);
    }
}

}  // namespace ETL_NAMESPACE

#endif  // ETL_UNORDEREDSETTEMPLATE_H_
