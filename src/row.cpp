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
#include "smartsqlite/row.h"

#include <cstring>
#include <vector>

#include "smartsqlite/exceptions.h"
#include "smartsqlite/sqlite3.h"
#include "smartsqlite/util.h"

namespace SmartSqlite {

RowIterator::RowIterator(sqlite3 *conn, sqlite3_stmt *stmt, Done done)
    : m_conn(conn), m_stmt(stmt), m_done(done == Done::True), m_row(stmt)
{
}

bool RowIterator::operator==(const RowIterator &rhs) const
{
    return (m_stmt == rhs.m_stmt) && (m_done == rhs.m_done);
}

bool RowIterator::operator!=(const RowIterator &rhs) const
{
    return !(*this == rhs);
}

RowIterator &RowIterator::operator++()
{
    int result = sqlite3_step(m_stmt);
    switch (result)
    {
    case SQLITE_ROW:
        m_done = false;
        break;
    case SQLITE_DONE:
        m_done = true;
        break;
    default:
        CHECK_RESULT_STMT(result, m_conn, m_stmt);
    }
    m_row.setColumns(sqlite3_column_count(m_stmt));
    return *this;
}

Row &RowIterator::operator*()
{
    return m_row;
}

Row *RowIterator::operator->()
{
    return &m_row;
}

Row::Row(sqlite3_stmt *stmt)
    : m_stmt(stmt)
{
}

bool Row::isNull(int pos) const
{
    return sqlite3_column_type(m_stmt, pos) == SQLITE_NULL;
}

void Row::setColumns(int columns)
{
    m_columns = columns;
}

void Row::checkPosRange(int pos) const
{
    if (pos < 0 || pos >= m_columns)
    {
        throw ColumnUnknown(pos);
    }
}

int Row::getPosByName(const char *column) const
{
    if (m_columnNames.empty())
    {
        for (int i = 0; i < m_columns; ++i)
        {
            m_columnNames[sqlite3_column_name(m_stmt, i)] = i;
        }
    }
    try
    {
        return m_columnNames.at(column);
    }
    catch (std::out_of_range)
    {
        throw ColumnUnknown(column);
    }
}

}
