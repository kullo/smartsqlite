#ifndef SQLITEWRAPPER_BLOB_H
#define SQLITEWRAPPER_BLOB_H

#include <memory>

struct sqlite3_blob;

namespace SqliteWrapper {

class Blob
{
public:
    enum Flags { READONLY = 0, READWRITE = 1 };

    explicit Blob(sqlite3_blob *blob);
    Blob(Blob &&other);
    Blob &operator=(Blob &&rhs);
    ~Blob();

    size_t size() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
#endif // SQLITEWRAPPER_BLOB_H
