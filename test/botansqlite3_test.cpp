/*
 * Copyright 2014–2017 The SmartSqlite contributors (see NOTICE.txt)
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
#include <cstdio>
#include <memory>
#include <string>

#include <gmock/gmock.h>

#include "smartsqlite/connection.h"
#include "smartsqlite/exceptions.h"

using namespace testing;

namespace {

// 96B = 768b
std::string SOME_KEY =
        "MTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0"
        "NTY3ODkwMTIxMjM0NTY3ODkwMTIzNDU2"
        "Nzg5MDEyMzQ1Njc4OTAxMjEyMzQ1Njc4"
        "OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEy";
std::string ANOTHER_KEY =
        "MDk4NzY1NDMyMTA5ODc2NTQzMjEwOTg3"
        "NjU0MzIxMDkwOTg3NjU0MzIxMDk4NzY1"
        "NDMyMTA5ODc2NTQzMjEwOTA5ODc2NTQz"
        "MjEwOTg3NjU0MzIxMDk4NzY1NDMyMTA5";

}

class BotanSqlite3: public Test
{
protected:
    BotanSqlite3()
        : dbFilename_(tempDbName())
        , attachedFilename_(tempDbName())
    {
    }

    std::string tempDbName()
    {
        char filename[L_tmpnam];
        return std::string(std::tmpnam(filename));
    }

    void connect(std::string filename = "")
    {
        if (filename.empty()) filename = dbFilename_;
        connection_ = std::unique_ptr<SmartSqlite::Connection>(
                    new SmartSqlite::Connection(filename));
    }

    void disconnect()
    {
        ASSERT_THAT(connection_, Not(IsNull()));
        connection_.reset();
    }

    void deleteDb(const std::string &path)
    {
        ASSERT_THAT(path, Not(IsEmpty()));
        std::remove(path.c_str());
    }

    void setKey(const std::string &key)
    {
        connection_->setKey(key);
    }

    void rekey(const std::string &key)
    {
        connection_->changeKey(key);
    }

    void createTable(const std::string db = "main")
    {
        connection_->exec(std::string("CREATE TABLE ") + db + ".tbl (foo INT, bar TEXT)");
        connection_->exec(std::string("INSERT INTO ") + db + ".tbl VALUES (21, 'half the truth')");
        connection_->exec(std::string("INSERT INTO ") + db + ".tbl VALUES (42, 'the truth')");
    }

    void attach()
    {
        connection_->exec(
                    std::string("ATTACH DATABASE '") + attachedFilename_ + "' "
                    + "AS att");
    }

    void attachWithKey(const std::string &key)
    {
        connection_->exec(
                    std::string("ATTACH DATABASE '") + attachedFilename_ + "' "
                    + "AS att KEY '" + key + "'");
    }

    void checkForTestData()
    {
        auto stmt = connection_->prepare("SELECT * FROM tbl");

        auto rowIter = stmt.begin();
        ASSERT_THAT(rowIter, Not(Eq(stmt.end())));

        EXPECT_THAT(rowIter->get<int>(0), Eq(21));
        EXPECT_THAT(rowIter->get<std::string>(1), Eq("half the truth"));

        ++rowIter;
        ASSERT_THAT(rowIter, Not(Eq(stmt.end())));

        EXPECT_THAT(rowIter->get<int>(0), Eq(42));
        EXPECT_THAT(rowIter->get<std::string>(1), Eq("the truth"));

        ++rowIter;
        ASSERT_THAT(rowIter, Eq(stmt.end()));
    }

    std::string dbFilename_;
    std::string attachedFilename_;
    std::unique_ptr<SmartSqlite::Connection> connection_;
};


TEST_F(BotanSqlite3, canAccessEncryptedDatabase)
{
    connect();
    setKey(SOME_KEY);
    createTable();
    disconnect();

    connect();
    setKey(SOME_KEY);
    checkForTestData();
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, preventsAccessWithoutKey)
{
    connect();
    setKey(SOME_KEY);
    createTable();
    disconnect();

    connect();
    EXPECT_THROW(checkForTestData(), SmartSqlite::SqliteException);
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, preventsAccessWithWrongKey)
{
    connect();
    setKey(SOME_KEY);
    createTable();
    disconnect();

    connect();
    setKey(ANOTHER_KEY);
    EXPECT_THROW(checkForTestData(), SmartSqlite::SqliteException);
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, preventsShortKey)
{
    auto shortKey = SOME_KEY;
    shortKey.back() = '='; // remove last char

    connect();
    EXPECT_THROW(setKey(shortKey), SmartSqlite::SqliteException);
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, preventsLongKey)
{
    auto longKey = SOME_KEY;
    longKey.append("Mw=="); // "3" + padding

    connect();
    EXPECT_THROW(setKey(longKey), SmartSqlite::SqliteException);
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, preventsInvalidBase64Key)
{
    auto invalidKey = SOME_KEY;
    invalidKey.back() = '%';

    connect();
    EXPECT_THROW(setKey(invalidKey), SmartSqlite::SqliteException);
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, encryptDatabase)
{
    connect();
    createTable();
    disconnect();

    connect();
    rekey(SOME_KEY);
    checkForTestData();
    disconnect();

    connect();
    setKey(SOME_KEY);
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, changeKey)
{
    connect();
    setKey(SOME_KEY);
    createTable();
    disconnect();

    connect();
    setKey(SOME_KEY);
    rekey(ANOTHER_KEY);
    checkForTestData();
    disconnect();

    connect();
    setKey(ANOTHER_KEY);
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, decryptDatabase)
{
    connect();
    setKey(SOME_KEY);
    createTable();
    disconnect();

    connect();
    setKey(SOME_KEY);
    rekey("");
    checkForTestData();
    disconnect();

    connect();
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, vacuumWorks)
{
    connect();
    setKey(SOME_KEY);
    createTable();
    disconnect();

    connect();
    setKey(SOME_KEY);
    connection_->exec("VACUUM");
    disconnect();

    connect();
    setKey(SOME_KEY);
    checkForTestData();
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, attachWithSameKey)
{
    connect();
    setKey(SOME_KEY);
    createTable();

    attach();
    createTable("att");
    disconnect();

    connect(attachedFilename_);
    setKey(SOME_KEY);
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
    deleteDb(attachedFilename_);
}

TEST_F(BotanSqlite3, attachWithoutKey)
{
    connect();
    setKey(SOME_KEY);
    createTable();

    attachWithKey("");
    createTable("att");
    disconnect();

    connect(attachedFilename_);
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
    deleteDb(attachedFilename_);
}

TEST_F(BotanSqlite3, attachWithDifferentKey)
{
    connect();
    setKey(SOME_KEY);
    createTable();

    attachWithKey(ANOTHER_KEY);
    createTable("att");
    disconnect();

    connect(attachedFilename_);
    setKey(ANOTHER_KEY);
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
    deleteDb(attachedFilename_);
}

TEST_F(BotanSqlite3, attachWhenMainIsUnencrypted)
{
    connect();
    createTable();

    attachWithKey(SOME_KEY);
    createTable("att");
    disconnect();

    connect(attachedFilename_);
    setKey(SOME_KEY);
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
    deleteDb(attachedFilename_);
}
