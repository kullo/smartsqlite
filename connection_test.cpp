#include <memory>
#include <gmock/gmock.h>

#include "connection.h"
#include "exceptions.h"

class Connection : public testing::Test
{
protected:
    std::unique_ptr<SqliteWrapper::Connection> conn;
};

TEST_F(Connection, cannotOpenFileInNonexistingFolder)
{
    EXPECT_THROW(conn.reset(new SqliteWrapper::Connection("/does/not/exist")),
                 SqliteWrapper::Exception);
}

TEST_F(Connection, canBeOpened)
{
    conn.reset(new SqliteWrapper::Connection(":memory:"));
}
