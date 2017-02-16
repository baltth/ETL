/**
\file
\date 2016.01.20. 19:44:12
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

#ifndef __ETL_ETLSUPPORT_H__
#define __ETL_ETLSUPPORT_H__

#include "langSupport.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {

#if (ETL_USE_CPP11 == 0)

template<typename T>
class Matcher {

// functions
public:

    virtual bool call(const T&) const = 0;

}

#endif

}

#endif /* __ETL_ETLSUPPORT_H__ */
