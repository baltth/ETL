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

#include <ETL/base/PoolBase.h>

using ETL_NAMESPACE::PoolBase;
using ETL_NAMESPACE::GenericProxy;


PoolBase::PoolBase(const GenericProxy& d, const Registry& reg) :
    data(d),
    registry(reg),
    freeCnt(reg.getCapacity()),
    firstFreeIx(0) {

    registry.fill(0);
}


void* PoolBase::pop(uint32_t n /* = 1 */) {

    void* result = NULL;

    if ((firstFreeIx >= registry.getCapacity()) || (getFreeCount() == 0)) {
        return NULL;        // Bad alloc
    }

    if (n != 1) {
        return NULL;        // Invalid arg
    }

    --freeCnt;
    result = data.getItemPointer(firstFreeIx);
    registry[firstFreeIx] = 1;

    firstFreeIx = searchFreeFrom(firstFreeIx + 1);

    return result;
}


bool PoolBase::push(void* item, uint32_t n /* = 1 */) {

    const uint8_t* const itemEnd = static_cast<uint8_t*>(item) + (n * data.getItemSize());
    const uint8_t* const regionStart = static_cast<uint8_t*>(data.getItemPointer(0));
    const uint8_t* const regionEnd = static_cast<uint8_t*>(data.getItemPointer(registry.getCapacity()));

    uint32_t ix = 0;

    if ((item < regionStart) || (itemEnd > regionEnd)) {
        return false;
    }

    if (n != 1) {
        return false;
    }

    ix = static_cast<uint32_t>(static_cast<uint8_t*>(item) - regionStart) / data.getItemSize();

    if (firstFreeIx > ix) {
        firstFreeIx = ix;
    }

    registry[ix] = 0;
    ++freeCnt;

    return true;
}


uint32_t PoolBase::searchFreeFrom(uint32_t ix) const {

    uint32_t nextFree = (ix > registry.getCapacity()) ? registry.getCapacity() : ix;
    bool freeFound = false;

    while ((!freeFound) && (nextFree < registry.getCapacity())) {
        if (registry[nextFree] == 0) {
            freeFound = true;
        } else {
            nextFree++;
        }
    }

    return nextFree;
}

