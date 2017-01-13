/**
\file
\date 2016.01.12. 15:48:19
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

#include "AVectorBase.h"


void AVectorBase::allocate(uint32_t len) {

    if(len > 0) {
        data = operator new(len * itemSize);
    } else {
        data = nullptr;
    }

    if(data != nullptr) {
        capacity = len;
    } else {
        capacity = 0;
    }
}


void AVectorBase::deallocatePtr(void* ptr) {

    operator delete(ptr);
}


void AVectorBase::swap(AVectorBase &other) {

    void* tmpData = data;
    uint32_t tmpNumElements = numElements;
    uint32_t tmpCapacity = capacity;

    data = other.data;
    numElements = other.numElements;
    capacity = other.capacity;

    other.data = tmpData;
    other.numElements = tmpNumElements;
    other.capacity = tmpCapacity;
}

