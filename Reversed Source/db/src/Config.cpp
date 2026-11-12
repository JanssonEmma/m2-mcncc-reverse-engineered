#include "stdafx.h"
#include "Config.h"
#include <charconv>
#include <sstream>
#include <string_view>

namespace
{
template <class T>
bool ParseInteger(std::string_view text, T& value)
{
    if (text.empty())
        return false;
    T parsed{};
    const auto result = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size())
        return false;
    value = parsed;
    return true;
}
}

CConfig::CConfig()
{
}

CConfig::~CConfig()
{
    Destroy();
}

void CConfig::Destroy()
{
    m_valueMap.clear();
}

void CConfig::NextLine(FILE* fp)
{
    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\n')
            return;
    }
}

bool CConfig::GetWord(FILE* fp, std::string& dest)
{
    dest.clear();
    bool quoted = false;
    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\r')
            continue;
        if (quoted)
        {
            if (c == '"')
                return true;
        }
        else
        {
            const bool delimiter = c == ' ' || c == '\t' || c == '\n';
            if (dest.empty())
            {
                if (c == '"')
                {
                    quoted = true;
                    continue;
                }
                if (delimiter)
                    continue;
            }
            if (delimiter)
                return true;
        }
        dest.push_back(static_cast<char>(c));
    }
    return !quoted && !dest.empty();
}

bool CConfig::GetLine(FILE* fp, std::string& dest)
{
    dest.clear();
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n')
        dest.push_back(static_cast<char>(c));
    return !ferror(fp);
}

bool CConfig::LoadFile(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
        return false;

    std::string token;
    std::string key;
    int mode = 0;
    while (GetWord(fp, token))
    {
        if (token == "//")
        {
            NextLine(fp);
            continue;
        }

        switch (mode)
        {
        case 0:
            key = token;
            ++mode;
            break;
        case 1:
            if (token.starts_with('='))
                ++mode;
            break;
        case 2:
            mode = 0;
            m_valueMap.emplace(key, token);
            break;
        }

        if (mode == 2 && key == "ITEM_ID_RANGE" && GetLine(fp, token))
        {
            m_valueMap.emplace(key, token);
            mode = 0;
        }
    }

    const bool complete = !ferror(fp) && mode == 0;
    fclose(fp);
    return complete;
}

std::string* CConfig::Search(const char* key)
{
    TValueMap::iterator entry = m_valueMap.find(key);
    if (entry == m_valueMap.end())
        return NULL;

    return &entry->second;
}

bool CConfig::GetParam(const char* key, int index, DWORD* dest)
{
    std::string* value = Search(key);
    if (!value)
        return false;

    if (index < 0)
        return false;

    std::istringstream input(*value);
    std::string token;
    for (int i = 0; i <= index; ++i)
    {
        if (!(input >> token))
            return false;
    }
    return ParseInteger(token, *dest);
}

const char* CConfig::Get(const char* key)
{
    std::string* value = Search(key);
    if (!value)
    {
        static std::string stTemp = "";
        return stTemp.c_str();
    }

    return value->c_str();
}

bool CConfig::GetValue(const char* key, int* dest)
{
    if (!Search(key))
        return false;

    return ParseInteger(Get(key), *dest);
}

bool CConfig::GetValue(const char* key, float* dest)
{
    if (!Search(key))
        return false;

    *dest = static_cast<float>(atof(Get(key)));
    return true;
}

bool CConfig::GetValue(const char* key, DWORD* dest)
{
    if (!Search(key))
        return false;

    return ParseInteger(Get(key), *dest);
}

bool CConfig::GetValue(const char* key, BYTE* dest)
{
    if (!Search(key))
        return false;

    *dest = static_cast<BYTE>(*Get(key));
    return true;
}

bool CConfig::GetValue(const char* key, char* dest, int size)
{
    const auto* value = Search(key);
    if (!value || size <= 0 || value->empty() || value->size() >= static_cast<size_t>(size) ||
        value->find('\0') != std::string::npos)
        return false;

    memcpy(dest, value->c_str(), value->size() + 1);
    return true;
}

bool CConfig::GetTwoValue(const char* key, DWORD* first, DWORD* second)
{
    if (!GetParam(key, 0, first))
        return false;

    return GetParam(key, 1, second);
}
