/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Balazs Toth.

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

#include "catch.hpp"

#include <etl/Vector.h>

#include "UnalignedTester.h"
#include "ContainerTester.h"
#include "sequenceTests.h"
#include "compatibilityTests.h"
#include "comparisionTests.h"

using ETL_NAMESPACE::Test::UnalignedTester;
using ETL_NAMESPACE::Test::ContainerTester;


// Etl::Vector tests ----------------------------------------------------------


template<class VecT>
void testVectorBasic() {

    VecT vec;

    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);

    vec.reserve(16);
    REQUIRE(vec.capacity() >= 16);
    REQUIRE(vec.size() == 0);

    vec.push_back(1);

    REQUIRE_FALSE(vec.empty());
    REQUIRE(vec.size() == 1);

    vec.push_back(2);

    REQUIRE_FALSE(vec.empty());
    REQUIRE(vec.size() == 2);

    typename VecT::iterator it = vec.begin();
    REQUIRE(*it == 1);
    REQUIRE(vec[0] == *it);

    ++it;
    *it = 3;

    vec.insert(vec.begin(), 11);
    REQUIRE(vec[0] == 11);
    vec.pop_front();
    vec.erase(vec.begin());

    REQUIRE(*vec.begin() == 3);
    REQUIRE(vec.size() == 1);
}

TEST_CASE("Etl::Dynamic::Vector<> basic test", "[vec][dynamic][etl][basic]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorBasic<VecT>();
}

TEST_CASE("Etl::Static::Vector<> basic test", "[vec][static][etl][basic]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorBasic<VecT>();
}


TEST_CASE("Etl::Dynamic::Vector<> push/pop test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testBackAccess<VecT>();
    testFrontAccess<VecT>();
}

TEST_CASE("Etl::Static::Vector<> push/pop test", "[vec][static][etl][basic]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testBackAccess<VecT>();
    testFrontAccess<VecT>();
}


TEST_CASE("Etl::Dynamic::Vector<> iteration test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    SECTION("iterator") {
        testIterationForward<VecT>();
        testIterationBackward<VecT>();
    }

    SECTION("reverse_iterator") {
        testReverseIterationForward<VecT>();
        testReverseIterationBackward<VecT>();
    }
}

TEST_CASE("Etl::Static::Vector<> iteration test", "[vec][static][etl][basic]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    SECTION("iterator") {
        testIterationForward<VecT>();
        testIterationBackward<VecT>();
    }

    SECTION("reverse_iterator") {
        testReverseIterationForward<VecT>();
        testReverseIterationBackward<VecT>();
    }
}


template<class VecT>
void testVectorInsertAndErase() {

    VecT vec(4, 0);

    CHECK(vec.size() == 4);

    SECTION("insert(const_iterator, uint32_t, const T&)") {

        typename VecT::iterator it = vec.begin() + 2;
        it = vec.insert(it, 2);
        REQUIRE(vec[2] == 2);
        REQUIRE(vec.size() == 5);
        REQUIRE(it == &vec[2]);

        ++it;
        it = vec.insert(it, 2, 3);
        REQUIRE(vec[2] == 2);
        REQUIRE(vec[3] == 3);
        REQUIRE(vec[4] == 3);
        REQUIRE(vec.size() == 7);
        REQUIRE(it == &vec[3]);

        CHECK(vec[1] == 0);
        CHECK(vec[5] == 0);

        it = vec.begin() + 2;
        it = vec.erase(it);
        REQUIRE(vec.size() == 6);
        REQUIRE(vec[2] == 3);
        REQUIRE(it == &vec[2]);

        it = vec.erase(it, it + 2);
        REQUIRE(vec.size() == 4);
        REQUIRE(vec[2] == 0);
        REQUIRE(it == &vec[2]);
    }

    SECTION("insert(const_iterator, InputIt, InputIt)") {

        vec[0] = 1;
        vec[1] = 2;
        vec[2] = 3;
        vec[3] = 4;

        VecT vec2(1, 0);
        CHECK(vec2.size() == 1);

        SECTION("Correct insert") {

            CHECK(vec.size() == 4);

            typename VecT::const_iterator last = vec.cend() - 1;
            typename VecT::iterator it = vec2.insert(vec2.end(), vec.cbegin(), last);

            REQUIRE(vec2.size() == 4);
            REQUIRE(it == &vec2[1]);

            REQUIRE(vec2[0] == 0);
            REQUIRE(vec2[1] == 1);
            REQUIRE(vec2[2] == 2);
            REQUIRE(vec2[3] == 3);
        }

        SECTION("Invalid insert") {

            typename VecT::iterator it = vec2.insert(vec2.end(), vec.cend(), vec.cbegin());

            REQUIRE(vec2.size() == 1);
            REQUIRE(it == vec2.end());
        }
    }
}

