/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Balazs Toth.

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

#ifndef __ETL_POOLALLOCATOR_H__
#define __ETL_POOLALLOCATOR_H__

#include <ETL/etlSupport.h>
#include <ETL/MemoryPool.h>

#include <new>

namespace ETL_NAMESPACE {

/**
Allocator template class.
Forwards the `std::allocator` interface to a MemoryPool.
\tparam T Item type
\tparam N Size of pool
*/
template<class T, uint32_t N>
class PoolAllocator {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef T ItemType;
    typedef T* PtrType;

  private:  // variables

    MemoryPool<sizeof(T), N> pool;

  public:   // functions

    uint32_t getSize() const {
        return pool.getCount();
    }

    uint32_t getCapacity() const {
        return pool.getCapacity();
    }

    bool isEmpty() const {
        return (getSize() == 0);
    }

    PtrType allocate(uint32_t n) {
        return static_cast<PtrType>(pool.pop(n));
    }

    void deallocate(PtrType ptr, uint32_t n) {
        pool.push(ptr, n);
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
Allocator template class with static pool.
Forwards the `std::allocator` interface to a static MemoryPool.
\tparam T Item type
\tparam N Size of pool
*/
template<class T, uint32_t N>
class CommonPoolAllocator {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef Etl::PoolAllocator<T, N> Allocator;
    typedef typename Allocator::ItemType ItemType;
    typedef typename Allocator::PtrType PtrType;

  public:   // functions

    uint32_t getSize() const {
        return allocator().getSize();
    }

    uint32_t getCapacity() const {
        return allocator().getCapacity();
    }

    bool isEmpty() const {
        return allocator().isEmpty();
    }

    PtrType allocate(uint32_t n) {
        return allocator().allocate(n);
    }

    void deallocate(PtrType ptr, uint32_t n) {
        allocator().deallocate(ptr, n);
    }

    void construct(PtrType ptr) {
        Allocator::construct(ptr);
    }

    void construct(PtrType ptr, const ItemType& other) {
        Allocator::construct(ptr, other);
    }

#if ETL_USE_CPP11

    template<typename... Args >
    void construct(PtrType ptr, Args&& ... args) {
        Allocator::construct(ptr, std::forward<Args>(args)...);
    }

#else

    template<typename A>
    void construct(PtrType ptr, A a) {
        Allocator::construct(ptr, a);
    }

    template<typename A, typename B>
    void construct(PtrType ptr, A a, B b) {
        Allocator::construct(ptr, a, b);
    }

    template<typename A, typename B, typename C>
    void construct(PtrType ptr, A a, B b, C c) {
        Allocator::construct(ptr, a, b, c);
    }

#endif

    void destroy(PtrType ptr) {
        Allocator::destroy(ptr);
    }

  private:

    static Allocator& allocator() {
        static Allocator alloc;
        return alloc;
    }

};


template<uint32_t N>
class PoolHelper {

  public:   // types

    template<class T>
    class Allocator : public PoolAllocator<T, N> {};

    template<class T>
    class CommonAllocator : public CommonPoolAllocator<T, N> {};

};



}

#endif /* __ETL_POOLALLOCATOR_H__ */

