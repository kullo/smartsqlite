/*
 * Copyright 2014–2015 The SmartSqlite contributors (see NOTICE.txt)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "smartsqlite/connection.h"

#include <cassert>
#include <memory>
#include <sstream>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/sqlite3.h"
#include "smartsqlite/util.h"

namespace SmartSqlite {

typedef void(Sqlite3Deleter)(sqlite3*);

static void sqlite3Deleter(sqlite3 *ptr)
{
    sqlite3_close_v2(ptr);
}

Connection::Connection(const std::string &connectionString)
    : conn_(nullptr, sqlite3Deleter)
{
    sqlite3 *rawConn = nullptr;
    auto result = sqlite3_open(connectionString.c_str(), &rawConn);
    conn_ = std::unique_ptr<sqlite3, Sqlite3Deleter*>(rawConn, sqlite3Deleter);
    CHECK_RESULT(result);

    CHECK_RESULT_CONN(sqlite3_extended_result_codes(conn_.get(), 1), conn_.get());
}

Connection::Connection(Connection &&other)
    : conn_(nullptr, sqlite3Deleter)
{
    std::swap(conn_, other.conn_);
}

Connection &Connection::operator=(Connection &&rhs)
{
    std::swap(conn_, rhs.conn_);
    return *this;
}

Connection::~Connection()
{
}

void Connection::setBusyTimeout(int ms)
{
    CHECK_RESULT_CONN(sqlite3_busy_timeout(conn_.get(), ms), conn_.get());
}

void *Connection::setTracingCallback(TracingCallback *callback, void *extraArg)
{
    return sqlite3_trace(conn_.get(), callback, extraArg);
}

void *Connection::setProfilingCallback(ProfilingCallback *callback, void *extraArg)
{
    return sqlite3_profile(
                conn_.get(),
                reinterpret_cast<void(*)(void *, const char *, sqlite3_uint64)>(callback),
                extraArg);
}

Statement Connection::prepare(const std::string &sql)
{
    sqlite3_stmt *stmtPtr;
    const char *tail;
    // size + 1 can be passed because c_str() is known to be null-terminated.
    // This will cause SQLite not to copy the input.
    CHECK_RESULT_CONN(sqlite3_prepare_v2(conn_.get(), sql.c_str(), sql.size() + 1, &stmtPtr, &tail),
                      conn_.get());
    Statement stmt(conn_.get(), stmtPtr);

    if (tail != nullptr && tail[0] != '\0')
    {
        throw Exception("Connection::prepare() doesn't support multiple SQL statements in a single call.");
    }

    return stmt;
}

void Connection::exec(const std::string &sql)
{
    char *errmsg;
    int result = sqlite3_exec(conn_.get(), sql.c_str(), nullptr, nullptr, &errmsg);

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
    return sqlite3_last_insert_rowid(conn_.get());
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
                    conn_.get(),
                    db.c_str(),
                    table.c_str(),
                    column.c_str(),
                    rowid,
                    flags,
                    &blob),
                conn_.get());
    return Blob(conn_.get(), blob);
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
