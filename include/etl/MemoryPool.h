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

    static_assert(S > 0, "Invalid item size");
    static_assert(N > 0, "Invalid pool length");

  private:  // types

    typedef uint64_t MinItemType;

    union ItemAlias {
        MinItemType minItem;                            // for aliasing alignment and size of Minimal Item
        uint8_t item[S];                                // for aliasing size of S
        uint8_t freeItem[sizeof(PoolBase::FreeItem)];   // for aliasing size of PoolBase::FreeItem
    };

    static const size_t ITEMSIZE = sizeof(ItemAlias);

  private:  // variables

    Array<ItemAlias, N> data;
    PoolBase base;

  public:   // functions

    MemoryPool() :
        data(),
        base(data) {};

    MemoryPool(const MemoryPool& other) = delete;
    MemoryPool& operator=(const MemoryPool& other) = delete;
    MemoryPool(MemoryPool&& other) = delete;
    MemoryPool& operator=(MemoryPool&& other) = delete;
    ~MemoryPool() = default;

    void* pop() {
        return base.pop();
    }

    bool push(void* item) {
        return base.push(item);
    }

    uint32_t getFreeCount() const {
        return base.getFreeCount();
    }

    uint32_t capacity() const {
        return N;
    }

    uint32_t getCount() const {
        return capacity() - getFreeCount();
    }

};

}

#endif /* __ETL_MEMORYPOOL_H__ */

