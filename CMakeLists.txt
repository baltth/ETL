cmake_minimum_required(VERSION 3.5)
project(ETL)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Interface library for compile flags and configuration
add_library(libetl-compile-if INTERFACE)
target_compile_options(libetl-compile-if INTERFACE -Wall -Wextra -pedantic -O2 -g)
target_compile_definitions(libetl-compile-if INTERFACE ETL_USE_EXCEPTIONS=0)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(libetl-compile-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
    target_link_libraries(libetl-compile-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
endif()

include(${PROJECT_SOURCE_DIR}/libetl.cmake)
include(${PROJECT_SOURCE_DIR}/etl-tests.cmake)
