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

#ifndef __ETL_KEYCOMPARE_H__
#define __ETL_KEYCOMPARE_H__

#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


template<typename K, class E>
struct KeyCompare {
    typedef std::pair<const K, E> ItemType;
    bool operator()(const ItemType& lhs, const ItemType& rhs) const {
        return std::less<K>()(lhs.first, rhs.first);
    }
    bool operator()(const ItemType& lhs, const K& rhs) const {
        return std::less<K>()(lhs.first, rhs);
    }
    bool operator()(const K& lhs, const ItemType& rhs) const {
        return std::less<K>()(lhs, rhs.first);
    }
};

}

#endif /* __ETL_KEYCOMPARE_H__ */

