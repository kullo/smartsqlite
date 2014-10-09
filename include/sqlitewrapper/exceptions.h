#ifndef SQLITEWRAPPER_EXCEPTIONS_H
#define SQLITEWRAPPER_EXCEPTIONS_H

#include <exception>
#include <string>

namespace SqliteWrapper {

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

class SqliteException : public Exception
{
public:
    SqliteException(const std::string &func, int resultCode);
    SqliteException(const std::string &func, int resultCode, const std::string &message);
};

}
#endif // SQLITEWRAPPER_EXCEPTIONS_H
