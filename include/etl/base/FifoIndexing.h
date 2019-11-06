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

#ifndef __ETL_FIFOINDEXING_H__
#define __ETL_FIFOINDEXING_H__

#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {

class AFifoIterator;


class FifoIndexing {

  private:  // variables

    uint32_t capacity_;
    uint32_t length_;

    uint32_t writeIx;
    uint32_t readIx;

  public:  // functions

    explicit FifoIndexing(uint32_t fifoSize) :
        capacity_(fifoSize) {

        resetIndexes();
    };

    bool empty() const {
        return (length_ == 0);
    }

    uint32_t size() const {
        return length_;
    }

    uint32_t capacity() const {
        return capacity_;
    }

    void clear() {
        setSize(0U);
    }

    void setSize(uint32_t len);

    void setCapacity(uint32_t c) {
        capacity_ = c;
    }

    uint32_t getIndexFromFront(uint32_t ix) const;
    uint32_t getIndexFromBack(uint32_t ix) const;

    uint32_t nextIndex(uint32_t ix) const;
    uint32_t previousIndex(uint32_t ix) const;

    void resetIndexes();

    void push();
    void pop();

    uint32_t getWriteIx() const {
        return writeIx;
    }

    uint32_t getReadIx() const {
        return readIx;
    }

  private:

    uint32_t limitIndexForLength(uint32_t ix) const;
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_FIFOINDEXING_H__
