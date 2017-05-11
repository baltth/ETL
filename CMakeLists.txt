cmake_minimum_required(VERSION 3.5)
project(etl_test)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "--std=c++03 -Wall")
#set (CMAKE_CXX_STANDARD 11)

add_definitions(-DETL_USE_EXCEPTIONS=0)

include_directories(.)

#However, the file(GLOB...) allows for wildcard additions:
list(APPEND ETL_TEST_SRCS "Base/AVectorBase.cpp")
list(APPEND ETL_TEST_SRCS "Base/AListBase.cpp")
list(APPEND ETL_TEST_SRCS "Base/LinkedList.cpp")
list(APPEND ETL_TEST_SRCS "Base/DoubleLinkedList.cpp")
list(APPEND ETL_TEST_SRCS "Base/FifoIndexing.cpp")

list(APPEND ETL_TEST_SRCS "Test/ContainerTester.cpp")
list(APPEND ETL_TEST_SRCS "Test/testArray.cpp")
list(APPEND ETL_TEST_SRCS "Test/testVector.cpp")
list(APPEND ETL_TEST_SRCS "Test/testList.cpp")
list(APPEND ETL_TEST_SRCS "Test/testSet.cpp")
list(APPEND ETL_TEST_SRCS "Test/testMap.cpp")
#list(APPEND ETL_TEST_SRCS "Test/testFifo.cpp")

add_executable(testEtl "Test/testEtl.cpp" ${ETL_TEST_SRCS})

enable_testing()

add_test(testArray testEtl "[array]")
add_test(testVector testEtl "[vector][dynamic]")
add_test(testStaticVector testEtl "[vector][static]")
add_test(testList testEtl "[list]")
add_test(testSet testEtl "[set]")
add_test(testMap testEtl "[map]")
#add_test(testFifo testEtl "[fifo]")

