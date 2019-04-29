/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Balazs Toth.

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

#include <etl/etlSupport.h>
#include <etl/traitSupport.h>
#include <etl/base/TypedVectorBase.h>
#include <etl/base/MemStrategies.h>

#undef min
#undef max

#include <utility>
#include <initializer_list>
#include <type_traits>

namespace ETL_NAMESPACE {


template<class T>
class Vector : protected Detail::TypedVectorBase<T> {

    static_assert(std::is_copy_constructible<T>::value, "Vector<>::value_type is not copy constructible");
    static_assert(std::is_copy_assignable<T>::value, "Vector<>::value_type is not copy assignable");

  public:   // types

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

  public:   // functions

    /// \name Construction, destruction, assignment
    /// \{

    Vector& operator=(const Vector& other) {
        if (&other != this) {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    Vector& operator=(Vector&& other) {
        this->replaceWith(std::move(other));
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
        static_assert(std::is_default_constructible<T>::value, "Vector<>::value_type is not default constructible");
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
        return insert(position, 1U, value);
    }

    iterator insert(const_iterator position, size_type num, const_reference value);

    template<typename InputIt>
    enable_if_t<!is_integral<InputIt>::value, iterator>
    insert(const_iterator position, InputIt first, InputIt last) {
        typename Base::template ContCreator<InputIt> cc(first, last);
        return insertWithCreator(position, cc.getLength(), cc);
    }

    void insert(const_iterator position, std::initializer_list<T> initList) {
        insert(position, initList.begin(), initList.end());
    }

    iterator insert(const_iterator position, T&& value);

    template<typename... Args >
    iterator emplace(const_iterator pos, Args&& ... args);

    template<typename... Args >
    void emplace_back(Args&& ... args) {
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

    template<class CR>
    iterator insertWithCreator(const_iterator position, size_type num, const CR& creatorCall);
    iterator insertWithCreator(const_iterator position, size_type num, CreateFunc&& creatorCall);

    void replaceWith(Vector&& other);

};


template<class T>
template<class CR>
auto Vector<T>::insertWithCreator(const_iterator position,
                                  size_type numToInsert,
                                  const CR& creatorCall) -> iterator {

    if (numToInsert > 0) {

        size_type requestedCapacity = Base::size() + numToInsert;

        if (requestedCapacity > this->capacity()) {
            size_type positionIx = position - this->begin();
            this->reserve(requestedCapacity);
            position = this->begin() + positionIx;
        }

        if (requestedCapacity <= this->capacity()) {
            position = Base::insertOperation(position, numToInsert, creatorCall);
        }
    }

    return iterator(position);
}


template<class T>
void Vector<T>::swap(Vector<T>& other) {

    if (&other != this) {
        if ((this->max_size() >= other.size()) && (other.max_size() >= this->size())) {

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

    return insertWithCreator(position, num, [this, &value](pointer item, size_type cnt, bool place) {
        for (size_type i = 0; i < cnt; ++i) {
            if (place) {
                this->placeValueTo(item + i, value);
            } else {
                this->assignValueTo(item + i, value);
            }
        }
    });
}


template<class T>
auto Vector<T>::insert(const_iterator position, T&& value) -> iterator {

    return insertWithCreator(position, 1, [this, &value](pointer item, size_type, bool place) {

        if (place) {
            this->placeValueTo(item, std::move(value));
        } else {
            this->assignValueTo(item, std::move(value));
        }
    });
}


template<class T>
template<typename... Args >
auto Vector<T>::emplace(const_iterator position, Args&& ... args) -> iterator {

    return insertWithCreator(position, 1, [&](pointer item, size_type, bool place) {
        if (place) {
            new (item) T(args...);
        } else {
            *item = T(args...);
        }
    });
}


template<class T>
auto Vector<T>::insertWithCreator(const_iterator position,
                                  size_type numToInsert,
                                  CreateFunc&& creatorCall) -> iterator {

    if (numToInsert > 0) {

        size_type requestedCapacity = Base::size() + numToInsert;

        if (requestedCapacity > this->capacity()) {

            size_type positionIx = position - this->begin();
            this->reserve(requestedCapacity);
            position = this->begin() + positionIx;
        }

        if (requestedCapacity <= this->capacity()) {
            position = Base::insertOperation(position, numToInsert, creatorCall);
        }
    }

    return iterator(position);
}


// Specialization for pointers

template class Vector<void*>;
template class Vector<const void*>;


template<class T>
class Vector<T*> : public Vector<typename CopyConst<T, void>::Type*> {

  public:   // types

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

  public:   // functions

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
        return *(static_cast<pointer>(Base::getItemPointer(ix)));
    }

    const_reference operator[](int32_t ix) const noexcept {
        return *(static_cast<const_pointer>(Base::getItemPointer(ix)));
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
        return reinterpret_cast<iterator>(Base::insert(reinterpret_cast<typename Base::const_iterator>(position),
                                                       value));
    }

    iterator insert(const_iterator position, size_type num, const_reference value) {
        return reinterpret_cast<iterator>(Base::insert(reinterpret_cast<typename Base::const_iterator>(position),
                                                       num,
                                                       value));
    }

    template<typename InputIt>
    iterator insert(const_iterator position, InputIt first, InputIt last) {
        return reinterpret_cast<iterator>(Base::insert(reinterpret_cast<typename Base::const_iterator>(position),
                                                       first,
                                                       last));
    }

    iterator erase(iterator pos) noexcept {
        return reinterpret_cast<iterator>(Base::erase(reinterpret_cast<typename Base::iterator>(pos)));
    }

    iterator erase(iterator first, iterator last) noexcept {
        return reinterpret_cast<iterator>(Base::erase(reinterpret_cast<typename Base::iterator>(first),
                                                      reinterpret_cast<typename Base::iterator>(last)));
    }
    /// \}

  protected:

    explicit Vector(AMemStrategy<StrategyBase>& s) noexcept :
        Base(s) {};

};

}

#endif /* __ETL_VECTORTEMPLATE_H__ */

