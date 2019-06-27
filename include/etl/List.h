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
#include <etl/base/AAllocator.h>
#include <etl/PoolAllocator.h>

#include <memory>

namespace ETL_NAMESPACE {

namespace Custom {

/// List with custom allocator.
template<class T, template<class> class A>
class List : public ETL_NAMESPACE::List<T> {

  public:   // types

    typedef ETL_NAMESPACE::List<T> Base;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::Node Node;

    typedef typename AllocatorType<Node, A>::Type Allocator;

  private:  // variables

    mutable Allocator allocator;

  public:   // functions

    List() :
        Base(allocator) {};

    List(const List& other) :
        List() {
        Base::operator=(other);
    }

    explicit List(const Base& other) :
        List() {
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

    List(List&& other) :
        List() {
        operator=(std::move(other));
    }

    List& operator=(List&& other) {
        this->swap(other);
        return *this;
    }

    List(std::initializer_list<T> initList) :
        List() {
        operator=(initList);
    }

    List& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~List() {
        this->clear();
    }

    Allocator& getAllocator() const {
        return allocator;
    }

};

}


namespace Dynamic {

/// List with dynamic memory allocation using std::allocator.
template<class T>
using List = ETL_NAMESPACE::Custom::List<T, std::allocator>;

}


namespace Static {

/// List with unique pool allocator.
template<class T, uint32_t N>
class List : public ETL_NAMESPACE::List<T> {

    static_assert(N > 0, "Invalid Etl::Static::List size");

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
        List() {
        Base::operator=(other);
    }

    explicit List(const Base& other) :
        List() {
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

    List(List&& other) :
        List() {
        operator=(std::move(other));
    }

    List& operator=(List&& other) {
        this->swap(other);
        return *this;
    }

    List(std::initializer_list<T> initList) :
        List() {
        operator=(initList);
    }

    List& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~List() {
        this->clear();
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

    static_assert(N > 0, "Invalid Etl::Pooled::List size");

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
        List() {
        Base::operator=(other);
    }

    explicit List(const Base& other) :
        List() {
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

    List(List&& other) :
        List() {
        operator=(std::move(other));
    }

    List& operator=(List&& other) {
        this->swap(other);
        return *this;
    }

    List(std::initializer_list<T> initList) :
        List() {
        operator=(initList);
    }

    List& operator=(std::initializer_list<T> initList) {
        Base::operator=(initList);
        return *this;
    }

    ~List() {
        this->clear();
    }

    Allocator& getAllocator() const {
        return allocator;
    }

};

}
}

#endif /* __ETL_LIST_H__ */

