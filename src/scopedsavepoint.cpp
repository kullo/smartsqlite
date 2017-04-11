/*
 * Copyright 2014–2017 The SmartSqlite contributors (see NOTICE.txt)
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
