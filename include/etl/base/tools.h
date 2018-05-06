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

#ifndef __ETL_TOOLS_H__
#define __ETL_TOOLS_H__

#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {

struct SizeDiff {

    uint32_t common;
    uint32_t lGreaterWith;
    uint32_t rGreaterWith;

    template<class L, class R>
    SizeDiff(const L& l, const R& r) :
        common(0),
        lGreaterWith(0),
        rGreaterWith(0) {

        if (l.getSize() > r.getSize()) {

            common = r.getSize();
            lGreaterWith = l.getSize() - r.getSize();

        } else {

            common = l.getSize();
            rGreaterWith = r.getSize() - l.getSize();
        }
    }
};

}

#endif /* __ETL_TOOLS_H__ */
