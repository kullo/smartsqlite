#include <gmock/gmock.h>

#include "version.h"

using namespace testing;

TEST(Version, returnsVersion)
{
    std::string version = SqliteWrapper::sqliteVersion();
    EXPECT_THAT(version, Not(IsEmpty()));
}

TEST(Version, versionCheckIsOk)
{
    EXPECT_THAT(SqliteWrapper::checkVersion(), Eq(true));
}
