/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019 Balazs Toth.

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
#include <catch2/catch.hpp>

#include <etl/BufStr.h>
#include <etl/Vector.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <vector>

#ifndef NDEBUG
#warning "Building benchmarks in Debug build, results may be invalid"
#endif

using Etl::Test::ContainerTester;

// Etl::Vector tests -----------------------------------------------------------

namespace {


template<typename T, size_t N, template<typename M> typename FUNC>
void testVectorsGross() {

    BENCHMARK("Static::Vector<T>") {
        Etl::Static::Vector<T, N> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        func.test(vec, N);
        (void)vec;
    };

    BENCHMARK("Dynamic::Vector<T>") {
        Etl::Dynamic::Vector<T> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        func.test(vec, N);
        (void)vec;
    };

    BENCHMARK("std::vector<T>") {
        std::vector<T> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        func.test(vec, N);
        (void)vec;
    };
}


template<typename T, size_t N, template<typename M> typename FUNC>
void testVectorsNet() {

    BENCHMARK_ADVANCED("Static::Vector<T>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::Vector<T, N> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        meter.measure([&func, &vec] {
            func.test(vec, N);
            func.reset(vec);
        });
        (void)vec;
    };

    BENCHMARK_ADVANCED("Dynamic::Vector<T>")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Vector<T> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        meter.measure([&func, &vec] {
            func.test(vec, N);
            func.reset(vec);
        });
        (void)vec;
    };

    BENCHMARK_ADVANCED("std::vector<T>")(Catch::Benchmark::Chronometer meter) {
        std::vector<T> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        meter.measure([&func, &vec] {
            func.test(vec, N);
            func.reset(vec);
        });
        (void)vec;
    };
}


template<typename T, size_t N, template<typename M> typename FUNC>
void testVectorsByBase() {

    BENCHMARK_ADVANCED("Static::Vector<T>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::Vector<T, N> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        meter.measure([&func, &vec] {
            func.test(static_cast<Etl::Vector<T>&>(vec), N);
            func.reset(vec);
        });
        (void)vec;
    };

    BENCHMARK_ADVANCED("Dynamic::Vector<T>")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Vector<T> vec;
        FUNC<decltype(vec)> func;
        func.prepare(vec, N);
        meter.measure([&func, &vec] {
            func.test(static_cast<Etl::Vector<T>&>(vec), N);
            func.reset(vec);
        });
        (void)vec;
    };
}


template<typename Dummy>
struct VectorInsertToFront {
    template<typename V>
    void prepare(V& vec, size_t cyc) {
        vec.reserve(cyc);
    };

    template<typename V>
    void test(V& vec, size_t cyc) {
        for (int i = 0; i < static_cast<int>(cyc); ++i) {
            vec.insert(vec.begin(), typename V::value_type {i});
        }
    }

    template<typename V>
    void reset(V& vec) {
        vec.clear();
    }
};


TEST_CASE("Etl::Vector<T> insert to front performance", "[vec][insert][etl]") {

    static constexpr size_t CNT = 1000U;

    SECTION("including construction, no reserve") {
        SECTION("T = int") {
            testVectorsGross<int, CNT, VectorInsertToFront>();
        }

        SECTION("T = ContainerTester") {
            testVectorsGross<ContainerTester, CNT, VectorInsertToFront>();
        }
    }

    SECTION("excluding construction, using reserve") {
        SECTION("T = int") {
            testVectorsNet<int, CNT, VectorInsertToFront>();
        }

        SECTION("T = ContainerTester") {
            testVectorsNet<ContainerTester, CNT, VectorInsertToFront>();
        }
    }
}


template<typename Dummy>
struct VectorPushBack {
    template<typename V>
    void prepare(V& vec, size_t cyc) {
        vec.reserve(cyc);
    };

    template<typename V>
    void test(V& vec, size_t cyc) {
        for (int i = 0; i < static_cast<int>(cyc); ++i) {
            vec.push_back(typename V::value_type {i});
        }
    }

    template<typename V>
    void reset(V& vec) {
        vec.clear();
    }
};


