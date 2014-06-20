#ifndef SQLITEWRAPPER_STATEMENT_H
#define SQLITEWRAPPER_STATEMENT_H

#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

#include "exceptions.h"
#include "nullable.h"
#include "row.h"
#include "util.h"

namespace SqliteWrapper {

class Statement;

template <typename... T>
class Binder
{
public:
    static int bind(sqlite3_stmt *, int, const T&...);
};

class NativeBinder
{
public:
    static int bindLongLong(sqlite3_stmt *stmt, int pos, long long value);
    static int bindDouble(sqlite3_stmt *stmt, int pos, double value);
    static int bindString(sqlite3_stmt *stmt, int pos, const std::string &value);
    static int bindBlob(sqlite3_stmt *stmt, int pos, const void* const &data, size_t size);
};

class Statement
{
public:
    explicit Statement(sqlite3_stmt *stmt);
    Statement(Statement &&other);
    Statement &operator=(Statement &&rhs);
    ~Statement();

    template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Statement &bind(int pos, const T& value)
    {
        checkResult(NativeBinder::bindLongLong(statementHandle(), pos, value));
        return *this;
    }

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
    Statement &bind(int pos, const T& value)
    {
        checkResult(NativeBinder::bindDouble(statementHandle(), pos, value));
        return *this;
    }

    template <typename... T>
    Statement &bind(int pos, const T&... values)
    {
        checkResult(Binder<T...>::bind(statementHandle(), pos, values...));
        return *this;
    }

    template <typename T>
    Statement &bind(int pos, const Nullable<T> &value)
    {
        if (value.isNull()) return bindNull(pos);
        return bind(pos, value.value());
    }

    Statement &bindNull(int pos);

    bool hasResults();
    RowIterator begin();
    RowIterator end();
    void execWithoutResult();
    Row execWithSingleResult();

    void clearBindings();
    void reset();

    sqlite3_stmt *statementHandle() const;

private:
    bool alreadyExecuted = false;

    struct Impl;
    std::unique_ptr<Impl> impl;
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
#endif // SQLITEWRAPPER_STATEMENT_H
