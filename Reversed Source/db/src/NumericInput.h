#ifndef MCNCC_DB_NUMERIC_INPUT_H
#define MCNCC_DB_NUMERIC_INPUT_H

#include <cerrno>
#include <cstdlib>
#include <limits>
#include <type_traits>

namespace dbinput
{
// Preserve the field width and use the empty-field value for invalid input.
template <class T>
T IntegerOrZero(long long value)
{
    if (value < (std::numeric_limits<T>::min)() || value > (std::numeric_limits<T>::max)())
        return 0;
    return static_cast<T>(value);
}

template <class T>
T ReadInteger(const char* text)
{
    if (!text)
        return 0;
    char* end;
    errno = 0;
    const long long value = std::strtoll(text, &end, 10);
    if (end == text || *end || errno == ERANGE)
        return 0;
    return IntegerOrZero<T>(value);
}
}

#endif
