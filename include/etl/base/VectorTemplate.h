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

namespace ETL_NAMESPACE {


template<class T>
class Vector : public TypedVectorBase<T> {

  public:   // types

    typedef TypedVectorBase<T> Base;
    typedef Base StrategyBase;

    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

#if ETL_USE_CPP11
    typedef typename Base::CreateFunc CreateFunc;
#endif

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

#if ETL_USE_CPP11

    void assign(std::initializer_list<T> initList) {
        assign(initList.begin(), initList.end());
    }

#endif

    /// \name Capacity
    /// \{
    void resize(uint32_t length) {
        strategy.resize(*this, length);
    }

    void reserve(uint32_t length) {
        strategy.reserve(*this, length);
    }

    size_t max_size() const {
        return strategy.getMaxCapacity();
    }

    void reserve_exactly(uint32_t length) {
        strategy.reserveExactly(*this, length);
    }

    void shrink_to_fit() {
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

#if ETL_USE_CPP11

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

#endif

    void push_front(const_reference value) {
        insert(this->begin(), value);
    }

    void push_back(const_reference value) {
        insert(this->end(), value);
    }

    bool swap(Vector& other);
    /// \}

    /// \name Lookup
    /// \{
#if ETL_USE_CPP11

    iterator find(MatchFunc<T>&& matcher) const {
        return find(this->begin(), this->end(), std::move(matcher));
    }

    iterator find(const_iterator startPos, const_iterator endPos, MatchFunc<T>&& matcher) const;

#endif

    iterator find(const Matcher<T>& matcher) const {
        return find(this->begin(), this->end(), matcher);
    }

    iterator find(const_iterator startPos, const_iterator endPos, const Matcher<T>& matcher) const;
    /// \}

  protected:

    explicit Vector(AMemStrategy<StrategyBase>& s) :
        strategy(s) {};

    Vector& operator=(const Vector& other) {
        if (&other != this) {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    template<class CR>
    iterator insertWithCreator(const_iterator position, uint32_t num, const CR& creatorCall);

#if ETL_USE_CPP11

    Vector& operator=(Vector&& other);

    Vector& operator=(std::initializer_list<T> initList) {
        assign(initList);
        return *this;
    }

    iterator insertWithCreator(const_iterator position, uint32_t num, CreateFunc&& creatorCall);

#endif

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
typename Vector<T>::iterator Vector<T>::find(const_iterator startPos,
                                             const_iterator endPos,
                                             const Matcher<T>& matcher) const {

    bool match = false;

    while (!match && (startPos < endPos)) {

        match = matcher.call(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return iterator(startPos);
}


template<class T>
bool Vector<T>::swap(Vector<T>& other) {

    bool swapped = false;

    if (&other != this) {
        if ((this->max_size() >= other.size()) && (other.max_size() >= this->size())) {

            this->reserve_exactly(other.size());
            other.reserve_exactly(this->size());

            if ((this->capacity() >= other.size()) && (other.capacity() >= this->size())) {

                this->swapElements(other);
                swapped = true;
            }
        }
    }

    return swapped;
}


#if ETL_USE_CPP11

template<class T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) {

    this->swap(other);
    return *this;
}


template<class T>
typename Vector<T>::iterator Vector<T>::insert(const_iterator position, uint32_t num, const_reference value) {

    return insertWithCreator(position, num, [&](T * item, bool place) {
        if (place) {
            this->placeValueTo(item, value);
        } else {
            this->assignValueTo(item, value);
        }
    });
}


template<class T>
typename Vector<T>::iterator Vector<T>::insert(const_iterator position, T&& value) {

    return insertWithCreator(position, 1, [&](T * item, bool place) {
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

    return insertWithCreator(position, 1, [&](T * item, bool place) {
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


template<class T>
typename Vector<T>::iterator Vector<T>::find(const_iterator startPos,
                                             const_iterator endPos,
                                             MatchFunc<T>&& matcher) const {

    bool match = false;

    while (!match && (startPos < endPos)) {

        match = matcher(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return iterator(startPos);
}


#else


template<class T>
typename Vector<T>::iterator Vector<T>::insert(const_iterator position,
                                               uint32_t num,
                                               const_reference value) {

    typename Base::CopyCreator cc(value);
    return insertWithCreator(position, num, cc);
}

#endif


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

#if ETL_USE_CPP11
    typedef typename Base::CreateFunc CreateFunc;
#endif

  public:   // functions

    value_type& operator[](int32_t ix) {
        return *(static_cast<value_type*>(Base::getItemPointer(ix)));
    }

    const value_type& operator[](int32_t ix) const {
        return *(static_cast<const value_type*>(Base::getItemPointer(ix)));
    }

    iterator begin() {
        return reinterpret_cast<iterator>(Base::begin());
    }

    const_iterator begin() const {
        return reinterpret_cast<const_iterator>(Base::begin());
    }

    const_iterator cbegin() const {
        return this->begin();
    }

    iterator end() {
        return reinterpret_cast<iterator>(Base::end());
    }

    const_iterator end() const {
        return reinterpret_cast<const_iterator>(Base::end());
    }

    const_iterator cend() const {
        return this->end();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(this->end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(this->cend());
    }

    reverse_iterator rend() {
        return reverse_iterator(this->begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(this->begin());
    }

    const_reverse_iterator crend() const {
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

    value_type* data() {
        return static_cast<value_type*>(Base::data());
    }

    const value_type* data() const {
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

    iterator erase(iterator pos) {
        return reinterpret_cast<iterator>(Base::erase(reinterpret_cast<typename Base::iterator>(pos)));
    }

    iterator erase(iterator first, iterator last) {
        return reinterpret_cast<iterator>(Base::erase(reinterpret_cast<typename Base::iterator>(first),
                                                      reinterpret_cast<typename Base::iterator>(last)));
    }

  protected:

    explicit Vector(AMemStrategy<StrategyBase>& s) :
        Base(s) {};

    Vector& operator=(const Vector& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector& operator=(Vector&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    Vector& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

};

}

#endif /* __ETL_VECTORTEMPLATE_H__ */

