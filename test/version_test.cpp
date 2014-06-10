#include <gmock/gmock.h>

#include "sqlitewrapper/version.h"

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
