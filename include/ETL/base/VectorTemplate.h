/**
\file
\date 2016.01.20.
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

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

#include "ETL/etlSupport.h"

#include "ETL/base/TypedVectorBase.h"
#include "ETL/base/MemStrategies.h"

#undef min
#undef max

#include <new>
#include <utility>

namespace ETL_NAMESPACE {


template<class T>
class Vector : public TypedVectorBase<T> {

  public:   // types

    typedef TypedVectorBase<T> base;
    typedef base StrategyBase;
    typedef T ItemType;
    typedef T* Iterator;
    typedef const T* ConstIterator;

    typedef typename base::CreatorFunctor CreatorFunctor;

  private: // variables

    AMemStrategy<StrategyBase>& strategy;

  public:   // functions

    inline Iterator insert(ConstIterator position, const T& value) {
        return insert(position, 1, value);
    }

    inline Iterator insert(ConstIterator position, uint32_t num, const T& value);
    inline Iterator insert(ConstIterator position, ConstIterator first, ConstIterator last);

    inline void pushFront(const T& value) {
        insert(base::begin(), value);
    }

    inline void pushBack(const T& value) {
        insert(base::end(), value);
    }

    inline void swap(Vector<T>& other) {
        AVectorBase::swap(other);
        ///\todo check if swappable
    }

#if ETL_USE_CPP11

    Iterator insert(ConstIterator position, T&& value);

    template<typename... Args >
    Iterator emplace(ConstIterator pos, Args&& ... args);

    template<typename... Args >
    inline void emplaceBack(Args&& ... args) {
        emplace(end(), args...);
    }

    Iterator find(std::function<bool(const T&)>&& matcher) const {
        return find(this->begin(), this->end(), std::move(matcher));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, std::function<bool(const T&)>&& matcher) const;

#endif

    Iterator find(const Matcher<T>& matcher) const {
        return find(this->begin(), this->end(), matcher);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<T>& matcher) const;

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

  protected:

    Vector(AMemStrategy<StrategyBase>& s) :
        strategy(s) {};

    Vector& operator=(const Vector& other);

#if ETL_USE_CPP11

    Vector& operator=(Vector&& other);
    Vector& operator=(const std::initializer_list<T>& initList);

    Iterator insertWithCreator(ConstIterator position, uint32_t num, std::function<void(T*, bool)>&& creatorCall);
    void initWith(const T* src, uint32_t num);

#else

    Iterator insertWithCreator(ConstIterator position, uint32_t num, const CreatorFunctor& creatorCall);

#endif

};


template<class T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {

    this->reserve(other.getSize());
    this->copyOperation(other.begin(), other.getSize());
    return *this;
}


#if ETL_USE_CPP11

template<class T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) {

    this->swap(other);
}


template<class T>
Vector<T>& Vector<T>::operator=(const std::initializer_list<T>& initList) {

    this->copyOperation(initList.begin(), initList.size());
    return *this;
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, uint32_t num, const T& value) {

    return insertWithCreator(position, num, [&value](T * item, bool place) {
        if (place) {
            placeValueTo(ptr, value);
        } else {
            assignValueTo(ptr, value);
        }
    });
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, T&& value) {

    return insertWithCreator(position, 1, [&value](T * item, bool place) {
        if (place) {
            placeValueTo(ptr, std::move(value));
        } else {
            assignValueTo(ptr, std::move(value));
        }
    });
}


template<class T>
template<typename... Args >
typename Vector<T>::Iterator Vector<T>::emplace(ConstIterator position, Args&& ... args) {

    return insertWithCreator(position, 1, [&](T * item, bool place) {
        if (place) {
            new (item) T(args...);
        } else {
            *item = T(args...);
        }
    });
}


template<class T>
void Vector<T>::initWith(const T* src, uint32_t num) {

    reserve(num);

    T* dataAlias = getData();

    for (uint32_t i = 0; i < num; ++i) {
        placeValueTo((dataAlias + i), src[i]);
    }

    proxy.setSize(num);
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insertWithCreator(ConstIterator position,
                                                          uint32_t numToInsert,
                                                          std::function<void(T*, bool)>&& creatorCall) {

    if (numToInsert > 0) {

        uint32_t requestedCapacity = base::getSize() + numToInsert;

        if (requestedCapacity > base::getCapacity()) {

            uint32_t positionIx = position - base::begin();
            this->reserveAtLeast(requestedCapacity);
            position = base::begin() + positionIx;
        }

        if (requestedCapacity <= base::getCapacity()) {
            position = base::insertOperation(position, numToInsert, creatorCall);
        }
    }

    return position;
}


template<class T>
typename Vector<T>::Iterator Vector<T>::find(ConstIterator startPos,
                                             ConstIterator endPos,
                                             std::function<bool(const T&)>&& matcher) const {

    bool match = false;

    while (!match && (startPos < endPos)) {

        match = matcher(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return Iterator(startPos);
}


#else


template<class T>
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, uint32_t num,
                                               const T& value) {

    typename base::CopyCreator cc(value);
    return insertWithCreator(position, num, cc);
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insertWithCreator(ConstIterator position,
                                                          uint32_t numToInsert,
                                                          const CreatorFunctor& creatorCall) {

    if (numToInsert > 0) {

        uint32_t requestedCapacity = base::getSize() + numToInsert;

        if (requestedCapacity > base::getCapacity()) {
            uint32_t positionIx = position - base::begin();
            this->reserveAtLeast(requestedCapacity);
            position = base::begin() + positionIx;
        }

        if (requestedCapacity <= base::getCapacity()) {
            position = base::insertOperation(position, numToInsert, creatorCall);
        }
    }

    return Iterator(position);
}

template<class T>
typename Vector<T>::Iterator Vector<T>::find(ConstIterator startPos,
                                             ConstIterator endPos,
                                             const Matcher<T>& matcher) const {

    bool match = false;

    while (!match && (startPos < endPos)) {

        match = matcher.call(*startPos);

        if (!match) {
            ++startPos;
        }
    }

    return Iterator(startPos);
}

#endif


template<class T>
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, ConstIterator first, ConstIterator last) {

    typename base::template ContCreator<ConstIterator> cc(first, last);
    return insertWithCreator(position, cc.getLength(), cc);
}


template class Vector<void*>;


template<class T>
class Vector<T*> : public Vector<void*> {

  public:   // types

    typedef T* ItemType;
    typedef Vector<void*> base;
    typedef base::StrategyBase StrategyBase;
    typedef ItemType* Iterator;
    typedef const ItemType* ConstIterator;

  public:   // functions

    inline ItemType& operator[](int32_t ix) {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline const ItemType& operator[](int32_t ix) const {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline Iterator begin() {
        return reinterpret_cast<Iterator>(base::begin());
    }

    inline ConstIterator begin() const {
        return reinterpret_cast<ConstIterator>(base::begin());
    }

    inline Iterator end() {
        return reinterpret_cast<Iterator>(base::end());
    }

    inline ConstIterator end() const {
        return reinterpret_cast<ConstIterator>(base::end());
    }

    inline ItemType& front() {
        return reinterpret_cast<ItemType&>(base::front());
    }

    inline const ItemType& front() const {
        return reinterpret_cast<const ItemType&>(base::front());
    }

    inline ItemType& back() {
        return reinterpret_cast<ItemType&>(base::back());
    }

    inline const ItemType& back() const {
        return reinterpret_cast<const ItemType&>(base::back());
    }

    inline ItemType* getData() {
        return static_cast<ItemType*>(base::getData());
    }

    inline const ItemType* getData() const {
        return static_cast<ItemType*>(base::getData());
    }

    inline Iterator insert(Iterator position, const ItemType& value) {
        return reinterpret_cast<Iterator>(base::insert(reinterpret_cast<base::Iterator>(position),
                                                       value));
    }

    inline Iterator insert(Iterator position, uint32_t num, const ItemType& value) {
        return reinterpret_cast<Iterator>(base::insert(reinterpret_cast<base::Iterator>(position),
                                                       num,
                                                       value));
    }

    inline Iterator insert(ConstIterator position, ConstIterator first, ConstIterator last) {
        return reinterpret_cast<Iterator>(base::insert(reinterpret_cast<base::Iterator>(position),
                                                       first,
                                                       last));
    }

    inline Iterator erase(Iterator pos) {
        return reinterpret_cast<Iterator>(base::erase(reinterpret_cast<base::Iterator>(pos)));
    }

    inline Iterator erase(Iterator first, Iterator last) {
        return reinterpret_cast<Iterator>(base::erase(reinterpret_cast<base::Iterator>(first),
                                                      reinterpret_cast<base::Iterator>(last)));
    }

    inline void pushBack(const ItemType& value) {
        base::insert(base::end(), 1, value);
    }

    inline void popBack() {
        base::erase(base::end());
    }

  protected:

    explicit Vector(AMemStrategy<StrategyBase>& s) :
        base(s) {};

    Vector& operator=(const Vector& other) {
        base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector& operator=(Vector&& other) {
        base::operator=(std::move(other));
        return *this;
    }

    Vector& operator=(const std::initializer_list<T>& initList) {
        base::operator=(initList);
        return *this;
    }

#endif

};

}

#endif /* __ETL_VECTORTEMPLATE_H__ */

