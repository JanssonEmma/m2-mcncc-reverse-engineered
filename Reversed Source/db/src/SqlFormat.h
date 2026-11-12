#ifndef MCNCC_DB_SQL_FORMAT_H
#define MCNCC_DB_SQL_FORMAT_H

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <fmt/printf.h>

namespace dbsql
{
template <typename... Args>
int snprintf(char* destination, std::size_t capacity, fmt::string_view format, const Args&... args)
{
    const auto query = fmt::sprintf(format, args...);
    if (capacity != 0)
    {
        const auto size = std::min(query.size(), capacity - 1);
        std::memcpy(destination, query.data(), size);
        destination[size] = '\0';
    }
    return static_cast<int>(query.size());
}

template <std::size_t Size, typename... Args>
int sprintf(char (&destination)[Size], fmt::string_view format, const Args&... args)
{
    return dbsql::snprintf(destination, Size, format, args...);
}
}

#endif
