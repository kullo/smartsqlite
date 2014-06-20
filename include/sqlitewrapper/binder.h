#ifndef SQLITEWRAPPER_BINDER_H
#define SQLITEWRAPPER_BINDER_H

#include <vector>

#include "sqlite3.h"
#include "statement.h"

namespace SqliteWrapper {

// BEGIN SQLite native types

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
        return Binder<std::int64_t>::bind(stmt, pos, value ? 1 : 0);
    }
};

template <>
class Binder<std::int8_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::int8_t &value)
    {
        return Binder<std::int64_t>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::uint8_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::uint8_t &value)
    {
        return Binder<std::int64_t>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::int16_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::int16_t &value)
    {
        return Binder<std::int64_t>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::uint16_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::uint16_t &value)
    {
        return Binder<std::int64_t>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::int32_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::int32_t &value)
    {
        return Binder<std::int64_t>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::uint32_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::uint32_t &value)
    {
        return Binder<std::int64_t>::bind(stmt, pos, value);
    }
};

template <>
class Binder<std::uint64_t>
{
public:
    static int bind(const Statement &stmt, int pos, const std::uint64_t &value)
    {
        return Binder<std::int64_t>::bind(stmt, pos, value);
    }
};

// Linux:
//   std::size_t = unsigned long = std::uint64_t
// Mac:
//   std::size_t = unsigned long != std::uint64_t
#ifdef __APPLE__
template <>
class Binder<unsigned long>
{
public:
    static int bind(const Statement &stmt, int pos, const unsigned long &value)
    {
        return Binder<std::int32_t>::bind(stmt, pos, value);
    }
};
#endif

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
