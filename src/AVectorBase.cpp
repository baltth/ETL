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

#include <etl/etlSupport.h>
#include <etl/base/AVectorBase.h>

using ETL_NAMESPACE::AVectorBase;


void AVectorBase::swapProxy(AVectorBase& other) {

    AVectorBase::Proxy tmpProxy(proxy);

    proxy.data = other.proxy.data;
    proxy.capacity = other.proxy.capacity;
    proxy.size = other.proxy.size;

    other.proxy.data = tmpProxy.data;
    other.proxy.capacity = tmpProxy.capacity;
    other.proxy.size = tmpProxy.size;
}

