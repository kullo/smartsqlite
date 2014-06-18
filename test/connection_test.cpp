#include <memory>
#include <gmock/gmock.h>
#include <type_traits>

#include "sqlitewrapper/connection.h"
#include "sqlitewrapper/exceptions.h"

using namespace testing;

TEST(Connection, canMove)
{
    EXPECT_THAT((std::is_move_constructible<SqliteWrapper::Connection>::value),
                Eq(true));
    EXPECT_THAT((std::is_move_assignable<SqliteWrapper::Connection>::value),
                Eq(true));
}

TEST(Connection, cannotCopy)
{
    EXPECT_THAT((std::is_copy_constructible<SqliteWrapper::Connection>::value),
                Eq(false));
    EXPECT_THAT((std::is_copy_assignable<SqliteWrapper::Connection>::value),
                Eq(false));
}

TEST(Connection, cannotOpenFileInNonexistingFolder)
{
    EXPECT_THROW(SqliteWrapper::Connection("/does/not/exist"),
                 SqliteWrapper::Exception);
}

TEST(Connection, canBeOpened)
{
    SqliteWrapper::Connection(":memory:");
}

TEST(Connection, canSetBusyTimeout)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.setBusyTimeout(42);
}

static void traceFun(void *, const char *) {}

TEST(Connection, canSetTracingCallback)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.setTracingCallback(&traceFun);
}

static void profileFun(void *, const char *, std::uint64_t) {}

TEST(Connection, canSetProfilingCallback)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.setProfilingCallback(&profileFun);
}

TEST(Connection, canPrepareStatement)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.prepare("PRAGMA user_version");
}

TEST(Connection, canExec)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.exec("PRAGMA user_version = 42");
}

TEST(Connection, canBeginTransaction)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.beginTransaction();
}

TEST(Connection, canBeginImmediateTransaction)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.beginTransaction(SqliteWrapper::Immediate);
}

TEST(Connection, canBeginExclusiveTransaction)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.beginTransaction(SqliteWrapper::Exclusive);
}

TEST(Connection, canCommitTransaction)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.beginTransaction();
    conn.commitTransaction();
}

TEST(Connection, canRollbackTransaction)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.beginTransaction();
    conn.rollbackTransaction();
}

TEST(Connection, canSetSavepoint)
{
    SqliteWrapper::Connection conn(":memory:");
    std::string savepoint = "abc123";
    conn.savepoint(savepoint);
}

TEST(Connection, canSetSavepointWithWeirdName)
{
    SqliteWrapper::Connection conn(":memory:");
    std::string savepoint = "abc'123";
    conn.savepoint(savepoint);
}

TEST(Connection, canReleaseSavepoint)
{
    SqliteWrapper::Connection conn(":memory:");
    std::string savepoint = "abc123";
    conn.savepoint(savepoint);
    conn.releaseSavepoint(savepoint);
}

TEST(Connection, canRollbackToSavepoint)
{
    SqliteWrapper::Connection conn(":memory:");
    std::string savepoint = "abc123";
    conn.savepoint(savepoint);
    conn.rollbackToSavepoint(savepoint);
}

TEST(Connection, canGetLastInsertRowId)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.exec("CREATE TABLE foo (id INTEGER PRIMARY KEY);"
              "INSERT INTO foo VALUES (42)");
    EXPECT_THAT(conn.lastInsertRowId(), Eq(42));
}
