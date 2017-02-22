/**
\file
\date 2017.02.15. 10:08:10
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

#ifndef __ETL_ARRAYPROXY_H__
#define __ETL_ARRAYPROXY_H__

#include "etlSupport.h"

#include "Base/SequenceContainerProxy.h"
#include "Array.h"


namespace ETL_NAMESPACE {


class ArrayProxy : public SequenceContainerProxy {

// functions
public:

    template<typename T, uint32_t N>
    ArrayProxy(Array<T, N>& array) :
        SequenceContainerProxy(sizeof(T), array.getData(), N) {};

protected:

    ArrayProxy(size_t itemSize, void* dataPointer, uint32_t len) :
        SequenceContainerProxy(itemSize, dataPointer, len) {};

};


template<typename T>
class TypedArrayProxy : public ArrayProxy {

// types
public:

    typedef T ItemType;

// functions
public:

    template<uint32_t N>
    TypedArrayProxy(Array<T, N>& array) :
        ArrayProxy(array) {};

    inline T &operator[](uint32_t ix) {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline const T &operator[](uint32_t ix) const {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    void fill(const T &value);

};


template<typename T>
void TypedArrayProxy<T>::fill(const T &value) {

    for(uint32_t i = 0; i < getCapacity(); ++i) {
        operator[](i) = value;
    }
}

}

#endif /* __ETL_ARRAYPROXY_H__ */

