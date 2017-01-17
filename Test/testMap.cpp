/**
\file
\date 2016.03.16. 12:27:49
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

#include "ETL/Map.h"
#include "ETL/Test/ContainerTester.h"

#include "testMap.h"


void testMap(ADebugInterface* debugIf) {

	Map<uint32_t, ContainerTester> numMap;

	auto printMap = [&numMap, debugIf]() {

		StrBuffer dbg("\r\nMap<uint32_t, ContainerTester> test:\r\n", 256);

		for(auto &item : numMap) {

			dbg.putDec(item.getKey());
			dbg.concat(": ");
			item.getElement().writeValueTo(dbg);
			dbg.concat("\r\n");
		}

		dbg.concat("\r\n");
		debugIf->debugPrint(dbg, debugIf->getDebugLevel());
	};

	ContainerTester a(4, debugIf);
	numMap.insertOrAssign(4, a);
	printMap();
	numMap.emplace(5, -5, debugIf);
	printMap();
	ContainerTester b(-4, debugIf);
	numMap.insertOrAssign(4, b);
	printMap();
	numMap.emplace(2, -2, debugIf);
	printMap();
	numMap.emplace(3, -3, debugIf);
	printMap();
	numMap.erase(3);
	printMap();

	debugIf->debugPrint("[4]", debugIf->getDebugLevel());
	numMap[4].reportValue();
	ContainerTester &c = numMap[4];
	c.reportValue();

}
