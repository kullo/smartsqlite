#include <memory>
#include <gmock/gmock.h>

#include "connection.h"
#include "exceptions.h"

TEST(Connection, cannotOpenFileInNonexistingFolder)
{
    EXPECT_THROW(SqliteWrapper::Connection("/does/not/exist"),
                 SqliteWrapper::Exception);
}

TEST(Connection, canBeOpened)
{
    SqliteWrapper::Connection(":memory:");
}

TEST(Connection, canPrepareStatement)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.prepare("PRAGMA user_version");
}
