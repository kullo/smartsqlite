set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")

set_source_files_properties(sqlite3.c shell.c
    PROPERTIES
    COMPILE_DEFINITIONS
    "HAVE_USLEEP=1;SQLITE_USE_URI=1;SQLITE_ENABLE_API_ARMOR"
)
if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set_source_files_properties(sqlite3.c shell.c
        PROPERTIES
        COMPILE_FLAGS
        "-Wno-array-bounds -Wno-cast-qual -Wno-missing-field-initializers -Wno-switch-default"
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    set_source_files_properties(sqlite3.c shell.c
        PROPERTIES
        COMPILE_FLAGS
        "-Wno-cast-align -Wno-cast-qual -Wno-conversion -Wno-disabled-macro-expansion -Wno-double-promotion -Wno-float-equal -Wno-missing-noreturn -Wno-missing-prototypes -Wno-missing-variable-declarations -Wno-parentheses-equality -Wno-reserved-id-macro -Wno-sign-conversion -Wno-undef -Wno-unreachable-code -Wno-unreachable-code-break -Wno-unused-macros -Wno-unused-value"
    )
endif()

set(PUBLIC_HEADERS_DIR "${INCLUDE_DIR}/smartsqlite")
set(PUBLIC_HEADERS
    ${PUBLIC_HEADERS_DIR}/binder.h
    ${PUBLIC_HEADERS_DIR}/blob.h
    ${PUBLIC_HEADERS_DIR}/connection.h
    ${PUBLIC_HEADERS_DIR}/exceptions.h
    ${PUBLIC_HEADERS_DIR}/extractor.h
    ${PUBLIC_HEADERS_DIR}/logging.h
    ${PUBLIC_HEADERS_DIR}/nullable.h
    ${PUBLIC_HEADERS_DIR}/row.h
    ${PUBLIC_HEADERS_DIR}/scopedsavepoint.h
    ${PUBLIC_HEADERS_DIR}/scopedtransaction.h
    ${PUBLIC_HEADERS_DIR}/sqlite3.h
    ${PUBLIC_HEADERS_DIR}/statement.h
    ${PUBLIC_HEADERS_DIR}/util.h
    ${PUBLIC_HEADERS_DIR}/version.h
)

set(PRIVATE_HEADERS
    result_names.h
)

add_library(smartsqlite
    ${PUBLIC_HEADERS}
    ${PRIVATE_HEADERS}
    sqlite3.c
    binder.cpp
    blob.cpp
    connection.cpp
    exceptions.cpp
    extractor.cpp
    logging.cpp
    row.cpp
    util.cpp
    scopedsavepoint.cpp
    scopedtransaction.cpp
    statement.cpp
    version.cpp
)
target_include_directories(smartsqlite PUBLIC $<BUILD_INTERFACE:${INCLUDE_DIR}>)
if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    target_compile_options(smartsqlite
        PRIVATE
            # due to sqlite3
            -Wno-old-style-cast
            -Wno-reserved-id-macro
    )
endif()

# SQLite needs threading
find_package(Threads)
target_link_libraries(smartsqlite PUBLIC ${CMAKE_THREAD_LIBS_INIT})

if(UNIX)
    target_link_libraries(smartsqlite PUBLIC dl)
endif()

if(NOT IOS)
    add_executable(sqlite3 shell.c)
    target_include_directories(sqlite3 PRIVATE ${INCLUDE_DIR}/smartsqlite)
    target_link_libraries(sqlite3 smartsqlite)

    install(TARGETS sqlite3
        RUNTIME DESTINATION bin
    )
endif()

install(TARGETS smartsqlite
    EXPORT smartsqlite
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)
install(EXPORT smartsqlite DESTINATION cmake/smartsqlite)
install(
    DIRECTORY ${PUBLIC_HEADERS_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
