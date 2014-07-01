#include "sqlitewrapper/connection.h"

#include <cassert>
#include <memory>
#include <sstream>

#include "sqlitewrapper/exceptions.h"
#include "sqlitewrapper/sqlite3.h"
#include "sqlitewrapper/util.h"

namespace SqliteWrapper {

struct Connection::Impl
{
    sqlite3 *conn = nullptr;
};

Connection::Connection(const std::string &connectionString)
    : impl(new Impl)
{
    checkResult(sqlite3_open(connectionString.c_str(), &(impl->conn)));
    checkResult(sqlite3_extended_result_codes(impl->conn, 1),
                impl->conn);
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
    checkResult(sqlite3_busy_timeout(impl->conn, ms), impl->conn);
}

void *Connection::setTracingCallback(TracingCallback *callback, void *extraArg)
{
    return sqlite3_trace(impl->conn, callback, extraArg);
}

void *Connection::setProfilingCallback(ProfilingCallback *callback, void *extraArg)
{
    return sqlite3_profile(
                impl->conn,
                reinterpret_cast<void(*)(void *, const char *, sqlite3_uint64)>(callback),
                extraArg);
}

Statement Connection::prepare(const std::string &sql)
{
    sqlite3_stmt *stmtPtr;
    const char *tail;
    // size + 1 can be passed because c_str() is known to be null-terminated.
    // This will cause SQLite not to copy the input.
    checkResult(sqlite3_prepare_v2(impl->conn, sql.c_str(), sql.size() + 1, &stmtPtr, &tail),
                impl->conn);
    Statement stmt(impl->conn, stmtPtr);

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
    return sqlite3_last_insert_rowid(impl->conn);
}

Blob Connection::openBlob(
        const std::string &db,
        const std::string &table,
        const std::string &column,
        std::int64_t rowid,
        Blob::Flags flags)
{
    sqlite3_blob *blob;
    checkResult(
                sqlite3_blob_open(
                    impl->conn,
                    db.c_str(),
                    table.c_str(),
                    column.c_str(),
                    rowid,
                    flags,
                    &blob),
                impl->conn);
    return Blob(impl->conn, blob);
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

}
