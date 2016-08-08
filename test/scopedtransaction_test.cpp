/*
 * Copyright 2014–2016 The SmartSqlite contributors (see NOTICE.txt)
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
#include <gmock/gmock.h>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/scopedtransaction.h"
#include "testutil.h"

using namespace testing;
using namespace TestUtil;


TEST(ScopedTransaction, ctorDoesntThrow)
{
    SmartSqlite::ScopedTransaction{makeConnection()};
}

TEST(ScopedTransaction, ctorBeginsTransaction)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedTransaction tx(conn);
    (void)tx;

    EXPECT_THROW(conn->beginTransaction(), SmartSqlite::SqliteException);
}

TEST(ScopedTransaction, dtorRollsBack)
{
    auto conn = makeConnection();
    int origVersion = 23;
    setUserVersion(conn, origVersion);
    {
        SmartSqlite::ScopedTransaction tx(conn);
        (void)tx;
        setUserVersion(conn, 42);
    }

    EXPECT_THAT(getUserVersion(conn), Eq(origVersion));
}

TEST(ScopedTransaction, commitEndsTransaction)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedTransaction tx(conn);
    tx.commit();

    // would throw if commit didn't end the transaction
    conn->beginTransaction();
}

TEST(ScopedTransaction, commitCancelsRollback)
{
    auto conn = makeConnection();
    setUserVersion(conn, 42);
    int version = 23;
    {
        SmartSqlite::ScopedTransaction tx(conn);
        tx.commit();

        conn->beginTransaction();
        setUserVersion(conn, version);
    }

    // user_version would be 42 if ~ScopedTransaction would rollback
    EXPECT_THAT(getUserVersion(conn), Eq(version));
}

TEST(ScopedTransaction, rollbackEndsTransaction)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedTransaction tx(conn);
    tx.rollback();

    // would throw if commit didn't end the transaction
    conn->beginTransaction();
}

TEST(ScopedTransaction, rollbackRollsBackChanges)
{
    auto conn = makeConnection();
    setUserVersion(conn, 23);

    SmartSqlite::ScopedTransaction tx(conn);
    setUserVersion(conn, 42);
    ASSERT_THAT(getUserVersion(conn), Eq(42));
    tx.rollback();

    EXPECT_THAT(getUserVersion(conn), Eq(23));
}