TEST_CASE("Etl::Vector<T> push back performance", "[vec][insert][etl]") {

    static constexpr size_t CNT = 1000U;

    SECTION("including construction, no reserve") {
        SECTION("T = int") {
            testVectorsGross<int, CNT, VectorPushBack>();
        }

        SECTION("T = ContainerTester") {
            testVectorsGross<ContainerTester, CNT, VectorPushBack>();
        }
    }

    SECTION("excluding construction, using reserve") {
        SECTION("T = int") {
            testVectorsNet<int, CNT, VectorPushBack>();
        }

        SECTION("T = ContainerTester") {
            testVectorsNet<ContainerTester, CNT, VectorPushBack>();
        }
    }
}


template<typename Dummy>
struct VectorRandom {
    template<typename V>
    void prepare(V& vec, size_t cyc) {};

    template<typename V>
    void test(V& vec, size_t cyc) {

        for (int i = 0; i < static_cast<int>(cyc); ++i) {
            auto item1 = typename V::value_type {i};
            auto item2 = typename V::value_type {i + 1};
            vec.push_back(item1);
            auto pos = vec.begin();
            vec.insert(++pos, 40U, item2);
            vec.insert(vec.begin(), item1);
            vec.erase(vec.end() - 42, vec.end());
        }
    }

    template<typename V>
    void reset(V& vec) {
        vec.clear();
    }
};


TEST_CASE("Etl::Vector<T> random op performance", "[vec][random][etl]") {

    static constexpr size_t CNT = 1000U;

    SECTION("T = int") {
        testVectorsNet<int, CNT, VectorRandom>();
    }

    SECTION("T = ContainerTester") {
        testVectorsNet<ContainerTester, CNT, VectorRandom>();
    }
}


template<typename VEC>
struct VectorCopy {

    template<typename V>
    void prepare(V& vec, size_t cyc) {
        VectorPushBack<VEC>().test(src, cyc);
        // to have at least one element
        vec.push_back(typename VEC::value_type {*src.begin()});
    };

    template<typename V>
    void test(V& vec, size_t cyc) {
        vec = src;
    }

    template<typename V>
    void reset(V& vec) {
        vec.clear();
    }

    VEC src;
};


TEST_CASE("Etl::Vector<T> copy performance", "[vec][copy][etl]") {

    static constexpr size_t CNT = 10000U;

    SECTION("T = int") {
        testVectorsNet<int, CNT, VectorCopy>();
    }

    SECTION("T = ContainerTester") {
        testVectorsNet<ContainerTester, CNT, VectorCopy>();
    }

    SECTION("by Etl::Vector<> base") {

        SECTION("T = int") {
            testVectorsByBase<int, CNT, VectorCopy>();
        }

        SECTION("T = ContainerTester") {
            testVectorsByBase<ContainerTester, CNT, VectorCopy>();
        }
    }
}


template<typename VEC>
struct VectorMove {

    template<typename V>
    void prepare(V& vec, size_t cyc) {
        VectorPushBack<VEC>().test(src, cyc);
        // to have at least one element
        vec.push_back(typename VEC::value_type {*src.begin()});
    };

    template<typename V>
    void test(V& vec, size_t cyc) {
        vec = std::move(src);
    }

    template<typename V>
    void reset(V& vec) {
        src = std::move(vec);
    }

    VEC src;
};


TEST_CASE("Etl::Vector<T> move performance", "[vec][move][etl]") {

    static constexpr size_t CNT = 10000U;

    SECTION("T = int") {
        testVectorsNet<int, CNT, VectorMove>();
    }

    SECTION("T = ContainerTester") {
        testVectorsNet<ContainerTester, CNT, VectorMove>();
    }

    SECTION("by Etl::Vector<> base") {

        SECTION("T = int") {
            testVectorsByBase<int, CNT, VectorMove>();
        }

        SECTION("T = ContainerTester") {
            testVectorsByBase<ContainerTester, CNT, VectorMove>();
        }
    }
}


