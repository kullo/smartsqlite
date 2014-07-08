#include "sqlitewrapper/statement.h"

#include <cstdint>
#include <vector>

#include "sqlitewrapper/exceptions.h"
#include "sqlitewrapper/sqlite3.h"

namespace SqliteWrapper {

struct Statement::Impl
{
    sqlite3 *conn = nullptr;
    sqlite3_stmt *stmt = nullptr;
};

Statement::Statement(sqlite3 *conn, sqlite3_stmt *stmt)
    : impl(new Impl)
{
    impl->conn = conn;
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
    checkResult(sqlite3_bind_null(impl->stmt, pos + 1), impl->conn);
    return *this;
}

Statement &Statement::bindNull(const char *parameter)
{
    return bindNull(getParameterPos(parameter));
}

bool Statement::hasResults()
{
    return begin() != end();
}

RowIterator Statement::begin()
{
    auto iter = RowIterator(impl->conn, impl->stmt, false);
    if (!alreadyExecuted)
    {
        // execute statement on first call to begin()
        ++iter;
        alreadyExecuted = true;
    }
    return iter;
}

RowIterator Statement::end()
{
    return RowIterator(impl->conn, impl->stmt, true);
}

void Statement::execWithoutResult()
{
    begin();
}

Row Statement::execWithSingleResult()
{
    return *begin();
}

void Statement::clearBindings()
{
    checkResult(sqlite3_clear_bindings(impl->stmt), impl->conn);
}

void Statement::reset()
{
    alreadyExecuted = false;
    checkResult(sqlite3_reset(impl->stmt), impl->conn);
}

sqlite3_stmt *Statement::statementHandle() const
{
    return impl->stmt;
}

int Statement::getParameterPos(const char *name)
{
    int pos = sqlite3_bind_parameter_index(impl->stmt, name);
    if (!pos) throw ParameterUnknown(name);
    return pos - 1;
}

}
