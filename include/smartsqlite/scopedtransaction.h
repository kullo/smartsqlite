#pragma once

#include <memory>

#include "smartsqlite/connection.h"

namespace SmartSqlite {

class ScopedTransaction final
{
public:
    explicit ScopedTransaction(
            std::shared_ptr<Connection> conn,
            TransactionType type = Deferred);
    ScopedTransaction(ScopedTransaction &&other);
    ScopedTransaction &operator=(ScopedTransaction &&rhs);
    ~ScopedTransaction();
    void commit();
    void rollback();

private:
    // ScopedTransaction is not copyable
    ScopedTransaction(const ScopedTransaction &) = delete;
    ScopedTransaction &operator=(const ScopedTransaction &) = delete;

    std::shared_ptr<Connection> conn_;
    bool finished_ = false;
};

}
