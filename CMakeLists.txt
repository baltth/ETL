cmake_minimum_required(VERSION 3.5)
project(etl_test)
set(CMAKE_BUILD_TYPE Release)


set(ETL_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(ETL_INCLUDE_DIR ${ETL_SOURCE_DIR}/include)
set(ETL_TESTS_DIR ${ETL_SOURCE_DIR}/tests)

set(BuildEtl03 ON)
set(BuildEtl11 On)

list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/AVectorBase.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/AListBase.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/LinkedList.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/DoubleLinkedList.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/FifoIndexing.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/PoolBase.cpp)
list(APPEND ETL_SRCS ${ETL_SOURCE_DIR}/src/BufStr.cpp)

list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/ContainerTester.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testArray.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testVector.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testList.cpp)
#list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testSet.cpp)
#list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMap.cpp)
#list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMultiMap.cpp)
#list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testFifo.cpp)
#list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testPool.cpp)
#list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testBufStr.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMain.cpp)

# C++03
if(BuildEtl03)

add_library(etl-03 ${ETL_SRCS})
target_include_directories(etl-03 PUBLIC ${ETL_INCLUDE_DIR})
target_compile_definitions(etl-03 PUBLIC ETL_USE_EXCEPTIONS=0)
target_compile_options(etl-03 PUBLIC --std=c++03 -Wall -Wno-parentheses -g)

add_executable(testEtl-03 ${ETL_TEST_SRCS})
target_link_libraries(testEtl-03 etl-03)

endif()

# C++11

if(BuildEtl11)

add_library(etl-11 ${ETL_SRCS})
target_include_directories(etl-11 PUBLIC ${ETL_INCLUDE_DIR})
target_compile_definitions(etl-11 PUBLIC ETL_USE_EXCEPTIONS=0)
target_compile_options(etl-11 PUBLIC --std=c++11 -Wall -g)

add_executable(testEtl-11 ${ETL_TEST_SRCS})
target_link_libraries(testEtl-11 etl-11)

endif()
