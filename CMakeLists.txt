cmake_minimum_required(VERSION 2.8)
project(sqlitewrapper)

set(CMAKE_CXX_FLAGS "-std=c++11 -O2 ${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wformat=2 -Wlogical-op -Wmissing-include-dirs -Wnon-virtual-dtor -Woverloaded-virtual -Wswitch-default -Wuninitialized")

# SQLite
add_subdirectory("sqlite")
include_directories("sqlite")

# Google Mock / Test
add_subdirectory("../gmock" "lib/gmock")
include_directories("${gmock_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}/include")

add_subdirectory("src")

enable_testing()
include_directories("include")
add_executable(${PROJECT_NAME}_tests
    connection_test.cpp
    statement_test.cpp
    version_test.cpp
)
target_link_libraries(${PROJECT_NAME}_tests sqlitewrapper sqlite gmock gmock_main dl)
add_test(all_tests ${PROJECT_NAME}_tests)
