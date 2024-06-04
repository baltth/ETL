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

#ifndef ETL_LISTTEMPLATE_H_
#define ETL_LISTTEMPLATE_H_

#include <etl/base/AAllocator.h>
#include <etl/base/TypedListBase.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <algorithm>
#include <initializer_list>

namespace ETL_NAMESPACE {


template<class T>
class List : private Detail::TypedListBase<T> {

  public:  // types

    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = T*;
    using const_pointer = const T*;

    using Base = Detail::TypedListBase<T>;
    using Node = typename Base::Node;
    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = typename Base::reverse_iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;

    using AllocatorBase = AAllocator<Node>;

    using size_type = typename Base::size_type;

    friend class Base::Node;

  private:  // variables

    AllocatorBase& allocator;

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{

    explicit List(AllocatorBase& a) noexcept :
        allocator {a} {}

    List& operator=(const List& other) {
        assign(other.begin(), other.end());
        return *this;
    }

    List& operator=(List&& other) {
        swap(other);
        return *this;
    }

    List& operator=(std::initializer_list<T> initList) {
        assign(initList);
        return *this;
    }

    List(const List& other) = delete;
    List(List&& other) = delete;

    ~List() noexcept(AllocatorBase::noexceptDestroy) {
        clear();
    }

    void assign(uint32_t num, const_reference value) {
        this->clear();
        insert(this->begin(), num, value);
    }

    template<typename InputIt>
    void assign(InputIt first, InputIt last) {
        this->clear();
        insert(this->begin(), first, last);
    }

    void assign(std::initializer_list<T> initList) {
        assign(initList.begin(), initList.end());
    }
    /// \}

    /// \name Capacity
    /// \{
    using Base::size;
    using Base::empty;

    size_type max_size() const noexcept {
        return allocator.max_size();
    }
    /// \}

    /// \name Element access
    /// \{
    using Base::front;
    using Base::back;
    /// \}

    /// \name Iterators
    /// \{
    using Base::begin;
    using Base::cbegin;
    using Base::end;
    using Base::cend;
    using Base::rbegin;
    using Base::crbegin;
    using Base::rend;
    using Base::crend;
    /// \}

    /// \name Modifiers
    /// \{

    void clear() noexcept(AllocatorBase::noexceptDestroy);

    void push_front(const T& item);
    void push_back(const T& item);

    void pop_front() noexcept(AllocatorBase::noexceptDestroy) {
        deleteNode(static_cast<Node*>(Detail::AListBase::popFront()));
    }

    void pop_back() noexcept(AllocatorBase::noexceptDestroy) {
        deleteNode(static_cast<Node*>(Detail::AListBase::popBack()));
    }

    iterator insert(const_iterator pos, const T& item);

    iterator insert(const_iterator pos, T&& item) {
        return emplace(pos, std::move(item));
    }

    iterator insert(const_iterator pos, size_type n, const T& item) {
        iterator it = this->end();
        while (n > 0) {
            it = insert(pos, item);
            --n;
        }
        return it;
    }

    template<typename InputIt>
    enable_if_t<!is_integral<InputIt>::value, iterator>
    insert(const_iterator position, InputIt first, InputIt last);

    void insert(const_iterator position, std::initializer_list<T> initList) {
        insert(position, initList.begin(), initList.end());
    }

    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    template<typename... Args>
    iterator emplace_front(Args&&... args) {
        return emplace(this->begin(), std::forward<Args>(args)...);
    }

    template<typename... Args>
    iterator emplace_back(Args&&... args) {
        return emplace(this->end(), std::forward<Args>(args)...);
    }

    iterator erase(iterator pos) noexcept(AllocatorBase::noexceptDestroy) {
        iterator next = pos;
        ++next;
        deleteNode(static_cast<Node*>(Detail::AListBase::remove(pos)));
        return next;
    }

    void swap(List& other) {
        if (this != &other) {
            if (allocator.handle() == other.allocator.handle()) {
                swapNodeList(other);
            } else {
                swapElements(other);
            }
        }
    }
    /// \}

    /// \name List operations
    /// \{

    void splice(const_iterator pos, List& other) {
        splice(pos, other, other.begin(), other.end());
    }

    void splice(const_iterator pos, List& other, const_iterator it) {
        const_iterator it2 = it;
        ++it2;
        splice(pos, other, it, it2);
    }

    void splice(const_iterator pos,
                List& other,
                const_iterator first,
                const_iterator last);

    /// \}

  protected:

    void swapNodeList(List& other) noexcept(
        noexcept(std::declval<List>().Detail::AListBase::swapNodeList(other))) {
        Detail::AListBase::swapNodeList(other);
    }

  private:

    Node* createNode(const T& item) {
        Node* p = allocator.allocate(1);
        if (p != nullptr) {
            allocator.construct(p, item);
        }
        return p;
    }

    Node* createNode(T&& item) {
        Node* p = allocator.allocate(1);
        if (p != nullptr) {
            allocator.construct(p, std::move(item));
        }
        return p;
    }

    void deleteNode(Node* ptr) noexcept(AllocatorBase::noexceptDestroy) {
        if (ptr) {
            allocator.destroy(ptr);
            allocator.deallocate(ptr, 1);
        }
    }

    /// Perform non-trivial swap, i.e. when two lists use different allocator.
    void swapElements(List<T>& other);

    /// Perform non-trivial splice, i.e. when two lists use different allocator.
    void spliceElements(const_iterator pos,
                        List& other,
                        const_iterator first,
                        const_iterator last);

    /// Helper to perform non-trivial swap on two elements of different lists.
    /// This overload is used when `T` conforms the contract of a `swap` function.
    template<class U = T>
    enable_if_t<Detail::UseSwapInCont<U>::value, std::pair<iterator, iterator>>
    swapN(iterator pos, List& other, iterator otherPos, size_type n) {
        (void)other;
        using std::swap;

        for (uint32_t i = 0; i < n; ++i) {
            swap(*pos, *otherPos);
            ++pos;
            ++otherPos;
        }

        return std::make_pair(pos, otherPos);
    }

    /// Helper to perform non-trivial swap on two elements of different lists.
    /// This overload is used when `T` does not conform the contract of a `swap` function.
    /// The function expects move-constructible type.
    template<class U = T>
    enable_if_t<!Detail::UseSwapInCont<U>::value, std::pair<iterator, iterator>>
    swapN(iterator pos, List& other, iterator otherPos, size_type n) {

        auto doSwap = [this, &other](iterator pos, iterator otherPos, size_type n) {
            for (uint32_t i = 0; i < n; ++i) {
                ETL_ASSERT(pos != end());
                ETL_ASSERT(otherPos != other.end());
                auto nextOther = stealElement(pos, other, otherPos);
                pos = other.stealElement(nextOther, *this, pos);
                otherPos = nextOther;
            }
            return std::make_pair(pos, otherPos);
        };

        // As doSwap() needs one empty slot,
        // the algorithm is specialized when this is full.
        if (allocator.reserve() == 0U) {
            // The first element is moved to a temporary to
            // free its capacity ...
            value_type tmp = std::move(*pos);
            pos = erase(pos);
            // ... then N - 1 swaps performed ...
            auto res = doSwap(pos, otherPos, n - 1U);
            ETL_ASSERT(res.second != other.end());
            // ... then the last of other is 'stolen' to the end...
            res.second = stealElement(res.first, other, res.second);
            // ... and finally the temporary is inserted to the front of other
            other.emplace(other.begin(), std::move(tmp));

            return res;

        } else {
            return doSwap(pos, otherPos, n);
        }
    }

    iterator stealElement(const_iterator pos,
                          List& other,
                          iterator toSteal) {
        insert(pos, std::move(*toSteal));
        return other.erase(toSteal);
    }
};


template<class T>
void List<T>::clear() noexcept(AllocatorBase::noexceptDestroy) {

    while (Base::size() > 0) {
        pop_back();
    }
}


template<class T>
void List<T>::push_front(const T& item) {

    Node* p = createNode(item);
    if (p != nullptr) {
        Detail::AListBase::pushFront(*p);
    }
}


template<class T>
void List<T>::push_back(const T& item) {

    Node* p = createNode(item);
    if (p != nullptr) {
        Detail::AListBase::pushBack(*p);
    }
}


template<class T>
auto List<T>::insert(const_iterator pos, const T& item) -> iterator {
    return emplace(pos, item);
}


template<class T>
template<typename... Args>
auto List<T>::emplace(const_iterator pos, Args&&... args) -> iterator {

    iterator it = this->end();
    Node* inserted = allocator.allocate(1);
    if (inserted != nullptr) {
        new (inserted) Node(std::forward<Args>(args)...);
        it = Base::insert(pos, *inserted);
    }

    return it;
}


template<class T>
template<typename InputIt>
enable_if_t<!is_integral<InputIt>::value, typename List<T>::iterator>
List<T>::insert(const_iterator position, InputIt first, InputIt last) {

    iterator res = Base::convert(position);

    if (first != last) {
        res = this->insert(position, *first);
        ++first;
        while (first != last) {
            this->insert(position, *first);
            ++first;
        }
    }

    return res;
}


template<class T>
void List<T>::splice(const_iterator pos,
                     List<T>& other,
                     const_iterator first,
                     const_iterator last) {

    if (this != &other) {
        if (allocator.handle() == other.allocator.handle()) {
            Detail::AListBase::splice(pos, other, first, last);
        } else {
            spliceElements(pos, other, first, last);
        }
    }
}


template<class T>
void List<T>::spliceElements(const_iterator pos,
                             List<T>& other,
                             const_iterator first,
                             const_iterator last) {

    ETL_ASSERT(static_cast<Base*>(&other) != static_cast<Base*>(this));

    iterator item = Base::convert(first);
    while (item != last) {
        item = stealElement(pos, other, item);
    }
}


template<class T>
void List<T>::swapElements(List<T>& other) {

    const auto diff = sizeDiff(*this, other);

    iterator ownIt = this->begin();
    iterator otherIt = other.begin();

    if (diff.common > 0) {
        auto res = swapN(ownIt, other, otherIt, diff.common);
        ownIt = res.first;
        otherIt = res.second;
    }

    if (diff.lGreaterWith > 0) {
        ETL_ASSERT(otherIt == other.end());
        other.spliceElements(other.end(), *this, ownIt, this->end());
    } else if (diff.rGreaterWith > 0) {
        ETL_ASSERT(ownIt == end());
        this->spliceElements(this->end(), other, otherIt, other.end());
    }
}


template<class T>
bool operator==(const List<T>& lhs, const List<T>& rhs) {
    return Detail::isEqual(lhs, rhs);
}

template<class T>
bool operator!=(const List<T>& lhs, const List<T>& rhs) {
    return !(lhs == rhs);
}

template<class T>
bool operator<(const List<T>& lhs, const List<T>& rhs) {
    return Detail::isLess(lhs, rhs);
}

template<class T>
bool operator<=(const List<T>& lhs, const List<T>& rhs) {
    return !(rhs < lhs);
}

template<class T>
bool operator>(const List<T>& lhs, const List<T>& rhs) {
    return (rhs < lhs);
}

template<class T>
bool operator>=(const List<T>& lhs, const List<T>& rhs) {
    return !(lhs < rhs);
}


template<class T>
void swap(List<T>& lhs, List<T>& rhs) {
    lhs.swap(rhs);
}

}  // namespace ETL_NAMESPACE

#endif  // ETL_LISTTEMPLATE_H_
