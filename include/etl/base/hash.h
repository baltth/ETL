/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2024 Balazs Toth.

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

#ifndef ETL_HASH_H_
#define ETL_HASH_H_

#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <functional>

namespace ETL_NAMESPACE {

namespace Detail {

// The well known hash_combine...
inline std::size_t hashCombineMethod(std::size_t seed, std::size_t hasValue) {

    return seed ^ (hashValue + 0x9e3779b9UL + (seed << 6) + (seed >> 2));
}

}


template<class T>
inline void hash_combine(std::size_t& seed, const T& v) {

    std::hash<T> hasher;
    seed = Detail::hashCombineMethod(seed, hasher(v));
}

}

#endif /* ETL_HASH_H_ */
