/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>
#include <string>

#include "blob.h"
#include "statement.h"

struct sqlite3;

namespace SmartSqlite {

enum TransactionType
{
    Deferred,
    Immediate,
    Exclusive
};

using TracingCallback = void(void *extraArg, const char *sql);
using ProfilingCallback = void(void *extraArg, const char *sql, std::uint64_t nanos);

// MSVC 2013 doesn't like "using" in this case, so we resort to plain old typedef
typedef void(Sqlite3Deleter)(sqlite3*);

class Connection
{
public:
    Connection(const std::string &connectionString);
    Connection(Connection &&other);
    Connection &operator=(Connection &&rhs);
    ~Connection();

    void setBusyTimeout(int ms);
    void *setTracingCallback(TracingCallback *callback, void *extraArg = nullptr);
    void *setProfilingCallback(ProfilingCallback *callback, void *extraArg = nullptr);
    Statement prepare(const std::string &sql);
    void exec(const std::string &sql);

    void setKey(const std::string &keyBase64);
    void changeKey(const std::string &keyBase64);

    void beginTransaction(TransactionType type = Deferred);
    void commitTransaction();
    void rollbackTransaction();

    void savepoint(const std::string &name);
    void releaseSavepoint(const std::string &name);
    void rollbackToSavepoint(const std::string &name);

    std::int64_t lastInsertRowId() const;
    int changes() const;

    Blob openBlob(
            const std::string &db,
            const std::string &table,
            const std::string &column,
            std::int64_t rowid,
            Blob::Flags flags);

private:
    static std::string escape(const std::string &original);

    std::unique_ptr<sqlite3, Sqlite3Deleter*> conn_;
};

}
