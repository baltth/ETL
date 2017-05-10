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


template<class T, template<class> class S = HeapUser>
class Vector : public VectorTemplate<T> {

  public:   // types

    typedef T ItemType;
    typedef VectorTemplate<T> Base;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;

  private:  // variables

    S<TypedVectorBase<T> > strategy;

  public:   // functions

    Vector() :
        Base(strategy),
        strategy(*this) {};

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const T& item);

    Vector(const Vector& other) :
        Base(other) {};

    Vector& operator=(const Vector& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector(Vector&& other) :
        Base(std::move(other)) {};

    Vector(const std::initializer_list<T>& initList) :
        Base(initList) {};

    Vector& operator=(Vector&& other) {
        return Base::operator=(std::move(other));
    }

    Vector& operator=(const std::initializer_list<T>& initList) {
        return Base::operator=(initList);
    }

#endif

  protected:

    Vector(ItemType* initData, uint32_t initCapacity) :
        Base(strategy),
        strategy(initData, initCapacity, *this) {};

};


template<class T, template<class> class S /* = HeapUser*/>
Vector<T, S>::Vector(uint32_t len) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, template<class> class S /* = HeapUser*/>
Vector<T, S>::Vector(uint32_t len, const T& item) {

    this->insert(this->begin(), len, item);
}


template<class T, template<class> class S>
class Vector<T*, S> : public VectorTemplate<void*> {

  public:   // types

    typedef T* ItemType;
    typedef VectorTemplate<void*> Base;
    typedef ItemType* Iterator;
    typedef const ItemType* ConstIterator;

  private:  // variables

    S<TypedVectorBase<void*> > strategy;

  public:   // functions

    Vector() :
        Base(strategy),
        strategy(*this) {};

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const ItemType& item);

    Vector(const Vector& other) :
        Base(other) {};

    Vector& operator=(const Vector& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector(Vector&& other) :
        Base(std::move(other)) {};

    Vector(const std::initializer_list<ItemType>& initList) :
        Base(initList) {};

    Vector& operator=(Vector&& other) {
        return Base::operator=(std::move(other));
    }

    Vector& operator=(const std::initializer_list<ItemType>& initList) {
        return Base::operator=(initList);
    }

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

  protected:

    Vector(ItemType* initData, uint32_t initCapacity) :
        Base(strategy),
        strategy(initData, initCapacity, *this) {};

};

template<class T, template<class> class S>
Vector<T*, S>::Vector(uint32_t len) {

    typename Base::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, template<class> class S>
Vector<T*, S>::Vector(uint32_t len, const ItemType& item) {

    this->insert(this->begin(), len, item);
}

}

#endif /* __ETL_VECTOR_H__ */

