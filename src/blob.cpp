#include "sqlitewrapper/blob.h"

#include "sqlitewrapper/sqlite3.h"

namespace SqliteWrapper {

struct Blob::Impl
{
    sqlite3_blob *blob = nullptr;
};

Blob::Blob(sqlite3_blob *blob)
    : impl(new Impl)
{
    impl->blob = blob;
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

}