TEST_CASE("Etl::Dynamic::Vector<> insert/erase test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorInsertAndErase<VecT>();
}

TEST_CASE("Etl::Static::Vector<> insert/erase test", "[vec][static][etl][basic]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorInsertAndErase<VecT>();
}


template<class VecT>
void testVectorAssignment() {

    typedef typename VecT::value_type ItemT;

    static const int PATTERN1 = 123;
    static const int PATTERN2 = 321;

    VecT vec1(4, ContainerTester(PATTERN1));
    VecT vec2(8, ContainerTester(PATTERN2));

    CHECK(vec1[0] != vec2[0]);
    CHECK(vec1.size() != vec2.size());

    REQUIRE(ItemT::getObjectCount() == (vec1.size() + vec2.size()));

    vec1 = vec2;

    REQUIRE(vec1[0] == vec2[0]);
    REQUIRE(vec1.begin() != vec2.begin());
    REQUIRE(vec1.size() == vec2.size());
    REQUIRE(ItemT::getObjectCount() == (2 * vec2.size()));
}

TEST_CASE("Etl::Dynamic::Vector<> assignment test", "[vec][dynamic][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorAssignment<VecT>();
}

TEST_CASE("Etl::Static::Vector<> assignment test", "[vec][static][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorAssignment<VecT>();
}


template<class VecT1, class VecT2>
void testVectorSwap() {

    VecT1 vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    vec1.push_back(3);

    VecT2 vec2;
    vec2.push_back(-1);
    vec2.push_back(-2);
    vec2.push_back(-3);

    CHECK(vec1.size() == 3);
    CHECK(vec2.size() == 3);

    SECTION("Swap ego") {

        vec1.swap(vec1);

        REQUIRE(vec1.size() == 3);

        REQUIRE(vec1[0] == 1);
        REQUIRE(vec1[1] == 2);
        REQUIRE(vec1[2] == 3);
    }

    SECTION("Swap equal length") {

        vec1.swap(vec2);

        REQUIRE(vec1.size() == 3);
        REQUIRE(vec2.size() == 3);

        REQUIRE(vec1[0] == -1);
        REQUIRE(vec1[1] == -2);
        REQUIRE(vec1[2] == -3);

        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
    }

    SECTION("Swap different length") {

        vec2.push_back(-4);
        vec2.push_back(-5);

        CHECK(vec2.size() == 5);

        vec1.swap(vec2);

        REQUIRE(vec1.size() == 5);
        REQUIRE(vec2.size() == 3);

        REQUIRE(vec1[0] == -1);
        REQUIRE(vec1[1] == -2);
        REQUIRE(vec1[2] == -3);
        REQUIRE(vec1[3] == -4);
        REQUIRE(vec1[4] == -5);

        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);

        vec1.swap(vec2);

        REQUIRE(vec1.size() == 3);
        REQUIRE(vec2.size() == 5);

        REQUIRE(vec1[0] == 1);
        REQUIRE(vec1[2] == 3);

        REQUIRE(vec2[0] == -1);
        REQUIRE(vec2[4] == -5);
    }

    SECTION("Swap empty") {

        VecT2 vec3;

        CHECK(vec3.empty());

        vec1.swap(vec3);

        REQUIRE(vec1.empty());
        REQUIRE(vec3.size() == 3);

        REQUIRE(vec3[0] == 1);
        REQUIRE(vec3[1] == 2);
        REQUIRE(vec3[2] == 3);

        vec1.swap(vec3);

        REQUIRE(vec1.size() == 3);
        REQUIRE(vec3.empty());

        REQUIRE(vec1[0] == 1);
        REQUIRE(vec1[1] == 2);
        REQUIRE(vec1[2] == 3);
    }
}

