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

#ifndef __ETL_LIST_H__
#define __ETL_LIST_H__

#include <etl/etlSupport.h>
#include <etl/base/ListTemplate.h>
#include <etl/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {

namespace Dynamic {

/// List with dynamic memory allocation, defaults to std::allocator.
template<class T, template<class> class A = std::allocator>
class List : public ETL_NAMESPACE::List<T> {

  public:   // types

    typedef ETL_NAMESPACE::List<T> Base;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::Node Node;

    typedef ETL_NAMESPACE::DynamicAllocator<typename Base::Node, A> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    List() :
        Base(allocator) {};

    List(const List& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit List(const Base& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    List& operator=(const List& other) {
        Base::operator=(other);
        return *this;
    }

    List& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    List(List&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    List(std::initializer_list<T> initList) :
        Base(allocator) {
        operator=(initList);
    }

    List& operator=(List&& other) {
        this->swap(other);
        return *this;
    }

    List& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    void swap(List& other) {
        AListBase::swapNodeList(other);
    }

    void swap(Base& other) {
        Base::swap(other);
    }

    Allocator& getAllocator() const {
        return allocator;
    }

};

}


namespace Static {

/// List with unique pool allocator.
template<class T, uint32_t N>
class List : public ETL_NAMESPACE::List<T> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::List<T> Base;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

    typedef typename ETL_NAMESPACE::PoolHelper<N>::template Allocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    List() :
        Base(allocator) {};

    List(const List& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit List(const Base& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    List& operator=(const List& other) {
        Base::operator=(other);
        return *this;
    }

    List& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    List(List&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    List(std::initializer_list<T> initList) :
        Base(allocator) {
        operator=(initList);
    }

    List& operator=(List&& other) {
        this->swap(other);
        return *this;
    }

    List& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    void swap(Base& other) {
        Base::swap(other);
    }

    Allocator& getAllocator() const {
        return allocator;
    }

};

}


namespace Pooled {

/// List with common pool allocator.
template<class T, uint32_t N>
class List : public ETL_NAMESPACE::List<T> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef ETL_NAMESPACE::List<T> Base;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

    typedef typename ETL_NAMESPACE::PoolHelper<N>::template CommonAllocator<typename Base::Node> Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    List() :
        Base(allocator) {};

    List(const List& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    explicit List(const Base& other) :
        Base(allocator) {
        Base::operator=(other);
    }

    List& operator=(const List& other) {
        Base::operator=(other);
        return *this;
    }

    List& operator=(const Base& other) {
        Base::operator=(other);
        return *this;
    }

#if ETL_USE_CPP11

    List(List&& other) :
        Base(allocator) {
        operator=(std::move(other));
    }

    List(std::initializer_list<T> initList) :
        Base(allocator) {
        operator=(initList);
    }

    List& operator=(List&& other) {
        this->swap(other);
        return *this;
    }

    List& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

#endif

    void swap(List& other) {
        AListBase::swapNodeList(other);
    }

    void swap(Base& other) {
        Base::swap(other);
    }

    Allocator& getAllocator() const {
        return allocator;
    }

};

}

}

#endif /* __ETL_LIST_H__ */

