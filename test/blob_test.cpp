#include <gmock/gmock.h>
#include <memory>

#include "sqlitewrapper/blob.h"
#include "sqlitewrapper/connection.h"

using namespace testing;

class Blob : public Test
{
protected:
    void SetUp()
    {
        conn.reset(new SqliteWrapper::Connection(":memory:"));
        conn->exec("CREATE TABLE blobs (data BLOB)");

        conn->exec("INSERT INTO blobs VALUES (zeroblob(42))");
        rowidZero = conn->lastInsertRowId();

        conn->exec("INSERT INTO blobs VALUES (x'0123456789abcdef')");
        rowidNonzero = conn->lastInsertRowId();
    }

    SqliteWrapper::Blob open(std::int64_t rowid)
    {
        return conn->openBlob("main", "blobs", "data", rowid,
                              SqliteWrapper::Blob::READONLY);
    }

    std::unique_ptr<SqliteWrapper::Connection> conn;
    std::int64_t rowidZero, rowidNonzero;
};

TEST_F(Blob, canOpen)
{
    open(rowidZero);
}

TEST_F(Blob, canGetSize)
{
    auto blob = open(rowidZero);
    EXPECT_THAT(blob.size(), Eq(42));

    blob = open(rowidNonzero);
    EXPECT_THAT(blob.size(), Eq(8));
}

TEST_F(Blob, canMoveToRow)
{
    auto blob = open(rowidZero);
    EXPECT_THAT(blob.size(), Eq(42));

    blob.moveToRow(rowidNonzero);
    EXPECT_THAT(blob.size(), Eq(8));
}

}
