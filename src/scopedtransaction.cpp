#include "sqlitewrapper/scopedtransaction.h"

#include <iostream>

namespace SqliteWrapper {

ScopedTransaction::ScopedTransaction(
        std::shared_ptr<SqliteWrapper::Connection> conn,
        TransactionType type)
    : conn_(conn)
{
    conn_->beginTransaction(type);
}

ScopedTransaction::~ScopedTransaction()
{
    if (!committed_) conn_->rollbackTransaction();
}

void ScopedTransaction::commit()
{
    committed_ = true;
    conn_->commitTransaction();
}

}
