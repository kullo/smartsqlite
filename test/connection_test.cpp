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
#include <memory>
#include <gmock/gmock.h>
#include <type_traits>

#include "smartsqlite/connection.h"
#include "smartsqlite/exceptions.h"

using namespace testing;


TEST(MakeConnection, shouldFailWhenOpeningFileInNonexistingFolder)
{
    EXPECT_THROW(SmartSqlite::makeConnection("/does/not/exist"),
                 SmartSqlite::Exception);
}

TEST(MakeConnection, shouldOpenInMemoryDatabase)
{
    SmartSqlite::makeConnection(":memory:");
}


class Connection : public Test
{
protected:
    SmartSqlite::Connection conn = SmartSqlite::makeConnection(":memory:");
};

TEST_F(Connection, canMove)
{
    EXPECT_THAT((std::is_move_constructible<SmartSqlite::Connection>::value),
                Eq(true));
    EXPECT_THAT((std::is_move_assignable<SmartSqlite::Connection>::value),
                Eq(true));
}

#ifndef _MSC_VER
TEST_F(Connection, cannotCopy)
{
    EXPECT_THAT((std::is_copy_constructible<SmartSqlite::Connection>::value),
                Eq(false));
    EXPECT_THAT((std::is_copy_assignable<SmartSqlite::Connection>::value),
                Eq(false));
}
#endif

TEST_F(Connection, canSetBusyTimeout)
{
    conn.setBusyTimeout(42);
}

static void traceFun(void *, const char *) {}

TEST_F(Connection, canSetTracingCallback)
{
    conn.setTracingCallback(&traceFun);
}

static void profileFun(void *, const char *, std::uint64_t) {}

TEST_F(Connection, canSetProfilingCallback)
{
    conn.setProfilingCallback(&profileFun);
}

TEST_F(Connection, canPrepareStatement)
{
    conn.prepare("PRAGMA user_version");
}

TEST_F(Connection, canExec)
{
    conn.exec("PRAGMA user_version = 42");
}

TEST_F(Connection, canBeginTransaction)
{
    conn.beginTransaction();
}

TEST_F(Connection, canBeginImmediateTransaction)
{
    conn.beginTransaction(SmartSqlite::Immediate);
}

TEST_F(Connection, canBeginExclusiveTransaction)
{
    conn.beginTransaction(SmartSqlite::Exclusive);
}

TEST_F(Connection, canCommitTransaction)
{
    conn.beginTransaction();
    conn.commitTransaction();
}

TEST_F(Connection, canRollbackTransaction)
{
    conn.beginTransaction();
    conn.rollbackTransaction();
}

TEST_F(Connection, canSetSavepoint)
{
    std::string savepoint = "abc123";
    conn.savepoint(savepoint);
}

TEST_F(Connection, canSetSavepointWithWeirdName)
{
    std::string savepoint = "abc'123";
    conn.savepoint(savepoint);
}

TEST_F(Connection, canReleaseSavepoint)
{
    std::string savepoint = "abc123";
    conn.savepoint(savepoint);
    conn.releaseSavepoint(savepoint);
}

TEST_F(Connection, canRollbackToSavepoint)
{
    std::string savepoint = "abc123";
    conn.savepoint(savepoint);
    conn.rollbackToSavepoint(savepoint);
}

TEST_F(Connection, canGetLastInsertRowId)
{
    conn.exec("CREATE TABLE foo (id INTEGER PRIMARY KEY);"
              "INSERT INTO foo VALUES (42)");
    EXPECT_THAT(conn.lastInsertRowId(), Eq(42));
}
