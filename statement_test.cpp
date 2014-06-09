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

TEST_F(Statement, canBindNull)
{
    makeSelect().bindNull(1);
}

TEST_F(Statement, canBindInt)
{
    int value = 42;
    makeSelect().bind(1, value);
}

TEST_F(Statement, canBindInt64)
{
    std::int64_t value = 42;
    makeSelect().bind(1, value);
}

TEST_F(Statement, canBindDouble)
{
    double value = 42;
    makeSelect().bind(1, value);
}

TEST_F(Statement, canBindString)
{
    std::string value = "Hello, world.";
    makeSelect().bind(1, value);
}

TEST_F(Statement, canBindBlobFromByteVector)
{
    std::vector<unsigned char> value = {42, 23, 5};
    makeSelect().bind(1, value);
}

TEST_F(Statement, canBindBlobFromPointer)
{
    std::vector<unsigned char> value = {42, 23, 5};
    makeSelect().bind(1, static_cast<void*>(value.data()), value.size());
}
