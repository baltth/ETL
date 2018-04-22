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

#ifndef __ETL_ARRAY_H__
#define __ETL_ARRAY_H__

#include <etl/etlSupport.h>

#if ETL_USE_EXCEPTIONS
#include <etl/ExceptionTypes.h>
#endif

namespace ETL_NAMESPACE {


template<typename T, uint32_t N>
class Array {

    STATIC_ASSERT(N > 0);

  public:   // types

    typedef T ItemType;
    typedef T* Iterator;
    typedef const T* ConstIterator;

  private:  // variables

    T data[N];

  public:   // functions

    Array<T, N>() {};

    Array<T, N>(const Array<T, N>& other) {
        operator=(other);
    }

    Array<T, N>& operator=(const Array<T, N>& other);

    /// \name Element access
    /// \{
    T& operator[](uint32_t ix) {
        return data[ix];
    }

    const T& operator[](uint32_t ix) const {
        return data[ix];
    }

    T* getData() {
        return data;
    }

    const T* getData() const {
        return data;
    }

#if ETL_USE_EXCEPTIONS
    inline T& at(uint32_t ix);
    inline const T& at(uint32_t ix) const;
#endif
    /// \}

    /// \name Iterators
    /// \{
    Iterator begin() {
        return static_cast<Iterator>(data);
    }

    ConstIterator begin() const {
        return static_cast<ConstIterator>(data);
    }

    ConstIterator cbegin() const {
        return begin();
    }

    Iterator end() {
        return static_cast<Iterator>(&data[N]);
    }

    ConstIterator end() const {
        return static_cast<ConstIterator>(&data[N]);
    }

    ConstIterator cend() const {
        return end();
    }
    /// \}

    /// \name Capacity
    /// \{
    size_t getSize() const {
        return N;
    }

    size_t getCapacity() const {
        return getSize();
    }
    /// \}

    /// \name stl compatible interface
    /// \{
    bool empty() const {
        return N == 0;
    }

    size_t size() const {
        return getSize();
    }
    /// \}

    void fill(const T& value);

};


template<typename T, uint32_t N>
Array<T, N>& Array<T, N>::operator=(const Array<T, N>& other) {

    for (uint32_t i = 0; i < N; ++i) {
        operator[](i) = other.operator[](i);
    }

    return *this;
}


template<typename T, uint32_t N>
void Array<T, N>::fill(const T& value) {

    for (uint32_t i = 0; i < N; ++i) {
        operator[](i) = value;
    }
}


#if ETL_USE_EXCEPTIONS

template<typename T, uint32_t N>
T& Array<T, N>::at(uint32_t ix) {

    if (ix >= N) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}


template<typename T, uint32_t N>
const T& Array<T, N>::at(uint32_t ix) const {

    if (ix >= N) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}

#endif

}

#endif /* __ETL_ARRAY_H__ */

