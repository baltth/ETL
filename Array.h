/**
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

#include <initializer_list>

#include "ETL/Base/TypedArrayBase.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<typename T, uint32_t N>
class Array : public TypedArrayBase<T> {

// types
public:

    typedef typename TypedArrayBase<T>::Iterator Iterator;
    typedef T ItemType;

// variables
private:

    T data[N];

// functions
public:

    Array<T, N>() :
        TypedArrayBase<T>(data, N) {};

    Array<T, N>(const Array<T, N> &other) :
        Array<T, N>() {

        operator=(other);
    }

    Array<T, N>(const std::initializer_list<T> &initList):
        Array<T, N>() {
        operator=(initList);
    }

    Array<T, N> &operator=(const Array<T, N> &other);
    Array<T, N> &operator=(const std::initializer_list<T> &initList);

};


template<typename T, uint32_t N>
Array<T, N> &Array<T, N>::operator=(const Array<T, N> &other) {

    uint32_t i = 0;

    for(auto &item : other) {
        TypedArrayBase<T>::operator[](i++) = item;
    }

    return *this;
}


template<typename T, uint32_t N>
Array<T, N> &Array<T, N>::operator=(const std::initializer_list<T> &initList) {

    //static_assert(N == initList.size(), "Invalid initializer list size");
    ///\todo

    uint32_t i = 0;

    for(auto &item : initList) {
        TypedArrayBase<T>::operator[](i++) = item;
    }

    return *this;
}

}

#endif /* __ETL_ARRAY_H__ */
