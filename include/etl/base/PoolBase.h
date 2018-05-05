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

#ifndef __ETL_POOLBASE_H__
#define __ETL_POOLBASE_H__

#include <etl/etlSupport.h>
#include <etl/Proxy.h>

namespace ETL_NAMESPACE {


class PoolBase {

  public:   // types

    struct FreeItem {
        FreeItem* next;
        FreeItem() :
            next(NULLPTR) {};
        explicit FreeItem(void* ptr) :
            next(static_cast<FreeItem*>(ptr)) {};
    };

  private:  // variables

    GenericProxy data;
    FreeItem freeList;

    uint32_t freeCnt;
    uint32_t nextFreeIx;

  public:   // functions

    explicit PoolBase(const GenericProxy& d) :
        data(d),
        freeCnt(data.getCapacity()),
        nextFreeIx(0) {};

    void* pop();
    bool push(void* item);

    uint32_t getFreeCount() const {
        return freeCnt;
    }

  private:

    // Non-copiable
    PoolBase(const PoolBase& other);
    PoolBase& operator=(const PoolBase& other);

};

}

#endif /* __ETL_POOLBASE_H__ */

