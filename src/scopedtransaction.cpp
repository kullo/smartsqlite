/*
 * Copyright 2014–2016 The SmartSqlite contributors (see NOTICE.txt)
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

ScopedTransaction::ScopedTransaction(ScopedTransaction &&other)
    : conn_(std::move(other.conn_))
    , finished_(other.finished_)
{
    // prevent dtor of other from doing anything
    other.finished_ = true;
}

ScopedTransaction &ScopedTransaction::operator=(ScopedTransaction &&rhs)
{
    // clean up this instance
    rollbackIfNotFinished();

    // move state from rhs
    conn_ = std::move(rhs.conn_);
    finished_ = rhs.finished_;

    // prevent rollback during destruction of rhs
    rhs.finished_ = true;
    return *this;
}

void ScopedTransaction::rollbackIfNotFinished()
{
    try {
        if (!finished_) conn_->rollbackTransaction();
    }
    catch (...)
    {
        // silence exception; dtor mustn't throw
    }
}

ScopedTransaction::~ScopedTransaction()
{
    rollbackIfNotFinished();
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
