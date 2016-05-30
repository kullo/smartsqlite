add_executable(smartsqlite_tests
    blob_test.cpp
    connection_test.cpp
    logging_test.cpp
    nullable_test.cpp
    scopedsavepoint_test.cpp
    scopedtransaction_test.cpp
    statement_test.cpp
    testutil.h
    version_test.cpp
)

target_link_libraries(smartsqlite_tests
    PUBLIC
        smartsqlite
        gmock
        gmock_main
)
