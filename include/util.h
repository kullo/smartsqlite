#ifndef SQLITEWRAPPER_UTIL_H
#define SQLITEWRAPPER_UTIL_H

#include <string>

namespace SqliteWrapper {

void checkResult(int result);
void checkResult(int result, const std::string &message);

}
#endif // SQLITEWRAPPER_UTIL_H
