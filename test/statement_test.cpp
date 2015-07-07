#include <gmock/gmock.h>
#include <ostream>
#include <type_traits>

#include "smartsqlite/connection.h"
#include "smartsqlite/exceptions.h"
#include "smartsqlite/nullable.h"

using namespace testing;

class Statement : public Test
{
protected:
    void SetUp()
    {
        conn = SmartSqlite::makeConnectionPtr(":memory:");
        conn->exec("CREATE TABLE all_types "
                   "(c_int INT, c_float FLOAT, c_text TEXT, c_blob BLOB)");
        conn->exec("INSERT INTO all_types "
                   "VALUES (42, 2.0, '6*7', x'deadbeef')");
        conn->exec("INSERT INTO all_types "
                   "VALUES (NULL, NULL, NULL, NULL)");
    }

    SmartSqlite::Statement makeSelect() const
    {
        return conn->prepare("SELECT c_text FROM all_types WHERE c_int = ?");
    }

    SmartSqlite::Statement makeSelectAll() const
    {
        return conn->prepare("SELECT * FROM all_types WHERE c_int IS NOT NULL");
    }

    SmartSqlite::Statement makeSelectAllNull() const
    {
        return conn->prepare("SELECT * FROM all_types WHERE c_int IS NULL");
    }

    std::vector<unsigned char> exampleBlob() const
    {
        std::vector<unsigned char> data(4, '\0');
        data[0] = '\xde';
        data[1] = '\xad';
        data[2] = '\xbe';
        data[3] = '\xef';
        return data;
    }

    std::unique_ptr<SmartSqlite::Connection> conn;
};

namespace SmartSqlite {
template<typename T>
void PrintTo(const Nullable<T> &nullable, std::ostream *os)
{
    if (!nullable)
    {
        *os << "Nullable()";
    }
    else
    {
        *os << "Nullable(" << ::testing::PrintToString(*nullable) << ")";
    }
}
}

TEST_F(Statement, canMove)
{
    EXPECT_THAT((std::is_move_constructible<SmartSqlite::Statement>::value),
                Eq(true));
    EXPECT_THAT((std::is_move_assignable<SmartSqlite::Statement>::value),
                Eq(true));
}

#ifndef _MSC_VER
TEST_F(Statement, cannotCopy)
{
    EXPECT_THAT((std::is_copy_constructible<SmartSqlite::Statement>::value),
                Eq(false));
    EXPECT_THAT((std::is_copy_assignable<SmartSqlite::Statement>::value),
                Eq(false));
}
#endif

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
    makeSelect().bindNull(0);
}

TEST_F(Statement, canBindBool)
{
    bool value = true;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindInt8)
{
    std::int8_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindUInt8)
{
    std::uint8_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindInt16)
{
    std::int16_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindUInt16)
{
    std::uint16_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindInt32)
{
    std::int32_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindUInt32)
{
    std::uint32_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindInt64)
{
    std::int64_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindUInt64)
{
    std::uint64_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindSizeT)
{
    std::size_t value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindFloat)
{
    float value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindDouble)
{
    double value = 42;
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindString)
{
    std::string value = "Hello, world.";
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindBlobFromByteVector)
{
    std::vector<unsigned char> value = {42, 23, 5};
    makeSelect().bind(0, value);
}

TEST_F(Statement, canBindBlobFromPointer)
{
    std::vector<unsigned char> value = {42, 23, 5};
    makeSelect().bindRawBlob(0, static_cast<void*>(value.data()), value.size());
}

TEST_F(Statement, canBindNullNullables)
{
    SmartSqlite::Statement stmt = conn->prepare(
                "INSERT INTO all_types "
                "VALUES (?, ?, ?, ?)");
    int pos = 0;
    stmt.bindNullable(pos++, SmartSqlite::Nullable<int>());
    stmt.bindNullable(pos++, SmartSqlite::Nullable<double>());
    stmt.bindNullable(pos++, SmartSqlite::Nullable<std::string>());
    stmt.bindNullable(pos++, SmartSqlite::Nullable<std::vector<unsigned char>>());
}

