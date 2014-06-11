#include "sqlitewrapper/statement.h"

#include <cstdint>
#include <vector>

#include "sqlitewrapper/binder.h"
#include "sqlitewrapper/exceptions.h"
#include "sqlitewrapper/sqlite3.h"

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
    checkResult(sqlite3_bind_null(impl->stmt, pos + 1));
    return *this;
}

RowIterator Statement::begin()
{
    return RowIterator(impl->stmt, false);
}

RowIterator Statement::end()
{
    return RowIterator(impl->stmt, true);
}

void Statement::clearBindings()
{
    checkResult(sqlite3_clear_bindings(impl->stmt));
}

void Statement::reset()
{
    checkResult(sqlite3_reset(impl->stmt));
}

sqlite3_stmt *Statement::statementHandle() const
{
    return impl->stmt;
}

RowIterator::RowIterator(sqlite3_stmt *stmt, bool done)
    : m_stmt(stmt), m_done(done), m_row(stmt)
{
    //FIXME make it possible to invoke .begin() multiple times without moving
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

bool Row::isNull(int pos) const
{
    return sqlite3_column_type(m_stmt, pos) == SQLITE_NULL;
}

void Row::setColumns(int columns)
{
    m_columns = columns;
}

void Row::checkPosRange(int pos) const
{
    if (pos < 0 || pos >= m_columns)
    {
        throw Exception("Column out of range");
    }
}

// BEGIN native types of SQLite

template <>
std::int32_t Row::getUnchecked(int pos) const
{
    return sqlite3_column_int(m_stmt, pos);
}

template <>
std::int64_t Row::getUnchecked(int pos) const
{
    return sqlite3_column_int64(m_stmt, pos);
}

template <>
double Row::getUnchecked(int pos) const
{
    return sqlite3_column_double(m_stmt, pos);
}

template <>
std::string Row::getUnchecked(int pos) const
{
    return std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, pos)));
}

template <>
std::vector<unsigned char> Row::getUnchecked(int pos) const
{
    int size = sqlite3_column_bytes(m_stmt, pos);
    if (size == 0) return std::vector<unsigned char>();

    const unsigned char *data = reinterpret_cast<const unsigned char*>(sqlite3_column_blob(m_stmt, pos));
    return std::vector<unsigned char>(data, data + size);
}

// END native types of SQLite

template <>
bool Row::getUnchecked(int pos) const
{
    return getUnchecked<std::int32_t>(pos) != 0;
}

template <>
std::int8_t Row::getUnchecked(int pos) const
{
    return getUnchecked<std::int32_t>(pos);
}

template <>
std::int16_t Row::getUnchecked(int pos) const
{
    return getUnchecked<std::int32_t>(pos);
}

template <>
float Row::getUnchecked(int pos) const
{
    return getUnchecked<double>(pos);
}

}
