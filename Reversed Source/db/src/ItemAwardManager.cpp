#include "stdafx.h"
#include "SqlFormat.h"
#include "QID.h"
#include "DBManager.h"
#include "ItemAwardManager.h"

ItemAwardManager::ItemAwardManager()
{
}

ItemAwardManager::~ItemAwardManager()
{
}

void ItemAwardManager::RequestLoad()
{
    char query[QUERY_MAX_LEN];
    dbsql::snprintf(query, sizeof(query),
             "SELECT id,login,vnum,count,socket0,socket1,socket2,mall,why FROM item_award WHERE taken_time IS NULL");
    CDBManager::instance().ReturnQuery(query, QID_ITEM_AWARD_LOAD, 0, NULL);
}

void ItemAwardManager::Load(SQLMsg* pMsg)
{
    MYSQL_RES* result = pMsg->Get()->pSQLResult;
    for (int i = 0; i < pMsg->Get()->iNumRows; ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        DWORD id = atoi(row[0]);
        if (m_map_award.find(id) != m_map_award.end())
            continue;

        TItemAward* award = new TItemAward;
        memset(award, 0, sizeof(*award));
        award->dwID = id;
        trim_and_lower(award->szLogin, row[1], LOGIN_MAX_LEN);
        award->dwVnum = atoi(row[2]);
        award->dwCount = atoi(row[3]);
        award->dwSocket0 = atoi(row[4]);
        award->dwSocket1 = atoi(row[5]);
        award->dwSocket2 = atoi(row[6]);
        award->bMall = atoi(row[7]) != 0;

        if (row[8])
        {
            strncpy(award->szWhy, row[8], ITEM_AWARD_WHY_MAX_LEN);
            award->szWhy[ITEM_AWARD_WHY_MAX_LEN] = '\0';
        }

        m_map_award.insert(std::make_pair(id, award));
        printf("ITEM_AWARD load id %lu bMall %d \n",
               static_cast<unsigned long>(id), award->bMall);
        sys_log(0, "ITEM_AWARD: load id %lu login %s vnum %lu count %u socket %lu",
                static_cast<unsigned long>(id), award->szLogin,
                static_cast<unsigned long>(award->dwVnum),
                static_cast<unsigned int>(award->dwCount),
                static_cast<unsigned long>(award->dwSocket0));
        m_map_kSetAwardByLogin[award->szLogin].insert(award);
    }
}

std::set<TItemAward*>* ItemAwardManager::GetByLogin(const char* login)
{
    std::map<std::string, std::set<TItemAward*> >::iterator it = m_map_kSetAwardByLogin.find(login);
    if (it == m_map_kSetAwardByLogin.end())
        return NULL;
    return &it->second;
}

void ItemAwardManager::Taken(DWORD awardID, DWORD itemID)
{
    std::map<DWORD, TItemAward*>::iterator it = m_map_award.find(awardID);
    if (it == m_map_award.end())
    {
        sys_log(0, "ITEM_AWARD: Taken ID not exist %lu", static_cast<unsigned long>(awardID));
        return;
    }

    it->second->bTaken = true;

    char query[QUERY_MAX_LEN];
    dbsql::snprintf(query, sizeof(query),
             "UPDATE item_award SET taken_time=NOW(),item_id=%lu WHERE id=%lu AND taken_time IS NULL",
             static_cast<unsigned long>(itemID), static_cast<unsigned long>(awardID));
    CDBManager::instance().ReturnQuery(query, QID_ITEM_AWARD_TAKEN, 0, NULL);
}
