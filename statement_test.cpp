#include <gmock/gmock.h>

#include "connection.h"

class Statement : public testing::Test
{
protected:
    void SetUp()
    {
        conn.reset(new SqliteWrapper::Connection(":memory:"));
    }

    std::unique_ptr<SqliteWrapper::Connection> conn;
};

TEST_F(Statement, canClearBindings)
{
    SqliteWrapper::Statement stmt = conn->prepare("PRAGMA user_version");
    stmt.clearBindings();
}

TEST_F(Statement, canBeReset)
{
    SqliteWrapper::Statement stmt = conn->prepare("PRAGMA user_version");
    stmt.reset();
}
