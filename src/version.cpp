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
#include "smartsqlite/version.h"

#include <cstring>
#include <iostream>

#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

namespace {
const auto SMARTSQLITE_VERSION_CODE = int{21};
const auto SMARTSQLITE_VERSION_STRING = "v" + std::to_string(SMARTSQLITE_VERSION_CODE);
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
    auto logPrefix = "SmartSqlite version check failed: ";
    bool success = true;

    if (sqlite3_libversion_number() != SQLITE_VERSION_NUMBER)
    {
        success = false;
        std::cerr << logPrefix
                  << "sqlite3_libversion_number()==" << sqlite3_libversion_number()
                  << ", SQLITE_VERSION_NUMBER==" << SQLITE_VERSION_NUMBER
                  << std::endl;
    }

    if (std::strcmp(sqlite3_sourceid(), SQLITE_SOURCE_ID))
    {
        success = false;
        std::cerr << logPrefix
                  << "sqlite3_sourceid()==" << sqlite3_sourceid()
                  << ", SQLITE_SOURCE_ID==" << SQLITE_SOURCE_ID
                  << std::endl;
    }

    if (std::strcmp(sqlite3_libversion(), SQLITE_VERSION))
    {
        success = false;
        std::cerr << logPrefix
                  << "sqlite3_libversion()==" << sqlite3_libversion()
                  << ", SQLITE_VERSION==" << SQLITE_VERSION
                  << std::endl;
    }

    return success;
}

}
