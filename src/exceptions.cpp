#include "sqlitewrapper/exceptions.h"

#include "result_names.h"
#include "sqlitewrapper/sqlite3.h"

namespace SqliteWrapper {

Exception::Exception(const std::string &message) throw()
    : m_message(message)
{
}

const char *Exception::what() const throw()
{
    return m_message.c_str();
}

ParameterUnknown::ParameterUnknown(const std::string &parameter)
    : Exception(
          std::string("Parameter not found in query: ") +
          parameter)
{
}

ColumnUnknown::ColumnUnknown(int &columnPos)
    : Exception(
          std::string("Column not found in result: ") +
          std::to_string(columnPos))
{
}

ColumnUnknown::ColumnUnknown(const std::string &columnName)
    : Exception(
          std::string("Column not found in result: ") +
          columnName)
{
}

SqliteException::SqliteException(int resultCode)
    : Exception(
          resultToResultName(resultCode) +
          " (" + sqlite3_errstr(resultCode) + ")"
          )
{
}

SqliteException::SqliteException(int resultCode, const std::string &message)
    : Exception(
          resultToResultName(resultCode) +
          " (" + sqlite3_errstr(resultCode) + "): " +
          message
          )
{
}

}
