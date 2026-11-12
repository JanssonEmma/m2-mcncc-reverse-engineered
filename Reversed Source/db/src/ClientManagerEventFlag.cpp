#include "stdafx.h"
#include "SqlFormat.h"
#include "SqlInput.h"
#include "ClientManager.h"
#include "DBManager.h"
#include "Main.h"
#include "Peer.h"

#include <memory>

void CClientManager::LoadEventFlag()
{
    char query[1025];
    dbsql::snprintf(query, 1024, "SELECT szName, lValue FROM quest%s WHERE dwPID = 0", GetTablePostfix());
    std::unique_ptr<SQLMsg> message(CDBManager::instance().DirectQuery(query));

    SQLResult* result = message->Get();
    if (!result->iNumRows)
        return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result->pSQLResult)))
    {
        TPacketSetEventFlag packet;
        strncpy(packet.szFlagName, row[0], 32);
        packet.szFlagName[32] = '\0';
        packet.lValue = atoi(row[1]);

        m_map_lEventFlag.insert(std::make_pair(std::string(packet.szFlagName), packet.lValue));
        ForwardPacket(HEADER_DG_SET_EVENT_FLAG, &packet, sizeof(packet), 0, NULL);
    }
}

void CClientManager::SetEventFlag(TPacketSetEventFlag* packet)
{
    const auto name = dbsql::EscapeField(packet->szFlagName);
    if (!name)
        return;

    ForwardPacket(HEADER_DG_SET_EVENT_FLAG, packet, sizeof(*packet), 0, NULL);

    TEventFlagMap::iterator entry = m_map_lEventFlag.find(packet->szFlagName);
    if (entry == m_map_lEventFlag.end())
    {
        m_map_lEventFlag.insert(std::make_pair(std::string(packet->szFlagName), packet->lValue));
    }
    else if (entry->second == packet->lValue)
        return;
    else
    {
        entry->second = packet->lValue;
    }

    char query[1025];
    dbsql::snprintf(query, 1024, "REPLACE INTO quest%s (dwPID, szName, szState, lValue) VALUES(0, '%s', '', %d)",
             GetTablePostfix(), *name, packet->lValue);
    query[1024] = '\0';
    CDBManager::instance().AsyncQuery(query);
}

void CClientManager::SendEventFlagsOnSetup(CPeer* peer)
{
    for (TEventFlagMap::iterator entry = m_map_lEventFlag.begin(); entry != m_map_lEventFlag.end(); ++entry)
    {
        TPacketSetEventFlag packet;
        strncpy(packet.szFlagName, entry->first.c_str(), 32);
        packet.szFlagName[32] = '\0';
        packet.lValue = entry->second;

        peer->EncodeHeader(HEADER_DG_SET_EVENT_FLAG, 0, sizeof(packet));
        peer->Encode(&packet, sizeof(packet));
    }
}
