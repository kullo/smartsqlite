/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "smartsqlite/extractor.h"

#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

long long NativeExtractor::extractLongLong(sqlite3_stmt *stmt, int pos)
{
    return sqlite3_column_int64(stmt, pos);
}

double NativeExtractor::extractDouble(sqlite3_stmt *stmt, int pos)
{
    return sqlite3_column_double(stmt, pos);
}

std::string NativeExtractor::extractString(sqlite3_stmt *stmt, int pos)
{
    return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, pos)));
}

std::vector<unsigned char> NativeExtractor::extractBlob(sqlite3_stmt *stmt, int pos)
{
    int size = sqlite3_column_bytes(stmt, pos);
    if (size == 0) return std::vector<unsigned char>();

    const unsigned char *data = reinterpret_cast<const unsigned char*>(sqlite3_column_blob(stmt, pos));
    return std::vector<unsigned char>(data, data + size);
}

}
