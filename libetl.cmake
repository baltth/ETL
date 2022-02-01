# libetl definition

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

add_library(libetl STATIC ${ETL_SRCS})
target_include_directories(libetl PUBLIC ${ETL_INCLUDE_DIR})
if(TARGET libetl-compile-if)
    target_link_libraries(libetl PUBLIC libetl-compile-if)
endif()
if(TARGET libetl-prv-compile-if)
    target_link_libraries(libetl PRIVATE libetl-prv-compile-if)
endif()
