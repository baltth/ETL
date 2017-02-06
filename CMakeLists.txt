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
list(APPEND ETL_TEST_SRCS "Test/test.cpp")

add_executable(etl_test ${ETL_TEST_SRCS})
