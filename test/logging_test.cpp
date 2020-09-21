/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include <gmock/gmock.h>

#include "smartsqlite/logging.h"

using namespace testing;

TEST(Logging, canConvertErrcodeToString)
{
    EXPECT_THAT(SmartSqlite::errcodeToString(5), "database is locked");
}

static void logCallback(void *, int, char *) {}

TEST(Logging, canRegisterLogFunction)
{
    EXPECT_NO_THROW(SmartSqlite::setLogCallback(logCallback));
}
