# libetl definition

if(ETL_CUSTOM_LIB_SUFFIX)
    set(ETL_NAME etl-${ETL_CUSTOM_LIB_SUFFIX})
    message(STATUS "Using custom target name ${ETL_NAME}")
else()
    set(ETL_NAME etl)
endif()

if(ETL_CUSTOM_INCLUDE_SUFFIX)
    set(ETL_INSTALL_INC_DIR include-${ETL_CUSTOM_INCLUDE_SUFFIX})
    message(STATUS "Using custom include dir ${ETL_INSTALL_INC_DIR}/etl for install")
else()
    set(ETL_INSTALL_INC_DIR include)
endif()

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

add_library(${ETL_NAME} STATIC ${ETL_SRCS})
target_include_directories(${ETL_NAME} PUBLIC ${ETL_INCLUDE_DIR})
target_compile_definitions(${ETL_NAME} PUBLIC ETL_USE_EXCEPTIONS=0)

if(TARGET ${ETL_NAME}-compile-if)
    target_link_libraries(${ETL_NAME} PUBLIC ${ETL_NAME}-compile-if)
elseif(TARGET etl-compile-if)
    target_link_libraries(${ETL_NAME} PUBLIC etl-compile-if)
endif()
if(TARGET ${ETL_NAME}-prv-compile-if)
    target_link_libraries(${ETL_NAME} PRIVATE ${ETL_NAME}-prv-compile-if)
elseif(TARGET etl-prv-compile-if)
    target_link_libraries(${ETL_NAME} PUBLIC etl-prv-compile-if)
endif()

install(TARGETS ${ETL_NAME} ARCHIVE DESTINATION lib)
install(DIRECTORY ${ETL_INCLUDE_DIR}/etl DESTINATION ${ETL_INSTALL_INC_DIR})