TEST_CASE("Etl::Dynamic::Vector<> swap test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorSwap<VecT, VecT>();

    SECTION("Swap is no-copy") {

        typedef Etl::Dynamic::Vector<ContainerTester> VecT2;

        VecT2 vec;

        vec.push_back(ContainerTester(1));
        vec.push_back(ContainerTester(2));

        VecT2 vec2;

        vec2.push_back(ContainerTester(-1));

        const uint32_t copyCnt = ContainerTester::getCopyCount();

        vec.swap(vec2);

        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
    }
}

TEST_CASE("Etl::Static::Vector<> swap test", "[vec][static][etl]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    SECTION("Same capacity") {
        testVectorSwap<VecT, VecT>();
    }

    SECTION("Different capacity") {

        typedef Etl::Static::Vector<ItemT, 24> VecT2;

        testVectorSwap<VecT, VecT2>();
    }

    SECTION("Capacity boundaries") {

        typedef Etl::Static::Vector<ItemT, 4> VecT2;

        VecT vec1(4, 1);
        VecT2 vec2;

        CHECK(vec1.size() == vec2.max_size());

        SECTION("Allow at max capacity") {

            SECTION("1->2") {

                vec1.swap(vec2);

                REQUIRE(vec1.empty());
                REQUIRE(vec2.size() == 4);
                REQUIRE(vec2[0] == 1);
                REQUIRE(vec2[3] == 1);

                vec1.swap(vec2);

                REQUIRE(vec1.size() == 4);
                REQUIRE(vec2.empty());
                REQUIRE(vec1[0] == 1);
                REQUIRE(vec1[3] == 1);
            }

            SECTION("2->1") {

                vec2.swap(vec1);

                REQUIRE(vec1.empty());
                REQUIRE(vec2.size() == 4);
                REQUIRE(vec2[0] == 1);
                REQUIRE(vec2[3] == 1);

                vec2.swap(vec1);

                REQUIRE(vec1.size() == 4);
                REQUIRE(vec2.empty());
                REQUIRE(vec1[0] == 1);
                REQUIRE(vec1[3] == 1);
            }
        }

        SECTION("Disallow above max capacity") {

            vec1.push_back(2);
            CHECK(vec1.size() > vec2.max_size());

            SECTION("1->2") {
                vec1.swap(vec2);
            }

            SECTION("2->1") {
                vec2.swap(vec1);
            }
        }
    }
}


template<class VecT>
void testVectorLeak() {

    typedef typename VecT::value_type ItemT;

    static const int PATTERN = 123;

    CHECK(ItemT::getObjectCount() == 0);
    if (ItemT::getObjectCount() == 0) {

        VecT vec(8, ContainerTester(PATTERN));
        CHECK(vec.size() == ItemT::getObjectCount());

        vec.pop_back();
        REQUIRE(vec.size() == ItemT::getObjectCount());

        vec.erase(vec.begin());
        REQUIRE(vec.size() == ItemT::getObjectCount());

        vec.erase((vec.begin() + 1), (vec.begin() + 3));
        REQUIRE(vec.size() == ItemT::getObjectCount());
    }

    REQUIRE(ItemT::getObjectCount() == 0);
}

TEST_CASE("Etl::Dynamic::Vector<> leak test", "[vec][dynamic][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorLeak<VecT>();
}

TEST_CASE("Etl::Static::Vector<> leak test", "[vec][static][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorLeak<VecT>();
}


