cmake_minimum_required(VERSION 3.5)
project(etl_test)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "--std=c++03 -Wall -g")

add_definitions(-DETL_USE_EXCEPTIONS=0)

include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND ETL_TEST_SRCS "src/AVectorBase.cpp")
list(APPEND ETL_TEST_SRCS "src/AListBase.cpp")
list(APPEND ETL_TEST_SRCS "src/LinkedList.cpp")
list(APPEND ETL_TEST_SRCS "src/DoubleLinkedList.cpp")
list(APPEND ETL_TEST_SRCS "src/FifoIndexing.cpp")
list(APPEND ETL_TEST_SRCS "src/PoolBase.cpp")

list(APPEND ETL_TEST_SRCS "tests/ContainerTester.cpp")
list(APPEND ETL_TEST_SRCS "tests/testArray.cpp")
list(APPEND ETL_TEST_SRCS "tests/testVector.cpp")
list(APPEND ETL_TEST_SRCS "tests/testList.cpp")
list(APPEND ETL_TEST_SRCS "tests/testSet.cpp")
list(APPEND ETL_TEST_SRCS "tests/testMap.cpp")
list(APPEND ETL_TEST_SRCS "tests/testMultiMap.cpp")
list(APPEND ETL_TEST_SRCS "tests/testFifo.cpp")
list(APPEND ETL_TEST_SRCS "tests/testPool.cpp")
list(APPEND ETL_TEST_SRCS "tests/testMain.cpp")

add_executable(testEtl ${ETL_TEST_SRCS})

enable_testing()

add_test(testArray testEtl "[array]")
add_test(testVector testEtl "[vector][dynamic]")
add_test(testStaticVector testEtl "[vector][static]")
add_test(testList testEtl "[list]")
add_test(testSet testEtl "[set]")
add_test(testMap testEtl "[map]")
add_test(testMultiMap testEtl "[multimap]")
add_test(testFifo testEtl "[fifo]")
add_test(testPool testEtl "[pool]")

