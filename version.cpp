#include "version.h"

#include <cstring>

#include "sqlite/sqlite3.h"

namespace SqliteWrapper {

std::string sqliteVersion()
{
    return sqlite3_libversion();
}

bool checkVersion()
{
    return
            (sqlite3_libversion_number() == SQLITE_VERSION_NUMBER) &&
            (std::strcmp(sqlite3_sourceid(), SQLITE_SOURCE_ID) == 0) &&
            (std::strcmp(sqlite3_libversion(), SQLITE_VERSION) == 0);
}

}
