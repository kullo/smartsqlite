#include "smartsqlite/logging.h"

#include "smartsqlite/sqlite3.h"

void SmartSqlite::setLogCallback(SmartSqlite::LogCallback *callback, void *extraArg)
{
    sqlite3_config(SQLITE_CONFIG_LOG, callback, extraArg);
}

std::string SmartSqlite::errcodeToString(int errcode)
{
    return sqlite3_errstr(errcode);
}
