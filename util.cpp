#include "util.h"

#include "exceptions.h"
#include "sqlite/sqlite3.h"

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
