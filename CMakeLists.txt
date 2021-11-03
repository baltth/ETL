cmake_minimum_required(VERSION 3.5)
project(ETL)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(USE_CLANG)
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)

list(APPEND ETL_COMPILE_OPTIONS -Wall -O2 -g)
list(APPEND ETL_CONFIG ETL_USE_EXCEPTIONS=0)

include(${PROJECT_SOURCE_DIR}/libetl.cmake)
include(${PROJECT_SOURCE_DIR}/etl-tests.cmake)
