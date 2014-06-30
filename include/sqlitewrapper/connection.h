#ifndef SQLITEWRAPPER_SQLITEWRAPPER_H
#define SQLITEWRAPPER_SQLITEWRAPPER_H

#include <memory>
#include <string>

#include "blob.h"
#include "statement.h"

namespace SqliteWrapper {

enum TransactionType
{
    Deferred,
    Immediate,
    Exclusive
};

typedef void(TracingCallback)(void *extraArg, const char *sql);
typedef void(ProfilingCallback)(void *extraArg, const char *sql, std::uint64_t nanos);

class Connection
{
public:
    explicit Connection(const std::string &connectionString);
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

    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
#endif // SQLITEWRAPPER_SQLITEWRAPPER_H
