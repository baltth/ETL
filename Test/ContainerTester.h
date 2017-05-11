/**
\file
\date 2016.03.16.
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

#include "etlSupport.h"

#define PRINT_TO_IOSTREAM   0

#if PRINT_TO_IOSTREAM
#include <iostream>
#endif

class ContainerTester {

  private:  // variables

    int32_t value;
    uint32_t objectId;

    static uint32_t objectCnt;
    static uint32_t objectRef;
    static uint32_t copyCnt;

#if ETL_USE_CPP11
    static uint32_t moveCnt;
#endif

  public:   // functions

    explicit ContainerTester(int32_t v = 0) :
        value(v),
        objectId(++objectRef) {

        ++objectCnt;
        reportConstructor();
        reportValue();
    }

    ContainerTester(const ContainerTester& other) :
        value(other.value),
        objectId(++objectRef) {

        ++objectCnt;
        ++copyCnt;
        reportCopyConstructor();
        reportValue();
    }

    ContainerTester& operator=(const ContainerTester& other) {

        value = other.value;
        ++copyCnt;
        reportCopyAssignment();
        reportValue();
        return *this;
    }

    ~ContainerTester() {
        --objectCnt;
        reportDesctructor();
        reportValue();
    }

    int32_t getValue() const {
        return value;
    }

    uint32_t getId() const {
        return objectId;
    }

    static uint32_t getObjectCount() {
        return objectCnt;
    }

    static uint32_t getLastObjectId() {
        return objectRef;
    }

    static uint32_t getCopyCount() {
        return copyCnt;
    }

#if ETL_USE_CPP11

    ContainerTester(ContainerTester&& other) :
        value(other.value),
        objectId(other.objectId) {

        ++objectCnt;
        ++moveCnt;
        reportMoveConstructor();
        reportValue();
    }

    ContainerTester& operator=(ContainerTester&& other) {

        value = other.value;
        value = other.objectId;
        ++moveCnt;
        reportMoveAssignment();
        reportValue();
        return *this;
    }

    static uint32_t getMoveCount() {
        return moveCnt;
    }

#endif


#if PRINT_TO_IOSTRREAM

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

#else

    void reportConstructor() {};
    void reportCopyConstructor() {};
    void reportCopyAssignment() {};
    void reportMoveConstructor() {};
    void reportMoveAssignment() {};
    void reportDesctructor() {};
    void reportValue() {};

#endif

};


inline bool operator==(const ContainerTester& lhs, const ContainerTester& rhs) {
    return lhs.getValue() == rhs.getValue();
}

inline bool operator!=(const ContainerTester& lhs, const ContainerTester& rhs) {
    return !(operator==(lhs, rhs));
}

inline bool operator<(const ContainerTester& lhs, const ContainerTester& rhs) {
    return lhs.getValue() < rhs.getValue();
}


#endif /* __ETL_CONTAINERTESTER_H__ */

