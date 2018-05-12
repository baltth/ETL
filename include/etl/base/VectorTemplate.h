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

    typedef typename Base::Creator Creator;

#if ETL_USE_CPP11
    typedef typename Base::CreateFunc CreateFunc;
#endif

  private:  // variables

    AMemStrategy<StrategyBase>& strategy;

  public:   // functions

    iterator insert(const_iterator position, const_reference value) {
        return insert(position, 1, value);
    }

    iterator insert(const_iterator position, uint32_t num, const_reference value);
    iterator insert(const_iterator position, const_iterator first, const_iterator last);

    void push_front(const_reference value) {
        insert(Base::begin(), value);
    }

    void push_back(const_reference value) {
        insert(Base::end(), value);
    }

    void pushFront(const_reference value) {
        push_front(value);
    }

    void pushBack(const_reference value) {
        push_back(value);
    }

#if ETL_USE_CPP11

    iterator insert(const_iterator position, T&& value);

    template<typename... Args >
    iterator emplace(const_iterator pos, Args&& ... args);

    template<typename... Args >
    void emplaceBack(Args&& ... args) {
        emplace(Base::end(), args...);
    }

    iterator find(MatchFunc<T>&& matcher) const {
        return find(this->begin(), this->end(), std::move(matcher));
    }

    iterator find(const_iterator startPos, const_iterator endPos, MatchFunc<T>&& matcher) const;

#endif

    iterator find(const Matcher<T>& matcher) const {
        return find(this->begin(), this->end(), matcher);
    }

    iterator find(const_iterator startPos, const_iterator endPos, const Matcher<T>& matcher) const;

    uint32_t getMaxCapacity() const {
        return strategy.getMaxCapacity();
    }

    void reserve(uint32_t length) {
        strategy.reserve(*this, length);
    }

    void reserveAtLeast(uint32_t length) {
        strategy.reserveAtLeast(*this, length);
    }

    void shrinkToFit() {
        strategy.shrinkToFit(*this);
    }

    void resize(uint32_t length) {
        strategy.resize(*this, length);
    }

    bool swap(Vector& other);

  protected:

    Vector(AMemStrategy<StrategyBase>& s) :
        strategy(s) {};

    Vector& operator=(const Vector& other);

    iterator insertWithCreator(const_iterator position, uint32_t num, const Creator& creatorCall);

#if ETL_USE_CPP11

    Vector& operator=(Vector&& other);
    Vector& operator=(std::initializer_list<T> initList);

    iterator insertWithCreator(const_iterator position, uint32_t num, CreateFunc&& creatorCall);

#endif

};


template<class T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {

    this->reserve(other.getSize());
    if (this->getCapacity() >= other.getSize()) {
        this->copyOperation(this->begin(), other.begin(), other.getSize());
    }
    return *this;
}


template<class T>
typename Vector<T>::iterator Vector<T>::insertWithCreator(const_iterator position,
                                                          uint32_t numToInsert,
                                                          const Creator& creatorCall) {

    if (numToInsert > 0) {

        uint32_t requestedCapacity = Base::getSize() + numToInsert;

        if (requestedCapacity > Base::getCapacity()) {
            uint32_t positionIx = position - Base::begin();
            this->reserveAtLeast(requestedCapacity);
            position = Base::begin() + positionIx;
        }

        if (requestedCapacity <= Base::getCapacity()) {
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
typename Vector<T>::iterator Vector<T>::insert(const_iterator position, const_iterator first, const_iterator last) {

    typename Base::template ContCreator<const_iterator> cc(first, last);
    return insertWithCreator(position, cc.getLength(), cc);
}


template<class T>
bool Vector<T>::swap(Vector<T>& other) {

    bool swapped = false;

    if (&other != this) {
        if ((this->getMaxCapacity() >= other.getSize()) && (other.getMaxCapacity() >= this->getSize())) {

            this->reserve(other.getSize());
            other.reserve(this->getSize());

            if ((this->getCapacity() >= other.getSize()) && (other.getCapacity() >= this->getSize())) {

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
Vector<T>& Vector<T>::operator=(std::initializer_list<T> initList) {

    this->reserve(initList.size());
    if (this->getCapacity() >= initList.size()) {
        this->copyOperation(this->begin(), initList.begin(), initList.size());
    }
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

        uint32_t requestedCapacity = Base::getSize() + numToInsert;

        if (requestedCapacity > Base::getCapacity()) {

            uint32_t positionIx = position - Base::begin();
            this->reserveAtLeast(requestedCapacity);
            position = Base::begin() + positionIx;
        }

        if (requestedCapacity <= Base::getCapacity()) {
            position = Base::insertOperation(position, numToInsert, std::move(creatorCall));
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

    typedef typename Base::Creator Creator;

#if ETL_USE_CPP11
    typedef typename Base::CreateFunc CreateFunc;
#endif

  public:   // functions

    value_type& operator[](int32_t ix) {
        return *(static_cast<value_type*>(Base::getItemPointer(ix)));
    }

    const value_type& operator[](int32_t ix) const {
        return *(static_cast<value_type*>(Base::getItemPointer(ix)));
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

    value_type* getData() {
        return static_cast<value_type*>(Base::getData());
    }

    const value_type* getData() const {
        return static_cast<value_type*>(Base::getData());
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

    iterator insert(const_iterator position, const_iterator first, const_iterator last) {
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

