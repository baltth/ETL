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

namespace Dynamic {

template<class T, template<class> class A = std::allocator>
class Vector : public ETL_NAMESPACE::Vector<T> {

  public:   // types

    typedef T ItemType;
    typedef ETL_NAMESPACE::Vector<T> Base;
    typedef typename Base::StrategyBase StrategyBase;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::ConstIterator ConstIterator;
    typedef A<typename StrategyBase::ItemType> Allocator;

  private:  // variables

    DynamicSized<StrategyBase, Allocator> strategy;

  public:   // functions

    Vector() :
        Base(strategy),
        strategy(*this) {};

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const T& item);

    Vector(const Vector& other) :
        Base(strategy),
        strategy(*this) {
        Base::operator=(other);
    };

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


template<class T, template<class> class A /* = std::allocator<T> */>
Vector<T, A>::Vector(uint32_t len) :
    Base(strategy),
    strategy(*this) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    this->insertWithCreator(this->begin(), len, dc);
}


template<class T, template<class> class A /* = std::allocator<T> */>
Vector<T, A>::Vector(uint32_t len, const T& item) :
    Base(strategy),
    strategy(*this) {

    this->insert(this->begin(), len, item);
}

}

}

#endif /* __ETL_VECTOR_H__ */

