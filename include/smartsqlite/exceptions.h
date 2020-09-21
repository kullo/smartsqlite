/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <exception>
#include <string>

namespace SmartSqlite {

class Exception : public std::exception
{
public:
    Exception(const std::string &message, const std::string &sql = "") noexcept;
    const char *what() const noexcept override;

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
