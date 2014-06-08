#ifndef SQLITEWRAPPER_SQLITEWRAPPER_H
#define SQLITEWRAPPER_SQLITEWRAPPER_H

#include <memory>
#include <string>

namespace SqliteWrapper {

class Connection
{
public:
    explicit Connection(const std::string &connectionString);
    ~Connection();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
#endif // SQLITEWRAPPER_SQLITEWRAPPER_H
