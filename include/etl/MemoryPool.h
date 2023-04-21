/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017-2023 Balazs Toth.

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

#include <etl/Array.h>
#include <etl/base/PoolBase.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {

/**
Memory pool template.
\tparam S Allocated block size
\tparam N Pool length
*/
template<std::size_t S, std::size_t N>
class MemoryPool {

    static_assert(S > 0, "Invalid item size");
    static_assert(N > 0, "Invalid pool length");

  private:  // types

    using MinItemType = std::uint64_t;

    union ItemAlias {
        MinItemType minItem;   // for aliasing alignment and size of Minimal Item
        std::uint8_t item[S];  // for aliasing size of S
        std::uint8_t freeItem[sizeof(
            Detail::PoolBase::FreeItem)];  // for aliasing size of PoolBase::FreeItem
    };

    static const std::size_t ITEM_SIZE {sizeof(ItemAlias)};

  private:  // variables

    Array<ItemAlias, N> data {};
    Detail::PoolBase base {data};

  public:  // functions

    MemoryPool() = default;
    MemoryPool(const MemoryPool& other) = delete;
    MemoryPool& operator=(const MemoryPool& other) = delete;
    MemoryPool(MemoryPool&& other) = delete;
    MemoryPool& operator=(MemoryPool&& other) = delete;
    ~MemoryPool() = default;

    void* pop() noexcept {
        return base.pop();
    }

    bool push(void* item) noexcept {
        return base.push(item);
    }

    std::size_t getFreeCount() const noexcept {
        return base.getFreeCount();
    }

    std::size_t capacity() const noexcept {
        return N;
    }

    std::size_t getCount() const noexcept {
        return capacity() - getFreeCount();
    }
};

}  // namespace ETL_NAMESPACE

#endif  // __ETL_MEMORYPOOL_H__
