/**
\file
\date 2016.01.20. 19:44:12
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

#include "etlSupport.h"

#include "Base/TypedVectorBase.h"

#undef min
#undef max

#include <new>
#include <utility>

namespace ETL_NAMESPACE {


template<class T>
class VectorTemplate : public TypedVectorBase<T> {

  public:   // types

    typedef TypedVectorBase<T> Base;
    typedef T ItemType;
    typedef T* Iterator;
    typedef const T* ConstIterator;

    typedef typename Base::CreatorFunctor CreatorFunctor;

  private: // variables

    AMemStrategy& strategy;

  public:   // functions

    inline Iterator insert(ConstIterator position, const T& value) {
        return insert(position, 1, value);
    }

    Iterator insert(ConstIterator position, uint32_t num, const T& value);

    inline void pushFront(const T& value) {
        insert(Base::begin(), value);
    }

    inline void pushBack(const T& value) {
        insert(Base::end(), value);
    }

    inline void swap(VectorTemplate<T>& other) {
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
        strategy.reserve(length);
    }

    void reserveAtLeast(uint32_t length) {
        strategy.reserveAtLeast(length);
    }

    void shrinkToFit() {
        strategy.shrinkToFit();
    }

    void resize(uint32_t newLength) {
        strategy.resize(newLength);
    }

  protected:

    VectorTemplate<T>(AMemStrategy& s) :
        strategy(s) {};

    VectorTemplate<T>(const VectorTemplate<T>& other);
    VectorTemplate<T>& operator=(const VectorTemplate<T>& other);

#if ETL_USE_CPP11

    VectorTemplate<T>(VectorTemplate<T>&& other);
    VectorTemplate<T>(const std::initializer_list<T>& initList);

    VectorTemplate<T>& operator=(VectorTemplate<T>&& other);
    VectorTemplate<T>& operator=(const std::initializer_list<T>& initList);

    Iterator insertWithCreator(ConstIterator position, uint32_t num, std::function<void(T*, bool)>&& creatorCall);
    void initWith(const T* src, uint32_t num);

#else

    Iterator insertWithCreator(ConstIterator position, uint32_t num, const CreatorFunctor& creatorCall);

#endif

};


template<class T>
VectorTemplate<T>::VectorTemplate(const VectorTemplate<T>& other) {

    operator=(other);
}


template<class T>
VectorTemplate<T>& VectorTemplate<T>::operator=(const VectorTemplate<T>& other) {

    this->reserve(other.getSize());
    this->copyOperation(other.begin(), other.getSize());
    return *this;
}


#if ETL_USE_CPP11

template<class T>
VectorTemplate<T>::VectorTemplate(VectorTemplate<T>&& other) {

    this->swap(other);
}


template<class T>
VectorTemplate<T>::VectorTemplate(const std::initializer_list<T>& initList) {

    initWith(initList.begin(), initList.size());
}


template<class T>
VectorTemplate<T>& VectorTemplate<T>::operator=(VectorTemplate<T>&& other) {

    this->swap(other);
    return *this;
}


template<class T>
VectorTemplate<T>& VectorTemplate<T>::operator=(const std::initializer_list<T>& initList) {

    this->copyOperation(initList.begin(), initList.size());
    return *this;
}

template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insert(ConstIterator position, uint32_t num, const T& value) {

    return insertWithCreator(position, num, [&value](T * item, bool place) {
        if (place) {
            placeValueTo(ptr, value);
        } else {
            assignValueTo(ptr, value);
        }
    });
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insert(ConstIterator position, T&& value) {

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
typename VectorTemplate<T>::Iterator VectorTemplate<T>::emplace(ConstIterator position, Args&& ... args) {

    return insertWithCreator(position, 1, [&](T * item, bool place) {
        if (place) {
            new (item) T(args...);
        } else {
            *item = T(args...);
        }
    });
}


template<class T>
void VectorTemplate<T>::initWith(const T* src, uint32_t num) {

    reserve(num);

    T* dataAlias = getData();

    for (uint32_t i = 0; i < num; ++i) {
        placeValueTo((dataAlias + i), src[i]);
    }

    proxy.setSize(num);
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insertWithCreator(ConstIterator position,
                                                                          uint32_t numToInsert,
                                                                          std::function<void(T*, bool)>&& creatorCall) {

    if (numToInsert > 0) {

        if ((Base::getSize() + numToInsert) > Base::getCapacity()) {

            uint32_t positionIx = position - Base::begin();
            this->reserveAtLeast(this->getSize() + numToInsert);
            position = Base::begin() + positionIx;
        }

        position = Base::insertOperation(position, numToInsert, creatorCall);
    }

    return position;
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::find(ConstIterator startPos,
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
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insert(ConstIterator position, uint32_t num,
                                                               const T& value) {

    typename Base::CopyCreator cc(value);
    return insertWithCreator(position, num, cc);
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insertWithCreator(ConstIterator position,
                                                                          uint32_t numToInsert,
                                                                          const CreatorFunctor& creatorCall) {

    if (numToInsert > 0) {

        if ((Base::getSize() + numToInsert) > Base::getCapacity()) {

            uint32_t positionIx = position - Base::begin();
            this->reserveAtLeast(this->getSize() + numToInsert);
            position = Base::begin() + positionIx;
        }

        position = Base::insertOperation(position, numToInsert, creatorCall);
    }

    return Iterator(position);
}

template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::find(ConstIterator startPos,
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

}

#endif /* __ETL_VECTORTEMPLATE_H__ */

