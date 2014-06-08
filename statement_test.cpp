#include <gmock/gmock.h>

#include "connection.h"

TEST(Statement, canBePrepared)
{
    SqliteWrapper::Connection conn(":memory:");
    conn.prepare("PRAGMA user_version");
}
