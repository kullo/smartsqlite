#include "statement.h"

#include <cstdint>
#include <vector>

#include "exceptions.h"
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
    : m_stmt(stmt), m_done(done), m_row(stmt)
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
    m_row.setColumns(sqlite3_column_count(m_stmt));
    return *this;
}

Row &RowIterator::operator*()
{
    return m_row;
}

Row *RowIterator::operator->()
{
    return &m_row;
}

Row::Row(sqlite3_stmt *stmt)
    : m_stmt(stmt)
{
}

void Row::setColumns(int columns)
{
    m_columns = columns;
}

void Row::checkPosRange(int pos)
{
    if (pos < 0 || pos >= m_columns)
    {
        throw Exception("Column out of range");
    }
}

template <>
int Row::getUnchecked(int pos)
{
    return sqlite3_column_int(m_stmt, pos);
}

template <>
std::int64_t Row::getUnchecked(int pos)
{
    return sqlite3_column_int64(m_stmt, pos);
}

template <>
double Row::getUnchecked(int pos)
{
    return sqlite3_column_double(m_stmt, pos);
}

template <>
std::string Row::getUnchecked(int pos)
{
    return std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, pos)));
}

template <>
std::vector<unsigned char> Row::getUnchecked(int pos)
{
    int size = sqlite3_column_bytes(m_stmt, pos);
    if (size == 0) return std::vector<unsigned char>();

    const unsigned char *data = reinterpret_cast<const unsigned char*>(sqlite3_column_blob(m_stmt, pos));
    return std::vector<unsigned char>(data, data + size);
}

}
