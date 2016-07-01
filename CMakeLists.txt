cmake_minimum_required(VERSION 2.8.12)  # version provided by Ubuntu 14.04

if((NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(install_prefix_suffix "-debug")
else()
    set(install_prefix_suffix "")
endif()
set(
    CMAKE_INSTALL_PREFIX
        "${CMAKE_CURRENT_BINARY_DIR}/../bin-smartsqlite${install_prefix_suffix}"
    CACHE PATH "destination for make install"
)

if(NOT ("${CMAKE_VERSION}" VERSION_LESS "3.1"))
    set(CMAKE_INSTALL_MESSAGE LAZY
        CACHE STRING "Show messages during install? Lazy means only changed.")
    set_property(CACHE CMAKE_INSTALL_MESSAGE
        PROPERTY STRINGS "ALWAYS" "LAZY" "NEVER")
endif()

project(smartsqlite)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(CompilerSettings)

# Google Mock / Google Test
add_subdirectory("lib/gmock")
include_directories("${gmock_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}/include")

add_subdirectory("src")
add_subdirectory("test")

enable_testing()
add_test(all_tests test/${PROJECT_NAME}_tests)
