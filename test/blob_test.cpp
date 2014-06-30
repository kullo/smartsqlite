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
        rowid = conn->lastInsertRowId();
    }

    std::unique_ptr<SqliteWrapper::Connection> conn;
    std::int64_t rowid;
};

TEST_F(Blob, canOpen)
{
    conn->openBlob("main", "blobs", "data", rowid,
                   SqliteWrapper::Blob::READONLY);
}
