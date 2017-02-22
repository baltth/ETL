/**
\file
\date 2016.01.11. 22:13:38
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

#ifndef __ETL_VECTOR_H__
#define __ETL_VECTOR_H__

#include "etlSupport.h"

#include "Base/VectorTemplate.h"
#include "Base/MemStrategies.h"


namespace ETL_NAMESPACE {


template<class T>
class Vector : public VectorTemplate<T, HeapUser> {
//friend DynamicResizeStrategy<Vector<T> >;

// types
public:

    typedef T ItemType;
    typedef VectorTemplate<T, HeapUser> Base;
    typedef typename Base::Iterator Iterator;

// functions
public:

    Vector<T>() {};

    explicit Vector<T>(uint32_t len);
    Vector<T>(uint32_t len, const T &item);

    Vector<T>(const Vector<T> &other) :
        Base(other) {};

    Vector<T> &operator=(const Vector<T> &other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector<T>(Vector<T> &&other) :
        Base(std::move(other)) {};

    Vector<T>(const std::initializer_list<T> &initList) :
        Base(initList) {};

    Vector<T> &operator=(Vector<T> &&other) {
        return Base::operator=(std::move(other));
    }

    Vector<T> &operator=(const std::initializer_list<T> &initList) {
        return Base::operator=(initList);
    };

    Iterator find(std::function<bool(const T &)> &&matcher) const {
        return find(Vector<T>::begin(), Vector<T>::end(), std::move(matcher));
    }

    Iterator find(Iterator startPos, Iterator endPos, std::function<bool(const T &)> &&matcher) const;

#else

    Iterator find(const Matcher<T> &matcher) const {
        return find(Vector<T>::begin(), Vector<T>::end(), matcher);
    }

    Iterator find(Iterator startPos, Iterator endPos, const Matcher<T> &matcher) const;

#endif

};


template<class T>
Vector<T>::Vector(uint32_t len) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T>
Vector<T>::Vector(uint32_t len, const T &item) {

    this->insert(this->begin(), len, item);
}


#if ETL_USE_CPP11

template<class T>
typename Vector<T>::Iterator Vector<T>::find(Iterator startPos,
                                             Iterator endPos,
                                             std::function<bool(const T &)> &&matcher) const {

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
typename Vector<T>::Iterator Vector<T>::find(Iterator startPos,
                                             Iterator endPos,
                                             const Matcher<T> &matcher) const {

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

/*
typedef VectorTemplate<void*, DynamicResizeStrategy<Vector<void*> > > PtrVectorBase;

template<class T>
class Vector<T*> : public PtrVectorBase {

// types
public:

    typedef T** Iterator;
    typedef T* ItemType;

// functions
public:

    Vector<ItemType>() :
        PtrVectorBase() {};

    explicit Vector<ItemType>(uint32_t len) :
        PtrVectorBase(len, NULLPTR) {};

    Vector<ItemType>(uint32_t len, const ItemType &item) :
        PtrVectorBase(len, item) {};

    Vector<ItemType>(const Vector<ItemType> &other) :
        PtrVectorBase(other) {};

    Vector<ItemType> &operator=(const Vector<ItemType> &other) {
        return PtrVectorBase::operator=(other);
    };

#if ETL_USE_CPP11

    Vector<ItemType>(Vector<ItemType> &&other) :
        PtrVectorBase(std::move(other)) {};

    Vector<ItemType>(const std::initializer_list<void*> &initList) :
        PtrVectorBase(initList) {};

    Vector<ItemType> &operator=(Vector<ItemType> &&other) {
        return PtrVectorBase::operator=(std::move(other));
    };

    Vector<ItemType> &operator=(const std::initializer_list<ItemType> &initList) {
        return PtrVectorBase::operator=(initList);
    };

#endif

    inline ItemType &operator[](int32_t ix) {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline const ItemType &operator[](int32_t ix) const {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline Iterator begin() const {
        return reinterpret_cast<Iterator>(PtrVectorBase::begin());
    }

    inline Iterator end() const {
        return reinterpret_cast<Iterator>(PtrVectorBase::end());
    }

    inline ItemType &front() {
        return reinterpret_cast<ItemType &>(PtrVectorBase::front());
    }

    inline const ItemType &front() const {
        return reinterpret_cast<const ItemType &>(PtrVectorBase::front());
    }

    inline ItemType &back() {
        return reinterpret_cast<ItemType &>(PtrVectorBase::back());
    }

    inline const ItemType &back() const {
        return reinterpret_cast<const ItemType &>(PtrVectorBase::back());
    }

    inline ItemType* getData() {
        return static_cast<ItemType*>(PtrVectorBase::getData());
    }

    inline const ItemType* getData() const {
        return static_cast<ItemType*>(PtrVectorBase::getData());
    }

    inline Iterator insert(Iterator position, const ItemType &value) {
        return reinterpret_cast<Iterator>(PtrVectorBase::insert(reinterpret_cast<PtrVectorBase::Iterator>(position),
                                                                1,
                                                                value));
    }

    inline Iterator insert(Iterator position, uint32_t num, const ItemType &value) {
        return reinterpret_cast<Iterator>(PtrVectorBase::insert(reinterpret_cast<PtrVectorBase::Iterator>(position),
                                                                num,
                                                                value));
    }

    inline Iterator erase(Iterator pos) {
        Iterator next = pos;
        ++next;
        return reinterpret_cast<Iterator>(PtrVectorBase::erase(reinterpret_cast<PtrVectorBase::Iterator>(pos),
                                                               reinterpret_cast<PtrVectorBase::Iterator>(next)));
    }

    inline Iterator erase(Iterator first, Iterator last) {
        return reinterpret_cast<Iterator>(PtrVectorBase::erase(reinterpret_cast<PtrVectorBase::Iterator>(first),
                                                               reinterpret_cast<PtrVectorBase::Iterator>(last)));
    }

    inline void pushBack(const ItemType &value) {
        PtrVectorBase::insert(PtrVectorBase::end(), 1, value);
    }

    inline void popBack() {
        PtrVectorBase::erase(PtrVectorBase::end());
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
typename Vector<T*>::Iterator Vector<T*>::find(Iterator startPos,
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
typename Vector<T*>::Iterator Vector<T*>::find(Iterator startPos,
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

#endif /* __ETL_VECTOR_H__ */
