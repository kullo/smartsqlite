#include "sqlitewrapper/blob.h"

#include "sqlitewrapper/sqlite3.h"
#include "sqlitewrapper/util.h"

namespace SqliteWrapper {

struct Blob::Impl
{
    sqlite3_blob *blob = nullptr;
    size_t size = 0;
};

Blob::Blob(sqlite3_blob *blob)
    : impl(new Impl)
{
    impl->blob = blob;
    impl->size = sqlite3_blob_bytes(impl->blob);
}

Blob::Blob(Blob &&other)
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
    checkResult(sqlite3_blob_reopen(impl->blob, rowid));
    impl->size = sqlite3_blob_bytes(impl->blob);
}

size_t Blob::size() const
{
    return impl->size;
}

size_t Blob::read(size_t offset, std::uint8_t *buffer, size_t size) const
{
    if (offset >= impl->size) return 0;
    size_t bytesToRead = std::min(size, impl->size - offset);
    checkResult(sqlite3_blob_read(impl->blob, buffer, bytesToRead, offset));
    return bytesToRead;
}

}
