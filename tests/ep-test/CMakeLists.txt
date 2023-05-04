# A simple test project for using ETL as CMake external project

cmake_minimum_required(VERSION 3.5)
message(STATUS "CMake ${CMAKE_VERSION}")

# project def

project(ETL-ep-test)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# include EP build
include(${CMAKE_CURRENT_LIST_DIR}/../../libetl-ep.cmake)

# A basic executable using ETL
add_executable(etl-ep-test ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
target_link_libraries(etl-ep-test PRIVATE etl)
