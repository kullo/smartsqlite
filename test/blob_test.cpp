#include <array>
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
        conn = SqliteWrapper::makeConnectionPtr(":memory:");
        conn->exec("CREATE TABLE blobs (data BLOB)");

        conn->exec("INSERT INTO blobs VALUES (zeroblob(42))");
        rowidZero = conn->lastInsertRowId();

        conn->exec("INSERT INTO blobs VALUES (x'0123456789abcdef')");
        rowidNonzero = conn->lastInsertRowId();
    }

    SqliteWrapper::Blob open(
            std::int64_t rowid,
            SqliteWrapper::Blob::Flags flags = SqliteWrapper::Blob::READONLY)
    {
        return conn->openBlob("main", "blobs", "data", rowid, flags);
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
    EXPECT_THAT(blob.size(), Eq(42U));

    blob = open(rowidNonzero);
    EXPECT_THAT(blob.size(), Eq(8U));
}

TEST_F(Blob, canMoveToRow)
{
    auto blob = open(rowidZero);
    EXPECT_THAT(blob.size(), Eq(42U));

    blob.moveToRow(rowidNonzero);
    EXPECT_THAT(blob.size(), Eq(8U));
}

TEST_F(Blob, read)
{
    auto blob = open(rowidNonzero);
    std::array<std::uint8_t, 8> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size()), Eq(8U));

    std::array<std::uint8_t, 8> expected = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithOffset)
{
    auto blob = open(rowidNonzero);
    std::array<std::uint8_t, 8> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size(), 2), Eq(6U));

    // the last two elements are uninitialized
    buf[6] = 0x13;
    buf[7] = 0x37;
    std::array<std::uint8_t, 8> expected = {
        0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x13, 0x37
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithSmallerBuffer)
{
    auto blob = open(rowidNonzero);
    std::array<std::uint8_t, 4> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size()), Eq(buf.size()));

    std::array<std::uint8_t, 4> expected = {
        0x01, 0x23, 0x45, 0x67
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithOffsetAndSmallerBuffer)
{
    auto blob = open(rowidNonzero);
    std::array<std::uint8_t, 4> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size(), 2), Eq(buf.size()));

    std::array<std::uint8_t, 4> expected = {
        0x45, 0x67, 0x89, 0xab
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithLargerBuffer)
{
    auto blob = open(rowidNonzero);
    std::array<std::uint8_t, 10> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size()), Eq(8U));

    // the last two elements are uninitialized
    buf[8] = 0x13;
    buf[9] = 0x37;
    std::array<std::uint8_t, 10> expected = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x13, 0x37
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithOffsetAndLargerBuffer)
{
    auto blob = open(rowidNonzero);
    std::array<std::uint8_t, 10> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size(), 2), Eq(6U));

    // the last two elements are uninitialized
    buf[6] = 0x13;
    buf[7] = 0x37;
    buf[8] = 0x42;
    buf[9] = 0x23;
    std::array<std::uint8_t, 10> expected = {
        0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x13, 0x37, 0x42, 0x23
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, write)
{
    auto blob = open(rowidNonzero, SqliteWrapper::Blob::READWRITE);
    std::array<std::uint8_t, 8> original = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
    };
    EXPECT_THAT(blob.write(original.data(), original.size()), Eq(8U));

    std::array<std::uint8_t, 8> readData;
    EXPECT_THAT(blob.read(readData.data(), readData.size()), Eq(8U));
    EXPECT_THAT(readData, Eq(original));
}
