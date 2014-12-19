#pragma once

#include <memory>

#include "sqlitewrapper/connection.h"

namespace SqliteWrapper {

class ScopedTransaction
{
public:
    explicit ScopedTransaction(
            std::shared_ptr<Connection> conn,
            TransactionType type = Deferred);
    ~ScopedTransaction();
    void commit();

private:
    std::shared_ptr<Connection> conn_;
    bool committed_ = false;
};

}
