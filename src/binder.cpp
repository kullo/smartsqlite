/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "smartsqlite/binder.h"

#include <cassert>
#include <limits>

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

    assert(size <= std::numeric_limits<int>::max());
    auto sizeInt = static_cast<int>(size);

    return sqlite3_bind_text(stmt, pos + 1, data, sizeInt, SQLITE_TRANSIENT);
}

int NativeBinder::bindBlob(sqlite3_stmt *stmt, int pos, const void *data, size_t size)
{
    if (!data && !size) data = EMPTY_CSTRING;

    assert(size <= std::numeric_limits<int>::max());
    auto sizeInt = static_cast<int>(size);

    return sqlite3_bind_blob(stmt, pos + 1, data, sizeInt, SQLITE_TRANSIENT);
}

}
