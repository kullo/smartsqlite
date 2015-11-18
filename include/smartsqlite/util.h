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
