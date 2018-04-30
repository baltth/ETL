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

using ETL_NAMESPACE::Test::UnalignedTester;
using ETL_NAMESPACE::Test::ContainerTester;


// Etl::Vector tests ----------------------------------------------------------


template<class VecT>
void testVectorBasic() {

    VecT vec;

    REQUIRE(vec.isEmpty());
    REQUIRE(vec.getSize() == 0);

    vec.reserve(16);
    REQUIRE(vec.getCapacity() >= 16);
    REQUIRE(vec.getSize() == 0);

    vec.pushBack(1);

    REQUIRE_FALSE(vec.isEmpty());
    REQUIRE(vec.getSize() == 1);

    vec.pushBack(2);

    REQUIRE_FALSE(vec.isEmpty());
    REQUIRE(vec.getSize() == 2);

    typename VecT::Iterator it = vec.begin();
    REQUIRE(*it == 1);
    REQUIRE(vec[0] == *it);

    ++it;
    *it = 3;

    vec.insert(vec.begin(), 11);
    REQUIRE(vec[0] == 11);
    vec.popFront();
    vec.erase(vec.begin());

    REQUIRE(*vec.begin() == 3);
    REQUIRE(vec.getSize() == 1);
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


template<class VecT>
void testVectorPushAndPop() {

    typedef typename VecT::ItemType ItemT;

    static const ItemT itemBack1 = 1;
    static const ItemT itemBack2 = 2;
    static const ItemT itemFront1 = 3;
    static const ItemT itemFront2 = 4;

    VecT vec;

    vec.pushBack(itemBack1);
    vec.pushBack(itemBack2);
    vec.pushFront(itemFront1);
    vec.pushFront(itemFront2);

    REQUIRE(vec.getSize() == 4);

    REQUIRE(vec[0] == itemFront2);
    REQUIRE(vec[1] == itemFront1);
    REQUIRE(vec[2] == itemBack1);
    REQUIRE(vec[3] == itemBack2);

    REQUIRE(vec.back() == itemBack2);
    vec.popBack();
    REQUIRE(vec.back() == itemBack1);

    REQUIRE(vec.front() == itemFront2);
    vec.popFront();
    REQUIRE(vec.front() == itemFront1);

    vec.popBack();
    vec.popFront();

    REQUIRE(vec.getSize() == 0);
}

TEST_CASE("Etl::Dynamic::Vector<> push/pop test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorPushAndPop<VecT>();
}

TEST_CASE("Etl::Static::Vector<> push/pop test", "[vec][static][etl][basic]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorPushAndPop<VecT>();
}


template<class VecT>
void testVectorInsertAndErase() {

    VecT vec(4, 0);

    CHECK(vec.getSize() == 4);

    SECTION("insert(ConstIterator, uint32_t, const T&)") {

        typename VecT::Iterator it = vec.begin() + 2;
        it = vec.insert(it, 2);
        REQUIRE(vec[2] == 2);
        REQUIRE(vec.getSize() == 5);
        REQUIRE(it == &vec[2]);

        ++it;
        it = vec.insert(it, 2, 3);
        REQUIRE(vec[2] == 2);
        REQUIRE(vec[3] == 3);
        REQUIRE(vec[4] == 3);
        REQUIRE(vec.getSize() == 7);
        REQUIRE(it == &vec[3]);

        CHECK(vec[1] == 0);
        CHECK(vec[5] == 0);

        it = vec.begin() + 2;
        it = vec.erase(it);
        REQUIRE(vec.getSize() == 6);
        REQUIRE(vec[2] == 3);
        REQUIRE(it == &vec[2]);

        it = vec.erase(it, it + 2);
        REQUIRE(vec.getSize() == 4);
        REQUIRE(vec[2] == 0);
        REQUIRE(it == &vec[2]);
    }

    SECTION("insert(ConstIterator, InputIt, InputIt)") {

        vec[0] = 1;
        vec[1] = 2;
        vec[2] = 3;
        vec[3] = 4;

        VecT vec2(1, 0);
        CHECK(vec2.getSize() == 1);

        SECTION("Correct insert") {

            CHECK(vec.getSize() == 4);

            typename VecT::ConstIterator last = vec.end() - 1;
            typename VecT::Iterator it = vec2.insert(vec2.end(), vec.begin(), last);

            REQUIRE(vec2.getSize() == 4);
            REQUIRE(it == &vec2[1]);

            REQUIRE(vec2[0] == 0);
            REQUIRE(vec2[1] == 1);
            REQUIRE(vec2[2] == 2);
            REQUIRE(vec2[3] == 3);
        }

        SECTION("Invalid insert") {

            typename VecT::Iterator it = vec2.insert(vec2.end(), vec.end(), vec.begin());

            REQUIRE(vec2.getSize() == 1);
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
void testVectorFind() {

    typedef typename VecT::ItemType ItemT;

    class IntMatcher : public Etl::Matcher<ItemT> {
        const ItemT value;
      public:
        IntMatcher(ItemT val) :
            value(val) {};

        virtual bool call(const ItemT& ref) const OVERRIDE {
            return value == ref;
        }
    };

    static const ItemT REF_VALUE = 123;

    CAPTURE(REF_VALUE);

    VecT vec;
    vec.pushBack(1);
    vec.pushBack(2);
    vec.pushBack(REF_VALUE);
    typename VecT::Iterator it1 = vec.end() - 1;
    vec.pushBack(4);
    vec.pushBack(REF_VALUE);
    typename VecT::Iterator it2 = vec.end() - 1;
    vec.pushBack(6);

    typename VecT::Iterator found = vec.find(IntMatcher(REF_VALUE));
    REQUIRE(found == it1);
    found = vec.find((++found), vec.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == it2);
    found = vec.find((++found), vec.end(), IntMatcher(REF_VALUE));
    REQUIRE(found == vec.end());
}

TEST_CASE("Etl::Dynamic::Vector<>::find(Etl::Matcher<>) test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorFind<VecT>();
}

TEST_CASE("Etl::Static::Vector<>::find(Etl::Matcher<>) test", "[vec][static][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorFind<VecT>();
}


template<class VecT>
void testVectorAssignment() {

    typedef typename VecT::ItemType ItemT;

    static const int PATTERN1 = 123;
    static const int PATTERN2 = 321;

    VecT vec1(4, ContainerTester(PATTERN1));
    VecT vec2(8, ContainerTester(PATTERN2));

    CHECK(vec1[0] != vec2[0]);
    CHECK(vec1.getSize() != vec2.getSize());

    REQUIRE(ItemT::getObjectCount() == (vec1.getSize() + vec2.getSize()));

    vec1 = vec2;

    REQUIRE(vec1[0] == vec2[0]);
    REQUIRE(vec1.begin() != vec2.begin());
    REQUIRE(vec1.getSize() == vec2.getSize());
    REQUIRE(ItemT::getObjectCount() == (2 * vec2.getSize()));
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
    vec1.pushBack(1);
    vec1.pushBack(2);
    vec1.pushBack(3);

    VecT2 vec2;
    vec2.pushBack(-1);
    vec2.pushBack(-2);
    vec2.pushBack(-3);

    CHECK(vec1.getSize() == 3);
    CHECK(vec2.getSize() == 3);

    SECTION("Swap ego") {

        REQUIRE_FALSE(vec1.swap(vec1));
        REQUIRE(vec1.getSize() == 3);

        REQUIRE(vec1[0] == 1);
        REQUIRE(vec1[1] == 2);
        REQUIRE(vec1[2] == 3);
    }

    SECTION("Swap equal length") {

        REQUIRE(vec1.swap(vec2));

        REQUIRE(vec1.getSize() == 3);
        REQUIRE(vec2.getSize() == 3);

        REQUIRE(vec1[0] == -1);
        REQUIRE(vec1[1] == -2);
        REQUIRE(vec1[2] == -3);

        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
    }

    SECTION("Swap different length") {

        vec2.pushBack(-4);
        vec2.pushBack(-5);

        CHECK(vec2.getSize() == 5);

        REQUIRE(vec1.swap(vec2));

        REQUIRE(vec1.getSize() == 5);
        REQUIRE(vec2.getSize() == 3);

        REQUIRE(vec1[0] == -1);
        REQUIRE(vec1[1] == -2);
        REQUIRE(vec1[2] == -3);
        REQUIRE(vec1[3] == -4);
        REQUIRE(vec1[4] == -5);

        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);

        REQUIRE(vec1.swap(vec2));

        REQUIRE(vec1.getSize() == 3);
        REQUIRE(vec2.getSize() == 5);

        REQUIRE(vec1[0] == 1);
        REQUIRE(vec1[2] == 3);

        REQUIRE(vec2[0] == -1);
        REQUIRE(vec2[4] == -5);
    }

    SECTION("Swap empty") {

        VecT2 vec3;

        CHECK(vec3.isEmpty());

        REQUIRE(vec1.swap(vec3));

        REQUIRE(vec1.isEmpty());
        REQUIRE(vec3.getSize() == 3);

        REQUIRE(vec3[0] == 1);
        REQUIRE(vec3[1] == 2);
        REQUIRE(vec3[2] == 3);

        REQUIRE(vec1.swap(vec3));

        REQUIRE(vec1.getSize() == 3);
        REQUIRE(vec3.isEmpty());

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

        vec.pushBack(ContainerTester(1));
        vec.pushBack(ContainerTester(2));

        VecT2 vec2;

        vec2.pushBack(ContainerTester(-1));

        const uint32_t copyCnt = ContainerTester::getCopyCount();

        REQUIRE(vec.swap(vec2));

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

        CHECK(vec1.getSize() == vec2.getMaxCapacity());

        SECTION("Allow at max capacity") {

            SECTION("1->2") {

                REQUIRE(vec1.swap(vec2));

                REQUIRE(vec1.isEmpty());
                REQUIRE(vec2.getSize() == 4);
                REQUIRE(vec2[0] == 1);
                REQUIRE(vec2[3] == 1);

                REQUIRE(vec1.swap(vec2));

                REQUIRE(vec1.getSize() == 4);
                REQUIRE(vec2.isEmpty());
                REQUIRE(vec1[0] == 1);
                REQUIRE(vec1[3] == 1);
            }

            SECTION("2->1") {

                REQUIRE(vec2.swap(vec1));

                REQUIRE(vec1.isEmpty());
                REQUIRE(vec2.getSize() == 4);
                REQUIRE(vec2[0] == 1);
                REQUIRE(vec2[3] == 1);

                REQUIRE(vec2.swap(vec1));

                REQUIRE(vec1.getSize() == 4);
                REQUIRE(vec2.isEmpty());
                REQUIRE(vec1[0] == 1);
                REQUIRE(vec1[3] == 1);
            }
        }

        SECTION("Disallow above max capacity") {

            vec1.pushBack(2);
            CHECK(vec1.getSize() > vec2.getMaxCapacity());

            SECTION("1->2") {
                REQUIRE_FALSE(vec1.swap(vec2));
            }

            SECTION("2->1") {
                REQUIRE_FALSE(vec2.swap(vec1));
            }
        }
    }
}


template<class VecT>
void testVectorLeak() {

    typedef typename VecT::ItemType ItemT;

    static const int PATTERN = 123;

    CHECK(ItemT::getObjectCount() == 0);
    if (ItemT::getObjectCount() == 0) {

        VecT vec(8, ContainerTester(PATTERN));
        CHECK(vec.getSize() == ItemT::getObjectCount());

        vec.popBack();
        REQUIRE(vec.getSize() == ItemT::getObjectCount());

        vec.erase(vec.begin());
        REQUIRE(vec.getSize() == ItemT::getObjectCount());

        vec.erase((vec.begin() + 1), (vec.begin() + 3));
        REQUIRE(vec.getSize() == ItemT::getObjectCount());
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

    REQUIRE(vec.getSize() == 0);

    vec.reserve(16);
    REQUIRE(vec.getCapacity() >= 16);
    REQUIRE(vec.getSize() == 0);

    int a = 1;
    int b = 2;
    int c = 3;

    vec.pushBack(&a);
    vec.pushBack(&b);

    REQUIRE(vec.getSize() == 2);

    typename VecT::Iterator it = vec.begin();
    REQUIRE(*it == &a);
    REQUIRE(vec[0] == *it);

    ++it;
    *it = &b;

    vec.insert(vec.begin(), &c);
    REQUIRE(vec[0] == &c);
    vec.popFront();
    vec.erase(vec.begin());

    REQUIRE(*vec.begin() == &b);
    REQUIRE(vec.getSize() == 1);
}

TEST_CASE("Etl::Dynamic::Vector<T*> test", "[vec][dynamic][etl][basic]") {

    typedef int* ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorWithPtrItem<VecT>();
}

TEST_CASE("Etl::Static::Vector<T*> test", "[vec][static][etl][basic]") {

    typedef int* ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorWithPtrItem<VecT>();
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
    CHECK(sVec.getSize() != dVec.getSize());

    REQUIRE(ItemT::getObjectCount() == (sVec.getSize() + dVec.getSize()));

    SECTION("S = D assignment") {

        sVec = dVec;

        REQUIRE(sVec[0] == dVec[0]);
        REQUIRE(sVec.getSize() == dVec.getSize());
        REQUIRE(ItemT::getObjectCount() == (2 * dVec.getSize()));
    }

    SECTION("D = S assignment") {

        dVec = sVec;

        REQUIRE(dVec[0] == sVec[0]);
        REQUIRE(dVec.getSize() == sVec.getSize());
        REQUIRE(ItemT::getObjectCount() == (2 * sVec.getSize()));
    }

    SECTION("S(D) construction") {

        SVecT sVec2(dVec);
        REQUIRE(sVec2[0] == dVec[0]);
        REQUIRE(sVec2.getSize() == dVec.getSize());
    }

    SECTION("D(S) construction") {

        DVecT dVec2(sVec);
        REQUIRE(dVec2[0] == sVec[0]);
        REQUIRE(dVec2.getSize() == sVec.getSize());
    }

    SECTION("Swap") {

        const uint32_t sInitSize = sVec.getSize();
        const uint32_t dInitSize = dVec.getSize();

        REQUIRE(sVec.swap(dVec));

        REQUIRE(sVec.getSize() == dInitSize);
        REQUIRE(dVec.getSize() == sInitSize);

        REQUIRE(sVec.front() == ContainerTester(PATTERN_D));
        REQUIRE(sVec.back() == ContainerTester(PATTERN_D));
        REQUIRE(dVec.front() == ContainerTester(PATTERN_S));
        REQUIRE(dVec.back() == ContainerTester(PATTERN_S));

        REQUIRE(dVec.swap(sVec));

        REQUIRE(sVec.getSize() == sInitSize);
        REQUIRE(dVec.getSize() == dInitSize);

        REQUIRE(sVec.front() == ContainerTester(PATTERN_S));
        REQUIRE(sVec.back() == ContainerTester(PATTERN_S));
        REQUIRE(dVec.front() == ContainerTester(PATTERN_D));
        REQUIRE(dVec.back() == ContainerTester(PATTERN_D));
    }
}


// Etl::Vector tests - C++11 --------------------------------------------------

#if ETL_USE_CPP11


template<class VecT>
void testVectorWithInitList() {

    SECTION("Construct") {

        SECTION("With paren") {

            VecT vec({1, 2});

            REQUIRE(vec.getSize() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }

        SECTION("With double braces") {

            VecT vec{{1, 2}};

            REQUIRE(vec.getSize() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }

        SECTION("Uniform") {

            VecT vec{1, 2};

            REQUIRE(vec.getSize() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }
    }

    SECTION("Assign") {

        VecT vec({4, 5, 6, 7});
        CHECK(vec.getSize() == 4);

        SECTION("Empty") {

            vec = {};

            REQUIRE(vec.getSize() == 0);
        }

        SECTION("Non-empty") {

            vec = {1, 2};

            REQUIRE(vec.getSize() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }
    }
}

TEST_CASE("Etl::Dynamic::Vector<> with std::initializer_list<>", "[vec][dynamic][etl][cpp11]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorWithInitList<VecT>();
}

TEST_CASE("Etl::Static::Vector<> with std::initializer_list<>", "[vec][static][etl][cpp11]") {

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorWithInitList<VecT>();
}


template<class VecT>
void testVectorEmplace() {

    auto moveCnt = ContainerTester::getMoveCount();
    auto copyCnt = ContainerTester::getCopyCount();

    VecT vec;

    SECTION("emplaceBack()") {

        vec.emplaceBack(-1);
        vec.emplaceBack(-2);
        vec.emplaceBack(-3);

        REQUIRE(vec.getSize() == 3);
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

        REQUIRE(vec.getSize() == 3);
        REQUIRE(vec[0] == ContainerTester(-4));
        REQUIRE(vec[1] == ContainerTester(-5));
        REQUIRE(vec[2] == ContainerTester(-6));
    }
}

TEST_CASE("Etl::Dynamic::Vector<> emplace test", "[vec][dynamic][etl][cpp11]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorEmplace<VecT>();
}

TEST_CASE("Etl::Static::Vector<> emplace test", "[vec][static][etl][cpp11]") {

    typedef ContainerTester ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorEmplace<VecT>();
}


template<class VecT>
void testVectorMove() {

    VecT vec;
    vec.pushBack(ContainerTester(-1));
    vec.pushBack(ContainerTester(-2));
    vec.pushBack(ContainerTester(-3));

    const auto size = vec.getSize();

    SECTION("Move constructor") {

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        //ContainerTester::enablePrint = true;
        VecT vec2(std::move(vec));
        //ContainerTester::enablePrint = false;

        REQUIRE(vec2.getSize() == size);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
        REQUIRE(ContainerTester::getMoveCount() >= moveCnt);
        REQUIRE(vec2[0] == ContainerTester(-1));
        REQUIRE(vec2[2] == ContainerTester(-3));
    }

    SECTION("Move assignment") {

        VecT vec2(2, ContainerTester(10));

        CHECK(vec2.getSize() != size);

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        vec2 = std::move(vec);

        REQUIRE(vec2.getSize() == size);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
        REQUIRE(ContainerTester::getMoveCount() >= moveCnt);
        REQUIRE(vec2[0] == ContainerTester(-1));
        REQUIRE(vec2[2] == ContainerTester(-3));
    }
}

TEST_CASE("Etl::Dynamic::Vector<> move test", "[vec][dynamic][etl][cpp11]") {

    typedef ContainerTester ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    testVectorMove<VecT>();
}

TEST_CASE("Etl::Static::Vector<> move test", "[vec][static][etl][cpp11]") {

    typedef ContainerTester ItemT;
    typedef Etl::Static::Vector<ItemT, 16> VecT;

    testVectorMove<VecT>();
}

#endif


TEST_CASE("Vector<> test cleanup", "[vec][static][dynamic][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}


// Etl::Dynamic::Vector tests -------------------------------------------------


TEST_CASE("Etl::Dynamic::Vector<> size/capacity test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    VecT vec;

    REQUIRE(vec.getSize() == 0);
    REQUIRE(vec.getCapacity() == 0);

    vec.reserve(16);
    REQUIRE(vec.getCapacity() >= 16);
    REQUIRE(vec.getSize() == 0);

    vec.pushBack(1);
    vec.pushBack(2);
    REQUIRE(vec.getSize() == 2);

    vec.shrinkToFit();
    REQUIRE(vec.getCapacity() == 2);

    vec.reserveAtLeast(5);
    REQUIRE(vec.getSize() == 2);
    REQUIRE(vec.getCapacity() >= 5);

    uint32_t newSize = vec.getSize() + 3;

    vec.resize(newSize);
    REQUIRE(vec.getSize() == newSize);

    uint32_t capacity = vec.getCapacity();
    newSize = capacity + 3;

    vec.resize(newSize);
    REQUIRE(vec.getSize() == newSize);
    REQUIRE(vec.getCapacity() >= newSize);

    vec.resize(2);
    REQUIRE(vec.getSize() == 2);
    REQUIRE(vec.getCapacity() >= 2);

    capacity = vec.getCapacity();
    vec.clear();
    REQUIRE(vec.getSize() == 0);
    REQUIRE(vec.getCapacity() == capacity);
}


TEST_CASE("Etl::Dynamic::Vector<> constructor test", "[vec][dynamic][etl]") {

    typedef int ItemT;
    typedef Etl::Dynamic::Vector<ItemT> VecT;

    static const ItemT INIT_VALUE = 123;

    VecT vec1;
    REQUIRE(vec1.getSize() == 0);
    REQUIRE(vec1.getCapacity() == 0);

    VecT vec2(4);
    REQUIRE(vec2.getSize() == 4);
    REQUIRE(vec2.getCapacity() >= 4);

    CAPTURE(INIT_VALUE);
    VecT vec3(4, INIT_VALUE);
    REQUIRE(vec3.getSize() == 4);
    REQUIRE(vec3.getCapacity() >= 4);
    REQUIRE(vec3[0] == INIT_VALUE);
    REQUIRE(vec3[3] == INIT_VALUE);

    VecT vec4(vec3);
    REQUIRE(vec4.getSize() == vec3.getSize());
    REQUIRE(vec4.getCapacity() >= vec4.getSize());
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

    REQUIRE(vec.getSize() == 0);
    REQUIRE(vec.getCapacity() == CAPACITY);

    vec.reserve(16);
    REQUIRE(vec.getCapacity() == CAPACITY);
    REQUIRE(vec.getSize() == 0);

    vec.pushBack(1);
    vec.pushBack(2);
    REQUIRE(vec.getSize() == 2);

    vec.shrinkToFit();
    REQUIRE(vec.getCapacity() == CAPACITY);

    vec.reserveAtLeast(5);
    REQUIRE(vec.getSize() == 2);
    REQUIRE(vec.getCapacity() == CAPACITY);

    uint32_t newSize = vec.getSize() + 3;

    vec.resize(newSize);
    REQUIRE(vec.getSize() == newSize);
    REQUIRE(vec.getCapacity() == CAPACITY);

    size_t oldSize = vec.getSize();
    newSize = CAPACITY + 3;

    vec.resize(newSize);
    REQUIRE(vec.getSize() == oldSize);
    REQUIRE(vec.getCapacity() == CAPACITY);

    vec.resize(2);
    REQUIRE(vec.getSize() == 2);
    REQUIRE(vec.getCapacity() == CAPACITY);

    vec.clear();
    REQUIRE(vec.getSize() == 0);
    REQUIRE(vec.getCapacity() == CAPACITY);
}


TEST_CASE("Etl::Static::Vector<> full push test", "[vec][static][etl][basic]") {

    static const uint32_t CAPACITY = 4;

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, CAPACITY> VecT;

    VecT vec;

    vec.insert(vec.begin(), 4, 1);

    CHECK(vec.getSize() == CAPACITY);

    vec.pushBack(5);

    REQUIRE(vec.getSize() == CAPACITY);
}


TEST_CASE("Etl::Static::Vector<> constructor test", "[vec][static][etl][basic]") {

    static const uint32_t CAPACITY = 16;
    static const uint32_t CAPACITY4 = 20;

    typedef int ItemT;
    typedef Etl::Static::Vector<ItemT, CAPACITY> VecT;
    typedef Etl::Static::Vector<ItemT, CAPACITY4> VecT4;

    static const ItemT INIT_VALUE = 123;

    VecT vec1;
    REQUIRE(vec1.getSize() == 0);
    REQUIRE(vec1.getCapacity() == CAPACITY);

    VecT vec2(4);
    REQUIRE(vec2.getSize() == 4);
    REQUIRE(vec2.getCapacity() == CAPACITY);

    CAPTURE(INIT_VALUE);
    VecT vec3(4, INIT_VALUE);
    REQUIRE(vec3.getSize() == 4);
    REQUIRE(vec3.getCapacity() == CAPACITY);
    REQUIRE(vec3[0] == INIT_VALUE);
    REQUIRE(vec3[3] == INIT_VALUE);

    SECTION("Copy from same size") {

        VecT vec4(vec3);
        REQUIRE(vec4.getSize() == vec3.getSize());
        REQUIRE(vec4.getCapacity() == CAPACITY);
        REQUIRE(vec4[0] == vec3[0]);
        REQUIRE(vec4[3] == vec3[3]);
        REQUIRE(vec4.begin() != vec3.begin());
    }

    SECTION("Copy from different size") {

        VecT4 vec4(vec3);
        REQUIRE(vec4.getSize() == vec3.getSize());
        REQUIRE(vec4.getCapacity() == CAPACITY4);
        REQUIRE(vec4[0] == vec3[0]);
        REQUIRE(vec4[3] == vec3[3]);
        REQUIRE(vec4.begin() != vec3.begin());

        VecT vec5(vec4);
        REQUIRE(vec5.getSize() == vec4.getSize());
        REQUIRE(vec5.getCapacity() == CAPACITY);
        REQUIRE(vec5[0] == vec4[0]);
        REQUIRE(vec5[3] == vec4[3]);
        REQUIRE(vec5.begin() != vec4.begin());
    }
}


TEST_CASE("Etl::Static::Vector<> test cleanup", "[vec][static][etl]") {

    CHECK(ContainerTester::getObjectCount() == 0);
}

