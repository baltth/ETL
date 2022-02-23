/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2022 Balazs Toth.

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

#include <cstddef>
#include <cstdint>
//#include <functional>
#include <iterator>
#include <stdexcept>  // For new overrides
#include <type_traits>


#ifndef ETL_DISABLE_ASSERT
#include <cassert>
#endif

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE Etl
#endif

#ifndef ETL_DISABLE_HEAP
#define ETL_DISABLE_HEAP 0
#endif

#ifndef ETL_DISABLE_ASSERT
#define ETL_DISABLE_ASSERT 0
#endif


// Assertions

#if ETL_DISABLE_ASSERT

#define ETL_ASSERT(cond)
#define ETL_ASSERTIONS_ON 0

#else

#define ETL_ASSERT(cond) assert(cond)

#ifdef NDEBUG
#define ETL_ASSERTIONS_ON 0
#else
#define ETL_ASSERTIONS_ON 1
#endif

#endif


// Language features
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::size_t;


// Utilities

namespace ETL_NAMESPACE {
namespace Detail {


template<typename T>
struct IsIterator {

    typedef char Yes;
    typedef long No;

    template<typename U, typename = typename U::iterator_category>
    struct HasIteratorCategory {
        static const Yes value;
    };

    template<typename U>
    static HasIteratorCategory<U> test(U*);

    template<typename U>
    static No test(...);

    static const bool value = (sizeof(test<T>(nullptr)) == sizeof(Yes));
};


template<typename T>
struct AlignmentOf {

    struct Aligned {
        char c;
        T i;
    };
    struct Basic {
        T i;
    };

    static const size_t value = sizeof(Aligned) - sizeof(Basic);
    static_assert(value > 0, "Error calculating alignment");
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE


// Heap usage

#if ETL_DISABLE_HEAP

inline void* operator new(std::size_t count, const std::nothrow_t& tag) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void* operator new[](std::size_t count, const std::nothrow_t& tag) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void* operator new(std::size_t) throw(std::bad_alloc) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void* operator new[](std::size_t) throw(std::bad_alloc) {
    extern void* invalid_operator_new_call();
    return invalid_operator_new_call();
}

inline void operator delete(void* ptr, const std::nothrow_t& tag) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

inline void operator delete[](void* ptr, const std::nothrow_t& tag) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

inline void operator delete(void* ptr) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

inline void operator delete[](void* ptr) {
    extern void invalid_operator_delete_call();
    invalid_operator_delete_call();
}

#endif

#endif  // __ETL_ETLSUPPORT_H__
