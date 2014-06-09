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

Statement &Statement::bindNull(int pos)
{
    checkResult(bindUncheckedNull(pos));
    return *this;
}

int Statement::bindUncheckedNull(int pos)
{
    return sqlite3_bind_null(impl->stmt, pos);
}

RowIterator Statement::begin()
{
    return RowIterator(impl->stmt, false);
}

RowIterator Statement::end()
{
    return RowIterator(impl->stmt, true);
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

RowIterator::RowIterator(sqlite3_stmt *stmt, bool done)
    : m_stmt(stmt), m_done(done)
{
    if (!done)
    {
        // get first result
        ++(*this);
    }
}

bool RowIterator::operator==(const RowIterator &rhs) const
{
    return (m_stmt == rhs.m_stmt) && (m_done == rhs.m_done);
}

bool RowIterator::operator!=(const RowIterator &rhs) const
{
    return !(*this == rhs);
}

RowIterator &RowIterator::operator++()
{
    int result = sqlite3_step(m_stmt);
    switch (result)
    {
    case SQLITE_ROW:
        m_done = false;
        break;
    case SQLITE_DONE:
        m_done = true;
        break;
    default:
        checkResult(result);
    }
    return *this;
}

}
