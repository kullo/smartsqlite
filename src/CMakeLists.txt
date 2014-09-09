set(INCLUDE_DIR "../include")
include_directories(${INCLUDE_DIR})

set_source_files_properties(sqlite3.c PROPERTIES COMPILE_FLAGS "-DHAVE_USLEEP=1 -DSQLITE_USE_URI=1")

set(PUBLIC_HEADERS_DIR "${INCLUDE_DIR}/sqlitewrapper")
set(PUBLIC_HEADERS
    ${PUBLIC_HEADERS_DIR}/binder.h
    ${PUBLIC_HEADERS_DIR}/blob.h
    ${PUBLIC_HEADERS_DIR}/connection.h
    ${PUBLIC_HEADERS_DIR}/exceptions.h
    ${PUBLIC_HEADERS_DIR}/extractor.h
    ${PUBLIC_HEADERS_DIR}/nullable.h
    ${PUBLIC_HEADERS_DIR}/row.h
    ${PUBLIC_HEADERS_DIR}/sqlite3.h
    ${PUBLIC_HEADERS_DIR}/statement.h
    ${PUBLIC_HEADERS_DIR}/util.h
    ${PUBLIC_HEADERS_DIR}/version.h
)

set(PRIVATE_HEADERS
    result_names.h
)

add_library(sqlitewrapper
    ${PUBLIC_HEADERS}
    ${PRIVATE_HEADERS}
    sqlite3.c
    binder.cpp
    blob.cpp
    connection.cpp
    exceptions.cpp
    extractor.cpp
    row.cpp
    util.cpp
    statement.cpp
    version.cpp
)