template<class VecT>
void testVectorWithPtrItem() {

    VecT vec;

    REQUIRE(vec.size() == 0);

    vec.reserve(16);
    REQUIRE(vec.capacity() >= 16);
    REQUIRE(vec.size() == 0);

    int a = 1;
    int b = 2;
    int c = 3;

    vec.push_back(&a);
    vec.push_back(&b);

    REQUIRE(vec.size() == 2);

    typename VecT::iterator it = vec.begin();
    REQUIRE(*it == &a);
    REQUIRE(vec[0] == *it);

    ++it;
    *it = &b;

    vec.insert(vec.begin(), &c);
    REQUIRE(vec[0] == &c);
    vec.pop_front();
    vec.erase(vec.begin());

    REQUIRE(*vec.begin() == &b);
    REQUIRE(vec.size() == 1);
}

TEST_CASE("Etl::Dynamic::Vector<T*> test", "[vec][dynamic][etl][basic]") {

    SECTION("for T*") {

        typedef int* ItemT;
        typedef Etl::Dynamic::Vector<ItemT> VecT;

        testVectorWithPtrItem<VecT>();
    }

    SECTION("for const T*") {

        typedef const int* ItemT;
        typedef Etl::Dynamic::Vector<ItemT> VecT;

        testVectorWithPtrItem<VecT>();
    }
}

TEST_CASE("Etl::Static::Vector<T*> test", "[vec][static][etl][basic]") {

    SECTION("for T*") {

        typedef int* ItemT;
        typedef Etl::Static::Vector<ItemT, 16> VecT;

        testVectorWithPtrItem<VecT>();
    }

    SECTION("for const T*") {

        typedef const int* ItemT;
        typedef Etl::Static::Vector<ItemT, 16> VecT;

        testVectorWithPtrItem<VecT>();
    }
}


TEST_CASE("Vector<> - Static-Dynamic interop test", "[vec][static][dynamic][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Static::Vector<ItemT, 16> SVecT;
    typedef Etl::Dynamic::Vector<ItemT> DVecT;

    static const int PATTERN_S = 123;
    static const int PATTERN_D = 321;

    SVecT sVec(4, ContainerTester(PATTERN_S));
    DVecT dVec(8, ContainerTester(PATTERN_D));

    CHECK(sVec[0] != dVec[0]);
    CHECK(sVec.size() != dVec.size());

    REQUIRE(ItemT::getObjectCount() == (sVec.size() + dVec.size()));

    SECTION("S = D assignment") {

        sVec = dVec;

        REQUIRE(sVec[0] == dVec[0]);
        REQUIRE(sVec.size() == dVec.size());
        REQUIRE(ItemT::getObjectCount() == (2 * dVec.size()));
    }

    SECTION("D = S assignment") {

        dVec = sVec;

        REQUIRE(dVec[0] == sVec[0]);
        REQUIRE(dVec.size() == sVec.size());
        REQUIRE(ItemT::getObjectCount() == (2 * sVec.size()));
    }

    SECTION("S(D) construction") {

        SVecT sVec2(dVec);
        REQUIRE(sVec2[0] == dVec[0]);
        REQUIRE(sVec2.size() == dVec.size());
    }

    SECTION("D(S) construction") {

        DVecT dVec2(sVec);
        REQUIRE(dVec2[0] == sVec[0]);
        REQUIRE(dVec2.size() == sVec.size());
    }

    SECTION("Swap") {

        const uint32_t sInitSize = sVec.size();
        const uint32_t dInitSize = dVec.size();

        sVec.swap(dVec);

        REQUIRE(sVec.size() == dInitSize);
        REQUIRE(dVec.size() == sInitSize);

        REQUIRE(sVec.front() == ContainerTester(PATTERN_D));
        REQUIRE(sVec.back() == ContainerTester(PATTERN_D));
        REQUIRE(dVec.front() == ContainerTester(PATTERN_S));
        REQUIRE(dVec.back() == ContainerTester(PATTERN_S));

        dVec.swap(sVec);

        REQUIRE(sVec.size() == sInitSize);
        REQUIRE(dVec.size() == dInitSize);

        REQUIRE(sVec.front() == ContainerTester(PATTERN_S));
        REQUIRE(sVec.back() == ContainerTester(PATTERN_S));
        REQUIRE(dVec.front() == ContainerTester(PATTERN_D));
        REQUIRE(dVec.back() == ContainerTester(PATTERN_D));
    }
}


