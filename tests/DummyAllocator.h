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

#ifndef __ETL_TEST_DUMMYALLOCATOR_H__
#define __ETL_TEST_DUMMYALLOCATOR_H__

namespace ETL_NAMESPACE {
namespace Test {


template<class T>
class DummyAllocator {

  public:  // types

    typedef T ItemType;
    static const uint32_t SIZE = 64;

  private:  // variables

    static uint8_t data[SIZE * sizeof(T)];
    static uint32_t cnt;
    static uint32_t deleteCnt;

  public:  // functions

    T* allocate(size_t n) {

        if (cnt + n > SIZE) {
            return NULL;
        }

        T* ptr = const_cast<T*>(ptrOfAllocation(cnt));
        cnt += n;

        return ptr;
    }

    void deallocate(T* ptr, uint32_t n) {
        (void)ptr;
        deleteCnt += n;
    }

    static const T* ptrOfAllocation(uint32_t n) {
        return reinterpret_cast<T*>(&data[n * sizeof(T)]);
    }

    static uint32_t getDeleteCount() {
        return deleteCnt;
    }

    static uint32_t getAllocCount() {
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
uint32_t DummyAllocator<T>::cnt = 0;

template<class T>
uint32_t DummyAllocator<T>::deleteCnt = 0;

}  // namespace Test
}  // namespace ETL_NAMESPACE

#endif  // __ETL_TEST_DUMMYALLOCATOR_H__
