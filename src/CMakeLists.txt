include_directories("../include")

if(APPLE)
  set_source_files_properties(sqlite3.c PROPERTIES COMPILE_FLAGS "-O2 -DHAVE_USLEEP=1 -g -D_FORTIFY_SOURCE=0")
else()
  set_source_files_properties(sqlite3.c PROPERTIES COMPILE_FLAGS "-O2 -DHAVE_USLEEP=1 -g")
endif()

add_library (sqlitewrapper
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
