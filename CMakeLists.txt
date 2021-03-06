cmake_minimum_required(VERSION 2.8.12)  # version provided by Ubuntu 14.04

if(((NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE STREQUAL "Debug")) AND (NOT CMAKE_GENERATOR STREQUAL "Xcode"))
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
if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    foreach(target gtest gtest_main gmock gmock_main)
        target_compile_options(${target}
            PUBLIC
                -Wno-deprecated
                -Wno-float-equal
                -Wno-missing-noreturn
                -Wno-shift-sign-overflow
                -Wno-undef
                -Wno-used-but-marked-unused
            PRIVATE
                -w
        )
    endforeach()
endif()

find_package(botan
    NO_MODULE
    PATHS "${CMAKE_CURRENT_BINARY_DIR}/../bin-botan${install_prefix_suffix}"
    NO_DEFAULT_PATH
)

add_subdirectory("src")
add_subdirectory("test")

enable_testing()
add_test(all_tests test/${PROJECT_NAME}_tests)
