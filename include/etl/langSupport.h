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

#ifndef ETL_LANGSUPPORT_H_
#define ETL_LANGSUPPORT_H_

#if (__cplusplus >= 201703L)
#define ETL_HAS_CPP17 1
#define ETL_HAS_CPP14 1
#elif (__cplusplus >= 201402L)
#define ETL_HAS_CPP17 0
#define ETL_HAS_CPP14 1
#elif (__cplusplus >= 201103L)
#define ETL_HAS_CPP17 0
#define ETL_HAS_CPP14 0
#else
#error "Deprecated"
#endif

#if (defined __GNUC__) && (__GNUC__ <= 5)
#define ETL_FULL_CONSTEXPR 0
#define ETL_COND_CONSTEXPR
#else
#define ETL_FULL_CONSTEXPR 1
#define ETL_COND_CONSTEXPR constexpr
#endif

#endif  // ETL_LANGSUPPORT_H_
