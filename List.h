/**
\file
\date 2015.04.28.
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

#include "etlSupport.h"

#include "Base/ListTemplate.h"

#include <memory>

namespace ETL_NAMESPACE {


template<class T, template<class> class A = std::allocator>
class List : public ListTemplate<T, A> {

  public:   // types

    typedef typename ListTemplate<T, A>::Iterator Iterator;
    typedef typename ListTemplate<T, A>::ConstIterator ConstIterator;
    typedef typename ListTemplate<T, A>::Node Node;

  public:   // functions

    List() {};

#if ETL_USE_CPP11

    List(const std::initializer_list<T>& initList) :
        ListTemplate<T>(initList) {};

#endif

};

}

#endif /* __ETL_LIST_H__ */

