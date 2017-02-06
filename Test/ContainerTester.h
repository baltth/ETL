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

#ifndef __ETL_CONTAINERTESTER_H__
#define __ETL_CONTAINERTESTER_H__

#include <iostream>


class ContainerTester {

// variables
private:

	int32_t value;

// functions
public:

	explicit ContainerTester(int32_t v = 0) :
		value(v) {
		reportConstructor();
		reportValue();
	}

	ContainerTester(const ContainerTester &other) :
		value(other.value) {
		reportCopyConstructor();
		reportValue();
	}

	ContainerTester &operator=(const ContainerTester &other) {

		value = other.value;
		reportCopyAssignment();
		reportValue();
		return *this;
	}

	ContainerTester(ContainerTester &&other) :
		value(other.value) {
		reportMoveConstructor();
		reportValue();
	}

	ContainerTester &operator=(ContainerTester &&other) {
		value = other.value;
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
	    std::cout << "C()     ";
	}

	void reportCopyConstructor() {
		std::cout << "C(C&)   ";
	}

	void reportCopyAssignment() {
	    std::cout << "C=(C&)  ";
	}

	void reportMoveConstructor() {
		std::cout << "C(C&&)  ";
	}

	void reportMoveAssignment() {
	    std::cout << "C=(C&&) ";
	}

	void reportDesctructor() {
		std::cout << "~C()    ";
	}

    void reportValue() {
        std::cout << "value @ " << this << ": " << value << std::endl;
	}

};

#endif /* __ETL_CONTAINERTESTER_H__ */
