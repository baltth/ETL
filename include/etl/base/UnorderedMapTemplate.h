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

#ifndef __ETL_UNORDEREDMAPTEMPLATE_H__
#define __ETL_UNORDEREDMAPTEMPLATE_H__

#include <etl/etlSupport.h>
#include <etl/base/UnorderedBase.h>
#include <etl/base/VectorTemplate.h>
#include <etl/base/AAllocator.h>

namespace ETL_NAMESPACE {


template<class K,
         class E,
         class H = std::hash<K>,
         class KE = std::equal_to<K>>
class UnorderedMap : public Detail::UnorderedBase<std::pair<const K, E>, H> {

  public:   // types

    using key_type = K;
    using mapped_type = E;
    using value_type = std::pair<const K, E>;

    using hasher = H;
    using key_equal = KE;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using Base = Detail::UnorderedBase<value_type, hasher>;
    using Node = typename Base::Node;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;

    using size_type = typename Base::size_type;

  public:   // functions

    /// \name Construction, destruction, assignment
    /// \{
    UnorderedMap(typename Base::BucketImpl& b, typename Base::NodeAllocator& a) :
        Base(b, a) {};

    UnorderedMap& operator=(const UnorderedMap& other);
    UnorderedMap& operator=(UnorderedMap&& other);

    UnorderedMap(const UnorderedMap& other) = delete;
    UnorderedMap(UnorderedMap&& other) = delete;

    ~UnorderedMap() = default;
    /// \}

    /// \name Capacity
    /// \{
    using Base::size;
    using Base::empty;
    /// \}

    /// \name Iterators
    /// \{
    using Base::begin;
    using Base::cbegin;
    using Base::end;
    using Base::cend;
    /// \}

    /// \name Modifiers
    /// \{
    using Base::clear;
    using Base::erase;

    std::pair<iterator, bool> insert(const value_type& val);

    //template<typename... Args>
    //std::pair<iterator, bool> insert(Args&& ... args) {
    //  return insert(value_type(args...));
    //}
    /// \}

  protected:

  private:

};


template<class K,
         class E,
         class H,
         class KE>
auto UnorderedMap<K, E, H, KE>::insert(const value_type& val) -> std::pair<iterator, bool> {

    auto hash = hasher()(val.first);
    auto range = this->equalHashRange(hash);
auto firstIt = this->makeConstIt(range.first);
auto secIt = this->makeConstIt(range.second);

    bool found = false;
    while((!found) && (firstIt != secIt)) {
      if(key_equal()(val.first, firstIt->first)) {
        found = true;
      } else {
        ++firstIt;
      }
    }

    if(!found) {
      auto it = Base::insert(val);
      return std::make_pair(it, (it != end()));
    } else {
      return std::make_pair(end(), false);
    }
}

}

#endif /* __ETL_UNORDEREDMAPTEMPLATE_H__ */
