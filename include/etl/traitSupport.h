/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2018 Balazs Toth.

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

#ifndef __ETL_TRAITSUPPORT_H__
#define __ETL_TRAITSUPPORT_H__

#include <etl/langSupport.h>


#if (ETL_HAS_CPP11 == 0)


// Type trait implementations mainly from https://en.cppreference.com

namespace std {


template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> {
    typedef T type;
};


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


namespace etl_detail {

template<typename T>
struct is_integral_root : std::false_type {};

template<>
struct is_integral_root<bool> : std::true_type {};
template<>
struct is_integral_root<signed char> : std::true_type {};
template<>
struct is_integral_root<unsigned char> : std::true_type {};
template<>
struct is_integral_root<wchar_t> : std::true_type {};
template<>
struct is_integral_root<short> : std::true_type {};
template<>
struct is_integral_root<unsigned short> : std::true_type {};
template<>
struct is_integral_root<int> : std::true_type {};
template<>
struct is_integral_root<unsigned int> : std::true_type {};
template<>
struct is_integral_root<long int> : std::true_type {};
template<>
struct is_integral_root<unsigned long int> : std::true_type {};

}


template<typename T>
struct is_integral : integral_constant<bool, etl_detail::is_integral_root<typename remove_cv<T>::type>::value> {};

template< class T >
struct is_floating_point : integral_constant <
    bool,
    is_same<float, typename remove_cv<T>::type>::value  ||
    is_same<double, typename remove_cv<T>::type>::value  ||
    is_same<long double, typename remove_cv<T>::type>::value
    > {};

template< class T >
struct is_arithmetic : integral_constant < bool,
    is_integral<T>::value ||
    is_floating_point<T>::value
    > {};


namespace etl_detail {

template<typename T, bool = std::is_integral<T>::value>
struct is_unsigned : std::integral_constant<bool, (T(0) < T(-1))> {};   // *NOPAD*

template<typename T>
struct is_unsigned<T, false> : std::false_type {};


template<typename T, bool = std::is_floating_point<T>::value ||         // *NOPAD*
         (std::is_integral<T>::value && (!is_unsigned<T>::value)) >
struct is_signed : std::true_type {};

template<typename T>
struct is_signed<T, false> : std::false_type {};

}

template<typename T>
struct is_unsigned : etl_detail::is_unsigned<T>::type {};

template<typename T>
struct is_signed : etl_detail::is_signed<T>::type {};

}


#else /* ETL_USE_CPP11 == 1 */

#include <type_traits>

#endif


#endif /* __ETL_TRAITSUPPORT_H__ */

