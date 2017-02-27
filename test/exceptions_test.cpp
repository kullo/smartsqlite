/*
 * Copyright 2014–2017 The SmartSqlite contributors (see NOTICE.txt)
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
