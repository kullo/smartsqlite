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
