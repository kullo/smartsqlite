/*
 * Copyright 2014–2017 The SmartSqlite contributors (see NOTICE.txt)
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

#include <iterator>
#include <map>
#include <vector>

#include "extractor.h"
#include "nullable.h"

struct sqlite3;
struct sqlite3_stmt;

namespace SmartSqlite {

class RowIterator;

class Row
{
public:
    Row(sqlite3_stmt *stmt);

    bool isNull(int pos) const;

    // extractor for integral types
    template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    T get(int pos) const
    {
        checkPosRange(pos);

        // MSVC complains about a potential performance penalty for assigning an int to a bool
        #ifdef _MSC_VER
            #pragma warning(suppress: 4800)
        #endif
        return static_cast<T>(NativeExtractor::extractLongLong(m_stmt, pos));
    }

    // extractor for floating point types
    template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
    T get(int pos) const
    {
        checkPosRange(pos);
        return static_cast<T>(NativeExtractor::extractDouble(m_stmt, pos));
    }

    // enabled only if T is _not_ numeric (so the previous specializations are not enabled)
    template <typename T,
              typename std::enable_if<
                  !(std::is_integral<T>::value || std::is_floating_point<T>::value)
                  >::type* = nullptr>
    T get(int pos) const
    {
        checkPosRange(pos);
        return Extractor<T>::extract(m_stmt, pos);
    }

    template <typename T>
    T get(const char *column) const
    {
        return get<T>(getPosByName(column));
    }

    template <typename T>
    Nullable<T> getNullable(int pos) const
    {
        checkPosRange(pos);
        if (isNull(pos)) return Nullable<T>();
        return Nullable<T>(get<T>(pos));
    }

    template <typename T>
    Nullable<T> getNullable(const char *column) const
    {
        return getNullable<T>(getPosByName(column));
    }

private:
    void setColumns(int columns);
    void checkPosRange(int pos) const;
    int getPosByName(const char *column) const;

    sqlite3_stmt *m_stmt = nullptr;
    int m_columns = 0;
    mutable std::map<std::string, int> m_columnNames;

    friend class RowIterator;
};

class RowIterator : public std::iterator<std::input_iterator_tag, Row, void>
{
public:
    enum struct Done {False, True};

    RowIterator(sqlite3 *conn, sqlite3_stmt *stmt, Done done);
    bool operator==(const RowIterator &rhs) const;
    bool operator!=(const RowIterator &rhs) const;
    RowIterator &operator++();
    Row &operator*();
    Row *operator->();

private:
    sqlite3 *m_conn;
    sqlite3_stmt *m_stmt;
    bool m_done;
    Row m_row;
};

}
