include_directories("../include")

set_source_files_properties(sqlite3.c PROPERTIES COMPILE_FLAGS "-O2 -DHAVE_USLEEP=1 -g")

add_library (sqlitewrapper
    sqlite3.c
    connection.cpp
    exceptions.cpp
    util.cpp
    statement.cpp
    version.cpp
)
