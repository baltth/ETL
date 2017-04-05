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

  public:   // types

    typedef T ItemType;
    typedef VectorTemplate<T, HeapUser> Base;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;


  public:   // functions

    Vector<T>() {};

    explicit Vector<T>(uint32_t len);
    Vector<T>(uint32_t len, const T& item);

    Vector<T>(const Vector<T>& other) :
        Base(other) {};

    Vector<T>& operator=(const Vector<T>& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector<T>(Vector<T>&& other) :
        Base(std::move(other)) {};

    Vector<T>(const std::initializer_list<T>& initList) :
        Base(initList) {};

    Vector<T>& operator=(Vector<T>&& other) {
        return Base::operator=(std::move(other));
    }

    Vector<T>& operator=(const std::initializer_list<T>& initList) {
        return Base::operator=(initList);
    }

    Iterator find(std::function<bool(const T&)>&& matcher) const {
        return find(this->begin(), this->end(), std::move(matcher));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, std::function<bool(const T&)>&& matcher) const;

#else

    Iterator find(const Matcher<T>& matcher) const {
        return find(this->begin(), this->end(), matcher);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<T>& matcher) const;

#endif

};


template<class T>
Vector<T>::Vector(uint32_t len) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T>
Vector<T>::Vector(uint32_t len, const T& item) {

    this->insert(this->begin(), len, item);
}


#if ETL_USE_CPP11

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


typedef VectorTemplate<void*, HeapUser> PtrVectorBase;

template<class T>
class Vector<T*> : public VectorTemplate<void*, HeapUser> {

// types
  public:

    typedef T* ItemType;
    typedef VectorTemplate<void*, HeapUser> Base;
    typedef ItemType* Iterator;
    typedef const ItemType* ConstIterator;

// functions
  public:

    Vector<ItemType>() {};

    explicit Vector<ItemType>(uint32_t len);
    Vector<ItemType>(uint32_t len, const ItemType& item);

    Vector<ItemType>(const Vector<ItemType>& other) :
        Base(other) {};

    Vector<ItemType>& operator=(const Vector<ItemType>& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector<ItemType>(Vector<T>&& other) :
        Base(std::move(other)) {};

    Vector<ItemType>(const std::initializer_list<ItemType>& initList) :
        Base(initList) {};

    Vector<ItemType>& operator=(Vector<ItemType>&& other) {
        return Base::operator=(std::move(other));
    }

    Vector<ItemType>& operator=(const std::initializer_list<ItemType>& initList) {
        return Base::operator=(initList);
    }

    Iterator find(std::function<bool(const ItemType&)>&& matcher) const {
        return find(this->begin(), this->end(), std::move(matcher));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, std::function<bool(const ItemType&)>&& matcher) const;

#else

    Iterator find(const Matcher<ItemType>& matcher) const {
        return find(this->begin(), this->end(), matcher);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const Matcher<ItemType>& matcher) const;

#endif

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

};


template<class T>
Vector<T*>::Vector(uint32_t len) {

    typename Base::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T>
Vector<T*>::Vector(uint32_t len, const ItemType& item) {

    this->insert(this->begin(), len, item);
}


#if ETL_USE_CPP11

template<class T>
typename Vector<T*>::Iterator Vector<T*>::find(ConstIterator startPos,
                                               ConstIterator endPos,
                                               std::function<bool(const T*&)>&& matcher) const {

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
typename Vector<T*>::Iterator Vector<T*>::find(ConstIterator startPos,
                                               ConstIterator endPos,
                                               const Matcher<T*>& matcher) const {

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

#endif /* __ETL_VECTOR_H__ */

