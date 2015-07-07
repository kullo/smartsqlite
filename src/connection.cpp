#include "smartsqlite/connection.h"

#include <cassert>
#include <memory>
#include <sstream>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/sqlite3.h"
#include "smartsqlite/util.h"

namespace SmartSqlite {

static void sqlite3Deleter(sqlite3 *ptr)
{
    sqlite3_close_v2(ptr);
}

struct Connection::Impl
{
    std::unique_ptr<sqlite3, Sqlite3Deleter*> conn;

    Impl()
        : conn(nullptr, sqlite3Deleter)
    {
    }
};

Connection::Connection(std::unique_ptr<sqlite3, Sqlite3Deleter*> &&conn)
    : impl(new Impl)
{
    impl->conn = std::move(conn);
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
}

void Connection::setBusyTimeout(int ms)
{
    CHECK_RESULT_CONN(sqlite3_busy_timeout(impl->conn.get(), ms), impl->conn.get());
}

void *Connection::setTracingCallback(TracingCallback *callback, void *extraArg)
{
    return sqlite3_trace(impl->conn.get(), callback, extraArg);
}

void *Connection::setProfilingCallback(ProfilingCallback *callback, void *extraArg)
{
    return sqlite3_profile(
                impl->conn.get(),
                reinterpret_cast<void(*)(void *, const char *, sqlite3_uint64)>(callback),
                extraArg);
}

Statement Connection::prepare(const std::string &sql)
{
    sqlite3_stmt *stmtPtr;
    const char *tail;
    // size + 1 can be passed because c_str() is known to be null-terminated.
    // This will cause SQLite not to copy the input.
    CHECK_RESULT_CONN(sqlite3_prepare_v2(impl->conn.get(), sql.c_str(), sql.size() + 1, &stmtPtr, &tail),
                      impl->conn.get());
    Statement stmt(impl->conn.get(), stmtPtr);

    if (tail != nullptr && tail[0] != '\0')
    {
        throw Exception("Connection::prepare() doesn't support multiple SQL statements in a single call.");
    }

    return stmt;
}

void Connection::exec(const std::string &sql)
{
    char *errmsg;
    int result = sqlite3_exec(impl->conn.get(), sql.c_str(), nullptr, nullptr, &errmsg);

    std::unique_ptr<char, void(*)(void*)> errmsgSafe(errmsg, sqlite3_free);
    if (errmsgSafe)
    {
        CHECK_RESULT_MSG(result, std::string(errmsgSafe.get()) + "\nSQL: " + sql);
    }
    else
    {
        CHECK_RESULT(result);
    }
}

void Connection::beginTransaction(TransactionType type)
{
    std::string typeStr;
    switch (type)
    {
    case Deferred:
        typeStr = "DEFERRED";
        break;
    case Immediate:
        typeStr = "IMMEDIATE";
        break;
    case Exclusive:
        typeStr = "EXCLUSIVE";
        break;
    default:
        assert(false && "Unknown transaction type");
    }

    exec(std::string("BEGIN ") + typeStr + " TRANSACTION");
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
    exec(std::string("SAVEPOINT '") + escape(name) + "'");
}

void Connection::releaseSavepoint(const std::string &name)
{
    exec(std::string("RELEASE SAVEPOINT '") + escape(name) + "'");
}

void Connection::rollbackToSavepoint(const std::string &name)
{
    exec(std::string("ROLLBACK TRANSACTION TO SAVEPOINT '") + escape(name) + "'");
}

std::int64_t Connection::lastInsertRowId() const
{
    return sqlite3_last_insert_rowid(impl->conn.get());
}

Blob Connection::openBlob(
        const std::string &db,
        const std::string &table,
        const std::string &column,
        std::int64_t rowid,
        Blob::Flags flags)
{
    sqlite3_blob *blob;
    CHECK_RESULT_CONN(
                sqlite3_blob_open(
                    impl->conn.get(),
                    db.c_str(),
                    table.c_str(),
                    column.c_str(),
                    rowid,
                    flags,
                    &blob),
                impl->conn.get());
    return Blob(impl->conn.get(), blob);
}

std::string Connection::escape(const std::string &original)
{
    std::stringstream result;
    for (auto origChar : original)
    {
        if (origChar == '\'')
        {
            // escape single quote by two single quotes
            result << "''";
        }
        else
        {
            result << origChar;
        }
    }
    return result.str();
}

Connection makeConnection(const std::string &connectionString)
{
    sqlite3 *rawConn = nullptr;
    auto result = sqlite3_open(connectionString.c_str(), &rawConn);
    std::unique_ptr<sqlite3, Sqlite3Deleter*> conn(rawConn, sqlite3Deleter);
    CHECK_RESULT(result);

    CHECK_RESULT_CONN(sqlite3_extended_result_codes(conn.get(), 1), conn.get());
    return Connection(std::move(conn));
}

std::unique_ptr<Connection> makeConnectionPtr(const std::string &connectionString)
{
    return std::unique_ptr<Connection>(new Connection(makeConnection(connectionString)));
}

}
