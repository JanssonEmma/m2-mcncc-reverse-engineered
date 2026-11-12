#ifndef MCNCC_ITEM_MANAGER_PRIVATE_TYPES_H
#define MCNCC_ITEM_MANAGER_PRIVATE_TYPES_H
#include "../../libthecore/include/typedef.h"
#include <vector>

class CItemDropInfo {
public:
    CItemDropInfo(int start, int end, int percent, DWORD vnum)
        : m_iLevelStart(start), m_iLevelEnd(end), m_iPercent(percent), m_dwVnum(vnum)
    {
    }

    int m_iLevelStart;
    int m_iLevelEnd;
    int m_iPercent;
    DWORD m_dwVnum;

    friend bool operator<(const CItemDropInfo& left, const CItemDropInfo& right)
    {
        return left.m_iLevelEnd < right.m_iLevelEnd;
    }
};

extern std::vector<CItemDropInfo> g_vec_pkCommonDropItem[6];
#endif

