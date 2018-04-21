/**
\file
\date 2016.01.11.
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

#include "ETL/etlSupport.h"

#include "ETL/base/VectorTemplate.h"
#include "ETL/base/MemStrategies.h"

namespace ETL_NAMESPACE {

namespace Static {

template<class T, size_t N>
class Vector : public ETL_NAMESPACE::Vector<T> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef T ItemType;
    typedef ETL_NAMESPACE::Vector<T> base;
    typedef typename base::StrategyBase StrategyBase;
    typedef typename base::Iterator Iterator;
    typedef typename base::ConstIterator ConstIterator;

  private:  // variables

    uint8_t data[N * sizeof(T)];
    StaticSized<StrategyBase> strategy;

  public:   // functions

    Vector() :
        base(strategy),
        strategy(data, N) {
        this->reserve(N);
    }

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const T& item);

    Vector(const Vector& other) :
        base(strategy),
        strategy(data, N) {
        operator=(other);
    }

    Vector(const base& other) :
        base(strategy),
        strategy(data, N) {
        operator=(other);
    }

    Vector& operator=(const Vector& other) {
        base::operator=(other);
        return *this;
    }

    Vector& operator=(const base& other) {
        base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector(Vector&& other) :
        base(strategy),
        strategy(data, N) {
        operator=(std::move(other))
    }

    Vector(const std::initializer_list<T>& initList) :
        base(strategy),
        strategy(data, N) {
        operator=(initList)
    }

    Vector& operator=(Vector&& other) {
        base::operator=(std::move(other));
        return *this;
    }

    Vector& operator=(const std::initializer_list<T>& initList) {
        base::operator=(initList);
        return *this;
    }

#endif

    ~Vector() {
        strategy.cleanup(*this);
    }

};

template<class T, size_t N>
Vector<T, N>::Vector(uint32_t len) :
    base(strategy),
    strategy(data, N) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, size_t N>
Vector<T, N>::Vector(uint32_t len, const T& item) :
    base(strategy),
    strategy(data, N) {

    this->insert(this->begin(), len, item);
}

}


namespace Dynamic {

template<class T, template<class> class A = std::allocator>
class Vector : public ETL_NAMESPACE::Vector<T> {

  public:   // types

    typedef T ItemType;
    typedef ETL_NAMESPACE::Vector<T> base;
    typedef typename base::StrategyBase StrategyBase;
    typedef typename base::Iterator Iterator;
    typedef typename base::ConstIterator ConstIterator;
    typedef A<typename StrategyBase::ItemType> Allocator;

  private:  // variables

    DynamicSized<StrategyBase, Allocator> strategy;

  public:   // functions

    Vector() :
        base(strategy) {};

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const T& item);

    Vector(const Vector& other) :
        base(strategy) {
        base::operator=(other);
    }

    Vector(const base& other) :
        base(strategy) {
        base::operator=(other);
    }

    Vector& operator=(const Vector& other) {
        base::operator=(other);
        return *this;
    }

    Vector& operator=(const base& other) {
        base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector(Vector&& other) :
        base(strategy) {
        operator=(std::move(other));
    }

    Vector(const std::initializer_list<T>& initList) :
        base(strategy) {
        operator=(initList);
    }

    Vector& operator=(Vector&& other) {
        return base::operator=(std::move(other));
    }

    Vector& operator=(const std::initializer_list<T>& initList) {
        return base::operator=(initList);
    }

#endif

    ~Vector() {
        strategy.cleanup(*this);
    }

};


template<class T, template<class> class A /* = std::allocator<T> */>
Vector<T, A>::Vector(uint32_t len) :
    base(strategy) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, template<class> class A /* = std::allocator<T> */>
Vector<T, A>::Vector(uint32_t len, const T& item) :
    base(strategy) {

    this->insert(this->begin(), len, item);
}

}

}

#endif /* __ETL_VECTOR_H__ */

