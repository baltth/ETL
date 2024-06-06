/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017-2024 Balazs Toth.

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

#ifndef ETL_POOLALLOCATOR_H_
#define ETL_POOLALLOCATOR_H_

#include <etl/MemoryPool.h>
#include <etl/base/AAllocator.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>

#include <mutex>

namespace ETL_NAMESPACE {

/**
Allocator template class.
Implements AAllocator based on MemoryPool.
\note This allocator supports allocating exactly _one element_ for one call.
\tparam T Item type
\tparam N Size of pool
\tparam L Lock type, defaults to 'null lock'
*/
template<class T, uint32_t N, class L = Detail::NullLock>
class PoolAllocator : public AAllocator<T> {

    static_assert(N > 0, "Invalid pool size");

  public:  // types

    using ItemType = T;
    using PtrType = T*;
    using Lock = L;

    static constexpr bool uniqueAllocator = true;

  private:  // variables

    MemoryPool<sizeof(T), N> pool {};
    mutable Lock l {};

  public:  // functions

    std::size_t max_size() const noexcept override {
        auto lg = Detail::lock(l);
        return pool.capacity();
    }

    std::size_t size() const noexcept override {
        auto lg = Detail::lock(l);
        return pool.getCount();
    }

    std::size_t reserve() const noexcept override {
        auto lg = Detail::lock(l);
        ETL_ASSERT(pool.capacity() >= pool.getCount());
        return pool.capacity() - pool.getCount();
    }

    PtrType allocate(std::size_t n) override {
        auto lg = Detail::lock(l);
        if (n == 1) {
            return static_cast<PtrType>(pool.pop());
        } else {
            return nullptr;
        }
    }

    void deallocate(PtrType ptr, std::size_t n) noexcept override {
        auto lg = Detail::lock(l);
        (void)n;
        pool.push(ptr);
    }

    const void* handle() const noexcept override {
        return &pool;
    }
};


/**
Allocator template class with static pool.
Forwards the `std::allocator` interface to a static MemoryPool.
This way each instance for the same template arguments use
a common pool.
\note This allocator supports allocating exactly _one element_ for one call.
\tparam T Item type
\tparam N Size of pool
\tparam L Lock type, defaults to std::mutex
*/
template<class T, uint32_t N, class L = std::mutex>
class CommonPoolAllocator : public AAllocator<T> {

    static_assert(N > 0, "Invalid pool size");

  public:  // types

    using Allocator = ETL_NAMESPACE::PoolAllocator<T, N, L>;
    using ItemType = typename Allocator::ItemType;
    using PtrType = typename Allocator::PtrType;
    using Lock = L;

    static constexpr bool uniqueAllocator = false;

  public:  // functions

    std::size_t max_size() const noexcept override {
        return allocator().max_size();
    }

    std::size_t size() const noexcept override {
        return allocator().size();
    }

    std::size_t reserve() const noexcept override {
        return allocator().reserve();
    }

    PtrType allocate(std::size_t n) override {
        return allocator().allocate(n);
    }

    void deallocate(PtrType ptr, std::size_t n) noexcept override {
        allocator().deallocate(ptr, n);
    }

    const void* handle() const noexcept override {
        return allocator().handle();
    }

  private:

    static Allocator& allocator() {
        static Allocator alloc;
        return alloc;
    }
};


template<uint32_t N>
class PoolHelperForSize {

  public:  // types

    template<class T>
    class Allocator : public PoolAllocator<T, N> {};

    template<class T, class L = std::mutex>
    class CommonAllocator : public CommonPoolAllocator<T, N, L> {};
};

}  // namespace ETL_NAMESPACE

#endif  // ETL_POOLALLOCATOR_H_
