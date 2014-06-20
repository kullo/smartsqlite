#ifndef SQLITEWRAPPER_BINDER_H
#define SQLITEWRAPPER_BINDER_H

#include <vector>

#include "sqlite3.h"
#include "statement.h"

namespace SqliteWrapper {

// BEGIN SQLite native types

template <>
class Binder<long long>
{
public:
    static int bind(const Statement &stmt, int pos, const long long &value)
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
class Binder<void*, std::size_t>
{
public:
    static int bind(const Statement &stmt, int pos, const void* const &value, const std::size_t &size)
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
        return Binder<long long>::bind(stmt, pos, value ? 1 : 0);
    }
};

template <>
class Binder<signed char>
{
public:
    static int bind(const Statement &stmt, int pos, const signed char &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<unsigned char>
{
public:
    static int bind(const Statement &stmt, int pos, const unsigned char &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<short>
{
public:
    static int bind(const Statement &stmt, int pos, const short &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<unsigned short>
{
public:
    static int bind(const Statement &stmt, int pos, const unsigned short &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<int>
{
public:
    static int bind(const Statement &stmt, int pos, const int &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<unsigned int>
{
public:
    static int bind(const Statement &stmt, int pos, const unsigned int &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<long>
{
public:
    static int bind(const Statement &stmt, int pos, const long &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<unsigned long>
{
public:
    static int bind(const Statement &stmt, int pos, const unsigned long &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<unsigned long long>
{
public:
    static int bind(const Statement &stmt, int pos, const unsigned long long &value)
    {
        return Binder<long long>::bind(stmt, pos, value);
    }
};

template <>
class Binder<float>
{
public:
    static int bind(const Statement &stmt, int pos, const bool &value)
    {
        return Binder<double>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::vector<unsigned char>>
{
public:
    static int bind(const Statement &stmt, int pos, const std::vector<unsigned char> &value)
    {
        return Binder<void*, std::size_t>::bind(stmt, pos, value.data(), value.size());
    }
};

}
#endif // SQLITEWRAPPER_BINDER_H
