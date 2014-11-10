cmake_minimum_required(VERSION 2.8)
project(sqlitewrapper)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -g ${CMAKE_CXX_FLAGS}")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -g")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -g ${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wformat=2 -Wlogical-op -Wmissing-include-dirs -Wnon-virtual-dtor -Woverloaded-virtual -Wswitch-default -Wuninitialized")

    # Hardening
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector -pie -fPIE -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -Wl,-z,relro,-z,now")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector -pie -fPIE -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -Wl,-z,relro,-z,now")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # use shared MSVC++ runtime, see http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
       if(${flag_var} MATCHES "/MT")
          string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
       endif()
    endforeach()
    # force gtest to use shared MSVC++ runtime
    set(gtest_force_shared_crt ON)
endif()

if(APPLE)
  # for compatibility during linking with code built with qmake, choose the same settings that qmake chooses
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -stdlib=libc++ -mmacosx-version-min=10.7")
endif()

# Google Mock / Google Test
add_subdirectory("lib/gmock")
include_directories("${gmock_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}/include")

# sqlitewrapper
add_subdirectory("src")
include_directories("include")

# tests
add_subdirectory("test")

enable_testing()
add_test(all_tests test/${PROJECT_NAME}_tests)
