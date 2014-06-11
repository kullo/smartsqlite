#include "sqlitewrapper/connection.h"

#include <memory>

#include "sqlitewrapper/exceptions.h"
#include "sqlitewrapper/sqlite3.h"
#include "sqlitewrapper/util.h"

namespace SqliteWrapper {

struct Connection::Impl
{
    sqlite3 *conn;
};

Connection::Connection(const std::string &connectionString)
    : impl(new Impl)
{
    checkResult(sqlite3_open(connectionString.c_str(), &(impl->conn)));
}

Connection::Connection(Connection &&other)
    : impl(new Impl)
{
    std::swap(impl, other.impl);
}

Connection &Connection::operator=(Connection &&rhs)
{
    std::swap(impl, rhs.impl);
    return *this;
}

Connection::~Connection()
{
    sqlite3_close(impl->conn);
}

void Connection::setBusyTimeout(int ms)
{
    checkResult(sqlite3_busy_timeout(impl->conn, ms));
}

Statement Connection::prepare(const std::string &sql)
{
    sqlite3_stmt *stmtPtr;
    const char *tail;
    // size + 1 can be passed because c_str() is known to be null-terminated.
    // This will cause SQLite not to copy the input.
    checkResult(sqlite3_prepare_v2(impl->conn, sql.c_str(), sql.size() + 1, &stmtPtr, &tail));
    Statement stmt(stmtPtr);

    if (tail != nullptr && tail[0] != '\0')
    {
        throw Exception("Connection::prepare() doesn't support multiple SQL statements in a single call.");
    }

    return stmt;
}

void Connection::exec(const std::string &sql)
{
    char *errmsg;
    int result = sqlite3_exec(impl->conn, sql.c_str(), nullptr, nullptr, &errmsg);

    std::unique_ptr<char, void(*)(void*)> errmsgSafe(errmsg, sqlite3_free);
    if (errmsgSafe)
    {
        checkResult(result, errmsgSafe.get());
    }
    else
    {
        checkResult(result);
    }
}

void Connection::beginTransaction()
{
    exec("BEGIN TRANSACTION");
}

void Connection::commitTransaction()
{
    exec("COMMIT TRANSACTION");
}

void Connection::rollbackTransaction()
{
    exec("ROLLBACK TRANSACTION");
}

void Connection::savepoint(const std::string &name)
{
    //FIXME escape
    exec(std::string("SAVEPOINT ") + name);
}

void Connection::releaseSavepoint(const std::string &name)
{
    //FIXME escape
    exec(std::string("RELEASE SAVEPOINT ") + name);
}

void Connection::rollbackToSavepoint(const std::string &name)
{
    //FIXME escape
    exec(std::string("ROLLBACK TRANSACTION TO SAVEPOINT ") + name);
}

std::int64_t Connection::lastInsertRowId() const
{
    return sqlite3_last_insert_rowid(impl->conn);
}

}
