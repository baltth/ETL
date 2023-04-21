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

#include <etl/base/PoolBase.h>

using ETL_NAMESPACE::Detail::PoolBase;


void* PoolBase::pop() {

    void* result = NULL;

    if (freeList.next != nullptr) {

        result = freeList.next;
        freeList.next = freeList.next->next;
        --freeCnt;

    } else if ((nextFreeIx < data.size()) && (getFreeCount() > 0)) {

        result = data.getItemPointer(nextFreeIx);
        ++nextFreeIx;
        --freeCnt;

    } else {
        // NOP
    }

    return result;
}


bool PoolBase::push(void* item) {

    const uint8_t* const itemEnd = static_cast<uint8_t*>(item) + data.getItemSize();
    const uint8_t* const regionStart = static_cast<uint8_t*>(data.getItemPointer(0));
    const uint8_t* const regionEnd = static_cast<uint8_t*>(data.getItemPointer(data.size()));

    if ((item < regionStart) || (itemEnd > regionEnd)) {
        return false;
    } else {

        FreeItem* alias = static_cast<FreeItem*>(item);
        alias->next = freeList.next;
        freeList.next = alias;
        ++freeCnt;

        return true;
    }
}
