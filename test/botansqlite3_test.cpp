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
    {
    }

    std::string tempDbName()
    {
        return std::string(std::tmpnam(nullptr)) + ".db";
    }

    void connect()
    {
        connection_ = std::unique_ptr<SmartSqlite::Connection>(
                    new SmartSqlite::Connection("file://" + dbFilename_));
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

    void createTable()
    {
        connection_->exec("CREATE TABLE tbl (foo INT, bar TEXT)");
        connection_->exec("INSERT INTO tbl VALUES (21, 'half the truth')");
        connection_->exec("INSERT INTO tbl VALUES (42, 'the truth')");
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
