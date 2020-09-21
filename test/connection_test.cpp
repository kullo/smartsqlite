/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include <memory>
#include <gmock/gmock.h>
#include <type_traits>

#include "smartsqlite/connection.h"
#include "smartsqlite/exceptions.h"

using namespace testing;

TEST(ConnectionConstructor, shouldFailWhenOpeningFileInNonexistingFolder)
{
    EXPECT_THROW(SmartSqlite::Connection("/does/not/exist"),
                 SmartSqlite::Exception);
}

TEST(ConnectionConstructor, constructInMemoryDatabase)
{
    SmartSqlite::Connection(":memory:");
}

TEST(ConnectionConstructor, canMove)
{
    EXPECT_THAT((std::is_move_constructible<SmartSqlite::Connection>::value),
                Eq(true));
    EXPECT_THAT((std::is_move_assignable<SmartSqlite::Connection>::value),
                Eq(true));
}

#ifndef _MSC_VER
TEST(ConnectionConstructor, cannotCopy)
{
    EXPECT_THAT((std::is_copy_constructible<SmartSqlite::Connection>::value),
                Eq(false));
    EXPECT_THAT((std::is_copy_assignable<SmartSqlite::Connection>::value),
                Eq(false));
}
#endif

class Connection : public Test
{
protected:
    SmartSqlite::Connection conn = SmartSqlite::Connection(":memory:");
};

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

TEST_F(Connection, canGetChanges)
{
    EXPECT_THAT(conn.changes(), Eq(0));
    conn.exec("CREATE TABLE foo (id INTEGER PRIMARY KEY, value INTEGER);"
              "INSERT INTO foo VALUES (42, NULL)");
    EXPECT_THAT(conn.changes(), Eq(1));
    conn.exec("INSERT INTO foo VALUES (43, NULL);"
              "UPDATE foo SET value = 23");
    EXPECT_THAT(conn.changes(), Eq(2));
}
