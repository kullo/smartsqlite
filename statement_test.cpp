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
        conn->exec("CREATE TABLE answers (question TEXT, answer INT)");
    }

    SqliteWrapper::Statement makeSelect() const
    {
        return conn->prepare("SELECT question FROM answers WHERE answer = ?");
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
    makeSelect().clearBindings();
}

TEST_F(Statement, canBeReset)
{
    makeSelect().reset();
}
