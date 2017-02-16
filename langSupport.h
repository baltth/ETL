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

#ifndef __ETL_LANGSUPPORT_H__
#define __ETL_LANGSUPPORT_H__

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {

#ifdef ETL_USE_CPP11

#if ((ETL_USE_CPP11 > 0) && (__cplusplus < 201103L))
#undef ETL_USE_CPP11
#define ETL_USE_CPP11   0
#endif

#else

#if __cplusplus > 201103L
#define ETL_USE_CPP11   1
#endif

#endif

#if (ETL_USE_CPP11 == 0)

// nullptr idiom from https://en.wikibooks.org/wiki/More_C++_Idioms/nullptr

const // It is a const object...
class NullptrT 
{
  public:
    template<class T>
    inline operator T*() const // convertible to any type of null non-member pointer...
    { return 0; }

    template<class C, class T>
    inline operator T C::*() const   // or any type of null member pointer...
    { return 0; }

  private:
    void operator&() const;  // Can't take address of nullptr

} NULLPTR = {};

#define OVERRIDE
#define FINAL

#else

#define NULLPTR         nullptr
#define OVERRIDE        override
#define FINAL           final

#endif

}

#endif /* __ETL_LANGSUPPORT_H__ */

