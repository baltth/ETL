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

#include "ContainerTester.h"

#include <sstream>

using Etl::Test::ContainerTester;


uint32_t ContainerTester::objectCnt = 0;
uint32_t ContainerTester::objectRef = 0;
uint32_t ContainerTester::copyCnt = 0;
uint32_t ContainerTester::moveCnt = 0;

bool ContainerTester::enablePrint = false;


std::string ContainerTester::toString() const {

    std::ostringstream str;
    str << "Id " << objectId;
    str << " Value " << value;
    return str.str();
}
