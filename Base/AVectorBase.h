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

#include "Base/AArrayBase.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


class AVectorBase : protected AArrayBase {

// types
public:

    typedef AArrayBase::ItemType ItemType;

    static const uint32_t RESIZE_STEP        = 8;

// variables
protected:

    uint32_t numElements = 0;

// functions
public:

    uint32_t getSize() const {
        return numElements;
    }

    uint32_t getCapacity() const {
        return AArrayBase::getSize();
    }

    inline void* getItemPointer(uint32_t ix) const {
        return AArrayBase::getItemPointer(ix);
    }

    virtual void reserve(uint32_t length) = 0;
    virtual void shrinkToFit() = 0;
    virtual void resize(uint32_t newLength) = 0;
    virtual void clear() = 0;


protected:

    explicit AVectorBase(size_t itemSize) :
        AArrayBase(itemSize, nullptr, 0) {};

    ~AVectorBase() {
        deallocate();
    }

    void allocate(uint32_t len);

    void deallocate() {
        AVectorBase::deallocatePtr(data);
    }

    static void deallocatePtr(void* ptr);

    void swap(AVectorBase &other);

};

}

#endif /* __ETL_AVECTORBASE_H__ */