TEST_CASE("Etl::Vector<T> copy vs move performance", "[vec][copy][move][etl]") {

    static constexpr size_t CNT = 10000U;

    SECTION("T = int") {
        SECTION("copy") {
            testVectorsByBase<int, CNT, VectorCopy>();
        }
        SECTION("move") {
            testVectorsByBase<int, CNT, VectorMove>();
        }
    }

    SECTION("T = ContainerTester") {
        SECTION("copy") {
            testVectorsByBase<ContainerTester, CNT, VectorCopy>();
        }
        SECTION("move") {
            testVectorsByBase<ContainerTester, CNT, VectorMove>();
        }
    }
}


template<typename SRC>
void testVectorInsert(const SRC& src) {

    using T = typename SRC::value_type;

    static constexpr size_t INIT_ELEMENTS = 300U;

    BENCHMARK_ADVANCED("Static::Vector<T>")(Catch::Benchmark::Chronometer meter) {
        static constexpr size_t N = 11000UL;
        Etl::Static::Vector<T, N> vec;
        REQUIRE((src.size() + INIT_ELEMENTS) < vec.capacity());

        vec.reserve(src.size() + INIT_ELEMENTS);
        meter.measure([&vec, &src] {
            vec.insert(vec.begin(), INIT_ELEMENTS, T {78});
            auto it = vec.begin() + 5;
            vec.insert(it, src.begin(), src.end());
            vec.clear();
        });
        (void)vec;
    };

    BENCHMARK_ADVANCED("Dynamic::Vector<T>")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Vector<T> vec;
        vec.reserve(src.size() + INIT_ELEMENTS);
        meter.measure([&vec, &src] {
            vec.insert(vec.begin(), INIT_ELEMENTS, T {78});
            auto it = vec.begin() + 5;
            vec.insert(it, src.begin(), src.end());
            vec.clear();
        });
        (void)vec;
    };

    BENCHMARK_ADVANCED("std::vector<T>")(Catch::Benchmark::Chronometer meter) {
        std::vector<T> vec;
        vec.reserve(src.size() + INIT_ELEMENTS);
        meter.measure([&vec, &src] {
            vec.insert(vec.begin(), INIT_ELEMENTS, T {78});
            auto it = vec.begin() + 5;
            vec.insert(it, src.begin(), src.end());
            vec.clear();
        });
        (void)vec;
    };
}

TEST_CASE("Etl::Vector<T> insert from...", "[vec][insert][foreign][etl]") {

    static constexpr size_t N = 10000UL;

    SECTION("...std::vector<T>") {
        SECTION("T = int") {
            using SRC = std::vector<int>;
            SRC src;
            for (size_t i = 0; i < N; ++i) {
                src.push_back(static_cast<int>(i));
            }

            testVectorInsert(src);
        }

        SECTION("T = ContainerTester") {
            using SRC = std::vector<ContainerTester>;
            SRC src;
            for (size_t i = 0; i < N; ++i) {
                src.push_back(ContainerTester(i));
            }

            testVectorInsert(src);
        }
    }

    SECTION("...std::list<T>") {
        SECTION("T = int") {
            using SRC = std::list<int>;
            SRC src;
            for (size_t i = 0; i < N; ++i) {
                src.push_back(static_cast<int>(i));
            }

            testVectorInsert(src);
        }

        SECTION("T = ContainerTester") {
            using SRC = std::list<ContainerTester>;
            SRC src;
            for (size_t i = 0; i < N; ++i) {
                src.push_back(ContainerTester(i));
            }

            testVectorInsert(src);
        }
    }
}


// Etl::BufStr performance tests ---------------------------------------------


TEST_CASE("Etl::BufStr performance", "[bufstr][perf][etl]") {

    typedef Etl::Static::BufStr<128> BufT;

    BENCHMARK_ADVANCED("with double")(Catch::Benchmark::Chronometer meter) {
        double val = (std::rand() * 100.0 / RAND_MAX) - 50.0;
        meter.measure([val] {
            BufT bs;
            bs << val;
        });
    };

    BENCHMARK_ADVANCED("with hex")(Catch::Benchmark::Chronometer meter) {
        auto i = static_cast<uint32_t>(std::rand());
        meter.measure([i] {
            BufT bs;
            bs << "0x";
            bs << Etl::BufStr::Hex(i, 10);
        });
    };
}

}  // namespace
