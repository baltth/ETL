# ETL library and test build

cmake_minimum_required(VERSION 3.5)
project(ETL)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# project options
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

option(ETL_BUILD_TESTS "Build tests" OFF)
option(ETL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(ETL_COVERAGE "Enable code coverage" OFF)

# threading lib used for std::mutex
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Interface library for compile flags and configuration
add_library(etl-compile-if INTERFACE)
target_compile_options(etl-compile-if INTERFACE -Wall -Wextra -pedantic)
target_link_libraries(etl-compile-if INTERFACE Threads::Threads)

if(ETL_COVERAGE)
    message(STATUS "Coverage enabled!")
    target_compile_options(etl-compile-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
    target_link_libraries(etl-compile-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
endif()

# library build
include(${PROJECT_SOURCE_DIR}/libetl.cmake)

# tests and more
if(ETL_BUILD_TESTS OR ETL_BUILD_BENCHMARKS)
    include(${PROJECT_SOURCE_DIR}/etl-tests.cmake)
endif()
