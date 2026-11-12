#include "stdafx.h"
#include "NumericInput.h"
#include "SqlFormat.h"
#include "SqlInput.h"
#include "Cache.h"
#include "ClientManager.h"
#include "DBManager.h"
#include "LoginData.h"
#include "Main.h"
#include "Peer.h"
#include "QID.h"
#include <memory>

extern std::string g_stLocale;
extern int g_test_server;
extern int g_log;

namespace
{
template <size_t Size>
void CopyLoginText(char (&dest)[Size], const char* source)
{
    strncpy(dest, source, Size - 1);
    dest[Size - 1] = '\0';
}
}

bool CClientManager::InsertLogonAccount(const char* login, DWORD handle, const char* ip)
{
    char normalizedLogin[LOGIN_MAX_LEN + 1];
    trim_and_lower(normalizedLogin, login, LOGIN_MAX_LEN);
    if (m_map_kLogonAccount.find(normalizedLogin) != m_map_kLogonAccount.end())
        return false;

    CLoginData* data = GetLoginDataByLogin(login);
    if (!data)
        return false;

    data->SetConnectedPeerHandle(handle);
    data->SetIP(ip);
    m_map_kLogonAccount.insert(TLogonAccountMap::value_type(normalizedLogin, data));
    return true;
}

bool CClientManager::DeleteLogonAccount(const char* login, DWORD handle)
{
    char normalizedLogin[LOGIN_MAX_LEN + 1];
    trim_and_lower(normalizedLogin, login, LOGIN_MAX_LEN);
    TLogonAccountMap::iterator it = m_map_kLogonAccount.find(normalizedLogin);
    if (it == m_map_kLogonAccount.end())
        return false;

    CLoginData* data = it->second;
    if (data->GetConnectedPeerHandle() != handle)
    {
        sys_err("%s tried to logout in other peer handle %lu, current handle %lu",
                normalizedLogin, handle, data->GetConnectedPeerHandle());
        return false;
    }

    if (data->IsPlay())
    {
        data->SetPlay(false);
        SendLoginToBilling(data, false);
    }

    data->SetConnectedPeerHandle(0);
    if (data->IsDeleted())
        delete data;

    m_map_kLogonAccount.erase(it);
    return true;
}

bool CClientManager::FindLogonAccount(const char* login)
{
    char normalizedLogin[LOGIN_MAX_LEN + 1];
    trim_and_lower(normalizedLogin, login, LOGIN_MAX_LEN);
    return m_map_kLogonAccount.find(normalizedLogin) != m_map_kLogonAccount.end();
}

void CClientManager::QUERY_LOGIN_BY_KEY(CPeer* peer, DWORD handle, TPacketGDLoginByKey* packet)
{
    CLoginData* login = GetLoginData(packet->dwLoginKey);
    if (!login)
    {
        sys_log(0, "LOGIN_BY_KEY key not found");
        peer->EncodeReturn(HEADER_DG_LOGIN_NOT_EXIST, handle);
        return;
    }

    TAccountTable& original = login->GetRef();
    if (FindLogonAccount(original.login))
    {
        sys_log(0, "LOGIN_BY_KEY already logged in, account %lu", static_cast<unsigned long>(original.id));
        TPacketDGLoginAlready reply;
        CopyLoginText(reply.szLogin, packet->szLogin);
        peer->EncodeHeader(HEADER_DG_LOGIN_ALREADY, handle, sizeof(reply));
        peer->Encode(&reply, sizeof(reply));
        return;
    }

    if (strcasecmp(original.login, packet->szLogin) != 0)
    {
        sys_log(0, "LOGIN_BY_KEY login mismatch");
        peer->EncodeReturn(HEADER_DG_LOGIN_NOT_EXIST, handle);
        return;
    }

    const DWORD* clientKey = login->GetClientKey();
    DWORD keyDifference = 0;
    for (int i = 0; i < 4; ++i)
        keyDifference |= clientKey[i] ^ packet->adwClientKey[i];
    if (keyDifference)
    {
        sys_log(0, "LOGIN_BY_KEY client key mismatch for account %lu", static_cast<unsigned long>(original.id));
        peer->EncodeReturn(HEADER_DG_LOGIN_NOT_EXIST, handle);
        return;
    }

    TAccountTable* account = new TAccountTable;
    memset(account, 0, sizeof(*account));
    account->id = original.id;
    trim_and_lower(account->login, original.login, LOGIN_MAX_LEN);
    CopyLoginText(account->passwd, original.passwd);
    CopyLoginText(account->social_id, original.social_id);
    CopyLoginText(account->status, "OK");

    ClientHandleInfo* info = new ClientHandleInfo(handle);
    info->pAccountTable = account;
    CopyLoginText(info->ip, packet->szIP);
    sys_log(0, "LOGIN_BY_KEY success, account %lu", static_cast<unsigned long>(original.id));

    char query[QUERY_MAX_LEN];
    dbsql::snprintf(query, sizeof(query),
             "SELECT pid1, pid2, pid3, pid4, empire FROM player_index%s WHERE id=%lu",
             GetTablePostfix(), static_cast<unsigned long>(original.id));
    CDBManager::instance().ReturnQuery(query, QID_LOGIN_BY_KEY, peer->GetHandle(), info);
}

