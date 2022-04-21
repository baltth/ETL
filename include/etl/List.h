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

#ifndef __ETL_LIST_H__
#define __ETL_LIST_H__

#include <etl/PoolAllocator.h>
#include <etl/base/AAllocator.h>
#include <etl/base/ListTemplate.h>
#include <etl/etlSupport.h>

#include <memory>

namespace ETL_NAMESPACE {

namespace Custom {

/// List with custom allocator.
template<class T, template<class> class A>
class List : public ETL_NAMESPACE::List<T> {

  public:  // types

    using Base = ETL_NAMESPACE::List<T>;
    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using Node = typename Base::Node;

    using Allocator = typename AllocatorTraits<Node, A>::Type;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    List() noexcept :
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

    List(List&& other) noexcept(noexcept(List().swap(other))) :
        List() {
        this->swap(other);
    }

    List& operator=(List&& other) noexcept(noexcept(List().swap(other))) {
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

    void swap(List& other) noexcept {
        static_assert(noexcept(List().swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!Allocator::UNIQUE_ALLOCATOR,
                      "Allocator should use UNIQUE_ALLOCATOR == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(List& lhs, List& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Custom


namespace Dynamic {

/// List with dynamic memory allocation using std::allocator.
template<class T>
using List = ETL_NAMESPACE::Custom::List<T, std::allocator>;

}  // namespace Dynamic


namespace Static {

/// List with unique pool allocator.
template<class T, uint32_t N>
class List : public ETL_NAMESPACE::List<T> {

    static_assert(N > 0, "Invalid Etl::Static::List size");

  public:  // types

    using Base = ETL_NAMESPACE::List<T>;
    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;

    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template Allocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    List() noexcept :
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

    List(List&& other) noexcept(noexcept(List().swap(other))) :
        List() {
        this->swap(other);
    }

    List& operator=(List&& other) noexcept(noexcept(List().swap(other))) {
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

    void swap(List& other) noexcept(noexcept(Detail::NothrowContract<T>::nothrowIfMovable)) {
        // Note: this operation is noexcept when T can be moved 'noexceptly',
        // however lower level functions are not annotated with noexcept qualifier.
        static_assert(Allocator::UNIQUE_ALLOCATOR,
                      "Allocator should use UNIQUE_ALLOCATOR == true");
        if (&other != this) {
            Base::swap(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(List& lhs, List& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Static


namespace Pooled {

/// List with common pool allocator.
template<class T, uint32_t N>
class List : public ETL_NAMESPACE::List<T> {

    static_assert(N > 0, "Invalid Etl::Pooled::List size");

  public:  // types

    using Base = ETL_NAMESPACE::List<T>;
    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;

    using Allocator =
        typename ETL_NAMESPACE::PoolHelperForSize<N>::template CommonAllocator<typename Base::Node>;

  private:  // variables

    mutable Allocator allocator;

  public:  // functions

    List() noexcept :
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

    List(List&& other) noexcept(noexcept(List().swap(other))) :
        List() {
        this->swap(other);
    }

    List& operator=(List&& other) noexcept(noexcept(List().swap(other))) {
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

    void swap(List& other) noexcept {
        static_assert(noexcept(List().swapNodeList(other)),
                      "noexcept contract violation");
        static_assert(!Allocator::UNIQUE_ALLOCATOR,
                      "Allocator should use UNIQUE_ALLOCATOR == false");
        if (&other != this) {
            Base::swapNodeList(other);
        }
    }

    using Base::swap;

  private:

    friend void swap(List& lhs, List& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

}  // namespace Pooled
}  // namespace ETL_NAMESPACE

#endif  // __ETL_LIST_H__
