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

#ifndef ETL_TEST_DUMMYALLOCATOR_H_
#define ETL_TEST_DUMMYALLOCATOR_H_

#include <etl/etlSupport.h>

namespace Etl {
namespace Test {


template<class T>
class DummyAllocator {

  public:  // types

    using ItemType = T;

    static const size_t SIZE {64U};

  private:  // variables

    static uint8_t data[SIZE * sizeof(T)];
    static size_t cnt;
    static size_t deleteCnt;

  public:  // functions

    T* allocate(size_t n) {

        if (cnt + n > SIZE) {
            return nullptr;
        }

        auto* ptr = const_cast<T*>(ptrOfAllocation(cnt));
        cnt += n;

        return ptr;
    }

    void deallocate(T* ptr, size_t n) {
        (void)ptr;
        deleteCnt += n;
    }

    static const T* ptrOfAllocation(size_t n) {
        return reinterpret_cast<T*>(&data[n * sizeof(T)]);
    }

    static size_t getDeleteCount() {
        return deleteCnt;
    }

    static size_t getAllocCount() {
        return cnt;
    }

    static void reset() {
        cnt = 0;
        deleteCnt = 0;
    }

    static size_t max_size() {
        return SIZE;
    }
};


template<class T>
uint8_t DummyAllocator<T>::data[SIZE * sizeof(T)];

template<class T>
size_t DummyAllocator<T>::cnt = 0;

template<class T>
size_t DummyAllocator<T>::deleteCnt = 0;

}  // namespace Test
}  // namespace Etl

#endif  // ETL_TEST_DUMMYALLOCATOR_H_
