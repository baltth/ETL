# etl definition

if(NOT DEFINED ETL_INCLUDE_DIR)
    set(ETL_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
endif()

if(NOT DEFINED ETL_SRCS_DIR)
    set(ETL_SRCS_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
endif()

list(APPEND ETL_SRCS ${ETL_SRCS_DIR}/AListBase.cpp)
list(APPEND ETL_SRCS ${ETL_SRCS_DIR}/SingleChain.cpp)
list(APPEND ETL_SRCS ${ETL_SRCS_DIR}/DoubleChain.cpp)
list(APPEND ETL_SRCS ${ETL_SRCS_DIR}/FifoIndexing.cpp)
list(APPEND ETL_SRCS ${ETL_SRCS_DIR}/PoolBase.cpp)
list(APPEND ETL_SRCS ${ETL_SRCS_DIR}/BufStr.cpp)
list(APPEND ETL_SRCS ${ETL_SRCS_DIR}/AHashTable.cpp)

add_library(etl STATIC ${ETL_SRCS})
target_include_directories(etl PUBLIC ${ETL_INCLUDE_DIR})
target_compile_definitions(etl PUBLIC ETL_USE_EXCEPTIONS=0)
set_target_properties(etl PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

if(TARGET etl-if)
    target_link_libraries(etl PRIVATE etl-if)
endif()

install(TARGETS etl)
install(DIRECTORY ${ETL_INCLUDE_DIR}/etl DESTINATION include)
