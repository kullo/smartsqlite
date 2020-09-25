SmartSqlite
=============

SmartSqlite is a C++11 wrapper around SQLite3.
We call it "smart" because of the consistent use of smart pointers.

A copy of SQLite is shipped with this library,
thanks [Richard](https://www.sqlite.org/crew.html)!

[![Build Status](https://travis-ci.org/kullo/smartsqlite.svg?branch=master)](https://travis-ci.org/kullo/smartsqlite)
[![Build status](https://ci.appveyor.com/api/projects/status/rx2v5ph0fuj4lume/branch/master?svg=true)](https://ci.appveyor.com/project/webmaster128/smartsqlite)

Get started
-----------

```
$ git clone --recursive https://github.com/kullo/smartsqlite.git
$ cmake .
$ make
$ ctest
```

Sample code:

```
#include <iostream>

#include <smartsqlite/connection.h>
#include <smartsqlite/row.h>

int main(int argc, char** argv)
{
    SmartSqlite::Connection conn(":memory:");
    conn.exec("CREATE TABLE temparatures "
              "(id INT, temperature FLOAT, city TEXT)");
    conn.exec("INSERT INTO temparatures "
              "VALUES (1, 2.1, 'Berlin')");
    conn.exec("INSERT INTO temparatures "
              "VALUES (2, 2.0, 'New York')");
    conn.exec("INSERT INTO temparatures "
              "VALUES (42, -1.8, 'Cape Town')");

    SmartSqlite::Statement stm = conn.prepare(
                "SELECT * FROM temparatures WHERE id >= :value");
    stm.bind(":value", 2);
    for (SmartSqlite::RowIterator itr = stm.begin(); itr != stm.end(); ++itr)
    {
        int id = itr->get<int>("id");
        float temparature = itr->get<float>("temperature");
        std::string city = itr->get<std::string>("city");
        std::cout << "(" << id << ") " << city << ": " << temparature << std::endl;
    }

    return 0;
}
```

Versions
----------------

SmartSqlite has a linear versioning and the API might break at any time.

Version     | SQLite version | Changes in SmartSqlite
----------- | -------------- | ----------------------
v26         | 3.31.1         | resultToResultName: Add new extended result codes
v25         | 3.29.0         | -
v24         | 3.26.0         | resultToResultName: Add new extended result codes; Fix build breakage due to changed sqlite3BtreeBeginTrans interface
v23         | 3.23.1         | resultToResultName: Add new extended result codes
v22         | 3.21.0         | resultToResultName: Add new extended result codes
v21         | 3.20.1         | tests: Fix temp file name generation for BotanSqlite3; Prevent using 64b filesystem API calls on Android
v20         | 3.19.2         | Enable FTS5 (full-text search)
v19         | 3.18.0         | -
v18         | 3.17.0         | resultToResultName: Add missing result names
v17         | 3.16.2         | -
v16         | 3.15.1         | -
v15         | 3.14.2         | -
v14         | 3.14.1         | Rework botansqlite3 key handling
v13         | 3.13.0         | Add a copy of botansqlite3 to the project
v12         | 3.13.0         | Update build system: use libc++ with Linux+clang. Add sqlite3 shell binary to build.
v11         | 3.13.0         | Add Connection::changes(); Add ScopedSavepoint
v10         | 3.13.0         | Improve cmake configuration; Add OS X build to Travis
v9          | 3.12.1         | NativeBinder: Correctly bind empty strings and blobs; NativeBinder: Copy pointer instead of referencing it in bindBlob(); Add SQL statement text to some exception messages
v8          | 3.12.0         | –
v7          | 3.11.1         | –
v6          | 3.11.0         | Fix possible nullptr dereference in Blob
v5          | 3.10.2         | –
v4          | 3.9.2          | Update implementation of Connection
v3          | 3.9.2          | Add version() and versionCode(); Rename checkVersion() to checkSqliteVersion()
v2          | 3.9.2          | Replace factory functions makeConnection*() with string constructor
v1          | 3.9.2          | Initial version


Compiler support
----------------

* GCC 4.8+
* Clang 3.4+
* MSVC 2015+


Contibutions
----------------

Contibutions respecting the library's license
are highly welcome via Github issues and pull requests.


Users
----------------

This library is used by:

* [Kullo](https://www.kullo.net/) (Windows, Linux, OS X, Android and iOS)

Please add yourself to the list using PRs.


License
-------

SmartSqlite is licensed under the 3-clause BSD license. See [LICENSE.txt](LICENSE.txt) for details.
The encryption module botansqlite3 is licensed under the Botan license, a 2-clause BSD license (see src/botansqlite3/LICENSE.txt).
The SQLite3 shell uses the Linenoise library for line editing. It is licensed under 2-clause BSD (see src/linenoise/LICENSE).
