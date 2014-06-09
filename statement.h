#ifndef SQLITEWRAPPER_STATEMENT_H
#define SQLITEWRAPPER_STATEMENT_H

#include <memory>

#include "util.h"

struct sqlite3_stmt;

namespace SqliteWrapper {

class Statement
{
public:
    explicit Statement(sqlite3_stmt *stmt);
    Statement(Statement &&other);
    Statement &operator=(Statement &&rhs);
    ~Statement();

    template <typename T>
    void bind(int pos, const T &value)
    {
        checkResult(bindUnchecked(pos, value));
    }

    // specialize this to add bindings for custom types
    template <typename T>
    int bindUnchecked(int pos, const T &value);

    void clearBindings();
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
#endif // SQLITEWRAPPER_STATEMENT_H
