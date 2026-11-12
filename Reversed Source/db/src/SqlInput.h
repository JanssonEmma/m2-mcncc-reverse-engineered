#ifndef MCNCC_DB_SQL_INPUT_H
#define MCNCC_DB_SQL_INPUT_H

#include "DBManager.h"
#include <cstring>
#include <optional>
#include <string>
#include <string_view>

namespace dbsql
{
inline std::optional<std::string_view> ReadField(const char* field, std::size_t capacity)
{
    for (std::size_t i = 0; i < capacity; ++i)
    {
        if (field[i] == '\0')
            return std::string_view(field, i);
    }
    return std::nullopt;
}

inline std::optional<std::string> EscapeLiteral(std::string_view input, int slot = SQL_PLAYER)
{
    std::string escaped(input.size() * 2 + 1, '\0');
    const auto size = CDBManager::instance().EscapeString(escaped.data(), input.data(), static_cast<unsigned long>(input.size()), slot);

    if (size >= escaped.size())
        return std::nullopt;

    escaped.resize(size);
    return escaped;
}

template <std::size_t Size>
std::optional<std::string> EscapeField(const char (&field)[Size], int slot = SQL_PLAYER)
{
    const auto input = ReadField(field, Size);
    return input ? EscapeLiteral(*input, slot) : std::nullopt;
}

inline bool IsIdentifier(std::string_view input)
{
    const auto alpha = [](char c) {
        return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_';
    };
    if (input.empty() || input.size() > 64 || !alpha(input.front()))
        return false;
    for (char c : input)
    {
        if (!alpha(c) && !(c >= '0' && c <= '9'))
            return false;
    }
    return true;
}
}

#endif