TEST_F(Statement, canBindNonNullNullables)
{
    SmartSqlite::Statement stmt = conn->prepare(
                "INSERT INTO all_types "
                "VALUES (?, ?, ?, ?)");
    int pos = 0;
    stmt.bindNullable(pos++, SmartSqlite::Nullable<int>(23));
    stmt.bindNullable(pos++, SmartSqlite::Nullable<double>(3.14));
    stmt.bindNullable(pos++, SmartSqlite::Nullable<std::string>("Hello, world."));
    stmt.bindNullable(pos++, SmartSqlite::Nullable<std::vector<unsigned char>>(exampleBlob()));
}

TEST_F(Statement, canBindNullByName)
{
    SmartSqlite::Statement stmt = conn->prepare(
                "SELECT * FROM all_types "
                "WHERE c_int = :answer");
    stmt.bindNull(":answer");
}

TEST_F(Statement, canBindIntByName)
{
    SmartSqlite::Statement stmt = conn->prepare(
                "SELECT * FROM all_types "
                "WHERE c_int = :answer");
    stmt.bind(":answer", 42);
}

TEST_F(Statement, beginThrowsOnRepeatedInvocation)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    stmt.begin();
    EXPECT_THROW(stmt.begin(), SmartSqlite::Exception);
}

TEST_F(Statement, operatorEqualsSwapsExecutedStatus)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    stmt.begin();

    // overwrite executed stmt with not executed stmt: should pass
    stmt = makeSelectAll();
    stmt.begin();

    // overwrite non-executed stmt with executed stmt: should fail
    SmartSqlite::Statement stmt2 = makeSelectAll();
    stmt2 = std::move(stmt);
    EXPECT_THROW(stmt2.begin(), SmartSqlite::Exception);
}

TEST_F(Statement, canCheckWhetherStatementHasResults)
{
    SmartSqlite::Statement stmt = makeSelect();
    stmt.bindNull(0);
    EXPECT_THAT(stmt.hasResults(), Eq(false));

    stmt.reset();
    stmt.clearBindings();
    stmt.bind(0, 42);
    EXPECT_THAT(stmt.hasResults(), Eq(true));
}

TEST_F(Statement, canStepThroughEmptyResult)
{
    SmartSqlite::Statement stmt = makeSelect();
    stmt.bindNull(0);

    int counter = 0;
    for (auto iter = stmt.begin(); iter != stmt.end(); ++iter)
    {
        ++counter;
    }
    EXPECT_THAT(counter, Eq(0));
}

TEST_F(Statement, execWithoutResultThrowsOnResultsNull)
{
    SmartSqlite::Statement stmt = makeSelectAllNull();

    EXPECT_THROW(stmt.execWithoutResult(), SmartSqlite::QueryReturnedRows);
}

TEST_F(Statement, execWithoutResultThrowsOnResultsNonNull)
{
    SmartSqlite::Statement stmt = makeSelect();
    stmt.bind(0, 42);

    EXPECT_THROW(stmt.execWithoutResult(), SmartSqlite::QueryReturnedRows);
}

TEST_F(Statement, canExecWithoutResult)
{
    SmartSqlite::Statement stmt = makeSelect();
    stmt.bind(0, 23);

    stmt.execWithoutResult();
}

TEST_F(Statement, canStepThroughNonemptyResult)
{
    SmartSqlite::Statement stmt = makeSelect();
    stmt.bind(0, 42);

    int counter = 0;
    for (auto iter = stmt.begin(); iter != stmt.end(); ++iter)
    {
        ++counter;
        EXPECT_THAT(iter->get<std::string>(0), Eq(std::string("6*7")));
    }
    EXPECT_THAT(counter, Eq(1));
}

