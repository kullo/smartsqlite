#include "exceptions.h"

#include "result_names.h"
#include "sqlite3.h"

namespace SqliteWrapper {

Exception::Exception(const std::string &message) throw()
    : m_message(message)
{
}

Exception::Exception(int resultCode)
    : m_message(
          resultToResultName(resultCode) + ": " +
          sqlite3_errstr(resultCode)
          )
{
}

Exception::Exception(int resultCode, const std::string &message)
    : m_message(
          resultToResultName(resultCode) + ": " +
          sqlite3_errstr(resultCode) + "\n" +
          message
          )
{
}

const char *Exception::what() const throw()
{
    return m_message.c_str();
}

}
