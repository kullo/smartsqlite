add_executable(smartsqlite_tests
    blob_test.cpp
    connection_test.cpp
    logging_test.cpp
    nullable_test.cpp
    scopedtransaction_test.cpp
    statement_test.cpp
    version_test.cpp
)

set(extra_link_libraries)
if(NOT WIN32)
    list(APPEND extra_link_libraries dl)
endif()

target_link_libraries(smartsqlite_tests
    smartsqlite
    gmock
    gmock_main
)
