#ifndef SQLITEWRAPPER_EXCEPTIONS_H
#define SQLITEWRAPPER_EXCEPTIONS_H

#include <exception>
#include <string>

namespace SqliteWrapper {

class Exception : public std::exception
{
public:
    Exception(const std::string &message) throw();
    Exception(int resultCode);
    Exception(int resultCode, const std::string &message);
    virtual const char *what() const throw();

private:
    std::string m_message;
};

}
#endif // SQLITEWRAPPER_EXCEPTIONS_H
