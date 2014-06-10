add_executable(${PROJECT_NAME}_tests
    connection_test.cpp
    statement_test.cpp
    version_test.cpp
)

set(extra_link_libraries)
if (NOT MINGW)
    list(APPEND extra_link_libraries dl)
endif()

target_link_libraries(${PROJECT_NAME}_tests sqlitewrapper sqlite gmock gmock_main ${extra_link_libraries})

enable_testing()
add_test(all_tests ${PROJECT_NAME}_tests)

