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

    void* data_;
    uint32_t size_;
    const size_t itemSize;

  public:   // functions

    GenericProxy(size_t itemSize, void* dataPointer, uint32_t s) :
        data_(dataPointer),
        size_(s),
        itemSize(itemSize) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    GenericProxy(C& container) :
        data_(container.data()),
        size_(container.size()),
        itemSize(sizeof(typename C::value_type)) {};

    void* getItemPointer(uint32_t ix) {
        return static_cast<void*>(static_cast<uint8_t*>(data_) + (ix * itemSize));
    }

    const void* getItemPointer(uint32_t ix) const {
        return static_cast<void*>(static_cast<uint8_t*>(data_) + (ix * itemSize));
    }

    void* data() {
        return data_;
    }

    const void* data() const {
        return data_;
    }

    uint32_t size() const {
        return size_;
    }

    uint32_t getItemSize() const {
        return itemSize;
    }

};


template<typename T>
class Proxy {

  public:   // types

    typedef T value_type;

  protected: // variables

    const value_type* data_;
    uint32_t size_;

  public:   // functions

    Proxy(const value_type* dataPointer, uint32_t s) :
        data_(dataPointer),
        size_(s) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    Proxy(const C& container) :
        data_(container.data()),
        size_(container.size()) {};

    const value_type* data() const {
        return data_;
    }

    const value_type* getItemPointer(uint32_t ix) const {
        return &(data_[ix]);
    }

    inline const value_type& operator[](uint32_t ix) const {
        return data_[ix];
    }

    uint32_t size() const {
        return size_;
    }

};


template<typename T>
class MutableProxy : public GenericProxy {

  public:   // types

    typedef T value_type;

  public:   // functions

    MutableProxy(T* data_, uint32_t c, uint32_t s) :
        GenericProxy(sizeof(T), data_, c, s) {};

    template<class C>   // cppcheck-suppress noExplicitConstructor
    MutableProxy(C& container) :
        GenericProxy(container) {};

    value_type* data() const {
        return static_cast<T*>(data_);
    }

    value_type* getItemPointer(uint32_t ix) const {
        return const_cast<value_type*>(static_cast<const value_type*>(GenericProxy::getItemPointer(ix)));
    }

    inline T& operator[](uint32_t ix) const {
        return *(getItemPointer(ix));
    }

    void fill(const T& value) const;

};


template<typename T>
void MutableProxy<T>::fill(const T& value) const {

    for (uint32_t i = 0; i < size(); ++i) {
        operator[](i) = value;
    }
}

}

#endif /* __ETL_PROXY_H__ */

