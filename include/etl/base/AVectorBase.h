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

#ifndef ETL_AVECTORBASE_H_
#define ETL_AVECTORBASE_H_

#include <etl/base/Proxy.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {


class AVectorBase {

  protected:  // types

    class Proxy : public Detail::Proxy {
        friend class AVectorBase;

      private:  // variables

        size_t capacity_ {0U};

      public:  // functions

        void setData(void* d) noexcept {
            data_ = d;
        }

        void setSize(size_t s) noexcept {
            size_ = s;
        }

        void setCapacity(size_t c) noexcept {
            capacity_ = c;
        }

        size_t capacity() const noexcept {
            return capacity_;
        }

      protected:

        explicit Proxy(size_t itemSize) noexcept :
            Detail::Proxy {itemSize, nullptr, 0U} {}
    };

  protected:  // variables

    AVectorBase::Proxy proxy;

  public:  // functions

    /// \name Capacity
    /// \{
    size_t capacity() const noexcept {
        return proxy.capacity();
    }

    size_t size() const noexcept {
        return proxy.size();
    }

    bool empty() const noexcept {
        return (proxy.size() == 0U);
    }
    /// \}

  protected:

    explicit AVectorBase(size_t itemSize) noexcept :
        proxy(itemSize) {}

    void* getItemPointer(size_t i) noexcept {
        return proxy.getItemPointer(i);
    }

    const void* getItemPointer(size_t i) const noexcept {
        return proxy.getItemPointer(i);
    }

    void swapProxy(AVectorBase& other) noexcept {

        ETL_ASSERT(proxy.getItemSize() == other.proxy.getItemSize());

        AVectorBase::Proxy tmp = proxy;
        proxy = other.proxy;
        other.proxy = tmp;
    }
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_AVECTORBASE_H_
