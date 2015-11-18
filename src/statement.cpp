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
#include "smartsqlite/statement.h"

#include <cstdint>
#include <vector>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/sqlite3.h"

namespace SmartSqlite {

struct Statement::Impl
{
    sqlite3 *conn = nullptr;
    sqlite3_stmt *stmt = nullptr;
    bool alreadyExecuted = false;
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

Statement &Statement::bindRawBlob(int pos, void *value, std::size_t size)
{
    CHECK_RESULT(NativeBinder::bindBlob(statementHandle(), pos, value, size));
    return *this;
}

Statement &Statement::bindRawBlob(const char *name, void *value, std::size_t size)
{
    return bindRawBlob(getParameterPos(name), value, size);
}

Statement &Statement::bindNull(int pos)
{
    CHECK_RESULT_CONN(sqlite3_bind_null(impl->stmt, pos + 1), impl->conn);
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
    auto iter = RowIterator(impl->conn, impl->stmt, RowIterator::Done::False);

    if (impl->alreadyExecuted) throw Exception("Statement::begin() can only be called once, it's an InputIterator");

    impl->alreadyExecuted = true;
    ++iter;
    return iter;
}

RowIterator Statement::end()
{
    return RowIterator(impl->conn, impl->stmt, RowIterator::Done::True);
}

void Statement::execWithoutResult()
{
    if (begin() != end()) throw QueryReturnedRows();
}

Row Statement::execWithSingleResult()
{
    auto first = begin();
    if (first == end()) throw QueryReturnedNoRows();
    return *first;
}

void Statement::clearBindings()
{
    CHECK_RESULT_CONN(sqlite3_clear_bindings(impl->stmt), impl->conn);
}

void Statement::reset()
{
    impl->alreadyExecuted = false;
    CHECK_RESULT_CONN(sqlite3_reset(impl->stmt), impl->conn);
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
