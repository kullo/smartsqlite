#include <gmock/gmock.h>

#include <type_traits>

#include "connection.h"

using namespace testing;

class Statement : public Test
{
protected:
    void SetUp()
    {
        conn.reset(new SqliteWrapper::Connection(":memory:"));
    }

    std::unique_ptr<SqliteWrapper::Connection> conn;
};

TEST_F(Statement, canMove)
{
    EXPECT_THAT((std::is_move_constructible<SqliteWrapper::Statement>::value),
                Eq(true));
    EXPECT_THAT((std::is_move_assignable<SqliteWrapper::Statement>::value),
                Eq(true));
}

TEST_F(Statement, cannotCopy)
{
    EXPECT_THAT((std::is_copy_constructible<SqliteWrapper::Statement>::value),
                Eq(false));
    EXPECT_THAT((std::is_copy_assignable<SqliteWrapper::Statement>::value),
                Eq(false));
}

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
