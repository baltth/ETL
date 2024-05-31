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

#ifndef ETL_POOLBASE_H_
#define ETL_POOLBASE_H_

#include <etl/base/Proxy.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {


class PoolBase {

  public:  // types

    struct FreeItem {
        FreeItem* next;
        FreeItem() :
            next(nullptr) {}
        explicit FreeItem(void* ptr) :
            next(static_cast<FreeItem*>(ptr)) {}
    };

  private:  // variables

    Detail::Proxy data;
    FreeItem freeList;

    std::size_t freeCnt;
    std::size_t nextFreeIx;

  public:  // functions

    explicit PoolBase(const Detail::Proxy& d) noexcept :
        data {d},
        freeCnt {data.size()},
        nextFreeIx {0} {}

    PoolBase() = delete;
    PoolBase(const PoolBase& other) = delete;
    PoolBase& operator=(const PoolBase& other) = delete;
    PoolBase(PoolBase&& other) = delete;
    PoolBase& operator=(PoolBase&& other) = delete;

    void* pop() noexcept;
    bool push(void* item) noexcept;

    std::size_t getFreeCount() const noexcept {
        return freeCnt;
    }
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_POOLBASE_H_
