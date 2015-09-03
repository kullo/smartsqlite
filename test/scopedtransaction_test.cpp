#include <memory>
#include <gmock/gmock.h>

#include "smartsqlite/connection.h"
#include "smartsqlite/exceptions.h"
#include "smartsqlite/scopedtransaction.h"

using namespace testing;
using ConnPtr = std::shared_ptr<SmartSqlite::Connection>;

static ConnPtr makeConnection()
{
    return std::make_shared<SmartSqlite::Connection>(
                SmartSqlite::makeConnection(":memory:"));
}

static int getUserVersion(ConnPtr conn)
{
    auto stmt = conn->prepare("PRAGMA user_version");
    return stmt.execWithSingleResult().get<int>(0);
}

static void setUserVersion(ConnPtr conn, int version)
{
    conn->exec("PRAGMA user_version = " + std::to_string(version));
}


TEST(ScopedTransaction, ctorDoesntThrow)
{
    SmartSqlite::ScopedTransaction tx(makeConnection());
}

TEST(ScopedTransaction, ctorBeginsTransaction)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedTransaction tx(conn);
    (void)tx;

    EXPECT_THROW(conn->beginTransaction(), SmartSqlite::SqliteException);
}

TEST(ScopedTransaction, dtorRollsBack)
{
    auto conn = makeConnection();
    int origVersion = 23;
    setUserVersion(conn, origVersion);
    {
        SmartSqlite::ScopedTransaction tx(conn);
        (void)tx;
        setUserVersion(conn, 42);
    }

    EXPECT_THAT(getUserVersion(conn), Eq(origVersion));
}

TEST(ScopedTransaction, commitEndsTransaction)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedTransaction tx(conn);
    tx.commit();

    // would throw if commit didn't end the transaction
    conn->beginTransaction();
}

TEST(ScopedTransaction, commitCancelsRollback)
{
    auto conn = makeConnection();
    setUserVersion(conn, 42);
    int version = 23;
    {
        SmartSqlite::ScopedTransaction tx(conn);
        tx.commit();

        conn->beginTransaction();
        setUserVersion(conn, version);
    }

    // user_version would be 42 if ~ScopedTransaction would rollback
    EXPECT_THAT(getUserVersion(conn), Eq(version));
}

TEST(ScopedTransaction, rollbackEndsTransaction)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedTransaction tx(conn);
    tx.rollback();

    // would throw if commit didn't end the transaction
    conn->beginTransaction();
}

TEST(ScopedTransaction, rollbackRollsBackChanges)
{
    auto conn = makeConnection();
    setUserVersion(conn, 23);

    SmartSqlite::ScopedTransaction tx(conn);
    setUserVersion(conn, 42);
    ASSERT_THAT(getUserVersion(conn), Eq(42));
    tx.rollback();

    EXPECT_THAT(getUserVersion(conn), Eq(23));
}
