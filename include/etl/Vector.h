/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2022 Balazs Toth.

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

#include <etl/base/MemStrategies.h>
#include <etl/base/VectorTemplate.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {

namespace Static {

/// Vector with static memory strategy.
template<class T, size_t N>
class Vector : public ETL_NAMESPACE::Vector<T> {

    static_assert(N > 0, "Invalid Etl::Static::Vector<> size");

  public:  // types

    using Base = ETL_NAMESPACE::Vector<T>;
    using StrategyBase = typename Base::StrategyBase;
    using Strategy = StaticSized<StrategyBase>;

  private:  // variables

    uint8_t data_[N * sizeof(T)];
    Strategy strategy;

  public:  // functions

    Vector() noexcept :
        Base(strategy),
        strategy(data_, N) {
        this->reserve(N);
    }

    explicit Vector(uint32_t len);
    Vector(uint32_t len, const T& item);

    Vector(const Vector& other) :
        Base(strategy),
        strategy(data_, N) {
        operator=(other);
    }

    explicit Vector(const Base& other) :
        Base(strategy),
        strategy(data_, N) {
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

    Vector(Vector&& other) noexcept(noexcept(Vector().moveAssignSameType(Vector()))) :
        Base(strategy),
        strategy(data_, N) {
        moveAssignSameType(std::move(other));
    }

    Vector(std::initializer_list<T> initList) :
        Base(strategy),
        strategy(data_, N) {
        operator=(initList);
    }

    Vector& operator=(Vector&& other) noexcept(noexcept(Vector().moveAssignSameType(Vector()))) {
        moveAssignSameType(std::move(other));
        return *this;
    }

    Vector& operator=(Base&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    Vector& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Vector() {
        strategy.cleanup(*this);
    }

  private:

    void moveAssignSameType(Vector&& other) noexcept(
        noexcept(Detail::NothrowContract<T>::nothrowIfMovable)) {
        if (&other != this) {
            ETL_ASSERT(this->max_size() >= other.size());
            this->reserve_exactly(other.size());
            ETL_ASSERT(this->capacity() >= other.size());
            this->moveFromOther(this->data(), other.data(), other.size());
            other.clear();
        }
    }
};

template<class T, size_t N>
Vector<T, N>::Vector(uint32_t len) :
    Base(strategy),
    strategy(data_, N) {

    this->insertDefault(this->cbegin(), len);
}


template<class T, size_t N>
Vector<T, N>::Vector(uint32_t len, const T& item) :
    Base(strategy),
    strategy(data_, N) {

    this->insert(this->cbegin(), len, item);
}


template<class T, size_t N>
void swap(Static::Vector<T, N>& lhs, Static::Vector<T, N>& rhs) {
    lhs.swap(rhs);
}

}  // namespace Static


namespace Custom {

/// Vector with dynamic memory strategy, using custom allocator.
template<class T, template<class> class A>
class Vector : public ETL_NAMESPACE::Vector<T> {

  public:  // types

    using Base = ETL_NAMESPACE::Vector<T>;
    using StrategyBase = typename Base::StrategyBase;
    using Allocator = A<typename StrategyBase::value_type>;
    using Strategy = DynamicSized<StrategyBase, Allocator>;

  private:  // variables

    Strategy strategy;

  public:  // functions

    Vector() noexcept :
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

    Vector(Vector&& other) noexcept(noexcept(Vector().swap(other))) :
        Base(strategy) {
        this->swap(other);
    }

    Vector(std::initializer_list<T> initList) :
        Base(strategy) {
        operator=(initList);
    }

    Vector& operator=(Vector&& other) noexcept(noexcept(Vector().swap(other))) {
        this->swap(other);
        return *this;
    }

    Vector& operator=(Base&& other) {
        Base::operator=(std::move(other));
        return *this;
    }

    Vector& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~Vector() {
        strategy.cleanup(*this);
    }

    void swap(Vector& other) noexcept(noexcept(Vector().swapSameType(other))) {
        if (&other != this) {
            swapSameType(other);
        }
    }

    void swap(Base& other) {
        if (&other != this) {
            Base::swap(other);
        }
    }

  private:

    template<bool UA = Strategy::UNIQUE_ALLOCATOR>
    enable_if_t<UA, void> swapSameType(Vector& other) {
        Base::swap(other);
    }

    template<bool UA = Strategy::UNIQUE_ALLOCATOR>
    enable_if_t<!UA, void>
    swapSameType(Vector& other) noexcept(noexcept(Vector().Detail::AVectorBase::swapProxy(other))) {
        Detail::AVectorBase::swapProxy(other);
    }
};


template<class T, template<class> class A>
Vector<T, A>::Vector(uint32_t len) :
    Base(strategy) {

    this->insertDefault(this->cbegin(), len);
}


template<class T, template<class> class A>
Vector<T, A>::Vector(uint32_t len, const T& item) :
    Base(strategy) {

    this->insert(this->cbegin(), len, item);
}


template<class T, template<class> class A>
void swap(Custom::Vector<T, A>& lhs, Custom::Vector<T, A>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

}  // namespace Custom


namespace Dynamic {

/// Vector with dynamic memory allocation using std::allocator.
template<class T>
using Vector = ETL_NAMESPACE::Custom::Vector<T, std::allocator>;

}  // namespace Dynamic

}  // namespace ETL_NAMESPACE

#endif  // __ETL_VECTOR_H__
