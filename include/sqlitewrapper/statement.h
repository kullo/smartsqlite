#ifndef SQLITEWRAPPER_STATEMENT_H
#define SQLITEWRAPPER_STATEMENT_H

#include <iterator>
#include <memory>

#include "exceptions.h"
#include "nullable.h"
#include "util.h"

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

class Statement;

template <typename... T>
class Binder
{
public:
    static int bind(const Statement &, int, const T&...);
};

class Statement
{
public:
    explicit Statement(sqlite3_stmt *stmt);
    Statement(Statement &&other);
    Statement &operator=(Statement &&rhs);
    ~Statement();

    template <typename... T>
    Statement &bind(int pos, const T&... values)
    {
        checkResult(Binder<T...>::bind(*this, pos + 1, values...));
        return *this;
    }

    template <typename T>
    Statement &bind(int pos, const Nullable<T> &value)
    {
        if (value.isNull()) return bindNull(pos);
        return bind(pos, value.value());
    }

    Statement &bindNull(int pos);

    bool hasResults();
    RowIterator begin();
    RowIterator end();
    void execWithoutResult();
    Row execWithSingleResult();

    void clearBindings();
    void reset();

    sqlite3_stmt *statementHandle() const;

private:
    bool alreadyExecuted = false;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
#endif // SQLITEWRAPPER_STATEMENT_H
