#include "sqlitewrapper/util.h"

#include "sqlite3.h"
#include "sqlitewrapper/exceptions.h"

namespace SqliteWrapper {

void checkResult(int result)
{
    if (result == SQLITE_OK) return;

    throw SqliteException(result);
}

void checkResult(int result, const std::string &message)
{
    if (result == SQLITE_OK) return;

    throw SqliteException(result, message);
}

}
