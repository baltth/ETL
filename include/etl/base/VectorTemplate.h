/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2021 Balazs Toth.

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

#ifndef __ETL_VECTORTEMPLATE_H__
#define __ETL_VECTORTEMPLATE_H__

#include <etl/base/MemStrategies.h>
#include <etl/base/TypedVectorBase.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#undef min
#undef max

#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>

#ifndef ETL_VEC_INSERT_WITH_SNR
#define ETL_VEC_INSERT_WITH_SNR 0
#endif

namespace ETL_NAMESPACE {


template<class T>
class Vector : protected Detail::TypedVectorBase<T> {

    static_assert(std::is_copy_constructible<T>::value,
                  "Vector<>::value_type is not copy constructible");
    static_assert(std::is_copy_assignable<T>::value, "Vector<>::value_type is not copy assignable");

  public:  // types

    typedef Detail::TypedVectorBase<T> Base;
    typedef Base StrategyBase;

    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef typename Base::size_type size_type;

    typedef typename Base::CreateFunc CreateFunc;

  private:  // variables

    AMemStrategy<StrategyBase>& strategy;

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{

    Vector& operator=(const Vector& other) {
        if (&other != this) {
            this->clear();
            this->reserve(other.size());
            assign(other.begin(), other.end());
        }
        return *this;
    }

    Vector& operator=(Vector&& other) {
        if (strategy.handle() == other.strategy.handle()) {
            swap(other);
        } else {
            this->replaceWith(std::move(other));
        }
        return *this;
    }

    Vector& operator=(std::initializer_list<T> initList) {
        assign(initList);
        return *this;
    }

    Vector(const Vector& other) = delete;
    Vector(Vector&& other) = delete;
    ~Vector() = default;

    void assign(size_type num, const_reference value) {
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
    using Base::capacity;

    void resize(size_type length) {
        static_assert(std::is_default_constructible<T>::value,
                      "Vector<>::value_type is not default constructible");
        strategy.resize(*this, length);
    }

    void resize(size_type length, const value_type& value) {
        strategy.resize(*this, length, value);
    }

    void reserve(size_type length) {
        strategy.reserve(*this, length);
    }

    size_t max_size() const noexcept {
        return strategy.getMaxCapacity();
    }

    void reserve_exactly(size_type length) {
        strategy.reserveExactly(*this, length);
    }

    void shrink_to_fit() noexcept {
        strategy.shrinkToFit(*this);
    }
    /// \}

    /// \name Element access
    /// \{
    using Base::operator[];
    using Base::front;
    using Base::back;
    using Base::data;

#if ETL_USE_EXCEPTIONS
    using Base::at;
#endif
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
    using Base::clear;
    using Base::pop_front;
    using Base::pop_back;
    using Base::erase;

    iterator insert(const_iterator position, const_reference value) {
        return emplace(position, value);
    }

    iterator insert(const_iterator position, size_type num, const_reference value);

    template<typename InputIt>
    enable_if_t<!is_integral<InputIt>::value, iterator>
    insert(const_iterator position, InputIt first, InputIt last) {
        return insertRange(
            position, first, last, typename std::iterator_traits<InputIt>::iterator_category {});
    }

    void insert(const_iterator position, std::initializer_list<T> initList) {
        insert(position, initList.begin(), initList.end());
    }

    iterator insert(const_iterator position, T&& value) {
        return emplace(position, std::move(value));
    }

    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    template<typename... Args>
    void emplace_back(Args&&... args) {
        emplace(this->end(), args...);
    }

    void push_front(const_reference value) {
        insert(this->begin(), value);
    }

    void push_back(const_reference value) {
        insert(this->end(), value);
    }

    void swap(Vector& other);
    /// \}

  protected:

    explicit Vector(AMemStrategy<StrategyBase>& s) noexcept :
        strategy(s) {};

    template<typename InputIt>
    iterator
    insertRange(const_iterator position, InputIt first, InputIt last, std::forward_iterator_tag) {
#if ETL_VEC_INSERT_WITH_SNR
        auto numToInsert = std::distance(first, last);
        if (numToInsert > 0) {
            auto res = prepareForInsert(position, numToInsert);
            if (res.first) {
                position = insertRangeWithSNR(res.second, first, last);
            }
        }

        return iterator(position);
#else
        typename Base::template ContCreator<InputIt> cc {first, last};
        return insertWithCreator(position, cc.getLength(), cc);
#endif
    }

    template<typename InputIt>
    iterator
    insertRange(const_iterator position, InputIt first, InputIt last, std::input_iterator_tag) {
        return insertRangeWithSNR(position, first, last);
    }

    template<class CR>
    iterator insertWithCreator(const_iterator position, size_type num, const CR& creatorCall);

    void replaceWith(Vector&& other);

  private:

    std::pair<bool, const_iterator> prepareForInsert(const_iterator position,
                                                     size_type numToInsert);

    template<typename InputIt>
    iterator insertRangeWithSNR(const_iterator position, InputIt first, InputIt last) noexcept(
        is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value);
};


template<class T>
auto Vector<T>::prepareForInsert(const_iterator position, size_type numToInsert)
    -> std::pair<bool, const_iterator> {

    bool success = true;
    if (numToInsert > 0) {

        size_type requestedCapacity = Base::size() + numToInsert;

        if (requestedCapacity > this->capacity()) {
            size_type positionIx = position - this->begin();
            this->reserve(requestedCapacity);
            position = this->begin() + positionIx;
        }

        if (requestedCapacity > this->capacity()) {
            success = false;
        }
    }

    return {success, position};
}


template<class T>
template<class CR>
auto Vector<T>::insertWithCreator(const_iterator position,
                                  size_type numToInsert,
                                  const CR& creatorCall) -> iterator {

    if (numToInsert > 0) {
        auto res = prepareForInsert(position, numToInsert);
        if (res.first) {
            position = Base::insertOperation(res.second, numToInsert, creatorCall);
        }
    }

    return iterator(position);
}


template<class T>
template<typename InputIt>
auto Vector<T>::insertRangeWithSNR(const_iterator position, InputIt first, InputIt last) noexcept(
    is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value) -> iterator {

    // This algorithm inserts elements to the end of the container,
    // then uses move operations to get the final element ordering.
    // This method is used to insert a range with unknown length,
    // its _sub-optimal_ when used in other scenarios.
    //
    //     original data
    // |aaaaaaaaaabbbbbbbb|
    //            |
    //  to insert |cccccccc|
    //
    // 1) insert to end
    // |aaaaaaaaaabbbbbbbb|cccccccc|
    //
    // 2) swap ranges
    // |aaaaaaaaaacccccccccbbbbbbbb|

    auto posIndex = std::distance(cbegin(), position);
    size_type origSize = size();
    ETL_ASSERT(origSize >= posIndex);

    size_t cnt = 0;
    while (first != last) {
        // Insert to back
        insert(end(), *first);
        ++cnt;
        ++first;
    }

    if ((cnt > 0U) && ((origSize + cnt) == size())) {
        // Reorder to position.
        Base::swapNeighbourRanges(posIndex, origSize);
    }

    return (cnt > 0) ? Base::getIterator(posIndex) : iterator(position);
}


template<class T>
void Vector<T>::swap(Vector<T>& other) {

    if (&other != this) {
        if (strategy.handle() == other.strategy.handle()) {
            Detail::AVectorBase::swapProxy(other);
        } else if ((this->max_size() >= other.size()) && (other.max_size() >= this->size())) {

            this->reserve_exactly(other.size());
            other.reserve_exactly(this->size());

            if ((this->capacity() >= other.size()) && (other.capacity() >= this->size())) {

                this->swapElements(other);
            }
        }
    }
}


template<class T>
void Vector<T>::replaceWith(Vector&& other) {

    if ((&other != this) && (this->max_size() >= other.size())) {

        this->reserve_exactly(other.size());
        if (this->capacity() >= other.size()) {

            this->moveOperation(this->data(), other.data(), other.size());
            other.clear();
        }
    }
}


template<class T>
auto Vector<T>::insert(const_iterator position, size_type num, const_reference value) -> iterator {

    return insertWithCreator(
        position, num, [this, &value](pointer item, size_type cnt, bool place) {
            if (place) {
                for (size_type i = 0; i < cnt; ++i) {
                    this->placeValueTo(item + i, value);
                }
            } else {
                for (size_type i = 0; i < cnt; ++i) {
                    this->assignValueTo(item + i, value);
                }
            }
        });
}  // namespace ETL_NAMESPACE


template<class T>
template<typename... Args>
auto Vector<T>::emplace(const_iterator position, Args&&... args) -> iterator {

    iterator result = const_cast<iterator>(position);
    auto res = prepareForInsert(position, 1U);
    if (res.first) {
        result = Base::insertOneOperation(res.second, [&args...](pointer item, bool place) {
            if (place) {
                new (item) T(args...);
            } else {
                *item = T(args...);
            }
        });
    }
    return result;
}


// Specialization for pointers

template class Vector<void*>;
template class Vector<const void*>;


template<class T>
class Vector<T*> : public Vector<typename CopyConst<T, void>::Type*> {

  public:  // types

    typedef T* value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef Vector<typename CopyConst<T, void>::Type*> Base;
    typedef typename Base::StrategyBase StrategyBase;

    typedef typename Base::size_type size_type;
    typedef typename Base::CreateFunc CreateFunc;

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{

    Vector& operator=(const Vector& other) {
        Base::operator=(other);
        return *this;
    }

    Vector& operator=(Vector&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    Vector& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

    Vector(const Vector& other) = delete;
    Vector(Vector&& other) = delete;
    ~Vector() = default;

    void assign(size_type num, const_reference value) {
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
    using Base::max_size;
    using Base::empty;
    using Base::capacity;
    using Base::resize;
    using Base::reserve;
    using Base::reserve_exactly;
    using Base::shrink_to_fit;
    /// \}

    /// \name Element access
    /// \{

    reference operator[](int32_t ix) noexcept {
        return *(reinterpret_cast<pointer>(Base::getItemPointer(ix)));
    }

    const_reference operator[](int32_t ix) const noexcept {
        return *(reinterpret_cast<const_pointer>(Base::getItemPointer(ix)));
    }

    reference front() {
        return reinterpret_cast<reference>(Base::front());
    }

    const_reference front() const {
        return reinterpret_cast<const_reference>(Base::front());
    }

    reference back() {
        return reinterpret_cast<reference>(Base::back());
    }

    const_reference back() const {
        return reinterpret_cast<const_reference>(Base::back());
    }

    pointer data() noexcept {
        return static_cast<pointer>(Base::data());
    }

    const_pointer data() const noexcept {
        return static_cast<pointer>(Base::data());
    }
    /// \}

    /// \name Iterators
    /// \{

    iterator begin() noexcept {
        return reinterpret_cast<iterator>(Base::begin());
    }

    const_iterator begin() const noexcept {
        return reinterpret_cast<const_iterator>(Base::begin());
    }

    const_iterator cbegin() const noexcept {
        return this->begin();
    }

    iterator end() noexcept {
        return reinterpret_cast<iterator>(Base::end());
    }

    const_iterator end() const noexcept {
        return reinterpret_cast<const_iterator>(Base::end());
    }

    const_iterator cend() const noexcept {
        return this->end();
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(this->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(this->cend());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(this->begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(this->begin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(this->cbegin());
    }
    /// \}

    /// \name Modifiers
    /// \{
    using Base::clear;

    iterator insert(const_iterator position, const_reference value) {
        return reinterpret_cast<iterator>(
            Base::insert(reinterpret_cast<typename Base::const_iterator>(position),
                         value));
    }

    iterator insert(const_iterator position, size_type num, const_reference value) {
        return reinterpret_cast<iterator>(
            Base::insert(reinterpret_cast<typename Base::const_iterator>(position),
                         num,
                         value));
    }

    template<typename InputIt>
    iterator insert(const_iterator position, InputIt first, InputIt last) {
        return reinterpret_cast<iterator>(
            Base::insert(reinterpret_cast<typename Base::const_iterator>(position),
                         first,
                         last));
    }

    iterator erase(iterator pos) noexcept {
        return reinterpret_cast<iterator>(
            Base::erase(reinterpret_cast<typename Base::iterator>(pos)));
    }

    iterator erase(iterator first, iterator last) noexcept {
        return reinterpret_cast<iterator>(
            Base::erase(reinterpret_cast<typename Base::iterator>(first),
                        reinterpret_cast<typename Base::iterator>(last)));
    }
    /// \}

  protected:

    explicit Vector(AMemStrategy<StrategyBase>& s) noexcept :
        Base(s) {};
};


template<class T>
bool operator==(const Vector<T>& lhs, const Vector<T>& rhs) {
    return Detail::isEqual(lhs, rhs);
}

template<class T>
bool operator!=(const Vector<T>& lhs, const Vector<T>& rhs) {
    return !(lhs == rhs);
}

template<class T>
bool operator<(const Vector<T>& lhs, const Vector<T>& rhs) {
    return Detail::isLess(lhs, rhs);
}

template<class T>
bool operator<=(const Vector<T>& lhs, const Vector<T>& rhs) {
    return !(rhs < lhs);
}

template<class T>
bool operator>(const Vector<T>& lhs, const Vector<T>& rhs) {
    return (rhs < lhs);
}

template<class T>
bool operator>=(const Vector<T>& lhs, const Vector<T>& rhs) {
    return !(lhs < rhs);
}


template<class T>
void swap(Vector<T>& lhs, Vector<T>& rhs) {
    lhs.swap(rhs);
}

}  // namespace ETL_NAMESPACE


#endif  // __ETL_VECTORTEMPLATE_H__
