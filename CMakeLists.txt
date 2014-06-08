cmake_minimum_required(VERSION 2.8)
project(sqlitewrapper)

set(INCLUDES
    connection.h
    exceptions.h
    util.h
    result_names.h
    version.h
)

set(LIB_SRC
    connection.cpp
    exceptions.cpp
    sqlite3.c
    util.cpp
    version.cpp
)

set(TEST_SRC
    connection_test.cpp
    version_test.cpp
)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS} -g")

# Google Mock / Test
add_subdirectory ("../gmock" "lib/gmock")
include_directories("${gmock_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}/include")

enable_testing()
add_executable(${PROJECT_NAME}_tests ${INCLUDES} ${TEST_SRC} ${LIB_SRC})
target_link_libraries(${PROJECT_NAME}_tests gmock gmock_main dl)
add_test(all_tests ${PROJECT_NAME}_tests)
