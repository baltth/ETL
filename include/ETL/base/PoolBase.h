/**
\file
\date 2017.05.07.
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

#ifndef __ETL_POOLBASE_H__
#define __ETL_POOLBASE_H__

#include "ETL/etlSupport.h"

#include "ETL/Proxy.h"

namespace ETL_NAMESPACE {


class PoolBase {

  public:   // types

    typedef MutableProxy<uint8_t> Registry;

  private:  // variables

    GenericProxy data;
    Registry registry;

    uint32_t freeCnt;
    uint32_t firstFreeIx;

  public:   // functions

    PoolBase(const GenericProxy& d, const Registry& reg);

    void* pop(uint32_t n = 1);
    bool push(void* item, uint32_t n = 1);

    uint32_t getFirstFree() const {
        return firstFreeIx;
    }

    uint32_t getFreeCount() const {
        return freeCnt;
    }

    uint32_t getSize() const {
        return registry.getCapacity();
    }

  private:

    uint32_t searchFreeFrom(uint32_t ix) const;

    // Non-copiable
    PoolBase(const PoolBase& other);
    PoolBase& operator=(const PoolBase& other);

};

}

#endif /* __ETL_POOLBASE_H__ */

