#include "smartsqlite/scopedtransaction.h"

#include <iostream>

namespace SmartSqlite {

ScopedTransaction::ScopedTransaction(
        std::shared_ptr<SmartSqlite::Connection> conn,
        TransactionType type)
    : conn_(conn)
{
    conn_->beginTransaction(type);
}

ScopedTransaction::~ScopedTransaction()
{
    try {
        if (!finished_) conn_->rollbackTransaction();
    }
    catch (...)
    {
        // silence exception; dtor mustn't throw
    }
}

void ScopedTransaction::commit()
{
    finished_ = true;
    conn_->commitTransaction();
}

void ScopedTransaction::rollback()
{
    finished_ = true;
    conn_->rollbackTransaction();
}

}
