#ifndef SQLITEWRAPPER_UTIL_H
#define SQLITEWRAPPER_UTIL_H

#include <string>

struct sqlite3;
struct sqlite3_stmt;

#define CHECK_RESULT(result) \
    SqliteWrapper::checkResult(__func__, (result))

#define CHECK_RESULT_CONN(result, conn) \
    SqliteWrapper::checkResult(__func__, (result), (conn))

#define CHECK_RESULT_STMT(result, conn, stmt) \
    SqliteWrapper::checkResult(__func__, (result), (conn), (stmt))

#define CHECK_RESULT_MSG(result, message) \
    SqliteWrapper::checkResult(__func__, (result), (message))

namespace SqliteWrapper {

void checkResult(const std::string &func, int result);
void checkResult(const std::string &func, int result, sqlite3 *conn, sqlite3_stmt *stmt = nullptr);
void checkResult(const std::string &func, int result, const std::string &message);

}
#endif // SQLITEWRAPPER_UTIL_H