template<class VecT>
void testVectorWithInitList() {

    SECTION("Construct") {

        SECTION("With paren") {

            VecT vec({1, 2});

            REQUIRE(vec.size() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }

        SECTION("With double braces") {

            VecT vec{{1, 2}};

            REQUIRE(vec.size() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }

        SECTION("Uniform") {

            VecT vec{1, 2};

            REQUIRE(vec.size() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }
    }

    SECTION("Assign") {

        VecT vec({4, 5, 6, 7});
        CHECK(vec.size() == 4);

        SECTION("Empty") {

            vec = {};

            REQUIRE(vec.size() == 0);
        }

        SECTION("Non-empty") {

            vec = {1, 2};

            REQUIRE(vec.size() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }
    }
}

TEST_CASE("Etl::Dynamic::Vector<> with std::initializer_list<>", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorWithInitList<VecT>();
}

TEST_CASE("Etl::Static::Vector<> with std::initializer_list<>", "[vec][static][etl]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorWithInitList<VecT>();
}


template<class VecT>
void testVectorEmplace() {

    auto moveCnt = ContainerTester::getMoveCount();
    auto copyCnt = ContainerTester::getCopyCount();

    VecT vec;

    SECTION("emplace_back()") {

        vec.emplace_back(-1);
        vec.emplace_back(-2);
        vec.emplace_back(-3);

        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == ContainerTester(-1));
        REQUIRE(vec[1] == ContainerTester(-2));
        REQUIRE(vec[2] == ContainerTester(-3));

        REQUIRE(ContainerTester::getMoveCount() == moveCnt);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
    }

    SECTION("emplace()") {

        vec.emplace(vec.begin(), -4);

        REQUIRE(ContainerTester::getMoveCount() == moveCnt);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);

        vec.emplace(vec.end(), -6);
        auto it = vec.begin();
        vec.emplace(++it, -5);

        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == ContainerTester(-4));
        REQUIRE(vec[1] == ContainerTester(-5));
        REQUIRE(vec[2] == ContainerTester(-6));
    }
}

TEST_CASE("Etl::Dynamic::Vector<> emplace test", "[vec][dynamic][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorEmplace<VecT>();
}

TEST_CASE("Etl::Static::Vector<> emplace test", "[vec][static][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorEmplace<VecT>();
}


template<class VecT>
void testVectorMove() {

    VecT vec;
    vec.push_back(ContainerTester(-1));
    vec.push_back(ContainerTester(-2));
    vec.push_back(ContainerTester(-3));

    const auto size = vec.size();

    SECTION("Move constructor") {

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        //ContainerTester::enablePrint = true;
        VecT vec2(std::move(vec));
        //ContainerTester::enablePrint = false;

        REQUIRE(vec2.size() == size);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
        REQUIRE(ContainerTester::getMoveCount() >= moveCnt);
        REQUIRE(vec2[0] == ContainerTester(-1));
        REQUIRE(vec2[2] == ContainerTester(-3));
    }

    SECTION("Move assignment") {

        VecT vec2(2, ContainerTester(10));

        CHECK(vec2.size() != size);

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        vec2 = std::move(vec);

        REQUIRE(vec2.size() == size);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
        REQUIRE(ContainerTester::getMoveCount() >= moveCnt);
        REQUIRE(vec2[0] == ContainerTester(-1));
        REQUIRE(vec2[2] == ContainerTester(-3));
    }
}

TEST_CASE("Etl::Dynamic::Vector<> move test", "[vec][dynamic][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorMove<VecT>();
}

TEST_CASE("Etl::Static::Vector<> move test", "[vec][static][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorMove<VecT>();
}


template<class SrcVecT, class T = typename SrcVecT::value_type>
void testVectorAssignToBase(Etl::Vector<T>& dst) {

    SrcVecT vec;
    vec.push_back(ContainerTester(-1));
    vec.push_back(ContainerTester(-2));
    vec.push_back(ContainerTester(-3));

    const auto size = vec.size();

    SECTION("Copy assignment") {

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        dst = vec;

        REQUIRE(dst.size() == size);
        REQUIRE(ContainerTester::getCopyCount() >= copyCnt);
        REQUIRE(ContainerTester::getMoveCount() == moveCnt);
        REQUIRE(dst[0] == ContainerTester(-1));
        REQUIRE(dst[2] == ContainerTester(-3));
    }

    SECTION("Move assignment") {

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        dst = std::move(vec);

        REQUIRE(dst.size() == size);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
        REQUIRE(ContainerTester::getMoveCount() >= moveCnt);
        REQUIRE(dst[0] == ContainerTester(-1));
        REQUIRE(dst[2] == ContainerTester(-3));
    }
}

TEST_CASE("Etl::Vector<> assignment to base", "[vec][dynamic][static][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> DVecT;
    typedef Etl::Static::Vector<ItemT, 16U> SVecT;

    SECTION("Assigning to Etl::Dynamic::Vector<>") {

        DVecT dst;

        SECTION("From Etl::Dynamic::Vector<>") {
            testVectorAssignToBase<DVecT>(dst);
        }

        SECTION("From Etl::Static::Vector<>") {
            testVectorAssignToBase<SVecT>(dst);
        }
    }

    SECTION("Assigning to Etl::Static::Vector<>") {

        SVecT dst;

        SECTION("From Etl::Dynamic::Vector<>") {
            testVectorAssignToBase<DVecT>(dst);
        }

        SECTION("From Etl::Static::Vector<>") {
            testVectorAssignToBase<SVecT>(dst);
        }
    }
}


template<class VecT, class T = typename VecT::value_type>
void testVectorAssignFromBase(Etl::Vector<T>&& src) {

    VecT dst;

    const auto size = src.size();
    CHECK(size >= 3);

    SECTION("Copy assignment") {

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        dst = src;

        REQUIRE(dst.size() == size);
        REQUIRE(ContainerTester::getCopyCount() >= copyCnt);
        REQUIRE(ContainerTester::getMoveCount() == moveCnt);
        REQUIRE(dst[0] == ContainerTester(-1));
        REQUIRE(dst[2] == ContainerTester(-3));
    }

    SECTION("Move assignment") {

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        dst = std::move(src);

        REQUIRE(dst.size() == size);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
        REQUIRE(ContainerTester::getMoveCount() >= moveCnt);
        REQUIRE(dst[0] == ContainerTester(-1));
        REQUIRE(dst[2] == ContainerTester(-3));
    }
}

TEST_CASE("Etl::Vector<> assignment from base", "[vec][dynamic][static][etl]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> DVecT;
    typedef Etl::Static::Vector<ItemT, 16U> SVecT;

    SECTION("Assigning from Etl::Dynamic::Vector<>") {

        DVecT src;
        src.push_back(ContainerTester(-1));
        src.push_back(ContainerTester(-2));
        src.push_back(ContainerTester(-3));

        SECTION("To Etl::Dynamic::Vector<>") {
            testVectorAssignFromBase<DVecT>(std::move(src));
        }

        SECTION("To Etl::Static::Vector<>") {
            testVectorAssignFromBase<SVecT>(std::move(src));
        }
    }

    SECTION("Assigning from Etl::Static::Vector<>") {

        SVecT src;
        src.push_back(ContainerTester(-1));
        src.push_back(ContainerTester(-2));
        src.push_back(ContainerTester(-3));

        SECTION("To Etl::Dynamic::Vector<>") {
            testVectorAssignFromBase<DVecT>(std::move(src));
        }

        SECTION("To Etl::Static::Vector<>") {
            testVectorAssignFromBase<SVecT>(std::move(src));
        }
    }
}


TEST_CASE("Vector<> test cleanup", "[vec][static][dynamic][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}


// Etl::Dynamic::Vector tests -------------------------------------------------


TEST_CASE("Etl::Dynamic::Vector<> size/capacity test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    VecT vec;

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == 0);

    vec.reserve(16);
    REQUIRE(vec.capacity() >= 16);
    REQUIRE(vec.size() == 0);

    vec.push_back(1);
    vec.push_back(2);
    REQUIRE(vec.size() == 2);

    vec.shrink_to_fit();
    REQUIRE(vec.capacity() == 2);

    vec.reserve(5);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() >= 5);

    uint32_t newSize = vec.size() + 3;

    vec.resize(newSize);
    REQUIRE(vec.size() == newSize);

    uint32_t capacity = vec.capacity();
    newSize = capacity + 3;

    vec.resize(newSize);
    REQUIRE(vec.size() == newSize);
    REQUIRE(vec.capacity() >= newSize);

    vec.resize(2);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() >= 2);

    capacity = vec.capacity();
    vec.clear();
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == capacity);
}


