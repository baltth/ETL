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

#ifndef __ETL_ETLSUPPORT_H__
#define __ETL_ETLSUPPORT_H__

#include <etl/langSupport.h>

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

#ifndef ETL_DISABLE_HEAP
#define ETL_DISABLE_HEAP    0
#endif

#if ETL_USE_CPP11

#include <cstdint>
#include <functional>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::size_t;

#else

#include <stdint.h>

#endif

#include <stdexcept>    // For new overrides

namespace ETL_NAMESPACE {


template<typename T>
struct Matcher {

    virtual bool call(const T&) const = 0;

};

#if ETL_USE_CPP11

template<typename T>
using MatchFunc = std::function<bool(const T&)>;

#endif

}

#if ETL_DISABLE_HEAP

inline void* operator new (std::size_t count, const std::nothrow_t& tag) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void* operator new[](std::size_t count, const std::nothrow_t& tag) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void* operator new (std::size_t) throw(std::bad_alloc) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void* operator new[](std::size_t) throw(std::bad_alloc) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void operator delete (void* ptr, const std::nothrow_t& tag) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

inline void operator delete[](void* ptr, const std::nothrow_t& tag) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

inline void operator delete (void* ptr) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

inline void operator delete[](void* ptr) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

#endif

#endif /* __ETL_ETLSUPPORT_H__ */
