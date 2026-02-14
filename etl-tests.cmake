# tests for libetl

if(NOT DEFINED ETL_TESTS_DIR)
    set(ETL_TESTS_DIR ${CMAKE_CURRENT_LIST_DIR}/tests)
endif()

list(
    APPEND
    ETL_TEST_CMN
    ${ETL_TESTS_DIR}/ContainerTester.cpp
    ${ETL_TESTS_DIR}/testMain.cpp
)

if(ETL_BUILD_TESTS)
    list(
        APPEND
        ETL_TEST_SRCS
        ${ETL_TESTS_DIR}/testVector.cpp
        ${ETL_TESTS_DIR}/testList.cpp
        ${ETL_TESTS_DIR}/testSet.cpp
        ${ETL_TESTS_DIR}/testMap.cpp
        ${ETL_TESTS_DIR}/testMultiMap.cpp
        ${ETL_TESTS_DIR}/testUnorderedMap.cpp
        ${ETL_TESTS_DIR}/testUnorderedMultiMap.cpp
        ${ETL_TESTS_DIR}/testUnorderedSet.cpp
        ${ETL_TESTS_DIR}/testFifo.cpp
        ${ETL_TESTS_DIR}/testPool.cpp
        ${ETL_TESTS_DIR}/testSpan.cpp
        ${ETL_TESTS_DIR}/testBufStr.cpp
        ${ETL_TESTS_DIR}/testComplexScenarios.cpp
        ${ETL_TESTS_DIR}/testStaticInstance.cpp
        ${ETL_TESTS_DIR}/testLegacyBufStr.cpp
    )

    add_executable(testEtl ${ETL_TEST_SRCS} ${ETL_TEST_CMN})
    target_link_libraries(testEtl PRIVATE etl etl-prv-compile-if)
    target_include_directories(testEtl PRIVATE ${ETL_TESTS_DIR})
endif()

if(ETL_BUILD_BENCHMARKS)
    add_executable(etlInfo ${ETL_TESTS_DIR}/info.cpp ${ETL_TEST_CMN})
    target_link_libraries(etlInfo PRIVATE etl etl-prv-compile-if)
    target_include_directories(etlInfo PRIVATE ${ETL_TESTS_DIR})
    target_compile_definitions(etlInfo PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
endif()
