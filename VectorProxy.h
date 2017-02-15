/**
\file
\date 2017.02.15. 16:42:10
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

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

#ifndef __ETL_VECTORPROXY_H__
#define __ETL_VECTORPROXY_H__

#include "Base/SequenceContainerProxy.h"
#include "Array.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {

class AVectorBase;

class VectorProxy : public SequenceContainerProxy {
friend AVectorBase;

// functions
public:

    void setCapacity(uint32_t c) {
        capacity = c;
    }

    void setData(void* d) {
        data = d;
    }

protected:

    VectorProxy(size_t itemSize, void* dataPointer, uint32_t len) :
        SequenceContainerProxy(itemSize, dataPointer, len) {};

};

}

#endif /* __ETL_VECTORPROXY_H__ */

