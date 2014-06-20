#ifndef SQLITEWRAPPER_ROW_H
#define SQLITEWRAPPER_ROW_H

#include <iterator>

#include "nullable.h"

struct sqlite3_stmt;

namespace SqliteWrapper {

class RowIterator;

class Row
{
public:
    Row(sqlite3_stmt *stmt);

    bool isNull(int pos) const;

    template <typename T>
    T get(int pos) const
    {
        checkPosRange(pos);
        return getUnchecked<T>(pos);
    }

    template <typename T>
    Nullable<T> getNullable(int pos) const
    {
        checkPosRange(pos);
        if (isNull(pos)) return Nullable<T>();
        return Nullable<T>(getUnchecked<T>(pos));
    }

    // specialize this to add bindings for custom types
    template <typename T>
    T getUnchecked(int pos) const;

private:
    void setColumns(int columns);
    void checkPosRange(int pos) const;

    sqlite3_stmt *m_stmt = nullptr;
    int m_columns = 0;

    friend class RowIterator;
};

class RowIterator : public std::iterator<std::input_iterator_tag, Row, void>
{
public:
    RowIterator(sqlite3_stmt *stmt, bool done);
    bool operator==(const RowIterator &rhs) const;
    bool operator!=(const RowIterator &rhs) const;
    RowIterator &operator++();
    Row &operator*();
    Row *operator->();

private:
    sqlite3_stmt *m_stmt;
    bool m_done;
    Row m_row;
};

}
#endif // SQLITEWRAPPER_ROW_H
