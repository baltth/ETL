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
    typedef T ItemType;
    typedef T* Iterator;
    typedef const T* ConstIterator;

    typedef typename Base::Creator Creator;

#if ETL_USE_CPP11
    typedef typename Base::CreateFunc CreateFunc;
#endif

  private: // variables

    AMemStrategy<StrategyBase>& strategy;

  public:   // functions

    inline Iterator insert(ConstIterator position, const T& value) {
        return insert(position, 1, value);
    }

    inline Iterator insert(ConstIterator position, uint32_t num, const T& value);
    inline Iterator insert(ConstIterator position, ConstIterator first, ConstIterator last);

    inline void pushFront(const T& value) {
        insert(Base::begin(), value);
    }

    inline void pushBack(const T& value) {
        insert(Base::end(), value);
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
        emplace(Base::end(), args...);
    }

    Iterator find(MatchFunc<T>&& matcher) const {
        return find(this->begin(), this->end(), std::move(matcher));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, MatchFunc<T>&& matcher) const;

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

    Iterator insertWithCreator(ConstIterator position, uint32_t num, const Creator& creatorCall);

#if ETL_USE_CPP11

    Vector& operator=(Vector&& other);
    Vector& operator=(const std::initializer_list<T>& initList);

    Iterator insertWithCreator(ConstIterator position, uint32_t num, CreateFunc&& creatorCall);
    void initWith(const T* src, uint32_t num);

#endif

};


template<class T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {

    this->reserve(other.getSize());
    this->copyOperation(other.begin(), other.getSize());
    return *this;
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insertWithCreator(ConstIterator position,
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


template<class T>
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, ConstIterator first, ConstIterator last) {

    typename Base::template ContCreator<ConstIterator> cc(first, last);
    return insertWithCreator(position, cc.getLength(), cc);
}


#if ETL_USE_CPP11

template<class T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) {

    this->swap(other);
    return *this;
}


template<class T>
Vector<T>& Vector<T>::operator=(const std::initializer_list<T>& initList) {

    this->copyOperation(initList.begin(), initList.size());
    return *this;
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, uint32_t num, const T& value) {

    return insertWithCreator(position, num, [&](T * item, bool place) {
        if (place) {
            this->placeValueTo(item, value);
        } else {
            this->assignValueTo(item, value);
        }
    });
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, T&& value) {

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

    T* dataAlias = this->getData();

    for (uint32_t i = 0; i < num; ++i) {
        this->placeValueTo((dataAlias + i), src[i]);
    }

    this->proxy.setSize(num);
}


template<class T>
typename Vector<T>::Iterator Vector<T>::insertWithCreator(ConstIterator position,
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

    return Iterator(position);
}


template<class T>
typename Vector<T>::Iterator Vector<T>::find(ConstIterator startPos,
                                             ConstIterator endPos,
                                             MatchFunc<T>&& matcher) const {

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
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position,
                                               uint32_t num,
                                               const T& value) {

    typename Base::CopyCreator cc(value);
    return insertWithCreator(position, num, cc);
}

#endif


// Specialization for pointers

template class Vector<void*>;


template<class T>
class Vector<T*> : public Vector<void*> {

  public:   // types

    typedef T* ItemType;
    typedef Vector<void*> Base;
    typedef Base::StrategyBase StrategyBase;
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
        return reinterpret_cast<Iterator>(Base::begin());
    }

    inline ConstIterator begin() const {
        return reinterpret_cast<ConstIterator>(Base::begin());
    }

    inline Iterator end() {
        return reinterpret_cast<Iterator>(Base::end());
    }

    inline ConstIterator end() const {
        return reinterpret_cast<ConstIterator>(Base::end());
    }

    inline ItemType& front() {
        return reinterpret_cast<ItemType&>(Base::front());
    }

    inline const ItemType& front() const {
        return reinterpret_cast<const ItemType&>(Base::front());
    }

    inline ItemType& back() {
        return reinterpret_cast<ItemType&>(Base::back());
    }

    inline const ItemType& back() const {
        return reinterpret_cast<const ItemType&>(Base::back());
    }

    inline ItemType* getData() {
        return static_cast<ItemType*>(Base::getData());
    }

    inline const ItemType* getData() const {
        return static_cast<ItemType*>(Base::getData());
    }

    inline Iterator insert(Iterator position, const ItemType& value) {
        return reinterpret_cast<Iterator>(Base::insert(reinterpret_cast<Base::Iterator>(position),
                                                       value));
    }

    inline Iterator insert(Iterator position, uint32_t num, const ItemType& value) {
        return reinterpret_cast<Iterator>(Base::insert(reinterpret_cast<Base::Iterator>(position),
                                                       num,
                                                       value));
    }

    inline Iterator insert(ConstIterator position, ConstIterator first, ConstIterator last) {
        return reinterpret_cast<Iterator>(Base::insert(reinterpret_cast<Base::Iterator>(position),
                                                       first,
                                                       last));
    }

    inline Iterator erase(Iterator pos) {
        return reinterpret_cast<Iterator>(Base::erase(reinterpret_cast<Base::Iterator>(pos)));
    }

    inline Iterator erase(Iterator first, Iterator last) {
        return reinterpret_cast<Iterator>(Base::erase(reinterpret_cast<Base::Iterator>(first),
                                                      reinterpret_cast<Base::Iterator>(last)));
    }

    inline void pushBack(const ItemType& value) {
        Base::insert(Base::end(), 1, value);
    }

    inline void popBack() {
        Base::erase(Base::end());
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

    Vector& operator=(const std::initializer_list<T>& initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

};

}

#endif /* __ETL_VECTORTEMPLATE_H__ */

