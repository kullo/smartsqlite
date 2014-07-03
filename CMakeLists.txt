cmake_minimum_required(VERSION 2.8)
project(sqlitewrapper)

set(CMAKE_CXX_FLAGS "-std=c++11 -O2 ${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wformat=2 -Wlogical-op -Wmissing-include-dirs -Wnon-virtual-dtor -Woverloaded-virtual -Wswitch-default -Wuninitialized")

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
