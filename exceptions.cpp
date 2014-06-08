#include "exceptions.h"

#include "result_names.h"
#include "sqlite3.h"

namespace SqliteWrapper {

Exception::Exception(const std::string &message) throw()
    : m_message(message)
{
}

Exception::Exception(int resultCode)
{
    m_message = resultToResultName(resultCode) + ": " + sqlite3_errstr(resultCode);
}

const char *Exception::what() const throw()
{
    return m_message.c_str();
}

}
