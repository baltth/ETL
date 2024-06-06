/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2024 Balazs Toth.

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

#ifndef ETL_PROXY_H_
#define ETL_PROXY_H_

#include <etl/etlSupport.h>

#include <cstddef>

namespace ETL_NAMESPACE {
namespace Detail {


class Proxy {

  protected:  // variables

    void* data_;
    size_t size_;
    size_t itemSize;

  public:  // functions

    Proxy() noexcept :
        Proxy {0U, nullptr, 0U} {}

    Proxy(size_t is, void* d, size_t s) noexcept :
        data_ {d},
        size_ {s},
        itemSize {is} {}

    template<class C>
    Proxy(C& container) noexcept :
        data_ {container.data()},
        size_ {container.size()},
        itemSize {sizeof(typename C::value_type)} {}

    Proxy(const Proxy& other) = default;
    Proxy& operator=(const Proxy& other) = default;

    Proxy(Proxy&& other) noexcept :
        Proxy {} {
        this->operator=(std::move(other));
    }

    Proxy& operator=(Proxy&& other) noexcept {

        data_ = other.data_;
        size_ = other.size_;
        itemSize = other.size_;

        other.data_ = nullptr;
        other.size_ = 0U;
        other.itemSize = 0U;

        return *this;
    }

    ~Proxy() = default;

    void* getItemPointer(size_t i) noexcept {
        return static_cast<void*>(static_cast<uint8_t*>(data_) + (i * itemSize));
    }

    const void* getItemPointer(size_t i) const noexcept {
        return static_cast<void*>(static_cast<uint8_t*>(data_) + (i * itemSize));
    }

    void* data() noexcept {
        return data_;
    }

    const void* data() const noexcept {
        return data_;
    }

    size_t size() const noexcept {
        return size_;
    }

    size_t getItemSize() const noexcept {
        return itemSize;
    }
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_PROXY_H_
