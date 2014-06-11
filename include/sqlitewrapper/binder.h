#ifndef SQLITEWRAPPER_BINDER_H
#define SQLITEWRAPPER_BINDER_H

#include <vector>

#include "sqlite3.h"
#include "statement.h"

namespace SqliteWrapper {

// BEGIN SQLite native types

template <>
class Binder<std::int32_t>
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

// END SQLite native types

template <>
class Binder<bool>
{
public:
    static int bind(const Statement &stmt, int pos, const bool &value)
    {
        return Binder<std::int32_t>::bind(stmt, pos, value ? 1 : 0);
    }
};

template <>
class Binder<std::int8_t>
{
public:
    static int bind(const Statement &stmt, int pos, const bool &value)
    {
        return Binder<std::int32_t>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::int16_t>
{
public:
    static int bind(const Statement &stmt, int pos, const bool &value)
    {
        return Binder<std::int32_t>::bind(stmt, pos, value);
    }
};

}
#endif // SQLITEWRAPPER_BINDER_H
