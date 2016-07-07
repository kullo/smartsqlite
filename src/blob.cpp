/*
 * Copyright 2014–2015 The SmartSqlite contributors (see NOTICE.txt)
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
#include "smartsqlite/blob.h"

#include <algorithm>
#include <cassert>
#include <limits>

#include "smartsqlite/sqlite3.h"
#include "smartsqlite/util.h"

namespace SmartSqlite {

struct Blob::Impl
{
    sqlite3 *conn = nullptr;
    sqlite3_blob *blob = nullptr;
    size_t size = 0;
};

Blob::Blob(sqlite3 *conn, sqlite3_blob *blob)
    : impl(new Impl)
{
    impl->conn = conn;
    impl->blob = blob;
    impl->size = static_cast<size_t>(sqlite3_blob_bytes(impl->blob));
}

Blob::Blob(Blob &&other)
    : impl(new Impl)
{
    std::swap(impl, other.impl);
}

Blob &Blob::operator=(Blob &&rhs)
{
    std::swap(impl, rhs.impl);
    return *this;
}

Blob::~Blob()
{
    sqlite3_blob_close(impl->blob);
}

void Blob::moveToRow(std::int64_t rowid)
{
    CHECK_RESULT_CONN(sqlite3_blob_reopen(impl->blob, rowid), impl->conn);
    impl->size = static_cast<size_t>(sqlite3_blob_bytes(impl->blob));
}

size_t Blob::size() const
{
    return impl->size;
}

size_t Blob::getAccessSize(size_t bufferSize, size_t offset) const
{
    if (offset >= impl->size) return 0;
    return std::min(bufferSize, impl->size - offset);
}

size_t Blob::read(void *buffer, size_t size, size_t offset) const
{
    size_t bytesToRead = getAccessSize(size, offset);
    assert(bytesToRead <= std::numeric_limits<int>::max());
    auto bytesToReadInt = static_cast<int>(bytesToRead);

    assert(offset <= std::numeric_limits<int>::max());
    auto offsetInt = static_cast<int>(offset);

    CHECK_RESULT_CONN(sqlite3_blob_read(impl->blob, buffer, bytesToReadInt, offsetInt), impl->conn);
    return bytesToRead;
}

size_t Blob::write(const void *buffer, size_t size, size_t offset)
{
    size_t bytesToWrite = getAccessSize(size, offset);
    assert(bytesToWrite <= std::numeric_limits<int>::max());
    auto bytesToWriteInt = static_cast<int>(bytesToWrite);

    assert(offset <= std::numeric_limits<int>::max());
    auto offsetInt = static_cast<int>(offset);

    CHECK_RESULT_CONN(sqlite3_blob_write(impl->blob, buffer, bytesToWriteInt, offsetInt), impl->conn);
    return bytesToWrite;
}

}
