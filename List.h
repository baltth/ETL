/**
\file
\date 2015. 04. 28. 9:34:04
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

#ifndef __ETL_LIST_H__
#define __ETL_LIST_H__

#include "Base/ListTemplate.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<class T>
class List : public ListTemplate<T> {

// types
public:

    typedef typename ListTemplate<T>::Iterator Iterator;

// functions
public:

    List() = default;
    List(const std::initializer_list<T> &initList) :
        ListTemplate<T>(initList) {};

    Iterator insert(const T &item);

};

}

#endif /* __ETL_LIST_H__ */

