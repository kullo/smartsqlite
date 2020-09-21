/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <memory>
#include <string>

#include "smartsqlite/connection.h"

namespace SmartSqlite {

class ScopedSavepoint final
{
public:
    explicit ScopedSavepoint(
            std::shared_ptr<Connection> conn,
            const std::string &name);
    ScopedSavepoint(ScopedSavepoint &&other);
    ScopedSavepoint &operator=(ScopedSavepoint &&rhs);
    ~ScopedSavepoint();
    void release();
    void rollback();

private:
    // ScopedSavepoint is not copyable
    ScopedSavepoint(const ScopedSavepoint &) = delete;
    ScopedSavepoint &operator=(const ScopedSavepoint &) = delete;

    void rollbackIfNotFinished();

    std::shared_ptr<Connection> conn_;
    bool finished_ = false;
    std::string name_;
};

}
