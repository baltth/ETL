/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2018 Balazs Toth.

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

#ifndef __ETL_AALLOCATOR_H__
#define __ETL_AALLOCATOR_H__

#include <etl/etlSupport.h>

#include <new>

namespace ETL_NAMESPACE {

/**
Allocator interface class.
\tparam T Item type
*/
template<class T>
class AAllocator {

  public:   // types

    typedef T ItemType;
    typedef T* PtrType;

  public:   // functions

    virtual uint32_t size() const = 0;
    virtual uint32_t capacity() const = 0;

    virtual PtrType allocate(uint32_t n) = 0;
    virtual void deallocate(PtrType ptr, uint32_t n) = 0;

    bool empty() const {
        return (size() == 0);
    }

    static void construct(PtrType ptr) {
        new (ptr) ItemType;
    }

    static void construct(PtrType ptr, const ItemType& other) {
        new (ptr) ItemType(other);
    }

#if ETL_USE_CPP11

    template<typename... Args >
    static void construct(PtrType ptr, Args&& ... args) {
        new (ptr) ItemType(std::forward<Args>(args)...);
    }

#else

    template<typename A>
    static void construct(PtrType ptr, A a) {
        new (ptr) ItemType(a);
    }

    template<typename A, typename B>
    static void construct(PtrType ptr, A a, B b) {
        new (ptr) ItemType(a, b);
    }

    template<typename A, typename B, typename C>
    static void construct(PtrType ptr, A a, B b, C c) {
        new (ptr) ItemType(a, b, c);
    }

#endif

    static void destroy(PtrType ptr) {
        ptr->~ItemType();
    }

};


/**
Allocator template class with dynamic memory.
Implements AAllocator based on dynamic memory handling.
\tparam T Item type
\tparam A allocator type to forward, defaults to std::allocator
*/
template<class T, template<class> class A = std::allocator>
class DynamicAllocator : public AAllocator<T> {

  public:   // types

    typedef T ItemType;
    typedef T* PtrType;

    typedef A<T> Allocator;

  private:  // variables

    Allocator allocator;

  public:   // functions

    virtual uint32_t size() const OVERRIDE {
        return allocator.size();
    }

    virtual uint32_t capacity() const OVERRIDE {
        return allocator.capacity();
    }

    virtual PtrType allocate(uint32_t n) OVERRIDE {
        return allocator.allocate(n);
    }

    virtual void deallocate(PtrType ptr, uint32_t n) OVERRIDE {
        allocator.deallocate(ptr, n);
    }

};

}

#endif /* __ETL_AALLOCATOR_H__ */

