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

#include <etl/etlSupport.h>
#include <etl/MemoryPool.h>
#include <etl/base/AAllocator.h>

namespace ETL_NAMESPACE {

/**
Allocator template class.
Implements AAllocator based on MemoryPool.
\tparam T Item type
\tparam N Size of pool
*/
template<class T, uint32_t N>
class PoolAllocator : public AAllocator<T> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef T ItemType;
    typedef T* PtrType;

  private:  // variables

    MemoryPool<sizeof(T), N> pool;

  public:   // functions

    virtual uint32_t size() const OVERRIDE {
        return pool.getCount();
    }

    virtual uint32_t capacity() const OVERRIDE {
        return pool.capacity();
    }

    virtual PtrType allocate(uint32_t n) OVERRIDE {
        if (n == 1) {
            return static_cast<PtrType>(pool.pop());
        } else {
            return NULLPTR;
        }
    }

    virtual void deallocate(PtrType ptr, uint32_t n) OVERRIDE {
        (void)n;
        pool.push(ptr);
    }

};


/**
Allocator template class with static pool.
Forwards the `std::allocator` interface to a static MemoryPool.
\tparam T Item type
\tparam N Size of pool
*/
template<class T, uint32_t N>
class CommonPoolAllocator : public AAllocator<T> {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef Etl::PoolAllocator<T, N> Allocator;
    typedef typename Allocator::ItemType ItemType;
    typedef typename Allocator::PtrType PtrType;

  public:   // functions

    virtual uint32_t size() const OVERRIDE {
        return allocator().size();
    }

    virtual uint32_t capacity() const OVERRIDE {
        return allocator().capacity();
    }

    virtual PtrType allocate(uint32_t n) OVERRIDE {
        return allocator().allocate(n);
    }

    virtual void deallocate(PtrType ptr, uint32_t n) OVERRIDE {
        allocator().deallocate(ptr, n);
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

