#include "exceptions.h"

#include "result_names.h"
#include "sqlite3.h"

namespace SqliteWrapper {

Exception::Exception(const std::string &message) throw()
    : m_message(message)
{
}

const char *Exception::what() const throw()
{
    return m_message.c_str();
}

SqliteException::SqliteException(int resultCode)
    : Exception(
          resultToResultName(resultCode) + ": " +
          sqlite3_errstr(resultCode)
          )
{
}

SqliteException::SqliteException(int resultCode, const std::string &message)
    : Exception(
          resultToResultName(resultCode) + ": " +
          sqlite3_errstr(resultCode) + "\n" +
          message
          )
{
}

}
