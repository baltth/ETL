/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Balazs Toth.

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

#ifndef __ETL_PROXY_H__
#define __ETL_PROXY_H__

#include <etl/etlSupport.h>

#include <cstddef>

namespace ETL_NAMESPACE {


class GenericProxy {

  protected: // variables

    void* data;
    uint32_t capacity;
    uint32_t size;
    const size_t itemSize;

  public:   // functions

    GenericProxy(size_t itemSize, void* dataPointer, uint32_t cap, uint32_t len) :
        data(dataPointer),
        capacity(cap),
        size(len),
        itemSize(itemSize) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    GenericProxy(C& container) :
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
class Proxy {

  public:   // types

    typedef T ItemType;

  protected: // variables

    const ItemType* data;
    uint32_t capacity;
    uint32_t size;

  public:   // functions

    Proxy(const ItemType* dataPointer, uint32_t cap, uint32_t len) :
        data(dataPointer),
        capacity(cap),
        size(len) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    Proxy(const C& container) :
        data(container.getData()),
        capacity(container.getCapacity()),
        size(container.getSize()) {};

    const ItemType* getData() const {
        return data;
    }

    const ItemType* getItemPointer(uint32_t ix) const {
        return &(data[ix]);
    }

    inline const ItemType& operator[](uint32_t ix) const {
        return data[ix];
    }

    uint32_t getCapacity() const {
        return capacity;
    }

    uint32_t getSize() const {
        return size;
    }

};


template<typename T>
class MutableProxy : public GenericProxy {

  public:   // types

    typedef T ItemType;

  public:   // functions

    MutableProxy(T* data, uint32_t cap, uint32_t len) :
        GenericProxy(sizeof(T), data, cap, len) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    MutableProxy(C& container) :
        GenericProxy(container) {};

    ItemType* getData() const {
        return static_cast<T*>(data);
    }

    ItemType* getItemPointer(uint32_t ix) const {
        return const_cast<ItemType*>(static_cast<const ItemType*>(GenericProxy::getItemPointer(ix)));
    }

    inline T& operator[](uint32_t ix) const {
        return *(getItemPointer(ix));
    }

    void fill(const T& value) const;

};


template<typename T>
void MutableProxy<T>::fill(const T& value) const {

    for (uint32_t i = 0; i < getCapacity(); ++i) {
        operator[](i) = value;
    }
}

}

#endif /* __ETL_PROXY_H__ */

