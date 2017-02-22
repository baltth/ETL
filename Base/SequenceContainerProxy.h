/**
\file
\date 2017.02.15. 10:03:40
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

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

#ifndef __ETL_SEQUENCECONTAINERPROXY_H__
#define __ETL_SEQUENCECONTAINERPROXY_H__

#include "etlSupport.h"

#include <cstddef>


namespace ETL_NAMESPACE {


class SequenceContainerProxy {

// variables
protected:

    void* data;
    uint32_t capacity;
    const size_t itemSize;

// functions
public:

    SequenceContainerProxy(size_t itemSize, void* dataPointer, uint32_t len) :
        data(dataPointer),
        capacity(len),
        itemSize(itemSize) {};

    void* getItemPointer(uint32_t ix) const {
        return static_cast<void*>(static_cast<uint8_t*>(data) + (ix * itemSize));
    }

    void* getData() {
        return data;
    }

    const void* getData() const {
        return data;
    }

    uint32_t getCapacity() const {
        return capacity;
    }

    uint32_t getItemSize() const {
        return itemSize;
    }

};

}

#endif /* __ETL_SEQUENCECONTAINERPROXY_H__ */

