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


class AVectorBase {

  protected: // types

    class Proxy : public GenericProxy {
        friend class AVectorBase;

      public:   // functions

        void setData(void* d) {
            data_ = d;
        }

        void setCapacity(uint32_t c) {
            capacity_ = c;
        }

        void setSize(uint32_t s) {
            size_ = s;
        }

      protected:

        explicit Proxy(size_t itemSize) :
            GenericProxy(itemSize, NULLPTR, 0, 0) {};

    };

  protected: // variables

    Proxy proxy;

  public:   // functions

    /// \name Capacity
    /// \{
    uint32_t capacity() const {
        return proxy.capacity();
    }

    uint32_t size() const {
        return proxy.size();
    }

    bool empty() const {
        return (proxy.size() == 0);
    }
    /// \}

    void* getItemPointer(uint32_t ix) {
        return proxy.getItemPointer(ix);
    }

    const void* getItemPointer(uint32_t ix) const {
        return proxy.getItemPointer(ix);
    }

  protected:

    explicit AVectorBase(size_t itemSize) :
        proxy(itemSize) {};

    void swapProxy(AVectorBase& other);

};

}

#endif /* __ETL_AVECTORBASE_H__ */

