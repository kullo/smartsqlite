/*
 * Copyright 2014–2015 The SmartSqlite contributors (see NOTICE.txt)
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
#include "smartsqlite/binder.h"

#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

namespace {

const char *EMPTY_CSTRING = "";

}

int NativeBinder::bindLongLong(sqlite3_stmt *stmt, int pos, long long value)
{
    return sqlite3_bind_int64(stmt, pos + 1, value);
}

int NativeBinder::bindDouble(sqlite3_stmt *stmt, int pos, double value)
{
    return sqlite3_bind_double(stmt, pos + 1, value);
}

int NativeBinder::bindString(sqlite3_stmt *stmt, int pos, const std::string &value)
{
    auto data = value.c_str();
    auto size = value.size();
    if (!data && !size) data = EMPTY_CSTRING;
    return sqlite3_bind_text(stmt, pos + 1, data, size, SQLITE_TRANSIENT);
}

int NativeBinder::bindBlob(sqlite3_stmt *stmt, int pos, const void *data, size_t size)
{
    if (!data && !size) data = EMPTY_CSTRING;
    return sqlite3_bind_blob(stmt, pos + 1, data, size, SQLITE_TRANSIENT);
}

}
