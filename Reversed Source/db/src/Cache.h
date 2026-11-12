#ifndef __INC_DB_CACHE_H__
#define __INC_DB_CACHE_H__

#include "CacheBase.h"

class CItemCache : public cache<TPlayerItem>
{
public:
    CItemCache();
    virtual ~CItemCache();
    void Delete();
    virtual void OnFlush();
};

class CPlayerTableCache : public cache<TPlayerTable>
{
public:
    CPlayerTableCache();
    virtual ~CPlayerTableCache();
    virtual void OnFlush();
    DWORD GetLastUpdateTime() { return m_lastUpdateTime; }
};

class CItemPriceListTableCache final : public cache<TItemPriceListTable>
{
public:
    CItemPriceListTableCache();
    void UpdateList(const TItemPriceListTable* update);
    virtual void OnFlush();

private:
    static const int s_nMinFlushSec;
};

#endif
