/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "smartsqlite/util.h"

#include "smartsqlite/exceptions.h"
#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

void checkResult(const std::string &func, int result)
{
    if (result == SQLITE_OK) return;

    throw SqliteException(func, result);
}

void checkResult(const std::string &func, int result, sqlite3 *conn, sqlite3_stmt *stmt)
{
    if (result == SQLITE_OK) return;

    std::string errmsg = sqlite3_errmsg(conn);
    if (stmt)
    {
        errmsg += "\nSQL: " + std::string(sqlite3_sql(stmt));
    }
    throw SqliteException(func, result, errmsg);
}

void checkResult(const std::string &func, int result, const std::string &message)
{
    if (result == SQLITE_OK) return;

    throw SqliteException(func, result, message);
}

}
