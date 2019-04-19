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

#include <etl/etlSupport.h>

#include <type_traits>

namespace ETL_NAMESPACE {

using std::enable_if;

using std::remove_const;
using std::remove_volatile;
using std::remove_cv;
using std::remove_reference;

using std::integral_constant;

using std::true_type;
using std::false_type;

using std::is_same;

using std::is_integral;
using std::is_floating_point;
using std::is_unsigned;
using std::is_signed;

}

#endif /* __ETL_TRAITSUPPORT_H__ */

