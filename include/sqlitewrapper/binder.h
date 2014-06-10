#ifndef SQLITEWRAPPER_BINDER_H
#define SQLITEWRAPPER_BINDER_H

#include <vector>

#include "sqlite3.h"
#include "statement.h"

namespace SqliteWrapper {

template <>
class Binder<int>
{
public:
    static int bind(const Statement &stmt, int pos, const int &value)
    {
        return sqlite3_bind_int(stmt.statementHandle(), pos, value);
    }
};

template <>
class Binder<std::int64_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::int64_t &value)
    {
        return sqlite3_bind_int64(stmt.statementHandle(), pos, value);
    }
};

template <>
class Binder<double>
{
public:
    static int bind(const Statement &stmt, int pos, const double &value)
    {
        return sqlite3_bind_double(stmt.statementHandle(), pos, value);
    }
};

template <>
class Binder<std::string>
{
public:
    static int bind(const Statement &stmt, int pos, const std::string &value)
    {
        return sqlite3_bind_text(stmt.statementHandle(), pos, value.c_str(), value.size(), SQLITE_TRANSIENT);
    }
};

template <>
class Binder<std::vector<unsigned char>>
{
public:
    static int bind(const Statement &stmt, int pos, const std::vector<unsigned char> &value)
    {
        return sqlite3_bind_blob(stmt.statementHandle(), pos, value.data(), value.size(), SQLITE_TRANSIENT);
    }
};

template <>
class Binder<void*, std::size_t>
{
public:
    static int bind(const Statement &stmt, int pos, void* const &value, const std::size_t &size)
    {
        return sqlite3_bind_blob(stmt.statementHandle(), pos, value, size, SQLITE_TRANSIENT);
    }
};

}
#endif // SQLITEWRAPPER_BINDER_H
