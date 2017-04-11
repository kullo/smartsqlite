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
