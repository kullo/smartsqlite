#ifndef SQLITEWRAPPER_VERSION_H
#define SQLITEWRAPPER_VERSION_H

#include <string>

namespace SqliteWrapper {
    std::string sqliteVersion();
    bool checkVersion();
}
#endif // SQLITEWRAPPER_VERSION_H
