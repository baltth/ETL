/**
\file
\date 2016.01.11. 22:18:19
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

#ifndef __ETL_AVECTORBASE_H__
#define __ETL_AVECTORBASE_H__

#include "langSupport.h"

#include "VectorProxy.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


class AVectorBase {

// variables
protected:

    VectorProxy proxy;
    uint32_t numElements;

// functions
public:

    uint32_t getSize() const {
        return numElements;
    }

    uint32_t getCapacity() const {
        return proxy.getCapacity();
    }

    void* getItemPointer(uint32_t ix) const {
        return proxy.getItemPointer(ix);
    }

protected:

    explicit AVectorBase(size_t itemSize) :
        proxy(itemSize, NULLPTR, 0),
        numElements(0) {};

    void setSize(uint32_t newSize) {
        numElements = newSize;
    }

    void swap(AVectorBase &other);

};

}

#endif /* __ETL_AVECTORBASE_H__ */
