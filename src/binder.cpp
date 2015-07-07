#include "smartsqlite/binder.h"

#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

int NativeBinder::bindLongLong(sqlite3_stmt *stmt, int pos, long long value)
{
    return sqlite3_bind_int64(stmt, pos + 1, value);
}

int NativeBinder::bindDouble(sqlite3_stmt *stmt, int pos, double value)
{
    return sqlite3_bind_double(stmt, pos + 1, value);
}

int NativeBinder::bindString(sqlite3_stmt *stmt, int pos, const std::string &value)
{
    return sqlite3_bind_text(stmt, pos + 1, value.c_str(), value.size(), SQLITE_TRANSIENT);
}

int NativeBinder::bindBlob(sqlite3_stmt *stmt, int pos, const void * const &data, size_t size)
{
    return sqlite3_bind_blob(stmt, pos + 1, data, size, SQLITE_TRANSIENT);
}

}
