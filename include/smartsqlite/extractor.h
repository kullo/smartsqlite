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
#pragma once

#include <string>
#include <vector>

struct sqlite3_stmt;

namespace SmartSqlite {

class NativeExtractor
{
public:
    static long long extractLongLong(sqlite3_stmt *stmt, int pos);
    static double extractDouble(sqlite3_stmt *stmt, int pos);
    static std::string extractString(sqlite3_stmt *stmt, int pos);
    static std::vector<unsigned char> extractBlob(sqlite3_stmt *stmt, int pos);
};

// extension point: specialize this to add support for custom types
template <typename T>
class Extractor
{
public:
    static T extract(sqlite3_stmt *stmt, int pos);
};

template <>
class Extractor<std::string>
{
public:
    static std::string extract(sqlite3_stmt *stmt, int pos)
    {
        return NativeExtractor::extractString(stmt, pos);
    }
};

template <>
class Extractor<std::vector<unsigned char>>
{
public:
    static std::vector<unsigned char> extract(sqlite3_stmt *stmt, int pos)
    {
        return NativeExtractor::extractBlob(stmt, pos);
    }
};

}
