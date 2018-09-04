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


#if (ETL_HAS_CPP11 == 0)

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


// Type trait implementations mainly from https://en.cppreference.com

namespace std {

template<class T>
struct remove_const {
    typedef T type;
};

template<class T>
struct remove_const<const T> {
    typedef T type;
};

template<class T>
struct remove_volatile {
    typedef T type;
};

template<class T>
struct remove_volatile<volatile T> {
    typedef T type;
};

template<class T>
struct remove_cv {
    typedef typename remove_volatile<typename remove_const<T>::type>::type type;
};

template<class T>
struct remove_reference {
    typedef T type;
};

template<class T>
struct remove_reference<T&> {
    typedef T type;
};


template<class T, T v>
struct integral_constant {
    static const T value = v;
    typedef T value_type;
    typedef integral_constant type;
    operator value_type() const throw() {
        return value;
    }
};

struct true_type : integral_constant<bool, true> {};
struct false_type : integral_constant<bool, false> {};


template<class T, class U>
struct is_same : false_type {};

template<class T>
struct is_same<T, T> : true_type {};


template<typename T>
struct is_integral_root : false_type {};

template<>
struct is_integral_root<bool> : true_type {};
template<>
struct is_integral_root<signed char> : true_type {};
template<>
struct is_integral_root<unsigned char> : true_type {};
template<>
struct is_integral_root<wchar_t> : true_type {};
template<>
struct is_integral_root<short> : true_type {};
template<>
struct is_integral_root<unsigned short> : true_type {};
template<>
struct is_integral_root<int> : true_type {};
template<>
struct is_integral_root<unsigned int> : true_type {};
template<>
struct is_integral_root<long int> : true_type {};
template<>
struct is_integral_root<unsigned long int> : true_type {};

template<typename T>
struct is_integral : integral_constant<bool, (is_integral_root<typename remove_cv<T>::type>::value)> {};

template< class T >
struct is_floating_point : integral_constant <
    bool,
    is_same<float, typename remove_cv<T>::type>::value  ||
    is_same<double, typename remove_cv<T>::type>::value  ||
    is_same<long double, typename remove_cv<T>::type>::value
    > {};


template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> {
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

#endif


#ifndef M_PI
#define M_PI    (3.141592653589793238462643383279502884197169399375105820974)
#endif


#endif /* __ETL_LANGSUPPORT_H__ */

