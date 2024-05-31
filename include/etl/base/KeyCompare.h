/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2024 Balazs Toth.

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

#ifndef ETL_KEYCOMPARE_H_
#define ETL_KEYCOMPARE_H_

#include <etl/base/tools.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {


template<typename Comp>
struct KeyCompare {

    template<class Item,
             typename = enable_if_t<HasFirstType<Item>::value>>
    bool operator()(const Item& lhs, const Item& rhs) const {
        return Comp()(lhs.first, rhs.first);
    }

    template<class Key,
             class Item,
             typename = enable_if_t<HasFirstType<Item>::value>>
    enable_if_t<is_same<Key, remove_cv_t<typename Item::first_type>>::value, bool>
    operator()(const Key& lhs, const Item& rhs) const {
        return Comp()(lhs, rhs.first);
    }

    template<class Key,
             class Item,
             typename = enable_if_t<HasFirstType<Item>::value>>
    enable_if_t<is_same<Key, remove_cv_t<typename Item::first_type>>::value, bool>
    operator()(const Item& lhs, const Key& rhs) const {
        return Comp()(lhs.first, rhs);
    }
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_KEYCOMPARE_H_
