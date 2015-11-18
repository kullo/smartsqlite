/*
 * Copyright 2014–2015 The SmartSqlite contributors (see NOTICE.txt)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <exception>
#include <string>

namespace SmartSqlite {

class Exception : public std::exception
{
public:
    Exception(const std::string &message) throw();
    virtual ~Exception() = default;
    virtual const char *what() const throw();

protected:
    std::string m_message;
};

class ParameterUnknown : public Exception
{
public:
    ParameterUnknown(const std::string &parameter);
};

class ColumnUnknown : public Exception
{
public:
    ColumnUnknown(int &columnPos);
    ColumnUnknown(const std::string &columnName);
};

class QueryReturnedRows : public Exception
{
public:
    QueryReturnedRows();
};

class QueryReturnedNoRows : public Exception
{
public:
    QueryReturnedNoRows();
};

class SqliteException : public Exception
{
public:
    SqliteException(const std::string &func, int resultCode);
    SqliteException(const std::string &func, int resultCode, const std::string &message);
};

}