TEST_CASE("Etl::Dynamic::Vector<> constructor test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    static const ItemT INIT_VALUE = 123;

    VecT vec1;
    REQUIRE(vec1.size() == 0);
    REQUIRE(vec1.capacity() == 0);

    VecT vec2(4);
    REQUIRE(vec2.size() == 4);
    REQUIRE(vec2.capacity() >= 4);

    CAPTURE(INIT_VALUE);
    VecT vec3(4, INIT_VALUE);
    REQUIRE(vec3.size() == 4);
    REQUIRE(vec3.capacity() >= 4);
    REQUIRE(vec3[0] == INIT_VALUE);
    REQUIRE(vec3[3] == INIT_VALUE);

    VecT vec4(vec3);
    REQUIRE(vec4.size() == vec3.size());
    REQUIRE(vec4.capacity() >= vec4.size());
    REQUIRE(vec4[0] == INIT_VALUE);
    REQUIRE(vec4[3] == INIT_VALUE);
    REQUIRE(vec4.begin() != vec3.begin());
}

#if ETL_USE_EXCEPTIONS

TEST_CASE("Etl::Dynamic::Vector<> exceptions", "[vec][dynamic][etl]") {

    static const uint32_t COUNT = 16;
    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    VecT vec(COUNT);

    ItemT val;
    CHECK_NOTHROW(val = vec[0]);
    CHECK_NOTHROW(val = vec[COUNT - 1]);

    REQUIRE_NOTHROW(val = vec.at(0));
    REQUIRE_NOTHROW(val = vec.at(COUNT - 1));

    REQUIRE_THROWS_AS(val = vec.at(COUNT), Etl::OutOfRangeException);
    REQUIRE_THROWS_AS(val = vec.at(COUNT + 100), Etl::OutOfRangeException);
}

