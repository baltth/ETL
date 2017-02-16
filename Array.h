﻿/**
\file
\date 2015. 07. 23. 12:11:20
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

#ifndef __ETL_ARRAY_H__
#define __ETL_ARRAY_H__

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

#ifdef ETL_USE_EXCEPTIONS
#include <Exception.h>
#endif

namespace ETL_NAMESPACE {


template<typename T, uint32_t N>
class Array {

// types
public:

    typedef T ItemType;
    typedef T* Iterator;

// variables
private:

    T data[N];

// functions
public:

    Array<T, N>() {};

    Array<T, N>(const Array<T, N> &other) {
        operator=(other);
    }

    Array<T, N> &operator=(const Array<T, N> &other);

    inline T &operator[](uint32_t ix) {
        return data[ix];
    }

    inline const T &operator[](uint32_t ix) const {
        return data[ix];
    }

    inline Iterator begin() {
        return static_cast<Iterator>(data);
    }

    inline const Iterator begin() const {
        return static_cast<const Iterator>(data);
    }

    inline Iterator end() {
        return static_cast<Iterator>(&data[N]);
    }

    inline const Iterator end() const {
        return static_cast<const Iterator>(&data[N]);
    }

    inline T* getData() {
        return data;
    }

    inline const T* getData() const {
        return data;
    }

    inline uint32_t getSize() const {
        return N;
    }

    void fill(const T &value);

#ifdef ETL_USE_EXCEPTIONS

    T& at(uint32_t ix);
    const T& at(uint32_t ix) const;

#endif

};


template<typename T, uint32_t N>
Array<T, N> &Array<T, N>::operator=(const Array<T, N> &other) {

    for(uint32_t i = 0; i < N; ++i) {
        operator[](i) = other.operator[](i);
    }

    return *this;
}


template<typename T, uint32_t N>
void Array<T, N>::fill(const T &value) {

    for(uint32_t i = 0; i < N; ++i) {
        operator[](i) = value;
    }
}


#ifdef ETL_USE_EXCEPTIONS

template<typename T, uint32_t N>
T& Array<T, N>::at(uint32_t ix) {

    if(ix >= N) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}


template<typename T, uint32_t N>
const T& Array<T, N>::at(uint32_t ix) const {

    if(ix >= N) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}

#endif

}

#endif /* __ETL_ARRAY_H__ */
