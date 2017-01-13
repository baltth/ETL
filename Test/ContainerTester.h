/**
\file
\date 2016.03.16. 15:42:16
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

#ifndef __CONTAINERTESTER_H__
#define __CONTAINERTESTER_H__

#include "EFpp/Process/ADebugInterface.h"

class ContainerTester {

// variables
private:

	int32_t value;
	ADebugInterface* debugIf;

// functions
public:

	explicit ContainerTester(int32_t v = 0, ADebugInterface* dbgIf = nullptr) :
		value(v),
		debugIf(dbgIf) {
		reportConstructor();
		reportValue();
	}

	ContainerTester(const ContainerTester &other) :
		value(other.value),
		debugIf(other.debugIf) {
		reportCopyConstructor();
		reportValue();
	}

	ContainerTester &operator=(const ContainerTester &other) {

		value = other.value;
		debugIf = other.debugIf;
		reportCopyAssignment();
		reportValue();
		return *this;
	}

	ContainerTester(ContainerTester &&other) :
		value(other.value),
		debugIf(other.debugIf) {
		reportMoveConstructor();
		reportValue();
	}

	ContainerTester &operator=(ContainerTester &&other) {
		value = other.value;
		debugIf = other.debugIf;
		reportMoveAssignment();
		reportValue();
		return *this;
	}

	~ContainerTester() {
		reportDesctructor();
		reportValue();
	}

	int32_t getValue() const {
		return value;
	}

	void reportConstructor() {
		if(debugIf) {
			debugIf->debugPrint("C()     ", debugIf->getDebugLevel(), false, false);
		}
	}

	void reportCopyConstructor() {
		if(debugIf) {
			debugIf->debugPrint("C(C&)   ", debugIf->getDebugLevel(), false, false);
		}
	}

	void reportCopyAssignment() {
		if(debugIf) {
			debugIf->debugPrint("C=(C&)  ", debugIf->getDebugLevel(), false, false);
		}
	}

	void reportMoveConstructor() {
		if(debugIf) {
			debugIf->debugPrint("C(C&&)  ", debugIf->getDebugLevel(), false, false);
		}
	}

	void reportMoveAssignment() {
		if(debugIf) {
			debugIf->debugPrint("C=(C&&) ", debugIf->getDebugLevel(), false, false);
		}
	}

	void reportDesctructor() {
		if(debugIf) {
			debugIf->debugPrint("~C()    ", debugIf->getDebugLevel(), false, false);
		}
	}

	void reportValue() {

		if(debugIf) {
			StrBuffer dbg("", 32);
			writeValueTo(dbg);
			debugIf->debugPrint(dbg, debugIf->getDebugLevel());
		}
	}

	void writeValueTo(StrBuffer &str) {
		str.concat("value @ ");
		str.putHex(reinterpret_cast<uint32_t>(this), 8);
		str.concat(": ");
		str.putDec(value);
		str.endString();
	}

};

#endif /* __CONTAINERTESTER_H__ */
