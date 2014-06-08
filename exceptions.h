#ifndef SQLITEWRAPPER_EXCEPTIONS_H
#define SQLITEWRAPPER_EXCEPTIONS_H

#include <exception>
#include <string>

namespace SqliteWrapper {

class Exception : public std::exception
{
public:
    Exception(int resultCode);
    virtual ~Exception() throw() = default;
    virtual const char *what() const throw();

private:
    Exception(const std::string &message) throw();
    std::string m_message;
};

}
#endif // SQLITEWRAPPER_EXCEPTIONS_H
