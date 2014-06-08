#include "connection.h"
#include "sqlite3.h"
#include "util.h"

namespace SqliteWrapper {

struct Connection::Impl
{
    sqlite3 *conn;
};

Connection::Connection(const std::string &connectionString)
    : impl(new Impl)
{
    checkResult(sqlite3_open(connectionString.c_str(), &(impl->conn)));
}

Connection::~Connection()
{
    checkResult(sqlite3_close(impl->conn));
}

}