#endif


TEST_CASE("Etl::Dynamic::Vector<> test cleanup", "[vec][dynamic][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}



// Etl::Static::Vector tests --------------------------------------------------


TEST_CASE("Etl::Static::Vector<> size/capacity test", "[vec][static][etl][basic]") {

    static const uint32_t CAPACITY = 16;

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, CAPACITY> VecT;

    CAPTURE(CAPACITY);

    VecT vec;

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == CAPACITY);

    vec.reserve(16);
    REQUIRE(vec.capacity() == CAPACITY);
    REQUIRE(vec.size() == 0);

    vec.push_back(1);
    vec.push_back(2);
    REQUIRE(vec.size() == 2);

    vec.shrink_to_fit();
    REQUIRE(vec.capacity() == CAPACITY);

    vec.reserve(5);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() == CAPACITY);

    uint32_t newSize = vec.size() + 3;

    vec.resize(newSize);
    REQUIRE(vec.size() == newSize);
    REQUIRE(vec.capacity() == CAPACITY);

    size_t oldSize = vec.size();
    newSize = CAPACITY + 3;

    vec.resize(newSize);
    REQUIRE(vec.size() == oldSize);
    REQUIRE(vec.capacity() == CAPACITY);

    vec.resize(2);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() == CAPACITY);

    vec.clear();
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == CAPACITY);
}


