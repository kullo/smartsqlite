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
#include <chrono>
#include <gmock/gmock.h>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/scopedsavepoint.h"
#include "testutil.h"

using namespace testing;
using namespace TestUtil;

namespace {

std::string SP_NAME = "test_savepoint";

ConnPtr makeFileBackedConnection()
{
    auto conn = std::make_shared<SmartSqlite::Connection>("test.db");
    conn->exec("PRAGMA journal_mode=WAL");
    return conn;
}

void makeTestTable(const ConnPtr &conn)
{
    conn->exec("DROP TABLE IF EXISTS foo");
    conn->exec("CREATE TABLE foo (bar INTEGER)");
}

void insertTestData(const ConnPtr &conn)
{
    conn->exec("INSERT INTO foo VALUES (42)");
}

void readTestData(const ConnPtr &conn)
{
    conn->exec("SELECT * FROM foo");
}

}

TEST(ScopedSavepoint, ctorDoesntThrow)
{
    SmartSqlite::ScopedSavepoint{makeConnection(), SP_NAME};
}

TEST(ScopedSavepoint, ctorSetsSavepoint)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
    (void)sp;

    EXPECT_NO_THROW(conn->rollbackToSavepoint(SP_NAME));
}

TEST(ScopedSavepoint, dtorRollsBack)
{
    auto conn = makeConnection();
    int origVersion = 23;
    setUserVersion(conn, origVersion);
    {
        SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
        (void)sp;
        setUserVersion(conn, 42);
    }

    EXPECT_THAT(getUserVersion(conn), Eq(origVersion));
}

TEST(ScopedSavepoint, releaseReleasesSavepoint)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
    sp.release();

    EXPECT_THROW(conn->rollbackToSavepoint(SP_NAME), SmartSqlite::SqliteException);
}

TEST(ScopedSavepoint, releaseCancelsRollback)
{
    auto conn = makeConnection();
    setUserVersion(conn, 42);
    int version = 23;
    {
        SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
        sp.release();

        conn->savepoint(SP_NAME);
        setUserVersion(conn, version);
    }

    // user_version would be 42 if ~ScopedSavepoint would rollback
    EXPECT_THAT(getUserVersion(conn), Eq(version));
}

TEST(ScopedSavepoint, rollbackRollsBackChanges)
{
    auto conn = makeConnection();
    setUserVersion(conn, 23);

    SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
    setUserVersion(conn, 42);
    ASSERT_THAT(getUserVersion(conn), Eq(42));
    sp.rollback();

    EXPECT_THAT(getUserVersion(conn), Eq(23));
}

TEST(ScopedSavepoint, writingSavepointWithConcurrentWriter)
{
    // create the database and a table
    auto conn1 = makeFileBackedConnection();
    makeTestTable(conn1);

    // create a savepoint and upgrade it to being a writer
    SmartSqlite::ScopedSavepoint sp1(conn1, SP_NAME); (void)sp1;
    insertTestData(conn1);

    // second connection with a busy timeout, which we want to test
    auto conn2 = makeFileBackedConnection();
    auto busyTimeout = std::chrono::milliseconds(500);
    conn2->setBusyTimeout(static_cast<int>(busyTimeout.count()));

    // Concurrent writers are not allowed, so when creating an implicit writing
    // transaction for the statement, SQLite waits for the first writer to
    // finish. This doesn't happen, so we finally get SQLITE_BUSY.
    bool exceptionCaught = false;
    std::string exceptionMessage;
    auto startTime = std::chrono::steady_clock::now();
    try
    {
        insertTestData(conn2);
    }
    catch (const SmartSqlite::SqliteException &ex)
    {
        exceptionCaught = true;
        exceptionMessage = ex.what();
    }
    auto endTime = std::chrono::steady_clock::now();
    ASSERT_THAT(exceptionCaught, Eq(true));

    // the busy timeout has passed
    EXPECT_THAT(endTime - startTime, Ge(busyTimeout));
    EXPECT_THAT(exceptionMessage, HasSubstr("SQLITE_BUSY"));
    EXPECT_THAT(exceptionMessage, Not(HasSubstr("SQLITE_BUSY_")));
}

TEST(ScopedSavepoint, savepointUpgradeWithConcurrentWriter)
{
    // create the database and a table
    auto conn1 = makeFileBackedConnection();
    makeTestTable(conn1);

    // create a savepoint and upgrade it to being a writer
    SmartSqlite::ScopedSavepoint sp1(conn1, SP_NAME); (void)sp1;
    insertTestData(conn1);

    // second connection with a busy timeout, which we want to test
    auto conn2 = makeFileBackedConnection();
    auto busyTimeout = std::chrono::milliseconds(500);
    conn2->setBusyTimeout(static_cast<int>(busyTimeout.count()));

    // create a savepoint on the second connection and make it a reader
    SmartSqlite::ScopedSavepoint sp2(conn2, SP_NAME); (void)sp2;
    readTestData(conn2);

    // Concurrent writers are not allowed, so trying to upgrade the second
    // savepoint from reader to writer, SQLite fails immediately with
    // SQLITE_BUSY to prevent inconsistencies.
    bool exceptionCaught = false;
    std::string exceptionMessage;
    auto startTime = std::chrono::steady_clock::now();
    try
    {
        insertTestData(conn2);
    }
    catch (const SmartSqlite::SqliteException &ex)
    {
        exceptionCaught = true;
        exceptionMessage = ex.what();
    }
    auto endTime = std::chrono::steady_clock::now();
    ASSERT_THAT(exceptionCaught, Eq(true));

    // the busy timeout has not passed
    EXPECT_THAT(endTime - startTime, Lt(busyTimeout / 2));
    EXPECT_THAT(exceptionMessage, HasSubstr("SQLITE_BUSY"));
    EXPECT_THAT(exceptionMessage, Not(HasSubstr("SQLITE_BUSY_")));
}

TEST(ScopedSavepoint, savepointUpgradeWithPreviousWrites)
{
    // create the database and a table
    auto conn1 = makeFileBackedConnection();
    makeTestTable(conn1);

    // second connection with a busy timeout, which we want to test
    auto conn2 = makeFileBackedConnection();
    auto busyTimeout = std::chrono::milliseconds(500);
    conn2->setBusyTimeout(static_cast<int>(busyTimeout.count()));

    // create a savepoint on the second connection and make it a reader
    SmartSqlite::ScopedSavepoint sp2(conn2, SP_NAME); (void)sp2;
    readTestData(conn2);

    // write something on the first connection, so that sp2 now operates on an
    // old snapshot
    insertTestData(conn1);

    // Writing based on an old snapshot is not allowed, so trying to upgrade sp2
    // from reader to writer, SQLite fails immediately with SQLITE_BUSY_SNAPSHOT
    // to prevent inconsistencies.
    bool exceptionCaught = false;
    std::string exceptionMessage;
    auto startTime = std::chrono::steady_clock::now();
    try
    {
        insertTestData(conn2);
    }
    catch (const SmartSqlite::SqliteException &ex)
    {
        exceptionCaught = true;
        exceptionMessage = ex.what();
    }
    auto endTime = std::chrono::steady_clock::now();
    ASSERT_THAT(exceptionCaught, Eq(true));

    // the busy timeout has not passed
    EXPECT_THAT(endTime - startTime, Lt(busyTimeout / 2));
    EXPECT_THAT(exceptionMessage, HasSubstr("SQLITE_BUSY_SNAPSHOT"));
}
