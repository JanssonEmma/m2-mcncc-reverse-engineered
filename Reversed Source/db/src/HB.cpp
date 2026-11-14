#include "stdafx.h"
#include "SqlFormat.h"
#include "HB.h"
#include "Main.h"
#include "DBManager.h"

PlayerHB::PlayerHB() : m_iExpireTime(3600)
{
}

PlayerHB::~PlayerHB()
{
}

bool PlayerHB::Initialize()
{
    char query[128];
    dbsql::snprintf(query, sizeof(query), "SHOW CREATE TABLE player%s", GetTablePostfix());

    SQLMsg* message = CDBManager::instance().DirectQuery(query);
    if (message->Get()->iNumRows == 0)
    {
        delete message;
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
    m_stCreateTableQuery = row[1];
    delete message;
    return true;
}

void PlayerHB::Put(DWORD id)
{
    auto entry = m_map_data.find(id);
    if (entry == m_map_data.end())
    {
        Query(id);
        m_map_data.insert(std::make_pair(id, static_cast<std::uint32_t>(get_dword_time())));
        return;
    }

    const auto elapsed = static_cast<std::uint32_t>(time(0)) - entry->second;
    if (static_cast<std::int32_t>(elapsed) > m_iExpireTime)
        Query(id);
}

bool PlayerHB::Query(DWORD id)
{
    time_t now = time(0);
    const struct tm local = *localtime(&now);
    char tableName[64];
    dbsql::snprintf(tableName, sizeof(tableName), "hb_%02d%02d%02d%02d_player%s",
             local.tm_year - 100, local.tm_mon + 1, local.tm_mday,
             local.tm_hour, GetTablePostfix());

    char query[4096];
    if (m_stTableName.compare(tableName))
    {
        char prefix[64];
        dbsql::snprintf(prefix, sizeof(prefix), "CREATE TABLE `player%s`", GetTablePostfix());
        if (!m_stCreateTableQuery.starts_with(prefix))
        {
            sys_err("cannot find %s ", prefix);
            return false;
        }

        dbsql::snprintf(query, sizeof(query), "CREATE TABLE IF NOT EXISTS %s%s",
                 tableName, m_stCreateTableQuery.c_str() + strlen(prefix));
        SQLMsg* message = CDBManager::instance().DirectQuery(query, SQL_HOTBACKUP);
        m_stTableName = tableName;
        delete message;
    }

    dbsql::snprintf(query, 256, "REPLACE INTO %s SELECT * FROM %splayer%s WHERE id=%lu",
             m_stTableName.c_str(), GetPlayerDBName(), GetTablePostfix(),
             static_cast<unsigned long>(id));
    CDBManager::instance().AsyncQuery(query, SQL_HOTBACKUP);
    return true;
}
