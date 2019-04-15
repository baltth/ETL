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

#ifndef __ETL_LANGSUPPORT_H__
#define __ETL_LANGSUPPORT_H__

#if (__cplusplus < 201103L)
#define ETL_HAS_CPP11   0
#error "Deprecated"
#else
#define ETL_HAS_CPP11   1
#endif


#ifdef ETL_USE_CPP11

#if ((ETL_USE_CPP11 > 0) && (ETL_HAS_CPP11 == 0))
#undef ETL_USE_CPP11
#define ETL_USE_CPP11   0
#warning "C++11 features can not be used with the actual compiler settings"
#endif

#else

#if (ETL_HAS_CPP11)
#define ETL_USE_CPP11   1
#endif

#endif


#define STATIC_ASSERT(x)        static_assert((x), "Assertion failed")
#define STATIC_ASSERT_(x, n)    static_assert((x), "Assertion failed: ## n")


#endif /* __ETL_LANGSUPPORT_H__ */

