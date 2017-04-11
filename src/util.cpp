/*
 * Copyright 2014–2017 The SmartSqlite contributors (see NOTICE.txt)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
