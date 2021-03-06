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

#include <etl/base/FifoIndexing.h>

using ETL_NAMESPACE::Detail::FifoIndexing;


void FifoIndexing::resetIndexes() {

    writeIx = capacity() - 1;
    readIx = writeIx;
    length_ = 0;
}


uint32_t FifoIndexing::getIndexFromFront(uint32_t ix) const {

    uint32_t bufferIx = nextIndex(readIx) + limitIndexForLength(ix);

    if (bufferIx >= capacity()) {
        bufferIx -= capacity();
    }

    return bufferIx;
}


uint32_t FifoIndexing::getIndexFromBack(uint32_t ix) const {

    int32_t bufferIx = writeIx - limitIndexForLength(ix);

    if (bufferIx < 0) {
        bufferIx += capacity();
    }

    return bufferIx;
}


uint32_t FifoIndexing::limitIndexForLength(uint32_t ix) const {

    if (ix >= length_) {
        if (length_ == 0) {
            ix = 0;
        } else {
            ix = length_ - 1;
        }
    }

    return ix;
}


uint32_t FifoIndexing::nextIndex(uint32_t ix) const {

    ++ix;

    if (ix >= capacity()) {
        ix = 0;
    }

    return ix;
}


uint32_t FifoIndexing::previousIndex(uint32_t ix) const {

    if ((ix == 0) && (capacity() > 0)) {
        ix = capacity() - 1;
    } else {
        --ix;
    }

    return ix;
}


void FifoIndexing::push() {

    uint32_t prevWriteIx = writeIx;
    writeIx = nextIndex(writeIx);

    if ((prevWriteIx == readIx) && (length_ > 0)) {
        readIx = writeIx;
        length_ = capacity();
    } else {
        ++length_;
    }
}


void FifoIndexing::pop() {

    if (length_ > 0) {
        readIx = nextIndex(readIx);
        --length_;
    }
}


void FifoIndexing::setSize(uint32_t len) {

    if (len >= capacity()) {
        len = capacity() - 1;
    }

    if (len > writeIx) {
        readIx = writeIx + capacity();
    } else {
        readIx = writeIx;
    }

    readIx -= len;
    length_ = len;
}
