/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2024 Balazs Toth.

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

#ifndef ETL_SORTEDLIST_H_
#define ETL_SORTEDLIST_H_

#include <etl/base/ListTemplate.h>
#include <etl/etlSupport.h>

#include <functional>

namespace ETL_NAMESPACE {
namespace Detail {


template<class T, class Comp>
class SortedList {

  public:  // types

    class Cont : public List<T> {
      public:
        using List<T>::List;
        using List<T>::operator=;
        using List<T>::swapNodeList;
    };

    using value_type = typename Cont::value_type;
    using reference = typename Cont::reference;
    using const_reference = typename Cont::const_reference;
    using pointer = typename Cont::pointer;
    using const_pointer = typename Cont::const_pointer;

    using iterator = typename Cont::iterator;
    using const_iterator = typename Cont::const_iterator;
    using reverse_iterator = typename Cont::reverse_iterator;
    using const_reverse_iterator = typename Cont::const_reverse_iterator;

    using size_type = typename Cont::size_type;

    using Node = typename Cont::Node;

  protected:  // variables

    Cont list;

  public:  // functions

    SortedList(typename Cont::AllocatorBase& a) noexcept :
        list(a) {};

    ///\name List<> forward
    /// \{
    size_type size() const noexcept {
        return list.size();
    }

    size_type max_size() const noexcept {
        return list.max_size();
    }

    bool empty() const noexcept {
        return list.empty();
    }

    iterator begin() noexcept {
        return list.begin();
    }

    const_iterator begin() const noexcept {
        return list.begin();
    }

    const_iterator cbegin() const noexcept {
        return list.cbegin();
    }

    iterator end() noexcept {
        return list.end();
    }

    const_iterator end() const noexcept {
        return list.end();
    }

    const_iterator cend() const noexcept {
        return list.cend();
    }

    reverse_iterator rbegin() noexcept {
        return list.rbegin();
    }

    const_reverse_iterator rbegin() const noexcept {
        return list.rbegin();
    }

    const_reverse_iterator crbegin() const noexcept {
        return list.crbegin();
    }

    reverse_iterator rend() noexcept {
        return list.rend();
    }

    const_reverse_iterator rend() const noexcept {
        return list.rend();
    }

    const_reverse_iterator crend() const noexcept {
        return list.crend();
    }

    void clear() noexcept(Cont::AllocatorBase::noexceptDestroy) {
        list.clear();
    }

    iterator erase(iterator pos) noexcept(Cont::AllocatorBase::noexceptDestroy) {
        return list.erase(pos);
    }

    void swap(SortedList& other) {
        list.swap(other.list);
    }
    /// \}

    void swapNodeList(SortedList& other) noexcept(
        noexcept(std::declval<Cont>().swapNodeList(other.list))) {
        list.swapNodeList(other.list);
    }

  protected:

    template<typename It, typename CV, class CF>
    std::pair<It, It> findSortedRangeBase(It it, It endIt, const CV& val, const CF& compare) const;

    template<typename CV>
    std::pair<iterator, iterator> findSortedRange(const CV& val) {
        return findSortedRangeBase(this->begin(), this->end(), val, Comp());
    }

    template<typename CV>
    std::pair<const_iterator, const_iterator> findSortedRange(const CV& val) const {
        return findSortedRangeBase(this->cbegin(), this->cend(), val, Comp());
    }

    template<typename CV, class CF>
    std::pair<iterator, iterator> findSortedRange(const CV& val, const CF& compare) {
        return findSortedRangeBase(this->begin(), this->end(), val, compare);
    }

    template<typename CV, class CF>
    std::pair<const_iterator, const_iterator> findSortedRange(const CV& val,
                                                              const CF& compare) const {
        return findSortedRangeBase(this->cbegin(), this->cend(), val, compare);
    }

    template<typename CV>
    std::pair<iterator, bool> findSortedPosition(const CV& val) {
        std::pair<iterator, iterator> res = findSortedRange(val);
        return std::pair<iterator, bool>(res.second, (res.first != res.second));
    }

    template<typename CV>
    std::pair<const_iterator, bool> findSortedPosition(const CV& val) const {
        std::pair<const_iterator, const_iterator> res = findSortedRange(val);
        return std::pair<const_iterator, bool>(res.second, (res.first != res.second));
    }

    iterator insert(const_reference item);
    std::pair<iterator, bool> insertUnique(const_reference item);

    iterator insertTo(const_iterator pos, const_reference item) {
        return list.insert(pos, item);
    }

    template<typename... Args>
    iterator emplaceTo(const_iterator pos, Args&&... args) {
        return list.emplace(pos, std::forward<Args>(args)...);
    }
};


template<class T, class Comp>
auto SortedList<T, Comp>::insert(const_reference item) -> iterator {

    auto found = findSortedPosition(item);
    return list.insert(found.first, item);
}


template<class T, class Comp>
auto SortedList<T, Comp>::insertUnique(const_reference item) -> std::pair<iterator, bool> {

    auto found = findSortedPosition(item);

    if (found.second == false) {

        found.first = list.insert(found.first, item);
        if (found.first != end()) {
            found.second = true;
        }

    } else {
        --found.first;
        found.second = false;
    }

    return found;
}


template<class T, class Comp>
template<typename It, typename CV, class CF>
auto SortedList<T, Comp>::findSortedRangeBase(It it,
                                              It endIt,
                                              const CV& val,
                                              const CF& compare) const -> std::pair<It, It> {

    // std::pair<It, It> res(endIt, endIt);
    auto res = std::make_pair(endIt, endIt);

    if (it != endIt) {

        bool notLessFound = false;

        while ((!notLessFound) && (it != endIt)) {

            if (compare(*it, val)) {
                ++it;
            } else {
                res.first = it;
                notLessFound = true;
            }
        }

        while (notLessFound && (it != endIt)) {

            if (compare(val, *it)) {
                res.second = it;
                notLessFound = false;
            } else {
                ++it;
            }
        }
    }

    return res;
}

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_SORTEDLIST_H_
