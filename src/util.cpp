#include "sqlitewrapper/util.h"

#include "sqlitewrapper/exceptions.h"
#include "sqlitewrapper/sqlite3.h"

namespace SqliteWrapper {

void checkResult(int result)
{
    if (result == SQLITE_OK) return;

    throw SqliteException(result);
}

void checkResult(int result, sqlite3 *conn)
{
    if (result == SQLITE_OK) return;

    throw SqliteException(result, sqlite3_errmsg(conn));
}

void checkResult(int result, const std::string &message)
{
    if (result == SQLITE_OK) return;

    throw SqliteException(result, message);
}

}
