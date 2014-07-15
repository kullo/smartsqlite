#ifndef SQLITEWRAPPER_BLOB_H
#define SQLITEWRAPPER_BLOB_H

#include <cstdint>
#include <memory>

struct sqlite3;
struct sqlite3_blob;

namespace SqliteWrapper {

class Blob
{
public:
    enum Flags { READONLY = 0, READWRITE = 1 };

    explicit Blob(sqlite3 *conn, sqlite3_blob *blob);
    Blob(Blob &&other);
    Blob &operator=(Blob &&rhs);
    ~Blob();

    void moveToRow(std::int64_t rowid);
    size_t size() const;
    size_t read(void *buffer, size_t size, size_t offset = 0) const;
    size_t write(const void *buffer, size_t size, size_t offset = 0);

private:
    size_t getAccessSize(size_t bufferSize, size_t offset) const;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
#endif // SQLITEWRAPPER_BLOB_H
