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

#include <memory>
#include "smartsqlite/connection.h"

namespace TestUtil {

using ConnPtr = std::shared_ptr<SmartSqlite::Connection>;

inline ConnPtr makeConnection()
{
    return std::make_shared<SmartSqlite::Connection>(":memory:");
}

inline int getUserVersion(ConnPtr conn)
{
    auto stmt = conn->prepare("PRAGMA user_version");
    return stmt.execWithSingleResult().get<int>(0);
}

inline void setUserVersion(ConnPtr conn, int version)
{
    conn->exec("PRAGMA user_version = " + std::to_string(version));
}

}
