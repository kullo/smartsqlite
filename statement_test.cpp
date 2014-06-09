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
        conn->exec("CREATE TABLE all_types "
                   "(c_int INT, c_float FLOAT, c_text TEXT, c_blob BLOB)");
        conn->exec("INSERT INTO all_types "
                   "VALUES (42, 2.0, '6*7', x'deadbeef')");
    }

    SqliteWrapper::Statement makeSelect() const
    {
        return conn->prepare("SELECT c_text FROM all_types WHERE c_int = ?");
    }

    SqliteWrapper::Statement makeSelectAll() const
    {
        return conn->prepare("SELECT * FROM all_types");
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

TEST_F(Statement, canStepThroughEmptyResult)
{
    SqliteWrapper::Statement stmt = makeSelect();
    stmt.bindNull(1);

    int counter = 0;
    for (auto iter = stmt.begin(); iter != stmt.end(); ++iter)
    {
        ++counter;
    }
    EXPECT_THAT(counter, Eq(0));
}

TEST_F(Statement, canStepThroughNonemptyResult)
{
    SqliteWrapper::Statement stmt = makeSelect();
    stmt.bind(1, 42);

    int counter = 0;
    for (auto iter = stmt.begin(); iter != stmt.end(); ++iter)
    {
        ++counter;
        EXPECT_THAT(iter->get<std::string>(0), Eq(std::string("6*7")));
    }
    EXPECT_THAT(counter, Eq(1));
}

TEST_F(Statement, canGetInt)
{
    SqliteWrapper::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<int>(0), Eq(42));
}

TEST_F(Statement, canGetInt64)
{
    SqliteWrapper::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::int64_t>(0), Eq(42));
}

TEST_F(Statement, canGetDouble)
{
    SqliteWrapper::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<double>(1), DoubleEq(2.0));
}

TEST_F(Statement, canGetString)
{
    SqliteWrapper::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::string>(2), Eq(std::string("6*7")));
}

TEST_F(Statement, canGetBlob)
{
    std::vector<unsigned char> data(4, '\0');
    data[0] = '\xde';
    data[1] = '\xad';
    data[2] = '\xbe';
    data[3] = '\xef';

    SqliteWrapper::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::vector<unsigned char>>(3),
                Eq(data));
}
