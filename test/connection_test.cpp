#include <memory>
#include <gmock/gmock.h>
#include <type_traits>

#include "sqlitewrapper/connection.h"
#include "sqlitewrapper/exceptions.h"

using namespace testing;


TEST(MakeConnection, shouldFailWhenOpeningFileInNonexistingFolder)
{
    EXPECT_THROW(SqliteWrapper::makeConnection("/does/not/exist"),
                 SqliteWrapper::Exception);
}

TEST(MakeConnection, shouldOpenInMemoryDatabase)
{
    SqliteWrapper::makeConnection(":memory:");
}


class Connection : public Test
{
protected:
    SqliteWrapper::Connection conn = SqliteWrapper::makeConnection(":memory:");
};

TEST_F(Connection, canMove)
{
    EXPECT_THAT((std::is_move_constructible<SqliteWrapper::Connection>::value),
                Eq(true));
    EXPECT_THAT((std::is_move_assignable<SqliteWrapper::Connection>::value),
                Eq(true));
}

TEST_F(Connection, cannotCopy)
{
    EXPECT_THAT((std::is_copy_constructible<SqliteWrapper::Connection>::value),
                Eq(false));
    EXPECT_THAT((std::is_copy_assignable<SqliteWrapper::Connection>::value),
                Eq(false));
}

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
    conn.beginTransaction(SqliteWrapper::Immediate);
}

TEST_F(Connection, canBeginExclusiveTransaction)
{
    conn.beginTransaction(SqliteWrapper::Exclusive);
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
