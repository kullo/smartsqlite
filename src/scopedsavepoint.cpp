/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "smartsqlite/scopedsavepoint.h"

#include <iostream>

namespace SmartSqlite {

ScopedSavepoint::ScopedSavepoint(
        std::shared_ptr<SmartSqlite::Connection> conn,
        const std::string &name)
    : conn_(conn)
    , name_(name)
{
    conn_->savepoint(name_);
}

ScopedSavepoint::ScopedSavepoint(ScopedSavepoint &&other)
    : conn_(std::move(other.conn_))
    , finished_(other.finished_)
    , name_(std::move(other.name_))
{
    // prevent dtor of other from doing anything
    other.finished_ = true;
}

ScopedSavepoint &ScopedSavepoint::operator=(ScopedSavepoint &&rhs)
{
    // clean up this instance
    rollbackIfNotFinished();

    // move state from rhs
    conn_ = std::move(rhs.conn_);
    finished_ = rhs.finished_;
    name_ = std::move(rhs.name_);

    // prevent rollback during destruction of rhs
    rhs.finished_ = true;
    return *this;
}

void ScopedSavepoint::rollbackIfNotFinished()
{
    try {
        if (!finished_) conn_->rollbackToSavepoint(name_);
    }
    catch (...)
    {
        // silence exception; dtor mustn't throw
    }
}

ScopedSavepoint::~ScopedSavepoint()
{
    rollbackIfNotFinished();
}

void ScopedSavepoint::release()
{
    finished_ = true;
    conn_->releaseSavepoint(name_);
}

void ScopedSavepoint::rollback()
{
    finished_ = true;
    conn_->rollbackToSavepoint(name_);
}

}
