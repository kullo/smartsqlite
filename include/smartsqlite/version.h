/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <string>

namespace SmartSqlite {

/// The SmartSqlite version
std::string version();

/// The SmartSqlite version as a linear number
int versionCode();

/// The SQLite version
std::string sqliteVersion();

/// Checks if SQLite header is the as same version as the linked SQLite library
bool checkSqliteVersion();

}
