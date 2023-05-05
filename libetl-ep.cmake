# ETL library as CMake external project - template

include(ExternalProject)

# parameters

if(NOT DEFINED ETL_INSTALL_DIR)
    set(ETL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ep)
endif()

if(NOT DEFINED ETL_VERSION)
    set(ETL_VERSION master)
    message(WARNING "Using ETL master, please set ETL_VERSION to a fix version")
endif()

message(STATUS "ETL ${ETL_VERSION} to be installed to ${ETL_INSTALL_DIR}")

# ETL fetch and build

ExternalProject_Add (etl-ep
    GIT_REPOSITORY  https://github.com/baltth/ETL.git
    GIT_TAG         ${ETL_VERSION}
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX:PATH=${ETL_INSTALL_DIR}
                    # build type is Release by default
                    -DCMAKE_BUILD_TYPE:STRING=Release
                    # or pass the build type of your project like
                    # -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
)

# interface library to link

add_library(etl INTERFACE)
target_include_directories(etl INTERFACE ${ETL_INSTALL_DIR}/include)
target_link_libraries(etl INTERFACE ${ETL_INSTALL_DIR}/lib/libetl.a)
add_dependencies(etl etl-ep)

