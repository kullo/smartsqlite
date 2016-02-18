/*
 * Copyright 2014–2015 The SmartSqlite contributors (see NOTICE.txt)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <array>
#include <gmock/gmock.h>
#include <memory>

#include "smartsqlite/blob.h"
#include "smartsqlite/connection.h"

using namespace testing;

class Blob : public Test
{
protected:
    Blob()
        : conn_(":memory:")
    {
    }

    void SetUp()
    {
        conn_.exec("CREATE TABLE blobs (data BLOB)");

        conn_.exec("INSERT INTO blobs VALUES (zeroblob(42))");
        rowidZero_ = conn_.lastInsertRowId();

        conn_.exec("INSERT INTO blobs VALUES (x'0123456789abcdef')");
        rowidNonzero_ = conn_.lastInsertRowId();
    }

    SmartSqlite::Blob open(
            std::int64_t rowid,
            SmartSqlite::Blob::Flags flags = SmartSqlite::Blob::READONLY)
    {
        return conn_.openBlob("main", "blobs", "data", rowid, flags);
    }

    SmartSqlite::Connection conn_;
    std::int64_t rowidZero_, rowidNonzero_;
};

TEST_F(Blob, moveCtor)
{
    auto blob1 = open(rowidZero_);

    ASSERT_THAT(blob1.size(), Eq(42));

    auto blob2 = std::move(blob1);

    EXPECT_THAT(blob2.size(), Eq(42));
}

TEST_F(Blob, moveAssignment)
{
    auto blob1 = open(rowidZero_);
    auto blob2 = open(rowidNonzero_);

    ASSERT_THAT(blob1.size(), Eq(42));
    ASSERT_THAT(blob2.size(), Eq(8));

    blob1 = std::move(blob2);

    EXPECT_THAT(blob1.size(), Eq(8));
}

TEST_F(Blob, canOpen)
{
    open(rowidZero_);
}

TEST_F(Blob, canGetSize)
{
    auto blob = open(rowidZero_);
    EXPECT_THAT(blob.size(), Eq(42U));

    blob = open(rowidNonzero_);
    EXPECT_THAT(blob.size(), Eq(8U));
}

TEST_F(Blob, canMoveToRow)
{
    auto blob = open(rowidZero_);
    EXPECT_THAT(blob.size(), Eq(42U));

    blob.moveToRow(rowidNonzero_);
    EXPECT_THAT(blob.size(), Eq(8U));
}

TEST_F(Blob, read)
{
    auto blob = open(rowidNonzero_);
    std::array<std::uint8_t, 8> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size()), Eq(8U));

    std::array<std::uint8_t, 8> expected = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithOffset)
{
    auto blob = open(rowidNonzero_);
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
    auto blob = open(rowidNonzero_);
    std::array<std::uint8_t, 4> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size()), Eq(buf.size()));

    std::array<std::uint8_t, 4> expected = {
        0x01, 0x23, 0x45, 0x67
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithOffsetAndSmallerBuffer)
{
    auto blob = open(rowidNonzero_);
    std::array<std::uint8_t, 4> buf;
    EXPECT_THAT(blob.read(buf.data(), buf.size(), 2), Eq(buf.size()));

    std::array<std::uint8_t, 4> expected = {
        0x45, 0x67, 0x89, 0xab
    };
    EXPECT_THAT(buf, Eq(expected));
}

TEST_F(Blob, readWithLargerBuffer)
{
    auto blob = open(rowidNonzero_);
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
    auto blob = open(rowidNonzero_);
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
    auto blob = open(rowidNonzero_, SmartSqlite::Blob::READWRITE);
    std::array<std::uint8_t, 8> original = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
    };
    EXPECT_THAT(blob.write(original.data(), original.size()), Eq(8U));

    std::array<std::uint8_t, 8> readData;
    EXPECT_THAT(blob.read(readData.data(), readData.size()), Eq(8U));
    EXPECT_THAT(readData, Eq(original));
}
