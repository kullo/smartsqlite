SmartSqlite
=============

SmartSqlite is a C++11 wrapper around SQLite3.
We call it "smart" because of the consistent use of smart pointers.

A copy of SQLite is shipped with this library,
thanks [Richard](https://www.sqlite.org/crew.html)!

[![Build Status](https://travis-ci.org/kullo/smartsqlite.svg)](https://travis-ci.org/kullo/smartsqlite)

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
* MSVC 2013+


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

```
Copyright 2014–2015 The SmartSqlite contributors (see NOTICE.txt)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
