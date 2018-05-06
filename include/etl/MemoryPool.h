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

#ifndef __ETL_MEMORYPOOL_H__
#define __ETL_MEMORYPOOL_H__

#include <etl/etlSupport.h>
#include <etl/base/PoolBase.h>
#include <etl/Array.h>

namespace ETL_NAMESPACE {

/**
Memory pool template.
\tparam S Allocated block size
\tparam N Pool length
*/
template<size_t S, uint32_t N>
class MemoryPool {

    STATIC_ASSERT_(S > 0, size);
    STATIC_ASSERT_(N > 0, length);

  private:  // types

    static const size_t ITEMSIZE = (S > sizeof(PoolBase::FreeItem)) ? S : sizeof(PoolBase::FreeItem);

    typedef struct {
        uint8_t dummy[ITEMSIZE];
    } ItemAlias;

  private:  // variables

    Array<ItemAlias, N> data;
    PoolBase base;

  public:   // functions

    MemoryPool<S, N>() :
        data(),
        base(data) {};

    inline void* pop() {
        return base.pop();
    }

    inline bool push(void* item) {
        return base.push(item);
    }

    inline uint32_t getFreeCount() const {
        return base.getFreeCount();
    }

    inline uint32_t getCapacity() const {
        return N;
    }

    inline uint32_t getCount() const {
        return getCapacity() - getFreeCount();
    }

  private:

    // Non-copiable
    MemoryPool<S, N>(const MemoryPool<S, N>& other);
    MemoryPool<S, N>& operator=(const MemoryPool<S, N>& other);

};

}

#endif /* __ETL_MEMORYPOOL_H__ */

