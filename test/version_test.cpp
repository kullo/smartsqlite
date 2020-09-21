/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include <gmock/gmock.h>

#include "smartsqlite/version.h"

using namespace testing;

TEST(Version, returnsVersion)
{
    std::string version = SmartSqlite::version();
    EXPECT_THAT(version, Not(IsEmpty()));
}

TEST(Version, returnsVersionCode)
{
    auto version = SmartSqlite::versionCode();
    EXPECT_THAT(version, Gt(0));
}

TEST(Version, versionEqualsVersionCode)
{
    auto version = SmartSqlite::version();
    auto fromVersionCode = "v" + std::to_string(SmartSqlite::versionCode());
    EXPECT_THAT(version, StrEq(fromVersionCode));
}

TEST(Version, returnsSqliteVersion)
{
    std::string version = SmartSqlite::sqliteVersion();
    EXPECT_THAT(version, Not(IsEmpty()));
}

TEST(Version, sqliteVersionCheckIsOk)
{
    EXPECT_THAT(SmartSqlite::checkSqliteVersion(), Eq(true));
}
