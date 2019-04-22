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

#ifndef __ETL_AVECTORBASE_H__
#define __ETL_AVECTORBASE_H__

#include <etl/etlSupport.h>
#include <etl/Proxy.h>

namespace ETL_NAMESPACE {
namespace Detail {


class AVectorBase {

  protected: // types

    class Proxy : public GenericProxy {
        friend class AVectorBase;

      protected: // variables

        uint32_t capacity_;

      public:   // functions

        void setData(void* d) noexcept {
            data_ = d;
        }

        void setSize(uint32_t s) noexcept {
            size_ = s;
        }

        void setCapacity(uint32_t c) noexcept {
            capacity_ = c;
        }

        uint32_t capacity() const noexcept {
            return capacity_;
        }

      protected:

        explicit Proxy(size_t itemSize) noexcept :
            GenericProxy(itemSize, nullptr, 0U),
            capacity_(0U) {};

    };

  protected: // variables

    Proxy proxy;

  public:   // functions

    /// \name Capacity
    /// \{
    uint32_t capacity() const noexcept {
        return proxy.capacity();
    }

    uint32_t size() const noexcept {
        return proxy.size();
    }

    bool empty() const noexcept {
        return (proxy.size() == 0U);
    }
    /// \}

    /// \name Access
    /// \{
    void* getItemPointer(uint32_t ix) noexcept {
        return proxy.getItemPointer(ix);
    }

    const void* getItemPointer(uint32_t ix) const noexcept {
        return proxy.getItemPointer(ix);
    }
    /// \}

  protected:

    explicit AVectorBase(size_t itemSize) noexcept :
        proxy(itemSize) {};

    void swapProxy(AVectorBase& other) noexcept {

        ETL_ASSERT(itemSize == other.itemSize);

        AVectorBase::Proxy tmp = proxy;
        proxy = other.proxy;
        other.proxy = tmp;
    }

};

}
}

#endif /* __ETL_AVECTORBASE_H__ */

