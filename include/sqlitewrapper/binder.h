#pragma once

#include <string>
#include <vector>

struct sqlite3_stmt;

namespace SqliteWrapper {

class NativeBinder
{
public:
    static int bindLongLong(sqlite3_stmt *stmt, int pos, long long value);
    static int bindDouble(sqlite3_stmt *stmt, int pos, double value);
    static int bindString(sqlite3_stmt *stmt, int pos, const std::string &value);
    static int bindBlob(sqlite3_stmt *stmt, int pos, const void* const &data, size_t size);
};

// extension point: specialize this to add support for custom types
template <typename... T>
class Binder
{
public:
    static int bind(sqlite3_stmt *, int, const T&...);
};

template <>
class Binder<std::string>
{
public:
    static int bind(sqlite3_stmt *stmt, int pos, const std::string &value)
    {
        return NativeBinder::bindString(stmt, pos, value);
    }
};

template <>
class Binder<void*, std::size_t>
{
public:
    static int bind(sqlite3_stmt *stmt, int pos, const void* const &value, const std::size_t &size)
    {
        return NativeBinder::bindBlob(stmt, pos, value, size);
    }
};

template <>
class Binder<std::vector<unsigned char>>
{
public:
    static int bind(sqlite3_stmt *stmt, int pos, const std::vector<unsigned char> &value)
    {
        return NativeBinder::bindBlob(stmt, pos, value.data(), value.size());
    }
};

}
