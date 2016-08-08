/*
 * Copyright 2014–2016 The SmartSqlite contributors (see NOTICE.txt)
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
#include <type_traits>

#include "binder.h"
#include "nullable.h"
#include "row.h"
#include "util.h"

namespace SmartSqlite {

class Statement
{
public:
    explicit Statement(sqlite3 *conn, sqlite3_stmt *stmt);
    Statement(Statement &&other);
    Statement &operator=(Statement &&rhs);
    ~Statement();

    // binder for integral types
    template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Statement &bind(int pos, const T& value)
    {
        CHECK_RESULT(NativeBinder::bindLongLong(statementHandle(), pos, static_cast<long long>(value)));
        return *this;
    }

    // binder for floating point types
    template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
    Statement &bind(int pos, const T& value)
    {
        CHECK_RESULT(NativeBinder::bindDouble(statementHandle(), pos, static_cast<double>(value)));
        return *this;
    }

    // handle all other types by specializations of the Binder template
    template <typename T, typename std::enable_if<!(std::is_integral<T>::value || std::is_floating_point<T>::value)>::type* = nullptr>
    Statement &bind(int pos, const T& values)
    {
        CHECK_RESULT(Binder<T>::bind(statementHandle(), pos, values));
        return *this;
    }

    // makes all types nullable
    template <typename T>
    Statement &bind(int pos, const Nullable<T> &value)
    {
        if (!value) return bindNull(pos);
        return bind(pos, value.value());
    }
    // ... and once again for MSVC++ 2013
    template <typename T>
    Statement &bindNullable(int pos, const Nullable<T> &value)
    {
        if (!value) return bindNull(pos);
        return bind(pos, *value);
    }

    // bind by parameter name instead of position
    template <typename T>
    Statement &bind(const char *name, const T& values)
    {
        return bind(getParameterPos(name), values);
    }

    Statement &bindRawBlob(int pos, void *value, std::size_t size);
    Statement &bindRawBlob(const char *name, void *value, std::size_t size);

    Statement &bindNull(int pos);
    Statement &bindNull(const char *parameter);

    bool hasResults();
    RowIterator begin();
    RowIterator end();
    void execWithoutResult();
    Row execWithSingleResult();

    void clearBindings();
    void reset();

private:
    sqlite3_stmt *statementHandle() const;
    int getParameterPos(const char *name);

    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
