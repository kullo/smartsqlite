#ifndef SQLITEWRAPPER_NULLABLE_H
#define SQLITEWRAPPER_NULLABLE_H

namespace SqliteWrapper {

template <typename T>
class Nullable
{
public:
    Nullable()
        : m_isNull(true), m_value(T())
    {
    }

    Nullable(const T &value)
        : m_isNull(false), m_value(value)
    {
    }

    bool isNull() const
    {
        return m_isNull;
    }

    void setNull()
    {
        m_isNull = true;
        m_value = T();
    }

    T value() const
    {
        return m_value;
    }

    void setValue(const T &value)
    {
        m_value = value;
    }

private:
    bool m_isNull;
    T m_value;
};

}
#endif // SQLITEWRAPPER_NULLABLE_H
