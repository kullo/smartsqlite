/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
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

    void rollbackIfNotFinished();

    std::shared_ptr<Connection> conn_;
    bool finished_ = false;
};

}
