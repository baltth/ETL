/**
\file
\date 2017.02.15.
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

#ifndef __ETL_CONTAINERPROXY_H__
#define __ETL_CONTAINERPROXY_H__

#include "etlSupport.h"

#include <cstddef>


namespace ETL_NAMESPACE {


class ContainerProxy {

  protected: // variables

    void* data;
    uint32_t capacity;
    uint32_t size;
    const size_t itemSize;

  public:   // functions

    ContainerProxy(size_t itemSize, void* dataPointer, uint32_t cap, uint32_t len) :
        data(dataPointer),
        capacity(cap),
        size(len),
        itemSize(itemSize) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    ContainerProxy(C& container) :
        data(container.getData()),
        capacity(container.getCapacity()),
        size(container.getSize()),
        itemSize(sizeof(typename C::ItemType)) {};

    void* getItemPointer(uint32_t ix) {
        return static_cast<void*>(static_cast<uint8_t*>(data) + (ix * itemSize));
    }

    const void* getItemPointer(uint32_t ix) const {
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

    uint32_t getSize() const {
        return size;
    }

    uint32_t getItemSize() const {
        return itemSize;
    }

};


template<typename T>
class TypedContainerProxy : public ContainerProxy {

  public:   // types

    typedef T ItemType;

  public:   // functions

    TypedContainerProxy(T* data, uint32_t cap, uint32_t len) :
        ContainerProxy(sizeof(T), data, cap, len) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    TypedContainerProxy(C& container) :
        ContainerProxy(container) {};

    inline T& operator[](uint32_t ix) {
        return *(static_cast<ItemType*>(getItemPointer(ix)));
    }

    inline const T& operator[](uint32_t ix) const {
        return *(static_cast<const ItemType*>(getItemPointer(ix)));
    }

    void fill(const T& value);

};


template<typename T>
void TypedContainerProxy<T>::fill(const T& value) {

    for (uint32_t i = 0; i < getCapacity(); ++i) {
        operator[](i) = value;
    }
}

}

#endif /* __ETL_CONTAINERPROXY_H__ */