TEST_F(Statement, execWithSingleResultThrowsOnZeroResults)
{
    SmartSqlite::Statement stmt = makeSelect();
    stmt.bind(0, 23);

    EXPECT_THROW(stmt.execWithSingleResult(), SmartSqlite::QueryReturnedNoRows);
}

TEST_F(Statement, canExecWithSingleResult)
{
    SmartSqlite::Statement stmt = makeSelect();
    stmt.bind(0, 42);

    auto result = stmt.execWithSingleResult();
    EXPECT_THAT(result.get<std::string>(0), Eq(std::string("6*7")));
}

TEST_F(Statement, canGetBool)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<bool>(0), Eq(true));
}

TEST_F(Statement, canGetInt8)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::int8_t>(0), Eq(42));
}

TEST_F(Statement, canGetUInt8)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::uint8_t>(0), Eq(42U));
}

TEST_F(Statement, canGetInt16)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::int16_t>(0), Eq(42));
}

TEST_F(Statement, canGetUInt16)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::uint16_t>(0), Eq(42U));
}

TEST_F(Statement, canGetInt32)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::int32_t>(0), Eq(42));
}

TEST_F(Statement, canGetUInt32)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::uint32_t>(0), Eq(42U));
}

TEST_F(Statement, canGetInt64)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::int64_t>(0), Eq(42));
}

TEST_F(Statement, canGetUInt64)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::uint64_t>(0), Eq(42U));
}

TEST_F(Statement, canGetSizeT)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::size_t>(0), Eq(42U));
}

TEST_F(Statement, canGetFloat)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<float>(1), FloatEq(2.0));
}

TEST_F(Statement, canGetDouble)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<double>(1), DoubleEq(2.0));
}

TEST_F(Statement, canGetString)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::string>(2), Eq(std::string("6*7")));
}

TEST_F(Statement, canGetBlob)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<std::vector<unsigned char>>(3),
                Eq(exampleBlob()));
}

TEST_F(Statement, canGetNullNullables)
{
    SmartSqlite::Statement stmt = makeSelectAllNull();
    auto iter = stmt.begin();
    EXPECT_THAT(iter->getNullable<int>(0),
                Eq(SmartSqlite::Nullable<int>()));
    EXPECT_THAT(iter->getNullable<std::int64_t>(0),
                Eq(SmartSqlite::Nullable<std::int64_t>()));
    EXPECT_THAT(iter->getNullable<double>(1),
                Eq(SmartSqlite::Nullable<double>()));
    EXPECT_THAT(iter->getNullable<std::string>(2),
                Eq(SmartSqlite::Nullable<std::string>()));
    EXPECT_THAT(iter->getNullable<std::vector<unsigned char>>(3),
                Eq(SmartSqlite::Nullable<std::vector<unsigned char>>()));
}

TEST_F(Statement, canGetNonNullNullables)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    auto iter = stmt.begin();
    EXPECT_THAT(iter->getNullable<int>(0),
                Eq(SmartSqlite::Nullable<int>(42)));
    EXPECT_THAT(iter->getNullable<std::int64_t>(0),
                Eq(SmartSqlite::Nullable<std::int64_t>(42)));
    EXPECT_THAT(iter->getNullable<double>(1),
                Eq(SmartSqlite::Nullable<double>(2.0)));
    EXPECT_THAT(iter->getNullable<std::string>(2),
                Eq(SmartSqlite::Nullable<std::string>("6*7")));
    EXPECT_THAT(iter->getNullable<std::vector<unsigned char>>(3),
                Eq(SmartSqlite::Nullable<std::vector<unsigned char>>(exampleBlob())));
}

TEST_F(Statement, canGetIntByName)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->get<int>("c_int"), Eq(42));
}

TEST_F(Statement, canGetNullableIntByName)
{
    SmartSqlite::Statement stmt = makeSelectAll();
    EXPECT_THAT(stmt.begin()->getNullable<int>("c_int"),
                Eq(SmartSqlite::Nullable<int>(42)));
}
