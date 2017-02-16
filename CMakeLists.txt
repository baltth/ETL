cmake_minimum_required(VERSION 3.5)
project(etl_test)
set(CMAKE_BUILD_TYPE Release)

#set (CMAKE_CXX_FLAGS "--std=c++11")
set (CMAKE_CXX_STANDARD 11)

include_directories(.)

#However, the file(GLOB...) allows for wildcard additions:
list(APPEND ETL_TEST_SRCS "Base/AListBase.cpp")
list(APPEND ETL_TEST_SRCS "Base/DoubleLinkedList.cpp")

list(APPEND ETL_TEST_SRCS "Test/testMap.cpp")
list(APPEND ETL_TEST_SRCS "Test/testArray.cpp")

add_executable(testEtl "Test/testEtl.cpp" ${ETL_TEST_SRCS})

enable_testing()

add_test(testArray testEtl "[array]")
add_test(testMap testEtl "[map]")
