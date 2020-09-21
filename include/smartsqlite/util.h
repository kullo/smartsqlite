/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <string>

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

struct sqlite3;
struct sqlite3_stmt;

#define CHECK_RESULT(result) \
    SmartSqlite::checkResult(__func__, (result))

#define CHECK_RESULT_CONN(result, conn) \
    SmartSqlite::checkResult(__func__, (result), (conn))

#define CHECK_RESULT_STMT(result, conn, stmt) \
    SmartSqlite::checkResult(__func__, (result), (conn), (stmt))

#define CHECK_RESULT_MSG(result, message) \
    SmartSqlite::checkResult(__func__, (result), (message))

namespace SmartSqlite {

void checkResult(const std::string &func, int result);
void checkResult(const std::string &func, int result, sqlite3 *conn, sqlite3_stmt *stmt = nullptr);
void checkResult(const std::string &func, int result, const std::string &message);

}
