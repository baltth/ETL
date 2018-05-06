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


#ifdef ETL_USE_CPP11

#if ((ETL_USE_CPP11 > 0) && (__cplusplus < 201103L))
#undef ETL_USE_CPP11
#define ETL_USE_CPP11   0
#warning "Incorrect compiler version for c++11"
#endif

#else

#if __cplusplus >= 201103L
#define ETL_USE_CPP11   1
#endif

#endif

#if (ETL_USE_CPP11 == 0)

// nullptr idiom from https://en.wikibooks.org/wiki/More_C++_Idioms/nullptr

const // It is a const object...
class NullptrT {
  public:
    template<class T>
    inline operator T* () const { // convertible to any type of null non-member pointer...
        return 0;
    }

    template<class C, class T>
    inline operator T C::* () const { // or any type of null member pointer...
        return 0;
    }

  private:
    void operator&() const;  // Can't take address of nullptr

} NULLPTR = {};


#define STATIC_ASSERT(x)        typedef char __STATIC_ASSERT__[(x) ? 1 : -1]
#define STATIC_ASSERT_(x, n)    typedef char __STATIC_ASSERT__ ## n[(x) ? 1 : -1]

#define OVERRIDE
#define FINAL
#define CONSTEXPR

namespace ETL_NAMESPACE {

template<class T>
struct RemoveReference {
    typedef T type;
};

template<class T>
struct RemoveReference<T&> {
    typedef T type;
};

}


#else /* ETL_USE_CPP11 == 1 */

#include <type_traits>

#define NULLPTR         nullptr
#define OVERRIDE        override
#define FINAL           final
#define CONSTEXPR       constexpr

#define STATIC_ASSERT(x)        static_assert((x), "Assertion failed")
#define STATIC_ASSERT_(x, n)    static_assert((x), "Assertion failed: ## n")

namespace ETL_NAMESPACE {

template<class T>
using RemoveReference = std::remove_reference<T>;

}

#endif

#endif /* __ETL_LANGSUPPORT_H__ */

