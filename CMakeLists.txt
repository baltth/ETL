cmake_minimum_required(VERSION 3.5)
project(ETL)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

option(ETL_BUILD_TESTS "Build tests" OFF)
option(ETL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(ETL_COVERAGE "Enable code coverage" OFF)

add_library(etl-if INTERFACE)
target_compile_options(etl-if INTERFACE -Wall -Wextra -pedantic)

if(ETL_COVERAGE)
    message(STATUS "Coverage enabled!")
    target_compile_options(etl-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
    target_link_libraries(etl-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
endif()

include(${PROJECT_SOURCE_DIR}/libetl.cmake)

if(ETL_BUILD_TESTS OR ETL_BUILD_BENCHMARKS)
    include(${PROJECT_SOURCE_DIR}/etl-tests.cmake)
endif()
