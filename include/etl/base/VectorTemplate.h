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

    typedef T ValueType;
    typedef T& Reference;
    typedef const T& ConstReference;
    typedef T* Pointer;
    typedef const T* ConstPointer;

    typedef Pointer Iterator;
    typedef ConstPointer ConstIterator;

    typedef typename Base::Creator Creator;

#if ETL_USE_CPP11
    typedef typename Base::CreateFunc CreateFunc;
#endif

  private:  // variables

    AMemStrategy<StrategyBase>& strategy;

  public:   // functions

    Iterator insert(ConstIterator position, ConstReference value) {
        return insert(position, 1, value);
    }

    Iterator insert(ConstIterator position, uint32_t num, ConstReference value);
    Iterator insert(ConstIterator position, ConstIterator first, ConstIterator last);

    void pushFront(ConstReference value) {
        insert(Base::begin(), value);
    }

    void pushBack(ConstReference value) {
        insert(Base::end(), value);
    }

#if ETL_USE_CPP11

    Iterator insert(ConstIterator position, T&& value);

    template<typename... Args >
    Iterator emplace(ConstIterator pos, Args&& ... args);

    template<typename... Args >
    void emplaceBack(Args&& ... args) {
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

    Iterator insertWithCreator(ConstIterator position, uint32_t num, const Creator& creatorCall);

#if ETL_USE_CPP11

    Vector& operator=(Vector&& other);
    Vector& operator=(std::initializer_list<T> initList);

    Iterator insertWithCreator(ConstIterator position, uint32_t num, CreateFunc&& creatorCall);

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
typename Vector<T>::Iterator Vector<T>::insert(ConstIterator position, uint32_t num, ConstReference value) {

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
                                               ConstReference value) {

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

    typedef T* ValueType;
    typedef Vector<typename CopyConst<T, void>::Type*> Base;
    typedef typename Base::StrategyBase StrategyBase;
    typedef ValueType* Iterator;
    typedef const ValueType* ConstIterator;

    typedef typename Base::Creator Creator;

#if ETL_USE_CPP11
    typedef typename Base::CreateFunc CreateFunc;
#endif

  public:   // functions

    ValueType& operator[](int32_t ix) {
        return *(static_cast<ValueType*>(Base::getItemPointer(ix)));
    }

    const ValueType& operator[](int32_t ix) const {
        return *(static_cast<ValueType*>(Base::getItemPointer(ix)));
    }

    Iterator begin() {
        return reinterpret_cast<Iterator>(Base::begin());
    }

    ConstIterator begin() const {
        return reinterpret_cast<ConstIterator>(Base::begin());
    }

    ConstIterator cbegin() const {
        return this->begin();
    }

    Iterator end() {
        return reinterpret_cast<Iterator>(Base::end());
    }

    ConstIterator end() const {
        return reinterpret_cast<ConstIterator>(Base::end());
    }

    ConstIterator cend() const {
        return this->end();
    }

    ValueType& front() {
        return reinterpret_cast<ValueType&>(Base::front());
    }

    const ValueType& front() const {
        return reinterpret_cast<const ValueType&>(Base::front());
    }

    ValueType& back() {
        return reinterpret_cast<ValueType&>(Base::back());
    }

    const ValueType& back() const {
        return reinterpret_cast<const ValueType&>(Base::back());
    }

    ValueType* getData() {
        return static_cast<ValueType*>(Base::getData());
    }

    const ValueType* getData() const {
        return static_cast<ValueType*>(Base::getData());
    }

    Iterator insert(ConstIterator position, const ValueType& value) {
        return reinterpret_cast<Iterator>(Base::insert(reinterpret_cast<typename Base::ConstIterator>(position),
                                                       value));
    }

    Iterator insert(ConstIterator position, uint32_t num, const ValueType& value) {
        return reinterpret_cast<Iterator>(Base::insert(reinterpret_cast<typename Base::ConstIterator>(position),
                                                       num,
                                                       value));
    }

    Iterator insert(ConstIterator position, ConstIterator first, ConstIterator last) {
        return reinterpret_cast<Iterator>(Base::insert(reinterpret_cast<typename Base::ConstIterator>(position),
                                                       first,
                                                       last));
    }

    Iterator erase(Iterator pos) {
        return reinterpret_cast<Iterator>(Base::erase(reinterpret_cast<typename Base::Iterator>(pos)));
    }

    Iterator erase(Iterator first, Iterator last) {
        return reinterpret_cast<Iterator>(Base::erase(reinterpret_cast<typename Base::Iterator>(first),
                                                      reinterpret_cast<typename Base::Iterator>(last)));
    }

    void pushBack(const ValueType& value) {
        Base::insert(Base::end(), 1, value);
    }

    void popBack() {
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

    Vector& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

};

}

#endif /* __ETL_VECTORTEMPLATE_H__ */

