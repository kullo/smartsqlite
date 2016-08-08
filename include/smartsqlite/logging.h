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
#pragma once

#include <string>

namespace SmartSqlite {

using LogCallback = void(void *extraArg, int errcode, char *message);

/**
 * @brief Registers a log function with SQLite.
 *
 * WARNING: Must only be called before any other access to SQLite is made
 * because sqlite3_config is used internally. See
 * https://www.sqlite.org/c3ref/config.html for details on this behavior.
 *
 * For details on SQLite error logging, see https://www.sqlite.org/errlog.html
 */
void setLogCallback(LogCallback *callback, void *extraArg = nullptr);

std::string errcodeToString(int errcode);

}
