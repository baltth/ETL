cmake_minimum_required(VERSION 3.5)
project(etl_test)
set(CMAKE_BUILD_TYPE Release)


set(ETL_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(ETL_INCLUDE_DIR ${ETL_SOURCE_DIR}/include)
set(ETL_TESTS_DIR ${ETL_SOURCE_DIR}/tests)


list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/AVectorBase.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/AListBase.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/LinkedList.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/DoubleLinkedList.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/FifoIndexing.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/PoolBase.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/BufStr.cpp)

add_library(etl ${ETL_SRCS})
target_include_directories(etl PUBLIC ${ETL_INCLUDE_DIR})
target_compile_definitions(etl PUBLIC ETL_USE_EXCEPTIONS=0)
target_compile_options(etl PUBLIC --std=c++03 -Wall -g)

list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/ContainerTester.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testArray.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testVector.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testList.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testSet.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMap.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMultiMap.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testFifo.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testPool.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testBufStr.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMain.cpp)

add_executable(testEtl ${ETL_TEST_SRCS})
target_link_libraries(testEtl etl)

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
add_test(testBufStr testEtl "[bufstr]")

