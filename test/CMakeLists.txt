add_executable(${PROJECT_NAME}_tests
    connection_test.cpp
    statement_test.cpp
    version_test.cpp
)
target_link_libraries(${PROJECT_NAME}_tests sqlitewrapper sqlite gmock gmock_main dl)

enable_testing()
add_test(all_tests ${PROJECT_NAME}_tests)

