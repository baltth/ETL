/**
\file
\date 2017.02.23.
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Tóth Balázs.

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

#include "etlSupport.h"

#include "MemoryPool.h"

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
    typedef T* PointerType;

  private:  // variables

    MemoryPool<sizeof(T), N> pool;

  public:   // functions

    inline PointerType allocate(uint32_t n) {
        return static_cast<PointerType>(pool.pop(n));
    }

    inline void deallocate(PointerType ptr, uint32_t n) {
        pool.push(ptr, n);
    }

};


template<uint32_t N>
class PoolHelper {

  public:   // types

    template<class T>
    class Allocator : public PoolAllocator<T, N> {};

};

}

#endif /* __ETL_POOLALLOCATOR_H__ */

