#ifndef SQLITEWRAPPER_STATEMENT_H
#define SQLITEWRAPPER_STATEMENT_H

#include <memory>
#include <type_traits>

#include "binder.h"
#include "nullable.h"
#include "row.h"
#include "util.h"

namespace SqliteWrapper {

class Statement
{
public:
    explicit Statement(sqlite3_stmt *stmt);
    Statement(Statement &&other);
    Statement &operator=(Statement &&rhs);
    ~Statement();

    // binder for integral types
    template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Statement &bind(int pos, const T& value)
    {
        checkResult(NativeBinder::bindLongLong(statementHandle(), pos, value));
        return *this;
    }

    // binder for floating point types
    template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
    Statement &bind(int pos, const T& value)
    {
        checkResult(NativeBinder::bindDouble(statementHandle(), pos, value));
        return *this;
    }

    // handle all other types by specializations of the Binder template
    template <typename... T>
    Statement &bind(int pos, const T&... values)
    {
        checkResult(Binder<T...>::bind(statementHandle(), pos, values...));
        return *this;
    }

    // makes all types nullable
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

private:
    sqlite3_stmt *statementHandle() const;

    bool alreadyExecuted = false;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
#endif // SQLITEWRAPPER_STATEMENT_H
