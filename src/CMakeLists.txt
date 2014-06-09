include_directories("../include" ${sqlite_SOURCE_DIR})

add_library (sqlitewrapper
    connection.cpp
    exceptions.cpp
    util.cpp
    statement.cpp
    version.cpp
)
