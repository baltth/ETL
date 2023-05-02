# ETL library and test build

cmake_minimum_required(VERSION 3.5)

message(STATUS "CMake ${CMAKE_VERSION}")

# project def

project(ETL)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR
            "Build type undefined, please explicitly configure with \
            -DCMAKE_BUILD_TYPE=[Release|Debug|DebugWithOpt...]")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS_DEBUGWITHOPT "${CMAKE_CXX_FLAGS_DEBUG} -O2")

# project options

option(ETL_BUILD_TESTS "Build tests" OFF)
option(ETL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(ETL_COVERAGE "Enable code coverage" OFF)
option(ETL_SANITIZE "Enable -fsanitize=..." OFF)
option(ETL_CUSTOM_NAMESPACE
       "Override 'Etl' namespace. \
       This should be defined to use multiple ETL versions in your project")
option(ETL_CUSTOM_LIB_SUFFIX
       "Suffix for the library name: `libetl-SUFFIX.a`. \
       This should be defined when using multiple ETL versions in your project.")
option(ETL_CUSTOM_INCLUDE_SUFFIX
       "Suffix for the installed include folder: `CMAKE_INSTALL_PREFIX/include-SUFFIX/etl`. \
       This should be defined when using multiple ETL versions in your project.")

# threading lib used for std::mutex
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Interface library for compile flags and configuration
add_library(etl-compile-if INTERFACE)
target_link_libraries(etl-compile-if INTERFACE Threads::Threads)

if(ETL_CUSTOM_NAMESPACE)
    message(STATUS "Using custom namespace '${ETL_CUSTOM_NAMESPACE}'")
    if(ETL_BUILD_TESTS OR ETL_BUILD_BENCHMARKS)
        message(FATAL_ERROR "Tests and benchmarks are not expected to build with custom namespace.")
    endif()

    target_compile_definitions(etl-compile-if INTERFACE ETL_NAMESPACE=${ETL_CUSTOM_NAMESPACE})
endif()

# Interface library for private compile flags and configuration
add_library(etl-prv-compile-if INTERFACE)
target_compile_options(etl-prv-compile-if INTERFACE -Wall -Wextra -pedantic)
target_link_libraries(etl-prv-compile-if INTERFACE etl-compile-if)

if(ETL_COVERAGE)
    message(STATUS "Coverage enabled")
    target_compile_options(etl-prv-compile-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
    target_link_libraries(etl-prv-compile-if INTERFACE -fprofile-arcs -ftest-coverage -fPIC)
endif()

if(ETL_SANITIZE)
    message(STATUS "Using -fsanitize=undefined -fsanitize=address")
    target_compile_options(etl-prv-compile-if INTERFACE -fsanitize=undefined -fsanitize=address)
    target_link_libraries(etl-prv-compile-if INTERFACE -fsanitize=undefined -fsanitize=address)
endif()

# library build
include(${PROJECT_SOURCE_DIR}/libetl.cmake)

# tests and more
if(ETL_BUILD_TESTS OR ETL_BUILD_BENCHMARKS)
    include(${PROJECT_SOURCE_DIR}/etl-tests.cmake)
endif()
