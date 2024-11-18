/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2024 Balazs Toth.

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
#include <etl/Map.h>
#include <etl/Span.h>
#include <etl/UnorderedMap.h>
#include <etl/Vector.h>
#include <etl/legacy/BufStr.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

#ifndef NDEBUG
#warning "Building benchmarks in Debug build, results may be invalid"
#endif

using Etl::Test::ContainerTester;

// Etl::Vector tests -----------------------------------------------------------

namespace {


template<typename T, size_t N, template<typename M> class FUNC>
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


template<typename T, size_t N, template<typename M> class FUNC>
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


template<typename T, size_t N, template<typename M> class FUNC>
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
    }

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
    }

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
    void prepare(V& vec, size_t cyc) {
        (void)vec;
        (void)cyc;
    }

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
        (void)vec;
        VectorPushBack<VEC>().test(src, cyc);
        // to have at least one element
        vec.push_back(typename VEC::value_type {*src.begin()});
    }

    template<typename V>
    void test(V& vec, size_t cyc) {
        (void)cyc;
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
        (void)vec;
        VectorPushBack<VEC>().test(src, cyc);
        // to have at least one element
        vec.push_back(typename VEC::value_type {*src.begin()});
    }

    template<typename V>
    void test(V& vec, size_t cyc) {
        (void)cyc;
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

static constexpr std::size_t SBS_SIZE = 128000UL;


TEST_CASE("Etl::BufStr constructor", "[bufstr][perf][etl]") {

    BENCHMARK("Etl:Static::BufStr<128>") {
        Etl::Static::BufStr<128U> s;
        return s.good();
    };

    BENCHMARK("Etl:Dynamic::BufStr") {
        Etl::Dynamic::BufStr s;
        return s.good();
    };

    BENCHMARK("Etl:Static::Legacy::BufStr<128>") {
        Etl::Static::Legacy::BufStr<128U> s;
        return s.size();
    };

    BENCHMARK("Etl:Dynamic::Legacy::BufStr") {
        Etl::Dynamic::Legacy::BufStr s;
        return s.size();
    };

    BENCHMARK("std::stringstream") {
        std::stringstream s;
        return s.good();
    };
}


TEST_CASE("Etl::BufStr << int", "[bufstr][perf][etl]") {

    auto get = []() -> int { return static_cast<int>(std::rand() * 100.0 / RAND_MAX) - 50; };

    BENCHMARK_ADVANCED("Etl:Static::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::BufStr<SBS_SIZE> s;
        auto val = get();
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::BufStr s;
        auto val = get();
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("Etl:Static::Legacy::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::Legacy::BufStr<SBS_SIZE> s;
        auto val = get();
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::Legacy::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Legacy::BufStr s;
        auto val = get();
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("std::stringstream")(Catch::Benchmark::Chronometer meter) {
        std::stringstream s;
        auto val = get();
        meter.measure([&s, val]() { s << val; });
    };
}


TEST_CASE("Etl::BufStr << double", "[bufstr][perf][etl]") {

    auto get = []() -> double { return (std::rand() * 100.0 / RAND_MAX) - 50.0; };

    BENCHMARK_ADVANCED("Etl:Static::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::BufStr<SBS_SIZE> s;
        auto val = get();
        s << std::setprecision(5);
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::BufStr s;
        auto val = get();
        s << std::setprecision(5);
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("Etl:Static::Legacy::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::Legacy::BufStr<SBS_SIZE> s;
        auto val = get();
        s << Etl::Legacy::BufStr::Prec(3);  // note: Prec of Legacy defines
                                            // the length after the decimal mark.
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::Legacy::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Legacy::BufStr s;
        auto val = get();
        s << Etl::Legacy::BufStr::Prec(3);
        meter.measure([&s, val]() { s << val; });
    };

    BENCHMARK_ADVANCED("std::stringstream")(Catch::Benchmark::Chronometer meter) {
        std::stringstream s;
        auto val = get();
        s << std::setprecision(5);
        meter.measure([&s, val]() { s << val; });
    };
}


TEST_CASE("Etl::BufStr << Hex()", "[bufstr][perf][etl]") {

    auto get = []() -> std::uint32_t {
        return static_cast<std::uint32_t>(std::rand() * UINT32_MAX / RAND_MAX);
    };

    BENCHMARK_ADVANCED("Etl:Static::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::BufStr<SBS_SIZE> s;
        auto val = get();
        meter.measure([&s, val]() { s << Etl::BufStr::Hex(val, 8); });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::BufStr s;
        auto val = get();
        s << std::setprecision(3);
        meter.measure([&s, val]() { s << Etl::BufStr::Hex(val, 8); });
    };

    BENCHMARK_ADVANCED("Etl:Static::Legacy::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::Legacy::BufStr<SBS_SIZE> s;
        auto val = get();
        s << Etl::Legacy::BufStr::Prec(3);
        meter.measure([&s, val]() { s << Etl::Legacy::BufStr::Hex(val, 8); });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::Legacy::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Legacy::BufStr s;
        auto val = get();
        s << Etl::Legacy::BufStr::Prec(3);
        meter.measure([&s, val]() { s << Etl::Legacy::BufStr::Hex(val, 8); });
    };

    BENCHMARK_ADVANCED("std::stringstream")(Catch::Benchmark::Chronometer meter) {
        std::stringstream s;
        auto val = get();
        s << std::setprecision(3);
        meter.measure([&s, val]() {
            s << std::hex << std::setfill('0') << std::setw(8) << val << std::dec
              << std::setfill(' ');
        });
    };
}


TEST_CASE("Etl::BufStr << const char*", "[bufstr][perf][etl]") {

    static constexpr char TEXT[] =
        "some ad hoc text, probably long enough to avoid SSO in std::stringstream";

    BENCHMARK_ADVANCED("Etl:Static::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::BufStr<SBS_SIZE> s;
        meter.measure([&s]() { s << TEXT; });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::BufStr s;
        meter.measure([&s]() { s << TEXT; });
    };

    BENCHMARK_ADVANCED("Etl:Static::Legacy::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::Legacy::BufStr<SBS_SIZE> s;
        meter.measure([&s]() { s << TEXT; });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::Legacy::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Legacy::BufStr s;
        meter.measure([&s]() { s << TEXT; });
    };

    BENCHMARK_ADVANCED("std::stringstream")(Catch::Benchmark::Chronometer meter) {
        std::stringstream s;
        meter.measure([&s]() { s << TEXT; });
    };
}


TEST_CASE("Etl::BufStr to C string", "[bufstr][perf][etl]") {

    auto get = []() -> std::uint32_t {
        return static_cast<std::uint32_t>(std::rand() * UINT32_MAX / RAND_MAX);
    };

    BENCHMARK_ADVANCED("Etl:Static::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::BufStr<SBS_SIZE> s;
        s << get();
        meter.measure([&s]() { return s.cStr(); });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::BufStr s;
        s << get();
        meter.measure([&s]() { return s.cStr(); });
    };

    BENCHMARK_ADVANCED("Etl:Static::Legacy::BufStr<>")(Catch::Benchmark::Chronometer meter) {
        Etl::Static::Legacy::BufStr<SBS_SIZE> s;
        s << get();
        meter.measure([&s]() { return s.cStr(); });
    };

    BENCHMARK_ADVANCED("Etl:Dynamic::Legacy::BufStr")(Catch::Benchmark::Chronometer meter) {
        Etl::Dynamic::Legacy::BufStr s;
        s << get();
        meter.measure([&s]() { return s.cStr(); });
    };

    BENCHMARK_ADVANCED("std::stringstream")(Catch::Benchmark::Chronometer meter) {
        std::stringstream s;
        s << get();
        meter.measure([&s]() { return s.str().c_str(); });
    };
}


TEST_CASE("Etl::BufStr to std::string", "[bufstr][perf][etl]") {

    auto get = []() -> std::uint32_t {
        return static_cast<std::uint32_t>(std::rand() * UINT32_MAX / RAND_MAX);
    };

    SECTION("small string") {

        BENCHMARK_ADVANCED("Etl:Static::BufStr<>")(Catch::Benchmark::Chronometer meter) {
            Etl::Static::BufStr<SBS_SIZE> s;
            s << get();
            meter.measure([&s]() { return s.str(); });
        };

        BENCHMARK_ADVANCED("Etl:Dynamic::BufStr")(Catch::Benchmark::Chronometer meter) {
            Etl::Dynamic::BufStr s;
            s << get();
            meter.measure([&s]() { return s.str(); });
        };

        BENCHMARK_ADVANCED("Etl:Static::Legacy::BufStr<>")(Catch::Benchmark::Chronometer meter) {
            Etl::Static::Legacy::BufStr<SBS_SIZE> s;
            s << get();
            meter.measure([&s]() { return std::string {s.cStr()}; });
        };

        BENCHMARK_ADVANCED("Etl:Dynamic::Legacy::BufStr")(Catch::Benchmark::Chronometer meter) {
            Etl::Dynamic::Legacy::BufStr s;
            s << get();
            meter.measure([&s]() { return std::string {s.cStr()}; });
        };

        BENCHMARK_ADVANCED("std::stringstream")(Catch::Benchmark::Chronometer meter) {
            std::stringstream s;
            s << get();
            meter.measure([&s]() { return s.str(); });
        };
    }

    SECTION("large string") {
        static constexpr int CNT = 35;

        BENCHMARK_ADVANCED("Etl:Static::BufStr<>")(Catch::Benchmark::Chronometer meter) {
            Etl::Static::BufStr<SBS_SIZE> s;
            for (int i = 0; i < CNT; ++i) {
                s << get();
            }
            meter.measure([&s]() { return s.str(); });
        };

        BENCHMARK_ADVANCED("Etl:Dynamic::BufStr")(Catch::Benchmark::Chronometer meter) {
            Etl::Dynamic::BufStr s;
            for (int i = 0; i < CNT; ++i) {
                s << get();
            }
            meter.measure([&s]() { return s.str(); });
        };

        BENCHMARK_ADVANCED("Etl:Static::Legacy::BufStr<>")(Catch::Benchmark::Chronometer meter) {
            Etl::Static::Legacy::BufStr<SBS_SIZE> s;
            for (int i = 0; i < CNT; ++i) {
                s << get();
            }
            meter.measure([&s]() { return std::string {s.cStr()}; });
        };

        BENCHMARK_ADVANCED("Etl:Dynamic::Legacy::BufStr")(Catch::Benchmark::Chronometer meter) {
            Etl::Dynamic::Legacy::BufStr s;
            for (int i = 0; i < CNT; ++i) {
                s << get();
            }
            meter.measure([&s]() { return std::string {s.cStr()}; });
        };

        BENCHMARK_ADVANCED("std::stringstream")(Catch::Benchmark::Chronometer meter) {
            std::stringstream s;
            for (int i = 0; i < CNT; ++i) {
                s << get();
            }
            meter.measure([&s]() { return s.str(); });
        };
    }
}


TEST_CASE("Etl::BufStr complex operations", "[bufstr][perf][etl]") {

    static constexpr std::uint32_t U_VAL = 566247UL;
    static constexpr double D_VAL = 576.799547337;
    static constexpr char TEXT[] = "Print some ad hoc values emulating a real life log: ";

    BENCHMARK("Etl:Static::BufStr<>") {
        Etl::Static::BufStr<256> s;
        s << TEXT << "0x" << Etl::BufStr::Hex(U_VAL, 8U);
        s << std::setprecision(6);
        s << " (" << D_VAL << ")" << std::endl;
        return s.str();
    };

    BENCHMARK("Etl:Dynamic::BufStr") {
        Etl::Dynamic::BufStr s;
        s << TEXT << "0x" << Etl::BufStr::Hex(U_VAL, 8U);
        s << std::setprecision(6);
        s << " (" << D_VAL << ")" << std::endl;
        return s.str();
    };

    BENCHMARK("Etl:Static::Legacy::BufStr<>") {
        Etl::Static::Legacy::BufStr<256> s;
        s << TEXT << "0x" << Etl::Legacy::BufStr::Hex(U_VAL, 8U);
        s << Etl::Legacy::BufStr::Prec(3);  // note: Prec of Legacy defines
                                            // the length after the decimal mark.
        s << " (" << D_VAL << ")" << Etl::Legacy::BufStr::Endl;
        return std::string {s.cStr()};
    };

    BENCHMARK("Etl:Dynamic::Legacy::BufStr") {
        Etl::Dynamic::Legacy::BufStr s;
        s << TEXT << "0x" << Etl::Legacy::BufStr::Hex(U_VAL, 8U);
        s << Etl::Legacy::BufStr::Prec(3);
        s << " (" << D_VAL << ")" << Etl::Legacy::BufStr::Endl;
        return std::string {s.cStr()};
    };

    BENCHMARK("std::stringstream") {
        std::stringstream s;
        s << TEXT << "0x" << std::hex << std::setfill('0') << std::setw(8) << U_VAL << std::dec
          << std::setfill(' ');
        s << std::setprecision(6);
        s << " (" << D_VAL << ")" << std::endl;
        return s.str();
    };
}


// Etl::UnorderedMap performance tests ---------------------------------------------

template<typename T, size_t N, template<typename> class FUNC>
void testMaps() {

    Etl::Static::Vector<int32_t, N> input;
    for (size_t i = 0; i < N; ++i) {
        input.push_back(static_cast<int32_t>(std::rand()));
    }

#if 0
    BENCHMARK_ADVANCED("Static::Map<int32_t, T>")(Catch::Benchmark::Chronometer meter) {
        using M = Etl::Static::Map<int32_t, T, N>;
        M map;
        FUNC<M> func;
        func.prepare(map, input);
        meter.measure([&map, &func, &input] {
            func.test(map, input);
            func.reset(map);
        });
    };

    BENCHMARK_ADVANCED("Dynamic::Map<int32_t, T>")(Catch::Benchmark::Chronometer meter) {
        using M = Etl::Dynamic::Map<int32_t, T>;
        M map;
        FUNC<M> func;
        func.prepare(map, input);
        meter.measure([&map, &func, &input] {
            func.test(map, input);
            func.reset(map);
        });
    };

    BENCHMARK_ADVANCED("std::map<int32_t, T>")(Catch::Benchmark::Chronometer meter) {
        using M = std::map<int32_t, T>;
        M map;
        FUNC<M> func;
        func.prepare(map, input);
        meter.measure([&map, &func, &input] {
            func.test(map, input);
            func.reset(map);
        });
    };
#endif

    BENCHMARK_ADVANCED("Static::UnorderedMap<int32_t, T>")
    (Catch::Benchmark::Chronometer meter) {
        using M = Etl::Static::UnorderedMap<int32_t, T, N, N>;
        M map;
        FUNC<M> func;
        func.prepare(map, input);
        meter.measure([&map, &func, &input] {
            auto res = func.test(map, input);
            func.reset(map);
            return res;
        });
    };

    BENCHMARK_ADVANCED("Dynamic::UnorderedMap<int32_t, T>")
    (Catch::Benchmark::Chronometer meter) {
        using M = Etl::Dynamic::UnorderedMap<int32_t, T>;
        M map;
        FUNC<M> func;
        func.prepare(map, input);
        meter.measure([&map, &func, &input] {
            auto res = func.test(map, input);
            func.reset(map);
            return res;
        });
    };

    BENCHMARK_ADVANCED("std::unordered_map<int32_t, T>")
    (Catch::Benchmark::Chronometer meter) {
        using M = std::unordered_map<int32_t, T>;
        M map;
        FUNC<M> func;
        func.prepare(map, input);
        meter.measure([&map, &func, &input] {
            auto res = func.test(map, input);
            func.reset(map);
            return res;
        });
    };
}


template<typename MAP>
struct MapInsert {

    template<typename M, typename INPUT>
    void prepare(M& map, const INPUT& data) {
        (void)map;
        (void)data;
    }

    template<typename M, typename INPUT>
    size_t test(M& map, const INPUT& data) {
        MapInsert::insert(map, data);
        return map.size();
    }

    template<typename M, typename INPUT>
    static void insert(M& map, const INPUT& data) {

        for (auto item : data) {
            map.insert(typename M::value_type {item, typename M::mapped_type {item}});
        }
    }

    template<typename M>
    void reset(M& map) {
        map.clear();
    }
};


TEST_CASE("Etl::UnorderedMap<int32_t, T> insert performance", "[map][unorderedmap][insert][etl]") {

    SECTION("T = int32_t") {
        SECTION("64 elements") {
            testMaps<int32_t, 64U, MapInsert>();
        }

        SECTION("256 elements") {
            testMaps<int32_t, 256U, MapInsert>();
        }

        SECTION("1024 elements") {
            testMaps<int32_t, 1024U, MapInsert>();
        }

        SECTION("4096 elements") {
            testMaps<int32_t, 4096U, MapInsert>();
        }
    }

    SECTION("T = ContainerTester") {
        SECTION("64 elements") {
            testMaps<ContainerTester, 64U, MapInsert>();
        }

        SECTION("256 elements") {
            testMaps<ContainerTester, 256U, MapInsert>();
        }

        SECTION("1024 elements") {
            testMaps<ContainerTester, 1024U, MapInsert>();
        }

        SECTION("4096 elements") {
            testMaps<ContainerTester, 4096U, MapInsert>();
        }
    }
}


template<typename MAP>
struct MapAccess {

    template<typename M, typename INPUT>
    void prepare(M& map, const INPUT& data) {
        MapInsert<MAP>::insert(map, data);
    }

    template<typename M, typename INPUT>
    size_t test(M& map, const INPUT& data) {
        auto it = map.begin();
        for (auto item : data) {
            it = map.find(item);
            (void)it;
        }
        return reinterpret_cast<size_t>(&*it);
    }

    template<typename M>
    void reset(M& map) {
        (void)map;
    }
};


TEST_CASE("Etl::UnorderedMap<int32_t, T> access performance", "[map][unorderedmap][access][etl]") {

    SECTION("T = int32_t") {
        SECTION("64 elements") {
            testMaps<int32_t, 64U, MapAccess>();
        }

        SECTION("256 elements") {
            testMaps<int32_t, 256U, MapAccess>();
        }

        SECTION("1024 elements") {
            testMaps<int32_t, 1024U, MapAccess>();
        }

        SECTION("4096 elements") {
            testMaps<int32_t, 4096U, MapAccess>();
        }
    }

    SECTION("T = ContainerTester") {
        SECTION("64 elements") {
            testMaps<ContainerTester, 64U, MapAccess>();
        }

        SECTION("256 elements") {
            testMaps<ContainerTester, 256U, MapAccess>();
        }

        SECTION("1024 elements") {
            testMaps<ContainerTester, 1024U, MapAccess>();
        }

        SECTION("4096 elements") {
            testMaps<ContainerTester, 4096U, MapAccess>();
        }
    }
}


template<typename MAP>
struct MapIteration {

    template<typename M, typename INPUT>
    void prepare(M& map, const INPUT& data) {
        MapInsert<MAP>::insert(map, data);
    }

    template<typename M, typename INPUT>
    size_t test(M& map, const INPUT& data) {
        (void)data;
        auto it = map.begin();
        while (it != map.end()) {
            ++it;
        }
        return reinterpret_cast<size_t>(&*it);
    }

    template<typename M>
    void reset(M& map) {
        (void)map;
    }
};


TEST_CASE("Etl::UnorderedMap<int32_t, T> iteration performance",
          "[map][unorderedmap][iteration][etl]") {

    SECTION("T = int32_t") {
        SECTION("64 elements") {
            testMaps<int32_t, 64U, MapIteration>();
        }

        SECTION("256 elements") {
            testMaps<int32_t, 256U, MapIteration>();
        }

        SECTION("1024 elements") {
            testMaps<int32_t, 1024U, MapIteration>();
        }

        SECTION("4096 elements") {
            testMaps<int32_t, 4096U, MapIteration>();
        }
    }

    SECTION("T = ContainerTester") {
        SECTION("64 elements") {
            testMaps<ContainerTester, 64U, MapIteration>();
        }

        SECTION("256 elements") {
            testMaps<ContainerTester, 256U, MapIteration>();
        }

        SECTION("1024 elements") {
            testMaps<ContainerTester, 1024U, MapIteration>();
        }

        SECTION("4096 elements") {
            testMaps<ContainerTester, 4096U, MapIteration>();
        }
    }
}


template<typename MAP>
struct MapCopy {

    template<typename M, typename INPUT>
    void prepare(M& map, const INPUT& data) {
        (void)map;
        MapInsert<MAP>::insert(src, data);
    }

    template<typename M, typename INPUT>
    size_t test(M& map, const INPUT& data) {
        (void)data;
        map = src;
        return map.size();
    }

    template<typename M>
    void reset(M& map) {
        map.clear();
    }

    MAP src;
};


TEST_CASE("Etl::UnorderedMap<int32_t, T> copy performance", "[map][unorderedmap][copy][etl]") {

    SECTION("T = int32_t") {
        SECTION("64 elements") {
            testMaps<int32_t, 64U, MapCopy>();
        }

        SECTION("4096 elements") {
            testMaps<int32_t, 4096U, MapCopy>();
        }
    }

    SECTION("T = ContainerTester") {
        SECTION("64 elements") {
            testMaps<ContainerTester, 64U, MapCopy>();
        }

        SECTION("4096 elements") {
            testMaps<ContainerTester, 4096U, MapCopy>();
        }
    }
}


template<typename MAP>
struct MapMove {

    template<typename M, typename INPUT>
    void prepare(M& map, const INPUT& data) {
        (void)map;
        MapInsert<MAP>::insert(src, data);
    }

    template<typename M, typename INPUT>
    size_t test(M& map, const INPUT& data) {
        (void)data;
        map = std::move(src);
        return map.size();
    }

    template<typename M>
    void reset(M& map) {
        src = std::move(map);
    }

    MAP src;
};


TEST_CASE("Etl::UnorderedMap<int32_t, T> move performance", "[map][unorderedmap][move][etl]") {

    SECTION("T = int32_t") {
        SECTION("64 elements") {
            testMaps<int32_t, 64U, MapMove>();
        }

        SECTION("4096 elements") {
            testMaps<int32_t, 4096U, MapMove>();
        }
    }

    SECTION("T = ContainerTester") {
        SECTION("64 elements") {
            testMaps<ContainerTester, 64U, MapMove>();
        }

        SECTION("4096 elements") {
            testMaps<ContainerTester, 4096U, MapMove>();
        }
    }
}


}  // namespace
