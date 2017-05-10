/**
\file
\date 2016.01.02. 18:57:21
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

#ifndef __ETL_AFIFOINDEXING_H__
#define __ETL_AFIFOINDEXING_H__

#include "etlSupport.h"

namespace ETL_NAMESPACE {

class AFifoIterator;


class FifoIndexing {
    friend AFifoIterator;

  private:  // variables

    uint32_t capacity;
    uint32_t length;

    uint32_t writeIx;
    uint32_t readIx;

  public:   // functions

    bool isEmpty() const {
        return (length == 0);
    }

    bool isFull() const {
        return (length == capacity);
    }

    uint32_t getLength() const {
        return length;
    }

    void setEmpty() {
        setLength(0);
    }

    void setLength(uint32_t len);

  protected:

    explicit FifoIndexing(uint32_t fifoSize) :
        capacity(fifoSize) {

        resetIndexes();
    };

    uint32_t getIndexFromFront(uint32_t ix) const;
    uint32_t getIndexFromBack(uint32_t ix) const;

    uint32_t nextIndex(uint32_t ix) const;
    uint32_t previousIndex(uint32_t ix) const;

    void resetIndexes();

    void setCapacity(uint32_t fifoSize) {
        capacity = fifoSize;
    }

    uint32_t getCapacity() const {
        return capacity;
    }

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

}

#endif /* __ETL_AFIFOINDEXING_H__ */

