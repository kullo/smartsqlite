#include <memory>
#include <gmock/gmock.h>

#include "sqlitewrapper/connection.h"
#include "sqlitewrapper/exceptions.h"
#include "sqlitewrapper/scopedtransaction.h"

using namespace testing;
using ConnPtr = std::shared_ptr<SqliteWrapper::Connection>;

static ConnPtr makeConnection()
{
    return std::make_shared<SqliteWrapper::Connection>(
                SqliteWrapper::makeConnection(":memory:"));
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
    SqliteWrapper::ScopedTransaction tx(makeConnection());
}

TEST(ScopedTransaction, ctorBeginsTransaction)
{
    auto conn = makeConnection();
    SqliteWrapper::ScopedTransaction tx(conn);
    (void)tx;

    EXPECT_THROW(conn->beginTransaction(), SqliteWrapper::SqliteException);
}

TEST(ScopedTransaction, dtorRollsBack)
{
    auto conn = makeConnection();
    int origVersion = 23;
    conn->exec("PRAGMA user_version = " + std::to_string(origVersion));
    {
        SqliteWrapper::ScopedTransaction tx(conn);
        (void)tx;
        setUserVersion(conn, 42);
    }

    EXPECT_THAT(getUserVersion(conn), Eq(origVersion));
}

TEST(ScopedTransaction, commitEndsTransaction)
{
    auto conn = makeConnection();
    SqliteWrapper::ScopedTransaction tx(conn);
    tx.commit();

    // would throw if commit didn't end the transaction
    conn->beginTransaction();
}

TEST(ScopedTransaction, commitCancelsRollback)
{
    auto conn = makeConnection();
    {
        SqliteWrapper::ScopedTransaction tx(conn);
        tx.commit();
    }
    // dtor would throw if commit() didn't cancel the rollback
}
