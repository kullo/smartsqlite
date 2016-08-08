/*
 * Copyright 2014–2016 The SmartSqlite contributors (see NOTICE.txt)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
