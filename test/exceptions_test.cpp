/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include <gmock/gmock.h>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/sqlite3.h"

using namespace testing;

TEST(Exceptions, sqliteExceptionForBasicCode)
{
    auto uut = SmartSqlite::SqliteException("foo", SQLITE_BUSY);
    EXPECT_THAT(uut.what(), HasSubstr("SQLITE_BUSY"));
    EXPECT_THAT(uut.what(), Not(HasSubstr("SQLITE_BUSY_")));
    EXPECT_THAT(uut.what(), Not(HasSubstr("UNKNOWN")));
}

TEST(Exceptions, sqliteExceptionForExtendedCode)
{
    auto uut = SmartSqlite::SqliteException("foo", SQLITE_BUSY_SNAPSHOT);
    EXPECT_THAT(uut.what(), HasSubstr("SQLITE_BUSY_SNAPSHOT"));
    EXPECT_THAT(uut.what(), Not(HasSubstr("UNKNOWN")));
}

TEST(Exceptions, sqliteExceptionForUnknownBasicCode)
{
    auto uut = SmartSqlite::SqliteException("foo", 250);
    EXPECT_THAT(uut.what(), HasSubstr("UNKNOWN"));
    EXPECT_THAT(uut.what(), Not(HasSubstr("UNKNOWN (SQLITE_")));
}

TEST(Exceptions, sqliteExceptionForUnknownExtendedCode)
{
    auto uut = SmartSqlite::SqliteException("foo", SQLITE_BUSY_SNAPSHOT | (1337 << 8));
    EXPECT_THAT(uut.what(), HasSubstr("UNKNOWN (SQLITE_BUSY)"));
}
