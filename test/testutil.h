/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
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
