/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2018-2024 Balazs Toth.

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

#ifndef ETL_TRAITSUPPORT_H_
#define ETL_TRAITSUPPORT_H_

#include <etl/etlSupport.h>

#include <type_traits>

namespace ETL_NAMESPACE {

// helper class:
using std::integral_constant;
using std::true_type;
using std::false_type;

// primary type categories:
using std::is_void;
using std::is_integral;
using std::is_floating_point;
using std::is_array;
using std::is_pointer;
using std::is_lvalue_reference;
using std::is_rvalue_reference;
using std::is_member_object_pointer;
using std::is_member_function_pointer;
using std::is_enum;
using std::is_union;
using std::is_class;
using std::is_function;

// composite type categories:
using std::is_reference;
using std::is_arithmetic;
using std::is_fundamental;
using std::is_object;
using std::is_scalar;
using std::is_compound;
using std::is_member_pointer;

// type properties:
using std::is_const;
using std::is_volatile;
using std::is_trivial;
using std::is_trivially_copyable;
using std::is_standard_layout;
using std::is_pod;
using std::is_empty;
using std::is_polymorphic;
using std::is_abstract;
using std::is_signed;
using std::is_unsigned;
using std::is_constructible;
using std::is_default_constructible;
using std::is_copy_constructible;
using std::is_move_constructible;
using std::is_assignable;
using std::is_copy_assignable;
using std::is_move_assignable;
using std::is_destructible;
using std::is_trivially_constructible;
using std::is_trivially_default_constructible;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;
using std::is_trivially_assignable;
using std::is_trivially_copy_assignable;
using std::is_trivially_move_assignable;
using std::is_trivially_destructible;
using std::is_nothrow_constructible;
using std::is_nothrow_default_constructible;
using std::is_nothrow_copy_constructible;
using std::is_nothrow_move_constructible;
using std::is_nothrow_assignable;
using std::is_nothrow_copy_assignable;
using std::is_nothrow_move_assignable;
using std::is_nothrow_destructible;
using std::has_virtual_destructor;

// type relations:
using std::is_same;
using std::is_base_of;
using std::is_convertible;

// const-volatile modifications:
using std::remove_const;
using std::remove_volatile;
using std::remove_cv;
using std::add_const;
using std::add_volatile;
using std::add_cv;

template<class T>
using remove_const_t = typename std::remove_const<T>::type;
template<class T>
using remove_volatile_t = typename std::remove_volatile<T>::type;
template<class T>
using remove_cv_t = typename std::remove_cv<T>::type;
template<class T>
using add_const_t = typename std::add_const<T>::type;
template<class T>
using add_volatile_t = typename std::add_volatile<T>::type;
template<class T>
using add_cv_t = typename std::add_cv<T>::type;

// reference modifications:
using std::remove_reference;
using std::add_lvalue_reference;
using std::add_rvalue_reference;

template<class T>
using remove_reference_t = typename std::remove_reference<T>::type;
template<class T>
using add_lvalue_reference_t = typename std::add_lvalue_reference<T>::type;
template<class T>
using add_rvalue_reference_t = typename std::add_rvalue_reference<T>::type;

// pointer modifications:
using std::remove_pointer;
using std::add_pointer;

template<class T>
using remove_pointer_t = typename std::remove_pointer<T>::type;
template<class T>
using add_pointer_t = typename std::add_pointer<T>::type;

// sign modifications:
using std::make_signed;
using std::make_unsigned;

template<class T>
using make_signed_t = typename std::make_signed<T>::type;
template<class T>
using make_unsigned_t = typename std::make_unsigned<T>::type;

// array modifications:
using std::remove_extent;
using std::remove_all_extents;

template<class T>
using remove_extent_t = typename std::remove_extent<T>::type;
template<class T>
using remove_all_extents_t = typename std::remove_all_extents<T>::type;

// other transformations:
using std::decay;
using std::enable_if;
using std::conditional;
using std::common_type;
using std::underlying_type;

template<class T>
using decay_t = typename std::decay<T>::type;
template<bool b, class T = void>
using enable_if_t = typename std::enable_if<b, T>::type;
template<bool b, class T, class F>
using conditional_t = typename std::conditional<b, T, F>::type;
template<class... T>
using common_type_t = typename std::common_type<T...>::type;
template<class T>
using underlying_type_t = typename std::underlying_type<T>::type;

}  // namespace ETL_NAMESPACE

#endif  // ETL_TRAITSUPPORT_H_
