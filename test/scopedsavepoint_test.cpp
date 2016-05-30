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
#include <gmock/gmock.h>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/scopedsavepoint.h"
#include "testutil.h"

using namespace testing;
using namespace TestUtil;

namespace {

std::string SP_NAME = "test_savepoint";

}

TEST(ScopedSavepoint, ctorDoesntThrow)
{
    SmartSqlite::ScopedSavepoint(makeConnection());
}

TEST(ScopedSavepoint, ctorSetsSavepoint)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
    (void)sp;

    EXPECT_NO_THROW(conn->rollbackToSavepoint(SP_NAME));
}

TEST(ScopedSavepoint, dtorRollsBack)
{
    auto conn = makeConnection();
    int origVersion = 23;
    setUserVersion(conn, origVersion);
    {
        SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
        (void)sp;
        setUserVersion(conn, 42);
    }

    EXPECT_THAT(getUserVersion(conn), Eq(origVersion));
}

TEST(ScopedSavepoint, releaseReleasesSavepoint)
{
    auto conn = makeConnection();
    SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
    sp.release();

    EXPECT_THROW(conn->rollbackToSavepoint(SP_NAME), SmartSqlite::SqliteException);
}

TEST(ScopedSavepoint, releaseCancelsRollback)
{
    auto conn = makeConnection();
    setUserVersion(conn, 42);
    int version = 23;
    {
        SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
        sp.release();

        conn->savepoint(SP_NAME);
        setUserVersion(conn, version);
    }

    // user_version would be 42 if ~ScopedSavepoint would rollback
    EXPECT_THAT(getUserVersion(conn), Eq(version));
}

TEST(ScopedSavepoint, rollbackRollsBackChanges)
{
    auto conn = makeConnection();
    setUserVersion(conn, 23);

    SmartSqlite::ScopedSavepoint sp(conn, SP_NAME);
    setUserVersion(conn, 42);
    ASSERT_THAT(getUserVersion(conn), Eq(42));
    sp.rollback();

    EXPECT_THAT(getUserVersion(conn), Eq(23));
}
