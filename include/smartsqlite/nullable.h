/*
 * Copyright 2014–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#pragma once

#include <cassert>

namespace SmartSqlite {

template <typename T>
class Nullable
{
public:
    Nullable()
        : m_isNull(true), m_value(T())
    {
    }

    explicit Nullable(const T &value)
        : m_isNull(false), m_value(value)
    {
    }

    bool operator==(const Nullable<T> &rhs) const
    {
        return (m_isNull == rhs.m_isNull) && (m_value == rhs.m_value);
    }

    bool operator!=(const Nullable<T> &rhs) const
    {
        return !(*this == rhs);
    }

    explicit operator bool() const
    {
        return !m_isNull;
    }

    const T &operator*() const
    {
        assert(!m_isNull);
        return m_value;
    }

    void setValue(const T &value)
    {
        m_isNull = false;
        m_value = value;
    }

    void setNull()
    {
        m_isNull = true;
        m_value = T();
    }

private:
    bool m_isNull;
    T m_value;
};

}
