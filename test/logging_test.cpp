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
