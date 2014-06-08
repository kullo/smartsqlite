#include "util.h"

#include "exceptions.h"
#include "sqlite3.h"

namespace SqliteWrapper {
    void checkResult(int result)
    {
        if (result == SQLITE_OK) return;

        throw Exception(result);
    }
}
