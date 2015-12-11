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

    void beginTransaction(TransactionType type = Deferred);
    void commitTransaction();
    void rollbackTransaction();

    void savepoint(const std::string &name);
    void releaseSavepoint(const std::string &name);
    void rollbackToSavepoint(const std::string &name);

    std::int64_t lastInsertRowId() const;

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
