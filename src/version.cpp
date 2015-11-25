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
#include "smartsqlite/version.h"

#include <cstring>

#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

namespace {
const auto SMARTSQLITE_VERSION_CODE = int{3};
const auto SMARTSQLITE_VERSION_STRING = std::string{"v3"};
}

std::string version()
{
    return SMARTSQLITE_VERSION_STRING;
}

int versionCode()
{
    return SMARTSQLITE_VERSION_CODE;
}

std::string sqliteVersion()
{
    return sqlite3_libversion();
}

bool checkSqliteVersion()
{
    return
            (sqlite3_libversion_number() == SQLITE_VERSION_NUMBER) &&
            (std::strcmp(sqlite3_sourceid(), SQLITE_SOURCE_ID) == 0) &&
            (std::strcmp(sqlite3_libversion(), SQLITE_VERSION) == 0);
}

}