void CClientManager::RESULT_LOGIN_BY_KEY(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    ClientHandleInfo* info = static_cast<ClientHandleInfo*>(queryInfo->pvData);
    if (message->uiSQLErrno != 0)
    {
        peer->EncodeReturn(HEADER_DG_LOGIN_NOT_EXIST, info->dwHandle);
        delete info;
        return;
    }

    char query[QUERY_MAX_LEN];
    if (message->Get()->iNumRows == 0)
    {
        dbsql::snprintf(query, sizeof(query), "INSERT IGNORE INTO player_index%s (id) VALUES(%lu)",
                 GetTablePostfix(), static_cast<unsigned long>(info->pAccountTable->id));
        CDBManager::instance().ReturnQuery(query, QID_PLAYER_INDEX_CREATE, peer->GetHandle(), info);
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
    for (int column = 0; column < PLAYER_PER_ACCOUNT; ++column)
        info->pAccountTable->players[column].dwID = atoi(row[column]);
    info->pAccountTable->bEmpire = dbinput::ReadInteger<decltype(info->pAccountTable->bEmpire)>(row[PLAYER_PER_ACCOUNT]);
    info->account_index = 1;

    if (g_stLocale == "gb2312")
    {
        dbsql::snprintf(query, sizeof(query),
                 "SELECT id, name, job, level, alignment, st, ht, dx, iq, part_main, part_hair, x, y, skill_group, change_name FROM player%s WHERE account_id=%lu",
                 GetTablePostfix(), static_cast<unsigned long>(info->pAccountTable->id));
    }
    else
    {
        dbsql::snprintf(query, sizeof(query),
                 "SELECT id, name, job, level, playtime, st, ht, dx, iq, part_main, part_hair, x, y, skill_group, change_name FROM player%s WHERE account_id=%lu",
                 GetTablePostfix(), static_cast<unsigned long>(info->pAccountTable->id));
    }
    CDBManager::instance().ReturnQuery(query, QID_LOGIN, peer->GetHandle(), info);
}

void CClientManager::RESULT_PLAYER_INDEX_CREATE(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    ClientHandleInfo* info = static_cast<ClientHandleInfo*>(queryInfo->pvData);
    char query[QUERY_MAX_LEN];
    dbsql::snprintf(query, sizeof(query),
             "SELECT pid1, pid2, pid3, pid4, empire FROM player_index%s WHERE id=%lu",
             GetTablePostfix(), static_cast<unsigned long>(info->pAccountTable->id));
    CDBManager::instance().ReturnQuery(query, QID_LOGIN_BY_KEY, peer->GetHandle(), info);
}

void CClientManager::QUERY_LOGOUT(CPeer* peer, DWORD, const char* payload)
{
    const TLogoutPacket* packet = reinterpret_cast<const TLogoutPacket*>(payload);
    if (!packet->login[0])
        return;

    char normalized[LOGIN_MAX_LEN + 1];
    trim_and_lower(normalized, packet->login, LOGIN_MAX_LEN);
    const auto active = m_map_kLogonAccount.find(normalized);
    if (active == m_map_kLogonAccount.end() ||
        active->second->GetConnectedPeerHandle() != peer->GetHandle())
        return;

    CLoginData* login = active->second;
    for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
    {
        int playerId = static_cast<int>(login->GetRef().players[i].dwID);
        if (!playerId)
        {
            if (g_test_server)
                sys_log(0, "LOGOUT %s %d", packet->login, playerId);
            continue;
        }

        if (g_log)
            sys_log(0, "LOGOUT InsertLogoutPlayer %s %d", packet->login, playerId);
        InsertLogoutPlayer(playerId);
    }

    if (DeleteLogonAccount(packet->login, peer->GetHandle()) && g_log)
        sys_log(0, "LOGOUT %s ", packet->login);
}

TAccountTable* CreateAccountTableFromRes(MYSQL_RES* result)
{
    MYSQL_ROW row = mysql_fetch_row(result);
    TAccountTable* account = new TAccountTable;
    memset(account, 0, sizeof(*account));

    int column = 0;
    char inputPassword[sizeof(account->passwd)]{};
    CopyLoginText(inputPassword, row[column++]);
    account->id = atoi(row[column++]);
    CopyLoginText(account->login, row[column++]);
    CopyLoginText(account->passwd, row[column++]);
    CopyLoginText(account->social_id, row[column++]);
    account->bEmpire = dbinput::ReadInteger<decltype(account->bEmpire)>(row[column++]);
    for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
        account->players[i].dwID = atoi(row[column++]);
    CopyLoginText(account->status, row[column]);

    if (strncmp(account->passwd, inputPassword, sizeof(inputPassword)) != 0)
    {
        delete account;
        return NULL;
    }
    return account;
}

void CreateAccountPlayerDataFromRes(MYSQL_RES* result, TAccountTable* account)
{
    if (!result)
        return;

    for (DWORD i = 0; i < mysql_num_rows(result); ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        DWORD playerId = row[0] ? atoi(row[0]) : 0;
        if (!playerId)
            continue;

        for (int j = 0; j < PLAYER_PER_ACCOUNT; ++j)
        {
            TSimplePlayer& player = account->players[j];
            if (player.dwID != playerId)
                continue;

            CPlayerTableCache* cache = CClientManager::instance().GetPlayerCache(playerId);
            TPlayerTable* cached = cache ? cache->Get(false) : NULL;
            if (cached)
            {
                CopyLoginText(player.szName, cached->name);
                player.byJob = dbinput::IntegerOrZero<decltype(player.byJob)>(cached->job);
                player.byLevel = cached->level;
                player.dwPlayMinutes = cached->playtime;
                player.byST = dbinput::IntegerOrZero<decltype(player.byST)>(cached->st);
                player.byHT = dbinput::IntegerOrZero<decltype(player.byHT)>(cached->ht);
                player.byDX = dbinput::IntegerOrZero<decltype(player.byDX)>(cached->dx);
                player.byIQ = dbinput::IntegerOrZero<decltype(player.byIQ)>(cached->iq);
                player.wMainPart = cached->parts[PART_MAIN];
                player.wHairPart = cached->parts[PART_HAIR];
                player.x = cached->x;
                player.y = cached->y;
                player.skill_group = cached->skill_group;
                player.bChangeName = 0;
            }
            else
            {
                if (row[1])
                    CopyLoginText(player.szName, row[1]);
                else
                    player.szName[0] = '\0';

                player.byJob = dbinput::ReadInteger<decltype(player.byJob)>(row[2]);
                player.byLevel = dbinput::ReadInteger<decltype(player.byLevel)>(row[3]);
                player.dwPlayMinutes = row[4] ? atoi(row[4]) : 0;
                player.byST = dbinput::ReadInteger<decltype(player.byST)>(row[5]);
                player.byHT = dbinput::ReadInteger<decltype(player.byHT)>(row[6]);
                player.byDX = dbinput::ReadInteger<decltype(player.byDX)>(row[7]);
                player.byIQ = dbinput::ReadInteger<decltype(player.byIQ)>(row[8]);
                player.wMainPart = dbinput::ReadInteger<decltype(player.wMainPart)>(row[9]);
                player.wHairPart = dbinput::ReadInteger<decltype(player.wHairPart)>(row[10]);
                player.x = row[11] ? atoi(row[11]) : 0;
                player.y = row[12] ? atoi(row[12]) : 0;
                player.skill_group = dbinput::ReadInteger<decltype(player.skill_group)>(row[13]);
                player.bChangeName = dbinput::ReadInteger<decltype(player.bChangeName)>(row[14]);
            }
            sys_log(0, "%s %lu %lu hair %u", player.szName,
                    static_cast<unsigned long>(player.x), static_cast<unsigned long>(player.y),
                    static_cast<unsigned int>(player.wHairPart));
            break;
        }
    }
}

void CClientManager::RESULT_LOGIN(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    ClientHandleInfo* info = static_cast<ClientHandleInfo*>(queryInfo->pvData);
    if (info->account_index == 0)
    {
        if (message->Get()->iNumRows == 0)
        {
            sys_log(0, "RESULT_LOGIN: no account");
            peer->EncodeHeader(HEADER_DG_LOGIN_NOT_EXIST, info->dwHandle, 0);
            delete info;
            return;
        }

        info->pAccountTable = CreateAccountTableFromRes(message->Get()->pSQLResult);
        if (!info->pAccountTable)
        {
            sys_log(0, "RESULT_LOGIN: no account : WRONG_PASSWD");
            peer->EncodeReturn(HEADER_DG_LOGIN_WRONG_PASSWD, info->dwHandle);
            delete info;
            return;
        }

        ++info->account_index;
        char query[512];
        if (g_stLocale == "gb2312")
        {
            dbsql::snprintf(query, sizeof(query),
                     "SELECT id, name, job, level, alignment, st, ht, dx, iq, part_main, part_hair, x, y, skill_group, change_name FROM player%s WHERE account_id=%lu",
                     GetTablePostfix(), static_cast<unsigned long>(info->pAccountTable->id));
        }
        else
        {
            dbsql::snprintf(query, sizeof(query),
                     "SELECT id, name, job, level, playtime, st, ht, dx, iq, part_main, part_hair, x, y, skill_group, change_name FROM player%s WHERE account_id=%lu",
                     GetTablePostfix(), static_cast<unsigned long>(info->pAccountTable->id));
        }
        CDBManager::instance().ReturnQuery(query, QID_LOGIN, peer->GetHandle(), info);
        return;
    }

    TAccountTable* account = info->pAccountTable;
    if (!account)
    {
        peer->EncodeReturn(HEADER_DG_LOGIN_WRONG_PASSWD, info->dwHandle);
        delete info;
        return;
    }

    if (!InsertLogonAccount(account->login, peer->GetHandle(), info->ip))
    {
        sys_log(0, "RESULT_LOGIN: already logon %s", account->login);
        TPacketDGLoginAlready reply;
        CopyLoginText(reply.szLogin, account->login);
        peer->EncodeHeader(HEADER_DG_LOGIN_ALREADY, info->dwHandle, sizeof(reply));
        peer->Encode(&reply, sizeof(reply));
    }
    else
    {
        const int rowCount = message->Get()->iNumRows;
        sys_log(0, "RESULT_LOGIN: login success %s rows: %d", account->login, rowCount);
        if (rowCount > 0)
            CreateAccountPlayerDataFromRes(message->Get()->pSQLResult, account);

        CLoginData* data = GetLoginDataByLogin(account->login);
        memcpy(&data->GetRef(), account, sizeof(*account));
        peer->EncodeHeader(HEADER_DG_LOGIN_SUCCESS, info->dwHandle, sizeof(*account));
        peer->Encode(account, sizeof(*account));
    }

    delete account;
    info->pAccountTable = NULL;
    delete info;
}

void CClientManager::QUERY_CHANGE_NAME(CPeer* peer, DWORD handle, TPacketGDChangeName* packet)
{
    const auto name = dbsql::EscapeField(packet->name);
    if (!name)
    {
        peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, handle, 0);
        return;
    }

    char query[QUERY_MAX_LEN];
    if (g_stLocale == "sjis")
    {
        dbsql::snprintf(query, sizeof(query),
                 "SELECT COUNT(*) as count FROM player%s WHERE name='%s' collate sjis_japanese_ci AND id <> %lu",
                 GetTablePostfix(), *name, static_cast<unsigned long>(packet->pid));
    }
    else
    {
        dbsql::snprintf(query, sizeof(query),
                 "SELECT COUNT(*) as count FROM player%s WHERE name='%s' AND id <> %lu",
                 GetTablePostfix(), *name, static_cast<unsigned long>(packet->pid));
    }

    std::unique_ptr<SQLMsg> count(CDBManager::instance().DirectQuery(query, SQL_PLAYER));
    if (!count->Get()->iNumRows || !count->Get()->pSQLResult)
    {
        peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, handle, 0);
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(count->Get()->pSQLResult);
    if (row[0][0] != '0')
    {
        peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, handle, 0);
        return;
    }

    dbsql::snprintf(query, sizeof(query), "UPDATE player%s SET name='%s',change_name=0 WHERE id=%lu",
             GetTablePostfix(), *name, static_cast<unsigned long>(packet->pid));
    std::unique_ptr<SQLMsg> update(CDBManager::instance().DirectQuery(query, SQL_PLAYER));

    TPacketDGChangeName reply;
    peer->EncodeHeader(HEADER_DG_CHANGE_NAME, handle, sizeof(reply));
    reply.pid = packet->pid;
    CopyLoginText(reply.name, packet->name);
    peer->Encode(&reply, sizeof(reply));
}
