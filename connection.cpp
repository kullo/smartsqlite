#include "connection.h"

#include <memory>

#include "exceptions.h"
#include "sqlite3.h"
#include "util.h"

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

}
