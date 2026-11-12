#include "stdafx.h"
#include "SqlFormat.h"
#include <optional>
#include "Cache.h"
#include "DBManager.h"
#include "Main.h"
#include "QID.h"

extern int g_iPlayerCacheFlushSeconds;
extern int g_iItemCacheFlushSeconds;
extern int g_iItemPriceListTableCacheFlushSeconds;
extern int g_test_server;
extern int g_item_count;
std::optional<std::string> CreatePlayerSaveQuery(const TPlayerTable& player);

namespace { const int CacheQueryLength = 8192; }

CItemCache::CItemCache()
{
    m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CItemCache::~CItemCache()
{
}

void CItemCache::Delete()
{
    if (m_data.vnum == 0)
        return;

    m_data.vnum = 0;
    m_bNeedQuery = true;
    m_lastUpdateTime = time(0);
    OnFlush();
}

void CItemCache::OnFlush()
{
    if (m_data.vnum == 0)
    {
        const auto query = fmt::sprintf("DELETE FROM item%s WHERE id=%u", GetTablePostfix(), m_data.id);
        CDBManager::instance().ReturnQuery(query.c_str(), QID_ITEM_DESTROY, 0, nullptr);
    }
    else
    {
        const std::int32_t emptySockets[ITEM_SOCKET_MAX_NUM]{};
        const TPlayerItemAttribute emptyAttributes[ITEM_ATTRIBUTE_MAX_NUM]{};
        const auto& item = m_data;

        std::string columns = "id, owner_id, `window`, pos, count, vnum";
        auto values = fmt::sprintf("%u, %u, %d, %d, %u, %u",
            item.id, item.owner, item.window, item.pos, item.count, item.vnum);

        if (memcmp(emptySockets, item.alSockets, sizeof(emptySockets)) != 0)
        {
            columns += ", socket0, socket1, socket2";
            values += fmt::sprintf(", %u, %u, %u",
                    static_cast<std::uint32_t>(item.alSockets[0]),
                    static_cast<std::uint32_t>(item.alSockets[1]),
                    static_cast<std::uint32_t>(item.alSockets[2]));
        }

        if (memcmp(emptyAttributes, item.aAttr, sizeof(emptyAttributes)) != 0)
        {
            columns += ", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3"
                       ", attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6";
            values += fmt::sprintf(", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                    item.aAttr[0].bType, item.aAttr[0].sValue,
                    item.aAttr[1].bType, item.aAttr[1].sValue,
                    item.aAttr[2].bType, item.aAttr[2].sValue,
                    item.aAttr[3].bType, item.aAttr[3].sValue,
                    item.aAttr[4].bType, item.aAttr[4].sValue,
                    item.aAttr[5].bType, item.aAttr[5].sValue,
                    item.aAttr[6].bType, item.aAttr[6].sValue);
        }

        const auto query = fmt::sprintf("REPLACE INTO item%s (%s) VALUES(%s)", GetTablePostfix(), columns, values);

        CDBManager::instance().ReturnQuery(query.c_str(), QID_ITEM_SAVE, 0, nullptr);
        ++g_item_count;
    }

    m_bNeedQuery = false;
}

CPlayerTableCache::CPlayerTableCache()
{
    m_expireTime = MIN(1800, g_iPlayerCacheFlushSeconds);
}

CPlayerTableCache::~CPlayerTableCache()
{
}

void CPlayerTableCache::OnFlush()
{
    const auto query = CreatePlayerSaveQuery(m_data);
    if (!query)
    {
        sys_err("Unable to construct player save query for %lu", m_data.id);
        return;
    }
    CDBManager::instance().ReturnQuery(query->c_str(), QID_PLAYER_SAVE, 0, NULL);
}

const int CItemPriceListTableCache::s_nMinFlushSec = 1800;

CItemPriceListTableCache::CItemPriceListTableCache()
{
    m_expireTime = MIN(s_nMinFlushSec, g_iItemPriceListTableCacheFlushSeconds);
}

void CItemPriceListTableCache::UpdateList(const TItemPriceListTable* update)
{
    std::vector<TItemPriceInfo> retained;
    const TItemPriceInfo* end = update->aPriceInfo + update->byCount;
    for (unsigned int i = 0; i < m_data.byCount; ++i)
    {
        const TItemPriceInfo* entry = update->aPriceInfo;
        while (entry != end && entry->dwVnum != m_data.aPriceInfo[i].dwVnum)
            ++entry;

        if (entry == end)
            retained.push_back(m_data.aPriceInfo[i]);
    }

    m_data.byCount = update->byCount;
    thecore_memcpy(m_data.aPriceInfo, update->aPriceInfo, sizeof(TItemPriceInfo) * update->byCount);

    size_t added = 0;
    if (update->byCount < SHOP_PRICELIST_MAX_NUM)
    {
        added = SHOP_PRICELIST_MAX_NUM - update->byCount;
        if (retained.size() < added)
            added = retained.size();

        if (added != 0)
            thecore_memcpy(m_data.aPriceInfo + update->byCount, &retained[0], sizeof(TItemPriceInfo) * added);
        m_data.byCount = static_cast<BYTE>(m_data.byCount + added);
    }

    m_bNeedQuery = true;
}

void CItemPriceListTableCache::OnFlush()
{
    char query[CacheQueryLength];
    dbsql::snprintf(query, sizeof(query), "DELETE FROM myshop_pricelist%s WHERE owner_id = %lu", GetTablePostfix(), static_cast<unsigned long>(m_data.dwOwnerID));
    CDBManager::instance().ReturnQuery(query, QID_ITEMPRICE_DESTROY, 0, NULL);

    for (unsigned int i = 0; i < m_data.byCount; ++i)
    {
        dbsql::snprintf(query, sizeof(query),
                 "INSERT INTO myshop_pricelist%s( owner_id, item_vnum, price ) VALUES( %lu, %lu, %lu )",
                 GetTablePostfix(), static_cast<unsigned long>(m_data.dwOwnerID),
                 static_cast<unsigned long>(m_data.aPriceInfo[i].dwVnum),
                 static_cast<unsigned long>(m_data.aPriceInfo[i].dwPrice));
        CDBManager::instance().ReturnQuery(query, QID_ITEMPRICE_SAVE, 0, NULL);
    }
    m_bNeedQuery = false;
}
