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

#ifndef __ETL_MATHSUPPORT_H_
#define __ETL_MATHSUPPORT_H_

#include <etl/etlSupport.h>

#include <cmath>
#include <limits>

#if (ETL_HAS_CPP11 == 0)

#include <etl/traitSupport.h>


#ifndef NAN
STATIC_ASSERT_(std::numeric_limits<float>::has_quiet_NaN, etl_math_NAN);
#define NAN         (std::numeric_limits<float>::quiet_NaN())
#endif

#ifndef INFINITY
STATIC_ASSERT_(std::numeric_limits<float>::has_infinity, etl_math_INF);
#define INFINITY    (std::numeric_limits<float>::infinity())
#endif

#ifndef M_PI
#define M_PI    (3.141592653589793238462643383279502884197169399375105820974)
#endif

namespace ETL_NAMESPACE {

template<typename T>
typename enable_if<is_floating_point<T>::value, bool>::type
isnan(T val) {
    STATIC_ASSERT(std::numeric_limits<T>::is_iec559);
    return (val != val);
}

template<typename T>
typename enable_if<is_integral<T>::value, bool>::type
isnan(T val) {
    return isnan(static_cast<double>(val));
}

template<typename T>
typename enable_if<is_floating_point<T>::value, bool>::type
isinf(T val) {
    STATIC_ASSERT(std::numeric_limits<T>::is_iec559);
    return (val > std::numeric_limits<T>::max()) ||
           (val < -std::numeric_limits<T>::max());
}

template<typename T>
typename enable_if<is_integral<T>::value, bool>::type
isinf(T val) {
    return isinf(static_cast<double>(val));
}

template<typename T>
bool isfinite(T val) {
    return (!isnan(val)) && (!isinf(val));
}

template<typename T>
typename enable_if<is_floating_point<T>::value, bool>::type
isnormal(T val) {
    return isfinite(val) &&
           ((val >= std::numeric_limits<T>::min()) || (val <= -std::numeric_limits<T>::min()));
}

template<typename T>
typename enable_if<is_integral<T>::value, bool>::type
isnormal(T val) {
    return isnormal(static_cast<double>(val));
}

}

#else

namespace ETL_NAMESPACE {

using std::isnan;
using std::isinf;
using std::isfinite;
using std::isnormal;

}

#endif

#endif /* __ETL_MATHSUPPORT_H_ */

