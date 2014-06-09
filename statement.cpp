#include "statement.h"

#include <cstdint>
#include <vector>

#include "sqlite3.h"

namespace SqliteWrapper {

struct Statement::Impl
{
    sqlite3_stmt *stmt;
};

Statement::Statement(sqlite3_stmt *stmt)
    : impl(new Impl)
{
    impl->stmt = stmt;
}

Statement::Statement(Statement &&other)
    : impl(new Impl)
{
    std::swap(impl, other.impl);
}

Statement &Statement::operator=(Statement &&rhs)
{
    std::swap(impl, rhs.impl);
    return *this;
}

Statement::~Statement()
{
    sqlite3_finalize(impl->stmt);
}

void Statement::bindNull(int pos)
{
    checkResult(bindUncheckedNull(pos));
}

int Statement::bindUncheckedNull(int pos)
{
    return sqlite3_bind_null(impl->stmt, pos);
}

template <>
int Statement::bindUnchecked(int pos, const int &value)
{
    return sqlite3_bind_int(impl->stmt, pos, value);
}

template <>
int Statement::bindUnchecked(int pos, const std::int64_t &value)
{
    return sqlite3_bind_int64(impl->stmt, pos, value);
}

template <>
int Statement::bindUnchecked(int pos, const double &value)
{
    return sqlite3_bind_double(impl->stmt, pos, value);
}

template <>
int Statement::bindUnchecked(int pos, const std::string &value)
{
    return sqlite3_bind_text(impl->stmt, pos, value.c_str(), value.size(), SQLITE_TRANSIENT);
}

template <>
int Statement::bindUnchecked(int pos, const std::vector<unsigned char> &value)
{
    return sqlite3_bind_blob(impl->stmt, pos, value.data(), value.size(), SQLITE_TRANSIENT);
}

template <>
int Statement::bindUnchecked(int pos, void* const &value, const std::size_t &size)
{
    return sqlite3_bind_blob(impl->stmt, pos, value, size, SQLITE_TRANSIENT);
}

void Statement::clearBindings()
{
    checkResult(sqlite3_clear_bindings(impl->stmt));
}

void Statement::reset()
{
    checkResult(sqlite3_reset(impl->stmt));
}

}
