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

#ifndef __AFIFOINDEXING_H__
#define __AFIFOINDEXING_H__

#include <cstdint>


class FifoIndexing {

// variables
protected:

    uint32_t capcity;
    uint32_t numItems;

    uint32_t writeIx;
    uint32_t readIx;

// functions
public:

    explicit FifoIndexing(uint32_t fifoSize, uint32_t num = 0) :
        capcity(fifoSize),
        numItems(num),
        writeIx(0),
        readIx(0) {};

    uint32_t getIndexFromFront(uint32_t ix) const;
    uint32_t getIndexFromBack(uint32_t ix) const;

    uint32_t nextIndex(uint32_t ix) const;
    uint32_t previousIndex(uint32_t ix) const;

    void resetIndexes();

    void setCapacity(uint32_t fifoSize) {
        capcity = fifoSize;
    }

    uint32_t getCapacity() const {
        return capcity;
    }

    uint32_t getNumItems() const {
        return numItems;
    }

    void push();
    void pop();

    uint32_t getWriteIx() const {
        return writeIx;
    }

    uint32_t getReadIx() const {
        return readIx;
    }

protected:

    uint32_t limitIndexForNumItems(uint32_t ix) const;

};

#endif /* __AFIFOINDEXING_H__ */
