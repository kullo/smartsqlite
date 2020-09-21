/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
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
