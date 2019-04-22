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

#include <new>
#include <utility>
#include <initializer_list>

namespace ETL_NAMESPACE {


template<class T>
class Vector : public Detail::TypedVectorBase<T> {

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

    typedef typename Base::CreateFunc CreateFunc;

  private:  // variables

    AMemStrategy<StrategyBase>& strategy;

  public:   // functions

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

    /// \name Capacity
    /// \{
    void resize(uint32_t length) {
        strategy.resize(*this, length);
    }

    void reserve(uint32_t length) {
        strategy.reserve(*this, length);
    }

    size_t max_size() const noexcept {
        return strategy.getMaxCapacity();
    }

    void reserve_exactly(uint32_t length) {
        strategy.reserveExactly(*this, length);
    }

    void shrink_to_fit() noexcept {
        strategy.shrinkToFit(*this);
    }
    /// \}

    /// \name Modifiers
    /// \{
    iterator insert(const_iterator position, const_reference value) {
        return insert(position, 1U, value);
    }

    iterator insert(const_iterator position, uint32_t num, const_reference value);

    template<typename InputIt>
    typename enable_if<!is_integral<InputIt>::value, iterator>::type        // *NOPAD*
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

    Vector& operator=(const Vector& other) {
        if (&other != this) {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    Vector& operator=(Vector&& other);

    Vector& operator=(std::initializer_list<T> initList) {
        assign(initList);
        return *this;
    }

    template<class CR>
    iterator insertWithCreator(const_iterator position, uint32_t num, const CR& creatorCall);
    iterator insertWithCreator(const_iterator position, uint32_t num, CreateFunc&& creatorCall);

};


template<class T>
template<class CR>
typename Vector<T>::iterator Vector<T>::insertWithCreator(const_iterator position,
                                                          uint32_t numToInsert,
                                                          const CR& creatorCall) {

    if (numToInsert > 0) {

        uint32_t requestedCapacity = Base::size() + numToInsert;

        if (requestedCapacity > this->capacity()) {
            uint32_t positionIx = position - this->begin();
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
Vector<T>& Vector<T>::operator=(Vector<T>&& other) {

    this->swap(other);
    return *this;
}


template<class T>
typename Vector<T>::iterator Vector<T>::insert(const_iterator position, uint32_t num, const_reference value) {

    return insertWithCreator(position, num, [this, &value](pointer item, uint32_t cnt, bool place) {
        for (uint32_t i = 0; i < cnt; ++i) {
            if (place) {
                this->placeValueTo(item + i, value);
            } else {
                this->assignValueTo(item + i, value);
            }
        }
    });
}


template<class T>
typename Vector<T>::iterator Vector<T>::insert(const_iterator position, T&& value) {

    return insertWithCreator(position, 1, [this, &value](pointer item, uint32_t, bool place) {

        if (place) {
            this->placeValueTo(item, std::move(value));
        } else {
            this->assignValueTo(item, std::move(value));
        }
    });
}


template<class T>
template<typename... Args >
typename Vector<T>::iterator Vector<T>::emplace(const_iterator position, Args&& ... args) {

    return insertWithCreator(position, 1, [&](pointer item, uint32_t, bool place) {
        if (place) {
            new (item) T(args...);
        } else {
            *item = T(args...);
        }
    });
}


template<class T>
typename Vector<T>::iterator Vector<T>::insertWithCreator(const_iterator position,
                                                          uint32_t numToInsert,
                                                          CreateFunc&& creatorCall) {

    if (numToInsert > 0) {

        uint32_t requestedCapacity = Base::size() + numToInsert;

        if (requestedCapacity > this->capacity()) {

            uint32_t positionIx = position - this->begin();
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
    typedef Vector<typename CopyConst<T, void>::Type*> Base;
    typedef typename Base::StrategyBase StrategyBase;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef typename Base::CreateFunc CreateFunc;

  public:   // functions

    value_type& operator[](int32_t ix) noexcept {
        return *(static_cast<value_type*>(Base::getItemPointer(ix)));
    }

    const value_type& operator[](int32_t ix) const noexcept {
        return *(static_cast<const value_type*>(Base::getItemPointer(ix)));
    }

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

    value_type& front() {
        return reinterpret_cast<value_type&>(Base::front());
    }

    const value_type& front() const {
        return reinterpret_cast<const value_type&>(Base::front());
    }

    value_type& back() {
        return reinterpret_cast<value_type&>(Base::back());
    }

    const value_type& back() const {
        return reinterpret_cast<const value_type&>(Base::back());
    }

    value_type* data() noexcept {
        return static_cast<value_type*>(Base::data());
    }

    const value_type* data() const noexcept {
        return static_cast<value_type*>(Base::data());
    }

    iterator insert(const_iterator position, const value_type& value) {
        return reinterpret_cast<iterator>(Base::insert(reinterpret_cast<typename Base::const_iterator>(position),
                                                       value));
    }

    iterator insert(const_iterator position, uint32_t num, const value_type& value) {
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

  protected:

    explicit Vector(AMemStrategy<StrategyBase>& s) noexcept :
        Base(s) {};

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

};

}

#endif /* __ETL_VECTORTEMPLATE_H__ */

