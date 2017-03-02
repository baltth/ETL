/**
\file
\date 2017.02.20. 11:44:12
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Tóth Balázs.

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

#ifndef __ETL_FIXEDVECTOR_H__
#define __ETL_FIXEDVECTOR_H__

#include "etlSupport.h"

#include "Base/VectorTemplate.h"
#include "Base/MemStrategies.h"


namespace ETL_NAMESPACE {


template<class T, uint32_t N>
class FixedVector : public VectorTemplate<T, StaticSized> {

  public:   // types

    typedef T ItemType;
    typedef VectorTemplate<T, StaticSized> Base;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

  private:  // variables

    uint8_t data[N * sizeof(T)];

  public:   // functions

    FixedVector<T, N>();
    explicit FixedVector<T, N>(uint32_t len);
    FixedVector<T, N>(uint32_t len, const T& item);

    FixedVector<T, N>(const FixedVector<T, N>& other) :
        Base(other) {};

    FixedVector<T, N>& operator=(const FixedVector<T, N>& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    FixedVector<T>(FixedVector<T>&& other) :
        Base(std::move(other)) {};

    FixedVector<T>(const std::initializer_list<T>& initList) :
        Bsae(initList) {};

    FixedVector<T>& operator=(FixedVector<T>&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    FixedVector<T>& operator=(const std::initializer_list<T>& initList) {
        Base::operator=(initList);
        return *this;
    };

    Iterator find(std::function<bool(const T&)>&& matcher) const {
        return find(FixedVector<T>::begin(), FixedVector<T>::end(), std::move(matcher));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, std::function<bool(const T&)>&& matcher) const;

#else

    Iterator find(const Matcher<T>& matcher) const {
        return find(FixedVector<T, N>::begin(), FixedVector<T, N>::end(), matcher);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<T>& matcher) const;

#endif

};


template<class T, uint32_t N>
FixedVector<T, N>::FixedVector() :
    Base(data, N) {

    this->reserve(N);
}


template<class T, uint32_t N>
FixedVector<T, N>::FixedVector(uint32_t len) :
    Base(data, N) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, uint32_t N>
FixedVector<T, N>::FixedVector(uint32_t len, const T& item) :
    Base(data, N) {

    this->insert(this->begin(), len, item);
}


#if ETL_USE_CPP11

template<class T>
typename FixedVector<T>::Iterator FixedVector<T>::find(ConstIterator startPos,
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

template<class T, uint32_t N>
typename FixedVector<T, N>::Iterator FixedVector<T, N>::find(ConstIterator startPos,
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

/*
typedef FixedVector<void*> FixedPtrVectorBase;

template<class T, uint32_t N>
class FixedVector<T*, N> : public FixedPtrVectorBase {

// types
public:

    typedef T** Iterator;
    typedef T* ItemType;

// functions
public:

    FixedVector<ItemType>() :
        FixedPtrVectorBase() {};

    explicit FixedVector<ItemType>(uint32_t len) :
        FixedPtrVectorBase(len, NULLPTR) {};

    FixedVector<ItemType>(uint32_t len, const ItemType &item) :
        FixedPtrVectorBase(len, item) {};

    FixedVector<ItemType>(const FixedVector<ItemType> &other) :
        FixedPtrVectorBase(other) {};

    FixedVector<ItemType> &operator=(const FixedVector<ItemType> &other) {
        return FixedPtrVectorBase::operator=(other);
    };

#if ETL_USE_CPP11

    FixedVector<ItemType>(FixedVector<ItemType> &&other) :
        FixedPtrVectorBase(std::move(other)) {};

    FixedVector<ItemType>(const std::initializer_list<void*> &initList) :
        FixedPtrVectorBase(initList) {};

    FixedVector<ItemType> &operator=(FixedVector<ItemType> &&other) {
        return FixedPtrVectorBase::operator=(std::move(other));
    };

    FixedVector<ItemType> &operator=(const std::initializer_list<ItemType> &initList) {
        return FixedPtrVectorBase::operator=(initList);
    };

#endif

    inline ItemType &operator[](int32_t ix) {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline const ItemType &operator[](int32_t ix) const {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline Iterator begin() const {
        return reinterpret_cast<Iterator>(FixedPtrVectorBase::begin());
    }

    inline Iterator end() const {
        return reinterpret_cast<Iterator>(FixedPtrVectorBase::end());
    }

    inline ItemType &front() {
        return reinterpret_cast<ItemType &>(FixedPtrVectorBase::front());
    }

    inline const ItemType &front() const {
        return reinterpret_cast<const ItemType &>(FixedPtrVectorBase::front());
    }

    inline ItemType &back() {
        return reinterpret_cast<ItemType &>(FixedPtrVectorBase::back());
    }

    inline const ItemType &back() const {
        return reinterpret_cast<const ItemType &>(FixedPtrVectorBase::back());
    }

    inline ItemType* getData() {
        return static_cast<ItemType*>(FixedPtrVectorBase::getData());
    }

    inline const ItemType* getData() const {
        return static_cast<ItemType*>(FixedPtrVectorBase::getData());
    }

    inline Iterator insert(Iterator position, const ItemType &value) {
        return reinterpret_cast<Iterator>(FixedPtrVectorBase::insert(reinterpret_cast<FixedPtrVectorBase::Iterator>(position),
                                                                1,
                                                                value));
    }

    inline Iterator insert(Iterator position, uint32_t num, const ItemType &value) {
        return reinterpret_cast<Iterator>(FixedPtrVectorBase::insert(reinterpret_cast<FixedPtrVectorBase::Iterator>(position),
                                                                num,
                                                                value));
    }

    inline Iterator erase(Iterator pos) {
        Iterator next = pos;
        ++next;
        return reinterpret_cast<Iterator>(FixedPtrVectorBase::erase(reinterpret_cast<FixedPtrVectorBase::Iterator>(pos),
                                                               reinterpret_cast<FixedPtrVectorBase::Iterator>(next)));
    }

    inline Iterator erase(Iterator first, Iterator last) {
        return reinterpret_cast<Iterator>(FixedPtrVectorBase::erase(reinterpret_cast<FixedPtrVectorBase::Iterator>(first),
                                                               reinterpret_cast<FixedPtrVectorBase::Iterator>(last)));
    }

    inline void pushBack(const ItemType &value) {
        FixedPtrVectorBase::insert(FixedPtrVectorBase::end(), 1, value);
    }

    inline void popBack() {
        FixedPtrVectorBase::erase(FixedPtrVectorBase::end());
    }

    virtual ItemType createItem() {     /// \todo
        return NULLPTR;
    }

#if ETL_USE_CPP11

    Iterator find(std::function<bool(const ItemType)> &&matcher) const {
        return find(begin(), end(), std::move(matcher));
    }

    inline Iterator find(Iterator startPos, Iterator endPos, std::function<bool(const ItemType)> &&matcher) const;

#else

    Iterator find(const Matcher<T*> &matcher) const {
        return find(begin(), end(), matcher);
    }

    inline Iterator find(Iterator startPos, Iterator endPos, const Matcher<T*> &matcher) const;

#endif

};


#if ETL_USE_CPP11

template<class T>
typename FixedVector<T*>::Iterator FixedVector<T*>::find(Iterator startPos,
                                               Iterator endPos,
                                               std::function<bool(const ItemType)> &&matcher) const {

    bool match = false;

    while(!match && (startPos < endPos)) {

        match = matcher(*startPos);

        if(!match) {
            ++startPos;
        }
    }

    return startPos;
}

#else

template<class T>
typename FixedVector<T*>::Iterator FixedVector<T*>::find(Iterator startPos,
                                               Iterator endPos,
                                               const Matcher<T*> &matcher) const {

    bool match = false;

    while(!match && (startPos < endPos)) {

        match = matcher.call(*startPos);

        if(!match) {
            ++startPos;
        }
    }

    return startPos;
}

#endif
*/
}

#endif /* __ETL_FIXEDVECTOR_H__ */

