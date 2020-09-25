/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "smartsqlite/version.h"

#include <cstring>
#include <iostream>

#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

namespace {
const auto SMARTSQLITE_VERSION_CODE = int{26};
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
