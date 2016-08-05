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
#include <cstdio>
#include <memory>
#include <string>

#include <gmock/gmock.h>

#include "smartsqlite/connection.h"
#include "smartsqlite/exceptions.h"

using namespace testing;

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
        return std::string(std::tmpnam(nullptr)) + ".db";
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
        connection_->exec(std::string("PRAGMA key='") + key + "'");
    }

    void rekey(const std::string &key)
    {
        connection_->exec(std::string("PRAGMA rekey='") + key + "'");
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
    setKey("somekey");
    createTable();
    disconnect();

    connect();
    setKey("somekey");
    checkForTestData();
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, preventsAccessWithoutKey)
{
    connect();
    setKey("somekey");
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
    setKey("somekey");
    createTable();
    disconnect();

    connect();
    setKey("anotherkey");
    EXPECT_THROW(checkForTestData(), SmartSqlite::SqliteException);
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, encryptDatabase)
{
    connect();
    createTable();
    disconnect();

    connect();
    rekey("somekey");
    checkForTestData();
    disconnect();

    connect();
    setKey("somekey");
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, changeKey)
{
    connect();
    setKey("somekey");
    createTable();
    disconnect();

    connect();
    setKey("somekey");
    rekey("anotherkey");
    checkForTestData();
    disconnect();

    connect();
    setKey("anotherkey");
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, decryptDatabase)
{
    connect();
    setKey("somekey");
    createTable();
    disconnect();

    connect();
    setKey("somekey");
    rekey("");
    checkForTestData();
    disconnect();

    connect();
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, hexkeyWorks)
{
    connect();
    setKey("somekey");
    createTable();
    disconnect();

    connect();
    connection_->exec("PRAGMA hexkey='736f6d656b6579'");
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, hexrekeyWorks)
{
    connect();
    createTable();
    connection_->exec("PRAGMA hexrekey='736f6d656b6579'");
    disconnect();

    connect();
    setKey("somekey");
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, vacuumWorks)
{
    connect();
    setKey("somekey");
    createTable();
    disconnect();

    connect();
    setKey("somekey");
    connection_->exec("VACUUM");
    disconnect();

    connect();
    setKey("somekey");
    checkForTestData();
    disconnect();
    deleteDb(dbFilename_);
}

TEST_F(BotanSqlite3, attachWithSameKey)
{
    connect();
    setKey("somekey");
    createTable();

    attach();
    createTable("att");
    disconnect();

    connect(attachedFilename_);
    setKey("somekey");
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
    deleteDb(attachedFilename_);
}

TEST_F(BotanSqlite3, attachWithoutKey)
{
    connect();
    setKey("somekey");
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
    setKey("somekey");
    createTable();

    attachWithKey("anotherkey");
    createTable("att");
    disconnect();

    connect(attachedFilename_);
    setKey("anotherkey");
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
    deleteDb(attachedFilename_);
}

TEST_F(BotanSqlite3, attachWhenMainIsUnencrypted)
{
    connect();
    createTable();

    attachWithKey("somekey");
    createTable("att");
    disconnect();

    connect(attachedFilename_);
    setKey("somekey");
    checkForTestData();
    disconnect();

    deleteDb(dbFilename_);
    deleteDb(attachedFilename_);
}
