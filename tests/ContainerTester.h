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

#ifndef ETL_TEST_CONTAINERTESTER_H_
#define ETL_TEST_CONTAINERTESTER_H_

#include <etl/etlSupport.h>
#include <functional>
#include <iostream>
#include <string>

namespace Etl {
namespace Test {


class ContainerTester {

  private:  // variables

    int32_t value;
    uint32_t objectId;

    static uint32_t objectCnt;
    static uint32_t objectRef;
    static uint32_t copyCnt;
    static uint32_t moveCnt;

  public:

    static bool enablePrint;

  public:  // functions

    explicit ContainerTester(int32_t v = 0) :
        value(v),
        objectId(++objectRef) {

        ++objectCnt;

        if (enablePrint) {
            reportConstructor();
            reportValue();
        }
    }

    ContainerTester(const ContainerTester& other) :
        value(other.value),
        objectId(++objectRef) {

        ++objectCnt;
        ++copyCnt;

        if (enablePrint) {
            reportCopyConstructor();
            reportValue();
        }
    }

    ContainerTester& operator=(const ContainerTester& other) {

        value = other.value;
        ++copyCnt;

        if (enablePrint) {
            reportCopyAssignment();
            reportValue();
        }
        return *this;
    }

    ~ContainerTester() {
        ETL_ASSERT(objectCnt > 0U);
        --objectCnt;
        if (enablePrint) {
            reportDesctructor();
            reportValue();
        }
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

    ContainerTester(ContainerTester&& other) :
        value(other.value),
        objectId(++objectRef) {

        ++objectCnt;
        ++moveCnt;
        if (enablePrint) {
            reportMoveConstructor();
            reportValue();
        }
    }

    ContainerTester& operator=(ContainerTester&& other) {

        value = other.value;

        ++moveCnt;
        if (enablePrint) {
            reportMoveAssignment();
            reportValue();
        }
        return *this;
    }

    static uint32_t getMoveCount() {
        return moveCnt;
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
        std::cout << "id " << objectId << " @ " << this << " value: " << value << std::endl;
    }

    std::string toString() const;
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


class NonAssignable {

  private:  // variables

    int val {};

  public:  // types

    int get() const {
        return val;
    }

    NonAssignable() = default;

    explicit NonAssignable(int v) :
        val {v} {};

    NonAssignable(const NonAssignable&) = default;
    NonAssignable(NonAssignable&&) = default;

    NonAssignable& operator=(const NonAssignable&) = delete;
    NonAssignable& operator=(NonAssignable&&) = delete;

    ~NonAssignable() = default;
};


inline bool operator==(const NonAssignable& lhs, const NonAssignable& rhs) {
    return lhs.get() == rhs.get();
}

inline bool operator!=(const NonAssignable& lhs, const NonAssignable& rhs) {
    return !(operator==(lhs, rhs));
}

inline bool operator<(const NonAssignable& lhs, const NonAssignable& rhs) {
    return lhs.get() < rhs.get();
}

}  // namespace Test
}  // namespace Etl


namespace std {

template<>
struct hash<Etl::Test::ContainerTester> {
    size_t operator()(Etl::Test::ContainerTester const& val) const noexcept {
        return hash<int32_t> {}(val.getValue());
    }
};

template<>
struct hash<Etl::Test::NonAssignable> {
    size_t operator()(Etl::Test::NonAssignable const& val) const noexcept {
        return hash<int32_t> {}(val.get());
    }
};

}  // namespace std

#endif  // ETL_TEST_CONTAINERTESTER_H_
