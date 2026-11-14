#ifndef MCNCC_TAFFECTFLAG_H
#define MCNCC_TAFFECTFLAG_H
#include "../../libthecore/include/typedef.h"
struct TAffectFlag {
    DWORD bits[2];

    TAffectFlag() : bits{0, 0} {}
    TAffectFlag(DWORD low, DWORD high = 0)
        : bits{static_cast<unsigned int>(low), static_cast<unsigned int>(high)} {}

    TAffectFlag& operator=(const TAffectFlag& other)
    {
        bits[0] = static_cast<unsigned int>(other.bits[0]);
        bits[1] = static_cast<unsigned int>(other.bits[1]);
        return *this;
    }

    bool IsSet(int flag)
    {
        if (flag < 1 || flag > 64)
            return false;
        const auto bit = static_cast<unsigned int>(flag - 1);
        return (bits[bit >> 5] & (1u << (bit & 31u))) != 0;
    }

    void Set(int flag)
    {
        if (flag < 1 || flag > 64)
            return;
        const auto bit = static_cast<unsigned int>(flag - 1);
        bits[bit >> 5] |= 1u << (bit & 31u);
    }

    void Reset(int flag)
    {
        if (flag < 1 || flag > 64)
            return;
        const auto bit = static_cast<unsigned int>(flag - 1);
        bits[bit >> 5] &= ~(1u << (bit & 31u));
    }
};

inline bool operator==(const TAffectFlag& left, const TAffectFlag& right)
{
    return static_cast<unsigned int>(left.bits[0]) == static_cast<unsigned int>(right.bits[0]) &&
           static_cast<unsigned int>(left.bits[1]) == static_cast<unsigned int>(right.bits[1]);
}

inline bool operator!=(const TAffectFlag& left, const TAffectFlag& right)
{
    return !(left == right);
}
#endif
