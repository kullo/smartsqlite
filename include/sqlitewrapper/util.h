#ifndef SQLITEWRAPPER_UTIL_H
#define SQLITEWRAPPER_UTIL_H

#include <string>

struct sqlite3;

namespace SqliteWrapper {

void checkResult(int result);
void checkResult(int result, sqlite3 *conn);
void checkResult(int result, const std::string &message);

}
#endif // SQLITEWRAPPER_UTIL_H
