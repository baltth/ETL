/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017-2024 Balazs Toth.

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

#include <catch2/catch.hpp>

#include <iterator>

#include <etl/Array.h>
#include <etl/Vector.h>

#include "AtScopeEnd.h"
#include "ContainerTester.h"
#include "DummyAllocator.h"
#include "UnalignedTester.h"
#include "comparisionTests.h"
#include "compatibilityTests.h"
#include "constructorTests.h"
#include "sequenceTests.h"
#include "swapTests.h"

using Etl::Test::UnalignedTester;
using Etl::Test::ContainerTester;
using Etl::Test::AtScopeEnd;

namespace {

namespace CompileTimeChecks {

using Etl::Detail::NothrowContract;

using SC = Etl::Static::Vector<int, 16U>;
using SCSC = Etl::Static::Vector<SC, 8U>;
using DC = Etl::Dynamic::Vector<int>;

TEMPLATE_TEST_CASE("Vector nothrow contract",
                   "[vector][etl][basic]",
                   SC,
                   SCSC,
                   DC) {

    static_assert(NothrowContract<TestType>::value, "nothrow contract violation");
    using std::swap;
    TestType c1;
    TestType c2;
    static_assert(noexcept(swap(c1, c2)), "swap() nothrow contract violation");
}

static_assert(std::is_same<std::iterator_traits<SC::iterator>::iterator_category,
                           std::random_access_iterator_tag>::value,
              "Wrong iterator category for Vector<>::iterator");

static_assert(std::is_same<std::iterator_traits<SC::const_iterator>::iterator_category,
                           std::random_access_iterator_tag>::value,
              "Wrong iterator category for Vector<>::const_iterator");

}  // namespace CompileTimeChecks


template<class Vec>
void testVectorBasic() {

    Vec vec;

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

    typename Vec::iterator it = vec.begin();
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


TEMPLATE_TEST_CASE("Etl::Vector<> basic test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<int>),
                   (Etl::Static::Vector<int, 16U>)) {

    testVectorBasic<TestType>();
}


TEMPLATE_TEST_CASE("Etl::Vector<> constructor test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<ContainerTester>),
                   (Etl::Static::Vector<ContainerTester, 16U>)) {

    SECTION("V(size_t)") {
        Etl::Test::constructForSize<TestType>();
    }

    SECTION("V(size_t, const value_type&)") {
        Etl::Test::constructForSizeAndValue<TestType>(ContainerTester {17});
    }

    SECTION("V(InputIt, IntputIt)") {
        Etl::Test::constructWithIterators<TestType>();
    }

    SECTION("V(std::initializer_list)") {
        Etl::Test::constructWithInitList<TestType>();
    }
}


TEMPLATE_TEST_CASE("Etl::Vector<> push/pop test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<int>),
                   (Etl::Static::Vector<int, 16U>)) {

    Etl::Test::testBackAccess<TestType>();
    Etl::Test::testFrontAccess<TestType>();
}


TEMPLATE_TEST_CASE("Etl::Vector<> iteration test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<int>),
                   (Etl::Static::Vector<int, 16U>)) {

    SECTION("iterator") {
        Etl::Test::testIterationForward<TestType>();
        Etl::Test::testIterationBackward<TestType>();
    }

    SECTION("reverse_iterator") {
        Etl::Test::testReverseIterationForward<TestType>();
        Etl::Test::testReverseIterationBackward<TestType>();
    }
}


template<class Vec>
void testVectorInsertAndErase() {

    SECTION("insert(const_iterator, uint32_t, const T&)") {

        Vec vec(4, 0);
        CHECK(vec.size() == 4);

        auto it = vec.begin() + 2;
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

        Vec vec = {1, 2, 3, 4};
        CHECK(vec.size() == 4);

        Vec vec2 = {0, -1, -2, -3};
        CHECK(vec2.size() == 4);

        auto last = vec.cend() - 1;
        auto pos = vec2.end() - 1;
        auto it = vec2.insert(pos, vec.cbegin(), last);

        REQUIRE(vec2.size() == 7);
        REQUIRE(it == &vec2[3]);

        REQUIRE(vec2[0] == 0);
        REQUIRE(vec2[1] == -1);
        REQUIRE(vec2[2] == -2);
        CHECK(vec2[3] == 1);
        CHECK(vec2[4] == 2);
        CHECK(vec2[5] == 3);
        CHECK(vec2[6] == -3);
    }

    SECTION("random insert(const_iterator, InputIt, InputIt)") {

        static constexpr int LIMIT = 100;
        int cntToInsert = GENERATE(take(3, random(0, LIMIT)));
        int cntBefore = GENERATE(take(3, random(0, LIMIT)));

        CAPTURE(cntToInsert);
        CAPTURE(cntBefore);

        static constexpr int BASE = 5U;
        Vec dst(BASE, 0U);
        for (int i = 0; i < cntBefore; ++i) {
            dst.push_back(i + 1);
        }

        Vec src;
        for (int i = 0; i < cntToInsert; ++i) {
            src.push_back(-(i + 1));
        }

        CHECK(dst.size() == static_cast<size_t>(BASE + cntBefore));
        CHECK(src.size() == static_cast<size_t>(cntToInsert));

        auto pos = dst.begin() + BASE;
        dst.insert(pos, src.begin(), src.end());

        REQUIRE(dst.size() == static_cast<size_t>(BASE + cntBefore + cntToInsert));

        for (int i = 0; i < BASE; ++i) {
            REQUIRE(dst[i] == 0U);
        }

        for (int i = BASE; i < (BASE + cntToInsert); ++i) {
            REQUIRE(dst[i] == -(i - BASE + 1));
        }

        for (int i = (BASE + cntToInsert); i < static_cast<int>(dst.size()); ++i) {
            REQUIRE(dst[i] == (i - BASE - cntToInsert + 1));
        }
    }

    SECTION("insert(const_iterator, InputIt, InputIt) with foreign InputIt") {

        Etl::Array<typename Vec::value_type, 4U> in = {1, 2, 3, 4};

        Vec vec;
        vec.push_back(0);
        CHECK(vec.size() == 1);

        CHECK(in.size() == 4);

        auto last = in.cend();
        --last;
        auto it = vec.insert(vec.end(), in.cbegin(), last);

        REQUIRE(vec.size() == 4);
        REQUIRE(it == &vec[1]);

        REQUIRE(vec[0] == 0);
        REQUIRE(vec[1] == 1);
        REQUIRE(vec[2] == 2);
        REQUIRE(vec[3] == 3);
    }
}


TEMPLATE_TEST_CASE("Etl::Vector<> insert/erase test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<int>),
                   (Etl::Static::Vector<int, 256U>)) {

    testVectorInsertAndErase<TestType>();
}


template<class Vec>
void testVectorAssignment() {

    using Item = typename Vec::value_type;

    static const int PATTERN1 = 123;
    static const int PATTERN2 = 321;

    Vec vec1(4, ContainerTester(PATTERN1));
    Vec vec2(8, ContainerTester(PATTERN2));

    CHECK(vec1[0] != vec2[0]);
    CHECK(vec1.size() != vec2.size());

    REQUIRE(Item::getObjectCount() == (vec1.size() + vec2.size()));

    vec1 = vec2;

    REQUIRE(vec1[0] == vec2[0]);
    REQUIRE(vec1.begin() != vec2.begin());
    REQUIRE(vec1.size() == vec2.size());
    REQUIRE(Item::getObjectCount() == (2 * vec2.size()));
}


TEMPLATE_TEST_CASE("Etl::Vector<> assignment test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<ContainerTester>),
                   (Etl::Static::Vector<ContainerTester, 16U>)) {

    testVectorAssignment<TestType>();
}


template<class VecT>
void testSwapIsNoCopy() {
    SECTION("Swap is no-copy") {

        VecT vec;

        vec.push_back(ContainerTester(1));
        vec.push_back(ContainerTester(2));

        VecT vec2;

        vec2.push_back(ContainerTester(-1));

        const uint32_t copyCnt = ContainerTester::getCopyCount();

        vec.swap(vec2);

        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
    }
}


TEST_CASE("Etl::Vector<> swap", "[vector][etl]") {

    using Etl::Test::NonAssignable;

    using SIC = Etl::Static::Vector<int, 4>;
    using DIC = Etl::Dynamic::Vector<int>;

    auto insert = [](Etl::Vector<int>& vec, int v) { vec.push_back(v); };

    SECTION("self: Static") {
        using Self = SIC;

        SECTION("other: Static") {
            Etl::Test::testSwapOrdered<Self, SIC>(insert);
        }

        SECTION("other: Dynamic") {
            Etl::Test::testSwapOrdered<Self, DIC>(insert);
        }
    }

    SECTION("self: Dynamic") {
        using Self = DIC;

        SECTION("other: Static") {
            Etl::Test::testSwapOrdered<Self, SIC>(insert);
        }

        SECTION("other: Dynamic") {
            Etl::Test::testSwapOrdered<Self, DIC>(insert);
        }
    }

    testSwapIsNoCopy<Etl::Dynamic::Vector<ContainerTester>>();
}


template<class Vec>
void testVectorLeak() {

    using Item = typename Vec::value_type;

    static const int PATTERN = 123;

    CHECK(Item::getObjectCount() == 0);
    if (Item::getObjectCount() == 0) {

        Vec vec(8, ContainerTester(PATTERN));
        CHECK(vec.size() == Item::getObjectCount());

        vec.pop_back();
        REQUIRE(vec.size() == Item::getObjectCount());

        vec.erase(vec.begin());
        REQUIRE(vec.size() == Item::getObjectCount());

        vec.erase((vec.begin() + 1), (vec.begin() + 3));
        REQUIRE(vec.size() == Item::getObjectCount());
    }

    REQUIRE(Item::getObjectCount() == 0);
}


TEMPLATE_TEST_CASE("Etl::Vector<> leak test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<ContainerTester>),
                   (Etl::Static::Vector<ContainerTester, 16U>)) {

    testVectorLeak<TestType>();
}


template<class Vec>
void testVectorWithPtrItem() {

    Vec vec;

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

    typename Vec::iterator it = vec.begin();
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


TEMPLATE_TEST_CASE("Etl::Vector<T*> test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<int*>),
                   (Etl::Dynamic::Vector<const int*>),
                   (Etl::Static::Vector<int*, 16U>),
                   (Etl::Static::Vector<const int*, 16U>)) {

    testVectorWithPtrItem<TestType>();
}


TEST_CASE("Vector<> - Static-Dynamic interop test", "[vec][static][dynamic][etl]") {

    using Item = ContainerTester;
    using SVecT = Etl::Static::Vector<Item, 16>;
    using DVecT = Etl::Dynamic::Vector<Item>;

    static const int PATTERN_S = 123;
    static const int PATTERN_D = 321;

    SVecT sVec(4, ContainerTester(PATTERN_S));
    DVecT dVec(8, ContainerTester(PATTERN_D));

    CHECK(sVec[0] != dVec[0]);
    CHECK(sVec.size() != dVec.size());

    REQUIRE(Item::getObjectCount() == (sVec.size() + dVec.size()));

    SECTION("S = D assignment") {

        sVec = dVec;

        REQUIRE(sVec[0] == dVec[0]);
        REQUIRE(sVec.size() == dVec.size());
        REQUIRE(Item::getObjectCount() == (2 * dVec.size()));
    }

    SECTION("D = S assignment") {

        dVec = sVec;

        REQUIRE(dVec[0] == sVec[0]);
        REQUIRE(dVec.size() == sVec.size());
        REQUIRE(Item::getObjectCount() == (2 * sVec.size()));
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


template<class Vec>
void testVectorWithInitList() {

    SECTION("Construct") {

        SECTION("With paren") {

            Vec vec({1, 2});

            REQUIRE(vec.size() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }

        SECTION("With double braces") {

            Vec vec {{1, 2}};

            REQUIRE(vec.size() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }

        SECTION("Uniform") {

            Vec vec {1, 2};

            REQUIRE(vec.size() == 2);
            REQUIRE(vec[0] == 1);
            REQUIRE(vec[1] == 2);
        }
    }

    SECTION("Assign") {

        Vec vec({4, 5, 6, 7});
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


TEMPLATE_TEST_CASE("Etl::Vector<> with std::initializer_list<>",
                   "[vec][etl]",
                   (Etl::Dynamic::Vector<int>),
                   (Etl::Static::Vector<int, 16U>)) {

    testVectorWithInitList<TestType>();
}


template<class Vec>
void testVectorEmplace() {

    auto moveCnt = ContainerTester::getMoveCount();
    auto copyCnt = ContainerTester::getCopyCount();

    Vec vec;

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


TEMPLATE_TEST_CASE("Etl::Vector<> emplace test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<ContainerTester>),
                   (Etl::Static::Vector<ContainerTester, 16U>)) {

    testVectorEmplace<TestType>();
}


template<class Vec>
void testVectorMove() {

    Vec vec;
    vec.push_back(ContainerTester(-1));
    vec.push_back(ContainerTester(-2));
    vec.push_back(ContainerTester(-3));

    const auto size = vec.size();

    SECTION("Move constructor") {

        const auto copyCnt = ContainerTester::getCopyCount();
        const auto moveCnt = ContainerTester::getMoveCount();

        // ContainerTester::enablePrint = true;
        Vec vec2(std::move(vec));
        // ContainerTester::enablePrint = false;

        REQUIRE(vec2.size() == size);
        REQUIRE(ContainerTester::getCopyCount() == copyCnt);
        REQUIRE(ContainerTester::getMoveCount() >= moveCnt);
        REQUIRE(vec2[0] == ContainerTester(-1));
        REQUIRE(vec2[2] == ContainerTester(-3));
    }

    SECTION("Move assignment") {

        Vec vec2(2, ContainerTester(10));

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


TEMPLATE_TEST_CASE("Etl::Vector<> move test",
                   "[vec][etl][basic]",
                   (Etl::Dynamic::Vector<ContainerTester>),
                   (Etl::Static::Vector<ContainerTester, 16U>)) {

    testVectorMove<TestType>();
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

    using Item = ContainerTester;
    using DVecT = Etl::Dynamic::Vector<Item>;
    using SVecT = Etl::Static::Vector<Item, 16U>;

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


template<class Vec, class T = typename Vec::value_type>
void testVectorAssignFromBase(Etl::Vector<T>&& src) {

    Vec dst;

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

    using Item = ContainerTester;
    using DVecT = Etl::Dynamic::Vector<Item>;
    using SVecT = Etl::Static::Vector<Item, 16U>;

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

    using Item = int;
    using Vec = Etl::Dynamic::Vector<Item>;

    Vec vec;

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

    using Item = int;
    using Vec = Etl::Dynamic::Vector<Item>;

    static const Item INIT_VALUE = 123;

    Vec vec1;
    REQUIRE(vec1.size() == 0);
    REQUIRE(vec1.capacity() == 0);

    Vec vec2(4);
    REQUIRE(vec2.size() == 4);
    REQUIRE(vec2.capacity() >= 4);

    CAPTURE(INIT_VALUE);
    Vec vec3(4, INIT_VALUE);
    REQUIRE(vec3.size() == 4);
    REQUIRE(vec3.capacity() >= 4);
    REQUIRE(vec3[0] == INIT_VALUE);
    REQUIRE(vec3[3] == INIT_VALUE);

    Vec vec4(vec3);
    REQUIRE(vec4.size() == vec3.size());
    REQUIRE(vec4.capacity() >= vec4.size());
    REQUIRE(vec4[0] == INIT_VALUE);
    REQUIRE(vec4[3] == INIT_VALUE);
    REQUIRE(vec4.begin() != vec3.begin());
}

#if ETL_USE_EXCEPTIONS

TEST_CASE("Etl::Dynamic::Vector<> exceptions", "[vec][dynamic][etl]") {

    static const uint32_t COUNT = 16;
    using Item = int;
    using Vec = Etl::Dynamic::Vector<Item>;

    Vec vec(COUNT);

    Item val;
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

    using Item = int;
    using Vec = Etl::Static::Vector<Item, CAPACITY>;

    CAPTURE(CAPACITY);

    Vec vec;

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

    using Item = int;
    using Vec = Etl::Static::Vector<Item, CAPACITY>;

    Vec vec;

    vec.insert(vec.begin(), 4, 1);

    CHECK(vec.size() == CAPACITY);

    vec.push_back(5);

    REQUIRE(vec.size() == CAPACITY);
}


TEST_CASE("Etl::Static::Vector<> constructor test", "[vec][static][etl][basic]") {

    static const uint32_t CAPACITY = 16;
    static const uint32_t CAPACITY4 = 20;

    using Item = int;
    using Vec = Etl::Static::Vector<Item, CAPACITY>;
    using VecT4 = Etl::Static::Vector<Item, CAPACITY4>;

    static const Item INIT_VALUE = 123;

    Vec vec1;
    REQUIRE(vec1.size() == 0);
    REQUIRE(vec1.capacity() == CAPACITY);

    Vec vec2(4);
    REQUIRE(vec2.size() == 4);
    REQUIRE(vec2.capacity() == CAPACITY);

    CAPTURE(INIT_VALUE);
    Vec vec3(4, INIT_VALUE);
    REQUIRE(vec3.size() == 4);
    REQUIRE(vec3.capacity() == CAPACITY);
    REQUIRE(vec3[0] == INIT_VALUE);
    REQUIRE(vec3[3] == INIT_VALUE);

    SECTION("Copy from same size") {

        Vec vec4(vec3);
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

        Vec vec5(vec4);
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


// Etl::Custom::Vector tests -----------------------------------------------

TEST_CASE("Etl::Custom::Vector<> allocation tests", "[vec][custom][etl]") {

    using Etl::Test::DummyAllocator;
    using VecType = Etl::Custom::Vector<void*, DummyAllocator>;
    using AllocatorType = VecType::Allocator;

    auto end = AtScopeEnd([]() {
        REQUIRE(AllocatorType::getDeleteCount() == AllocatorType::getAllocCount());
        AllocatorType::reset();
    });

    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    VecType v(32U);

    REQUIRE(v.size() == 32U);

    REQUIRE(AllocatorType::getAllocCount() == 32);
}


// Etl::Vector comparision tests -------------------------------------------


TEST_CASE("Etl::Vector<> comparision", "[vector][etl]") {

    SECTION("Etl::Vector<> vs Etl::Vector<>") {

        using VecType = Etl::Dynamic::Vector<int>;
        using Base = Etl::Vector<int>;

        VecType lhs;
        VecType rhs;

        auto inserter = [](Base& cont, int val) { cont.push_back(val); };

        testComparision(static_cast<Base&>(lhs),
                        static_cast<Base&>(rhs),
                        inserter,
                        inserter);
    }

    SECTION("Etl::Dynamic::Vector<> vs Etl::Static::Vector<>") {

        using LType = Etl::Dynamic::Vector<int>;
        using RType = Etl::Static::Vector<int, 32U>;

        LType lhs;
        RType rhs;

        auto lInserter = [](LType& cont, int val) { cont.push_back(val); };
        auto rInserter = [](RType& cont, int val) { cont.push_back(val); };

        testComparision(lhs,
                        rhs,
                        lInserter,
                        rInserter);
    }
}


// Etl::Vector compatibility tests ---------------------------------------------


TEST_CASE("Etl::Vector<> with std::accumulate()", "[vec][comp][etl]") {

    using Item = int;
    using Vec = Etl::Dynamic::Vector<Item>;

    testAccumulate<Vec>();
}

TEST_CASE("Etl::Vector<> with std::partial_sum()", "[vec][comp][etl]") {

    using Item = int;
    using Vec = Etl::Dynamic::Vector<Item>;

    testPartialSum<Vec>();
}

TEST_CASE("Etl::Vector<> with std::inner_product()", "[vec][comp][etl]") {

    using Item = int;
    using Vec = Etl::Dynamic::Vector<Item>;

    testInnerProduct<Vec, Vec>();
}


TEST_CASE("Etl::Vector<> stability issues", "[vec][etl][stab]") {

    SECTION("S1 - Vector<const T*>") {

        using Vec = Etl::Dynamic::Vector<const int*>;

        Etl::Array<int, 8U> nums = {0, 1, 2, 3, 4, 5, 6, 7};

        Vec vec;
        for (auto& item : nums) {
            vec.push_front(&item);
        }

        auto verify = [](const Vec& vec) {
            // Note: the actual issue was a compile error related to
            // indexing a `const Vector<const T*>&`.
            for (size_t i = 0; i < vec.size(); ++i) {
                REQUIRE(*vec[i] >= 0);
            }
        };

        verify(vec);
    }
}

}  // namespace
