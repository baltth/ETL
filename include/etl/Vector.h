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

#ifndef __ETL_VECTOR_H__
#define __ETL_VECTOR_H__

#include <etl/etlSupport.h>
#include <etl/base/VectorTemplate.h>
#include <etl/base/MemStrategies.h>

namespace ETL_NAMESPACE {

namespace Static {

/// Vector with static memory strategy.
template<class T, size_t N>
class Vector : public ETL_NAMESPACE::Vector<T> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::Vector<T> Base;
    typedef typename Base::StrategyBase StrategyBase;

  private:  // variables

    uint8_t data[N * sizeof(T)];
    StaticSized<StrategyBase> strategy;

  public:   // functions

    Vector() :
        Base(strategy),
        strategy(data, N) {
        this->reserve(N);
    }

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const T& item);

    Vector(const Vector& other) :
        Base(strategy),
        strategy(data, N) {
        operator=(other);
    }

    explicit Vector(const Base& other) :
        Base(strategy),
        strategy(data, N) {
        operator=(other);
    }

    Vector& operator=(const Vector& other) {
        Base::operator=(other);
        return *this;
    }

    Vector& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector(Vector&& other) :
        Base(strategy),
        strategy(data, N) {
        operator=(std::move(other));
    }

    Vector(std::initializer_list<T> initList) :
        Base(strategy),
        strategy(data, N) {
        operator=(initList);
    }

    Vector& operator=(Vector&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    Vector& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    ~Vector() {
        strategy.cleanup(*this);
    }

};

template<class T, size_t N>
Vector<T, N>::Vector(uint32_t len) :
    Base(strategy),
    strategy(data, N) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, size_t N>
Vector<T, N>::Vector(uint32_t len, const T& item) :
    Base(strategy),
    strategy(data, N) {

    this->insert(this->begin(), len, item);
}

}


namespace Dynamic {

/// Vector with dynamic memory strategy, defaults to std::allocator.
template<class T, template<class> class A = std::allocator>
class Vector : public ETL_NAMESPACE::Vector<T> {

  public:   // types

    typedef ETL_NAMESPACE::Vector<T> Base;
    typedef typename Base::StrategyBase StrategyBase;
    typedef A<typename StrategyBase::value_type> Allocator;

  private:  // variables

    DynamicSized<StrategyBase, Allocator> strategy;

  public:   // functions

    Vector() :
        Base(strategy) {};

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const T& item);

    Vector(const Vector& other) :
        Base(strategy) {
        Base::operator=(other);
    }

    explicit Vector(const Base& other) :
        Base(strategy) {
        Base::operator=(other);
    }

    Vector& operator=(const Vector& other) {
        Base::operator=(other);
        return *this;
    }

    Vector& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    Vector(Vector&& other) :
        Base(strategy) {
        operator=(std::move(other));
    }

    Vector(std::initializer_list<T> initList) :
        Base(strategy) {
        operator=(initList);
    }

    Vector& operator=(Vector&& other) {
        this->swap(other);
        return *this;
    }

    Vector& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    ~Vector() {
        strategy.cleanup(*this);
    }

    bool swap(Vector& other) {
        if (&other != this) {
            AVectorBase::swapProxy(other);
            return true;
        } else {
            return false;
        }
    }

    bool swap(Base& other) {
        return Base::swap(other);
    }

};


template<class T, template<class> class A /* = std::allocator<T> */>
Vector<T, A>::Vector(uint32_t len) :
    Base(strategy) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, template<class> class A /* = std::allocator<T> */>
Vector<T, A>::Vector(uint32_t len, const T& item) :
    Base(strategy) {

    this->insert(this->begin(), len, item);
}

}

}

#endif /* __ETL_VECTOR_H__ */

