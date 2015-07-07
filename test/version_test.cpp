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