TEST_CASE("Etl::Static::Vector<> full push test", "[vec][static][etl][basic]") {

    static const uint32_t CAPACITY = 4;

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, CAPACITY> VecT;

    VecT vec;

    vec.insert(vec.begin(), 4, 1);

    CHECK(vec.size() == CAPACITY);

    vec.push_back(5);

    REQUIRE(vec.size() == CAPACITY);
}


TEST_CASE("Etl::Static::Vector<> constructor test", "[vec][static][etl][basic]") {

    static const uint32_t CAPACITY = 16;
    static const uint32_t CAPACITY4 = 20;

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, CAPACITY> VecT;
    typedef Etl::Static::Vector<ItemT, CAPACITY4> VecT4;

    static const ItemT INIT_VALUE = 123;

    VecT vec1;
    REQUIRE(vec1.size() == 0);
    REQUIRE(vec1.capacity() == CAPACITY);

    VecT vec2(4);
    REQUIRE(vec2.size() == 4);
    REQUIRE(vec2.capacity() == CAPACITY);

    CAPTURE(INIT_VALUE);
    VecT vec3(4, INIT_VALUE);
    REQUIRE(vec3.size() == 4);
    REQUIRE(vec3.capacity() == CAPACITY);
    REQUIRE(vec3[0] == INIT_VALUE);
    REQUIRE(vec3[3] == INIT_VALUE);

    SECTION("Copy from same size") {

        VecT vec4(vec3);
        REQUIRE(vec4.size() == vec3.size());
        REQUIRE(vec4.capacity() == CAPACITY);
        REQUIRE(vec4[0] == vec3[0]);
        REQUIRE(vec4[3] == vec3[3]);
        REQUIRE(vec4.begin() != vec3.begin());
    }

    SECTION("Copy from different size") {

        VecT4 vec4(vec3);
        REQUIRE(vec4.size() == vec3.size());
        REQUIRE(vec4.capacity() == CAPACITY4);
        REQUIRE(vec4[0] == vec3[0]);
        REQUIRE(vec4[3] == vec3[3]);
        REQUIRE(vec4.begin() != vec3.begin());

        VecT vec5(vec4);
        REQUIRE(vec5.size() == vec4.size());
        REQUIRE(vec5.capacity() == CAPACITY);
        REQUIRE(vec5[0] == vec4[0]);
        REQUIRE(vec5[3] == vec4[3]);
        REQUIRE(vec5.begin() != vec4.begin());
    }
}


TEST_CASE("Etl::Static::Vector<> test cleanup", "[vec][static][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}


// Etl::Vector comparision tests -------------------------------------------


TEST_CASE("Etl::Vector<> comparision", "[vector][etl]") {

    SECTION("Etl::Vector<> vs Etl::Vector<>") {

        Etl::Dynamic::Vector<int> lhs;
        Etl::Dynamic::Vector<int> rhs;

        auto inserter = [](Etl::Vector<int>& cont, int val) {
            cont.push_back(val);
        };

        testComparision(static_cast<Etl::Vector<int>&>(lhs),
                        static_cast<Etl::Vector<int>&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::Vector<> vs Etl::Static::Vector<>") {

        Etl::Dynamic::Vector<int> lhs;
        Etl::Static::Vector<int, 32U> rhs;

        auto lInserter = [](Etl::Dynamic::Vector<int>& cont, int val) {
            cont.push_back(val);
        };

        auto rInserter = [](Etl::Static::Vector<int, 32U>& cont, int val) {
            cont.push_back(val);
        };

        testComparision(lhs,
                        rhs,
                        lInserter,
                        rInserter);
    }
}


// Etl::Vector compatibility tests ---------------------------------------------


TEST_CASE("Etl::Vector<> with std::accumulate()", "[vec][comp][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testAccumulate<VecT>();
}

TEST_CASE("Etl::Vector<> with std::partial_sum()", "[vec][comp][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testPartialSum<VecT>();
}

TEST_CASE("Etl::Vector<> with std::inner_product()", "[vec][comp][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testInnerProduct<VecT, VecT>();
}

