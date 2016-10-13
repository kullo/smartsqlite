/*
 * Copyright 2014–2016 The SmartSqlite contributors (see NOTICE.txt)
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

#if defined(_MSC_VER) && (_MSC_VER < 1900)
  // in MSVS 2013, where is no noexcept keyword
  #include <yvals.h>
  #define SMARTSQLITE_NOEXCEPT _NOEXCEPT
#else
  #define SMARTSQLITE_NOEXCEPT noexcept
#endif

namespace SmartSqlite {

class Exception : public std::exception
{
public:
    Exception(const std::string &message, const std::string &sql = "") SMARTSQLITE_NOEXCEPT;
    const char *what() const SMARTSQLITE_NOEXCEPT override;

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
    QueryReturnedRows(const std::string &sql = "");
};

class QueryReturnedNoRows : public Exception
{
public:
    QueryReturnedNoRows(const std::string &sql = "");
};

class SqliteException : public Exception
{
public:
    SqliteException(const std::string &func, int resultCode);
    SqliteException(const std::string &func, int resultCode, const std::string &message);
};

class FeatureUnavailable : public Exception
{
public:
    FeatureUnavailable(const std::string &feature);
};

}
