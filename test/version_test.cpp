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

#include "smartsqlite/version.h"

using namespace testing;

TEST(Version, returnsVersion)
{
    std::string version = SmartSqlite::sqliteVersion();
    EXPECT_THAT(version, Not(IsEmpty()));
}

TEST(Version, versionCheckIsOk)
{
    EXPECT_THAT(SmartSqlite::checkVersion(), Eq(true));
}
