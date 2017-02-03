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

#include "ETL/Base/VectorTemplate.h"


#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<class T>
class Vector : public VectorTemplate<T> {

// types
public:

    typedef typename VectorTemplate<T>::Iterator Iterator;
    typedef T ItemType;

// functions
public:

    Vector<T>() :
        VectorTemplate<T>() {};

    explicit Vector<T>(uint32_t len) :
        VectorTemplate<T>(len) {};

    Vector<T>(uint32_t len, const T &item) :
        VectorTemplate<T>(len, item) {};

    Vector<T>(const Vector<T> &other) :
        VectorTemplate<T>(other) {};

    Vector<T>(Vector<T> &&other) :
        VectorTemplate<T>(std::move(other)) {};

    Vector<T>(const std::initializer_list<T> &initList) :
        VectorTemplate<T>(initList) {};

    Vector<T> &operator=(const Vector<T> &other) {
        return VectorTemplate<T>::operator=(other);
    }

    Vector<T> &operator=(Vector<T> &&other) {
        return VectorTemplate<T>::operator=(std::move(other));
    }

    Vector<T> &operator=(const std::initializer_list<T> &initList) {
        return VectorTemplate<T>::operator=(initList);
    };

    Iterator find(std::function<bool(const T &)> &&matchCall) const {
        return find(Vector<T>::begin(), Vector<T>::end(), std::move(matchCall));
    }

    Iterator find(Iterator startPos, Iterator endPos, std::function<bool(const T &)> &&matchCall) const;

};


template<class T>
typename Vector<T>::Iterator Vector<T>::find(Iterator startPos,
                                             Iterator endPos,
                                             std::function<bool(const T &)> &&matchCall) const {

    bool match = false;

    while(!match && (startPos < endPos)) {

        match = matchCall(*startPos);

        if(!match) {
            ++startPos;
        }
    }

    return startPos;
}


using PtrVectorBase = VectorTemplate<void*>;

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
        PtrVectorBase(len, nullptr) {};

    Vector<ItemType>(uint32_t len, const ItemType &item) :
        PtrVectorBase(len, item) {};

    Vector<ItemType>(const Vector<ItemType> &other) :
        PtrVectorBase(other) {};

    Vector<ItemType>(Vector<ItemType> &&other) :
        PtrVectorBase(std::move(other)) {};

    Vector<ItemType>(const std::initializer_list<void*> &initList) :
        PtrVectorBase(initList) {};

    Vector<ItemType> &operator=(const Vector<ItemType> &other) {
        return PtrVectorBase::operator=(other);
    };

    Vector<ItemType> &operator=(Vector<ItemType> &&other) {
        return PtrVectorBase::operator=(std::move(other));
    };

    Vector<ItemType> &operator=(const std::initializer_list<ItemType> &initList) {
        return PtrVectorBase::operator=(initList);
    };

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

    Iterator find(std::function<bool(const ItemType)> &&matchCall) const {
        return find(begin(), end(), std::move(matchCall));
    }

    inline Iterator find(Iterator startPos, Iterator endPos, std::function<bool(const ItemType)> &&matchCall) const;

    virtual ItemType createItem() {     /// \todo
        return nullptr;
    }

};


template<class T>
typename Vector<T*>::Iterator Vector<T*>::find(Iterator startPos,
                                               Iterator endPos,
                                               std::function<bool(const ItemType)> &&matchCall) const {

    bool match = false;

    while(!match && (startPos < endPos)) {

        match = matchCall(*startPos);

        if(!match) {
            ++startPos;
        }
    }

    return startPos;
}

}

#endif /* __ETL_VECTOR_H__ */
