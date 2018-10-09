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

#include <etl/langSupport.h>

#include <cmath>
#include <limits>


#ifndef NAN
STATIC_ASSERT_(std::numeric_limits<float>::has_quiet_NaN, etl_math_NAN);
#define NAN     (std::numeric_limits<float>::quiet_NaN())
#endif


#ifndef M_PI
#define M_PI    (3.141592653589793238462643383279502884197169399375105820974)
#endif


#endif /* __ETL_MATHSUPPORT_H_ */

