/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
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
