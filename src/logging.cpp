/*
 * Copyright 2014–2016 The SmartSqlite contributors (see NOTICE.txt)
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
#include "smartsqlite/logging.h"

#include "smartsqlite/sqlite3.h"

void SmartSqlite::setLogCallback(SmartSqlite::LogCallback *callback, void *extraArg)
{
    sqlite3_config(SQLITE_CONFIG_LOG, callback, extraArg);
}

std::string SmartSqlite::errcodeToString(int errcode)
{
    return sqlite3_errstr(errcode);
}
