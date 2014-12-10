#pragma once

#include <string>
#include <vector>

struct sqlite3_stmt;

namespace SqliteWrapper {

class NativeExtractor
{
public:
    static long long extractLongLong(sqlite3_stmt *stmt, int pos);
    static double extractDouble(sqlite3_stmt *stmt, int pos);
    static std::string extractString(sqlite3_stmt *stmt, int pos);
    static std::vector<unsigned char> extractBlob(sqlite3_stmt *stmt, int pos);
};

// extension point: specialize this to add support for custom types
template <typename T>
class Extractor
{
public:
    static T extract(sqlite3_stmt *stmt, int pos);
};

template <>
class Extractor<std::string>
{
public:
    static std::string extract(sqlite3_stmt *stmt, int pos)
    {
        return NativeExtractor::extractString(stmt, pos);
    }
};

template <>
class Extractor<std::vector<unsigned char>>
{
public:
    static std::vector<unsigned char> extract(sqlite3_stmt *stmt, int pos)
    {
        return NativeExtractor::extractBlob(stmt, pos);
    }
};

}
