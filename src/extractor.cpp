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
