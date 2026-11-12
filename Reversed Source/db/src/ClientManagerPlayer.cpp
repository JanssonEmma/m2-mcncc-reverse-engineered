#include "stdafx.h"
#include "NumericInput.h"
#include "SqlFormat.h"
#include "SqlInput.h"
#include "ClientManager.h"
#include "Cache.h"
#include "DBManager.h"
#include "HB.h"
#include "Main.h"
#include "QID.h"

#include <memory>

extern bool g_bHotBackup;
extern std::string g_stLocale;
extern int g_test_server;
extern int g_log;
extern int g_iLogoutSeconds;

namespace
{
enum
{
    PlayerLoadSuccess = 35,
    PlayerLoadFailed = 36,
    PlayerCreateSuccess = 37,
    PlayerCreateAlready = 38,
    PlayerCreateFailed = 39,
    PlayerDeleteSuccess = 40,
    PlayerDeleteFailed = 41,
    ItemLoad = 42,
    QuestLoad = 44,
    AffectLoad = 50
};

std::string BlobLiteral(const void* data, std::size_t size)
{
    constexpr char hex[] = "0123456789abcdef";
    const auto* bytes = static_cast<const unsigned char*>(data);
    std::string literal = "X'";
    literal.reserve(size * 2 + 3);
    for (std::size_t i = 0; i < size; ++i)
    {
        literal.push_back(hex[bytes[i] >> 4]);
        literal.push_back(hex[bytes[i] & 0xf]);
    }
    literal.push_back('\'');
    return literal;
}
}

bool CreateItemTableFromRes(MYSQL_RES* result, std::vector<TPlayerItem>* items, DWORD pid)
{
    if (!result)
    {
        items->clear();
        return true;
    }

    int rows = static_cast<int>(mysql_num_rows(result));
    if (rows <= 0)
    {
        items->clear();
        return true;
    }

    items->resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        TPlayerItem& item = items->at(i);
        int col = 0;

        item.id = atoi(row[col++]);
        item.window = dbinput::ReadInteger<decltype(item.window)>(row[col++]);
        item.pos = dbinput::ReadInteger<decltype(item.pos)>(row[col++]);
        item.count = atoi(row[col++]);
        item.vnum = atoi(row[col++]);
        item.alSockets[0] = static_cast<std::int32_t>(std::strtoull(row[col++], nullptr, 10));
        item.alSockets[1] = static_cast<std::int32_t>(std::strtoull(row[col++], nullptr, 10));
        item.alSockets[2] = static_cast<std::int32_t>(std::strtoull(row[col++], nullptr, 10));

        for (int j = 0; j < 7; ++j)
        {
            item.aAttr[j].bType = dbinput::ReadInteger<std::remove_reference_t<decltype(item.aAttr[j].bType)>>(row[col++]);
            item.aAttr[j].sValue = dbinput::ReadInteger<std::remove_reference_t<decltype(item.aAttr[j].sValue)>>(row[col++]);
        }
        item.owner = pid;
    }

    return true;
}

std::optional<std::string> CreatePlayerSaveQuery(const TPlayerTable& player)
{
    const auto ip = dbsql::EscapeField(player.ip);
    if (!ip)
        return std::nullopt;

    std::string statement = fmt::sprintf(
            "UPDATE player%s SET "
            "job = %d, voice = %d, dir = %d, x = %ld, y = %ld, z = %ld, "
            "map_index = %ld, exit_x = %ld, exit_y = %ld, exit_map_index = %ld, "
            "hp = %d, mp = %d, stamina = %d, random_hp = %d, random_sp = %d, "
            "playtime = %d, level = %d, level_step = %d, st = %d, ht = %d, dx = %d, iq = %d, "
            "gold = %ld, exp = %ld, stat_point = %d, skill_point = %d, sub_skill_point = %d, "
            "stat_reset_count = %d, ip = '%s', part_main = %d, part_hair = %d, last_play = NOW(), "
            "skill_group = %d, alignment = %ld, horse_level = %d, horse_riding = %d, "
            "horse_hp = %d, horse_hp_droptime = %lu, horse_stamina = %d, horse_skill_point = %d, ",
            GetTablePostfix(), player.job, player.voice, player.dir,
            static_cast<long>(player.x), static_cast<long>(player.y), static_cast<long>(player.z),
            static_cast<long>(player.lMapIndex), static_cast<long>(player.lExitX),
            static_cast<long>(player.lExitY), static_cast<long>(player.lExitMapIndex),
            player.hp, player.sp, player.stamina, player.sRandomHP, player.sRandomSP,
            player.playtime, player.level, player.level_step, player.st, player.ht, player.dx,
            player.iq, static_cast<long>(player.gold),
            static_cast<long>(static_cast<std::int32_t>(player.exp)), player.stat_point,
            player.skill_point, player.sub_skill_point, player.stat_reset_count,
            *ip, player.parts[0], player.parts[3], player.skill_group, static_cast<long>(player.lAlignment),
            player.horse.bLevel, player.horse.bRiding, player.horse.sHealth,
            static_cast<unsigned long>(player.horse.dwHorseHealthDropTime), player.horse.sStamina,
            player.horse_skill_point);
    statement += fmt::sprintf("skill_level = %s, quickslot = %s WHERE id=%ld",
                             BlobLiteral(player.skills, sizeof(player.skills)),
                             BlobLiteral(player.quickslot, sizeof(player.quickslot)),
                             static_cast<long>(player.id));
    return statement;
}

CPlayerTableCache* CClientManager::GetPlayerCache(DWORD id)
{
    TPlayerTableCacheMap::iterator it = m_map_playerCache.find(id);
    if (it == m_map_playerCache.end())
        return NULL;

    CPlayerTableCache* entry = it->second;
    entry->Get(false)->logoff_interval = GetCurrentTime() - entry->GetLastUpdateTime();
    return entry;
}

void CClientManager::PutPlayerCache(TPlayerTable* player)
{
    CPlayerTableCache* entry = GetPlayerCache(player->id);
    if (!entry)
    {
        entry = new CPlayerTableCache;
        m_map_playerCache.insert(TPlayerTableCacheMap::value_type(player->id, entry));
    }

    if (g_bHotBackup)
        PlayerHB::instance().Put(player->id);

    entry->Put(player);
}

void CClientManager::QUERY_PLAYER_SAVE(CPeer*, DWORD, TPlayerTable* player)
{
    if (!dbsql::ReadField(player->ip, sizeof(player->ip)) ||
        !dbsql::ReadField(player->name, sizeof(player->name)))
        return;

    if (g_test_server)
        sys_log(0, "PLAYER_SAVE: %s", player->name);

    PutPlayerCache(player);
}

bool CreatePlayerTableFromRes(MYSQL_RES * res, TPlayerTable * pkTab)
{
	if (mysql_num_rows(res) == 0)
		return false;

	memset(pkTab, 0, sizeof(TPlayerTable));

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
		return false;
	const auto* lengths = mysql_fetch_lengths(res);
	if (!lengths)
		return false;

	int	col = 0;

	pkTab->id = atoi(row[col++]);
	strncpy(pkTab->name, row[col++], sizeof(pkTab->name) - 1);
	pkTab->name[sizeof(pkTab->name) - 1] = '\0';
	pkTab->job = dbinput::ReadInteger<decltype(pkTab->job)>(row[col++]);
	pkTab->voice = dbinput::ReadInteger<decltype(pkTab->voice)>(row[col++]);
	pkTab->dir = dbinput::ReadInteger<decltype(pkTab->dir)>(row[col++]);
	pkTab->x = atoi(row[col++]);
	pkTab->y = atoi(row[col++]);
	pkTab->z = atoi(row[col++]);
	pkTab->lMapIndex = atoi(row[col++]);
	pkTab->lExitX = atoi(row[col++]);
	pkTab->lExitY = atoi(row[col++]);
	pkTab->lExitMapIndex = atoi(row[col++]);
	pkTab->hp = dbinput::ReadInteger<decltype(pkTab->hp)>(row[col++]);
	pkTab->sp = dbinput::ReadInteger<decltype(pkTab->sp)>(row[col++]);
	pkTab->stamina = dbinput::ReadInteger<decltype(pkTab->stamina)>(row[col++]);
	pkTab->sRandomHP = dbinput::ReadInteger<decltype(pkTab->sRandomHP)>(row[col++]);
	pkTab->sRandomSP = dbinput::ReadInteger<decltype(pkTab->sRandomSP)>(row[col++]);
	pkTab->playtime = atoi(row[col++]);
	pkTab->gold = atoi(row[col++]);
	pkTab->level = dbinput::ReadInteger<decltype(pkTab->level)>(row[col++]);
	pkTab->level_step = dbinput::ReadInteger<decltype(pkTab->level_step)>(row[col++]);
	pkTab->st = dbinput::ReadInteger<decltype(pkTab->st)>(row[col++]);
	pkTab->ht = dbinput::ReadInteger<decltype(pkTab->ht)>(row[col++]);
	pkTab->dx = dbinput::ReadInteger<decltype(pkTab->dx)>(row[col++]);
	pkTab->iq = dbinput::ReadInteger<decltype(pkTab->iq)>(row[col++]);
	pkTab->exp = atoi(row[col++]);
	pkTab->stat_point = dbinput::ReadInteger<decltype(pkTab->stat_point)>(row[col++]);
	pkTab->skill_point = dbinput::ReadInteger<decltype(pkTab->skill_point)>(row[col++]);
	pkTab->sub_skill_point = dbinput::ReadInteger<decltype(pkTab->sub_skill_point)>(row[col++]);
	pkTab->stat_reset_count = dbinput::ReadInteger<decltype(pkTab->stat_reset_count)>(row[col++]);
	pkTab->part_base = dbinput::ReadInteger<decltype(pkTab->part_base)>(row[col++]);
	pkTab->parts[3] = dbinput::ReadInteger<std::remove_reference_t<decltype(pkTab->parts[3])>>(row[col++]);

	if (row[col])
		thecore_memcpy(pkTab->skills, row[col], std::min<std::size_t>(lengths[col], sizeof(pkTab->skills)));

	col++;

	if (row[col])
		thecore_memcpy(pkTab->quickslot, row[col], std::min<std::size_t>(lengths[col], sizeof(pkTab->quickslot)));

	col++;

	pkTab->skill_group = dbinput::ReadInteger<decltype(pkTab->skill_group)>(row[col++]);
	pkTab->lAlignment = atoi(row[col++]);

	if (row[col])
	{
		strncpy(pkTab->szMobile, row[col], sizeof(pkTab->szMobile) - 1);
		pkTab->szMobile[sizeof(pkTab->szMobile) - 1] = '\0';
	}

	col++;

	pkTab->horse.bLevel = dbinput::ReadInteger<decltype(pkTab->horse.bLevel)>(row[col++]);
	pkTab->horse.bRiding = dbinput::ReadInteger<decltype(pkTab->horse.bRiding)>(row[col++]);
	pkTab->horse.sHealth = dbinput::ReadInteger<decltype(pkTab->horse.sHealth)>(row[col++]);
	pkTab->horse.dwHorseHealthDropTime = atoi(row[col++]);
	pkTab->horse.sStamina = dbinput::ReadInteger<decltype(pkTab->horse.sStamina)>(row[col++]);
	pkTab->logoff_interval = atoi(row[col++]);
	pkTab->horse_skill_point = dbinput::ReadInteger<decltype(pkTab->horse_skill_point)>(row[col++]);
	return true;
}

void CClientManager::RESULT_COMPOSITE_PLAYER(CPeer* peer, SQLMsg* message, DWORD qid)
{
    CQueryInfo* query = static_cast<CQueryInfo*>(message->pvUserData);
    std::unique_ptr<ClientHandleInfo> info(static_cast<ClientHandleInfo*>(query->pvData));

    CLoginData* login = GetLoginDataByAID(info->account_id);
    if (!login || login->GetConnectedPeerHandle() != peer->GetHandle() ||
        (qid != QID_PLAYER && !login->IsPlay()))
        return;

    MYSQL_RES* result = message->Get() ? message->Get()->pSQLResult : nullptr;
    if (!result)
    {
        if (qid == QID_PLAYER)
            peer->EncodeReturn(PlayerLoadFailed, info->dwHandle);
        sys_err("null MYSQL_RES QID %lu", qid);
        return;
    }

    switch (qid)
    {
        case QID_PLAYER:
            sys_log(0, "QID_PLAYER %lu %lu", info->dwHandle, info->player_id);
            RESULT_PLAYER_LOAD(peer, result, info.get());
            break;

        case QID_ITEM:
            sys_log(0, "QID_ITEM %lu", info->dwHandle);
            RESULT_ITEM_LOAD(peer, result, info->dwHandle, info->player_id);
            break;

        case QID_QUEST:
            sys_log(0, "QID_QUEST %lu", info->dwHandle);
            RESULT_QUEST_LOAD(peer, result, info->dwHandle, info->player_id);
            break;

        case QID_AFFECT:
            sys_log(0, "QID_AFFECT %lu", info->dwHandle);
            RESULT_AFFECT_LOAD(peer, result, info->dwHandle);
            break;
    }
}

void CClientManager::RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo)
{
	TPlayerTable tab;

	if (!CreatePlayerTableFromRes(pRes, &tab))
	{
		peer->EncodeHeader(PlayerLoadFailed, pkInfo->dwHandle, 0);
		return;
	}

	CLoginData * pkLD = GetLoginDataByAID(pkInfo->account_id);

	if (!pkLD || pkLD->GetConnectedPeerHandle() != peer->GetHandle() || pkLD->IsPlay())
	{
		sys_log(0, "PLAYER_LOAD_ERROR: LoginData %p IsPlay %d", pkLD, pkLD ? pkLD->IsPlay() : 0);
		peer->EncodeHeader(PlayerLoadFailed, pkInfo->dwHandle, 0);
		return;
	}

	pkLD->SetPlay(true);
	SendLoginToBilling(pkLD, true);
	thecore_memcpy(tab.aiPremiumTimes, pkLD->GetPremiumPtr(), sizeof(tab.aiPremiumTimes));

	peer->EncodeHeader(PlayerLoadSuccess, pkInfo->dwHandle, sizeof(TPlayerTable));
	peer->Encode(&tab, sizeof(TPlayerTable));

}

void CClientManager::RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID)
{
	static std::vector<TPlayerItem> s_items;
	CreateItemTableFromRes(pRes, &s_items, dwPID);
	DWORD dwCount = s_items.size();

	const auto wireCount = static_cast<std::uint32_t>(dwCount);
	peer->EncodeHeader(ItemLoad, dwHandle, sizeof(wireCount) + sizeof(TPlayerItem) * dwCount);
	peer->Encode(&wireCount, sizeof(wireCount));

	CreateItemCacheSet(dwPID);

	sys_log(0, "ITEM_LOAD: count %lu pid %lu", dwCount, dwPID);

	if (dwCount)
	{
		peer->Encode(&s_items[0], sizeof(TPlayerItem) * dwCount);

		for (DWORD i = 0; i < dwCount; ++i)
			PutItemCache(&s_items[i], true);
	}
}

void CClientManager::RESULT_AFFECT_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle)
{
	const auto rowCount = mysql_num_rows(pRes);
	constexpr auto maxRows = ((std::numeric_limits<DWORD>::max)() - 2 * sizeof(DWORD)) / sizeof(TPacketAffectElement);
	if (rowCount > maxRows)
	{
		sys_err("SQL result exceeds the packet row limit");
		return;
	}
	const auto iNumRows = static_cast<size_t>(rowCount);

	if (iNumRows == 0)
		return;

	static std::vector<TPacketAffectElement> s_elements;
	s_elements.resize(iNumRows);

	DWORD dwPID = 0;

	MYSQL_ROW row;

	for (size_t i = 0; i < iNumRows; ++i)
	{
		TPacketAffectElement & r = s_elements[i];
		row = mysql_fetch_row(pRes);

		if (dwPID == 0)
			dwPID = atoi(row[0]);

		r.dwType = atoi(row[1]);
		r.bApplyOn = dbinput::ReadInteger<decltype(r.bApplyOn)>(row[2]);
		r.lApplyValue = atoi(row[3]);
		r.dwFlag = strtoul(row[4], NULL, 10);
		r.lDuration = atoi(row[5]);
		r.lSPCost = atoi(row[6]);
	}

	sys_log(0, "AFFECT_LOAD: count %d PID %lu", static_cast<int>(s_elements.size()), dwPID);

	const auto wirePID = static_cast<std::uint32_t>(dwPID);
	const auto dwCount = static_cast<std::uint32_t>(s_elements.size());

	peer->EncodeHeader(AffectLoad, dwHandle, sizeof(std::uint32_t) + sizeof(std::uint32_t) + sizeof(TPacketAffectElement) * dwCount);
	peer->Encode(&wirePID, sizeof(std::uint32_t));
	peer->Encode(&dwCount, sizeof(std::uint32_t));
	peer->Encode(&s_elements[0], sizeof(TPacketAffectElement) * dwCount);
}

void CClientManager::RESULT_QUEST_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD)
{
	const auto rowCount = mysql_num_rows(pRes);
	constexpr auto maxRows = ((std::numeric_limits<std::uint32_t>::max)() - sizeof(std::uint32_t)) / sizeof(TQuestTable);
	if (rowCount > maxRows)
	{
		sys_err("SQL result exceeds the packet row limit");
		return;
	}
	const auto iNumRows = static_cast<size_t>(rowCount);

	if (iNumRows == 0)
	{
		const std::uint32_t dwCount = 0;
		peer->EncodeHeader(QuestLoad, dwHandle, sizeof(dwCount));
		peer->Encode(&dwCount, sizeof(dwCount));
		return;
	}

	static std::vector<TQuestTable> s_table;
	s_table.resize(iNumRows);

	MYSQL_ROW row;

	for (size_t i = 0; i < iNumRows; ++i)
	{
		TQuestTable & r = s_table[i];

		row = mysql_fetch_row(pRes);

		r.dwPID = atoi(row[0]);
		strncpy(r.szName, row[1], sizeof(r.szName) - 1);
		r.szName[sizeof(r.szName) - 1] = '\0';
		strncpy(r.szState, row[2], sizeof(r.szState) - 1);
		r.szState[sizeof(r.szState) - 1] = '\0';
		r.lValue = atoi(row[3]);
	}

	sys_log(0, "QUEST_LOAD: count %d PID %lu", static_cast<int>(s_table.size()),
	        static_cast<unsigned long>(s_table[0].dwPID));

	const auto dwCount = static_cast<std::uint32_t>(s_table.size());

	peer->EncodeHeader(QuestLoad, dwHandle, sizeof(dwCount) + sizeof(TQuestTable) * dwCount);
	peer->Encode(&dwCount, sizeof(dwCount));
	peer->Encode(&s_table[0], sizeof(TQuestTable) * dwCount);
}

void CClientManager::QUERY_ADD_AFFECT(CPeer*, TPacketGDAddAffect * p)
{
	char queryStr[8192] = {};
	dbsql::snprintf(queryStr, sizeof(queryStr),
			"REPLACE INTO affect%s (dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost) "
			"VALUES(%u, %u, %u, %d, %u, %d, %d)",
			GetTablePostfix(),
			p->dwPID,
			p->elem.dwType,
			p->elem.bApplyOn,
			p->elem.lApplyValue,
			p->elem.dwFlag,
			p->elem.lDuration,
			p->elem.lSPCost);

	CDBManager::instance().AsyncQuery(queryStr);
}

void CClientManager::QUERY_REMOVE_AFFECT(CPeer*, TPacketGDRemoveAffect * p)
{
	char queryStr[8192] = {};

	dbsql::snprintf(queryStr, sizeof(queryStr),
			"DELETE FROM affect%s WHERE dwPID=%u AND bType=%u AND bApplyOn=%u",
			GetTablePostfix(), p->dwPID, p->dwType, p->bApplyOn);

	CDBManager::instance().AsyncQuery(queryStr);
}

void CClientManager::QUERY_PLAYER_LOAD(CPeer* peer, DWORD handle, TPlayerLoadPacket* packet)
{
    CLoginData* login = GetLoginDataByAID(packet->account_id);
    if (!login || login->GetConnectedPeerHandle() != peer->GetHandle() || login->IsPlay() ||
        !packet->player_id || packet->account_index >= PLAYER_PER_ACCOUNT)
    {
        peer->EncodeReturn(PlayerLoadFailed, handle);
        return;
    }

    // Reconnected game peers do not send character slots in the setup packet.
    // Verify a cache miss against the current account slot before reading any character data.
    const auto makeQueryInfo = [handle, packet]
    {
        auto* info = new ClientHandleInfo(handle, packet->player_id);
        info->account_id = packet->account_id;
        return info;
    };
    TSimplePlayer& slot = login->GetRef().players[packet->account_index];
    if (slot.dwID != packet->player_id)
    {
        char ownershipQuery[512];
        dbsql::snprintf(ownershipQuery, sizeof(ownershipQuery),
                "SELECT p.id FROM player_index%s AS i JOIN player%s AS p ON p.id=i.pid%u "
                "WHERE i.id=%lu AND p.account_id=i.id AND p.id=%lu",
                GetTablePostfix(), GetTablePostfix(), packet->account_index + 1U,
                static_cast<unsigned long>(packet->account_id), static_cast<unsigned long>(packet->player_id));
        std::unique_ptr<SQLMsg> ownership(CDBManager::instance().DirectQuery(ownershipQuery));
        if (!ownership || ownership->uiSQLErrno != 0 || !ownership->Get() || !ownership->Get()->iNumRows)
        {
            peer->EncodeReturn(PlayerLoadFailed, handle);
            return;
        }
        slot.dwID = packet->player_id;
    }

    for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
    {
        DWORD playerID = login->GetRef().players[i].dwID;
        if (playerID)
        {
            DeleteLogoutPlayer(static_cast<int>(playerID));
            sys_log(0, "LOGIN : DeleteLogoutPlayer aid(%d) pid(%d)", packet->account_id, playerID);
        }
        else
        {
            sys_log(0, "LOGIN : PLAYER_LOAD : aid(%d) pid(%d)", packet->account_id, playerID);
        }
    }

    if (g_log)
        sys_log(0, "PLAYER_LOAD : Delete Logout player pid(%d) account(%d) account_index(%d) ",
                packet->player_id, packet->account_id, packet->account_index);

    CPlayerTableCache* cache = GetPlayerCache(packet->player_id);
    if (cache)
    {
        TPlayerTable* player = cache->Get();
        login->SetPlay(true);
        SendLoginToBilling(login, true);
        thecore_memcpy(player->aiPremiumTimes, login->GetPremiumPtr(), sizeof(player->aiPremiumTimes));
        peer->EncodeHeader(PlayerLoadSuccess, handle, sizeof(TPlayerTable));
        peer->Encode(player, sizeof(TPlayerTable));

        char query[1024] = {};
        TItemCacheSet* itemCache = GetItemCacheSet(player->id);
        sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %d ", player->name,
                static_cast<int>(player->id), player->gold);

        if (itemCache)
        {
            static std::vector<TPlayerItem> items;
            items.resize(itemCache->size());

            DWORD count = 0;
            for (TItemCacheSet::iterator it = itemCache->begin(); it != itemCache->end(); ++it)
            {
                TPlayerItem* item = (*it)->Get();
                if (item->vnum)
                    thecore_memcpy(&items[count++], item, sizeof(TPlayerItem));
            }

            if (g_test_server)
                sys_log(0, "ITEM_CACHE: HIT! %s count: %lu", player->name, count);

            const auto wireCount = static_cast<std::uint32_t>(count);
            peer->EncodeHeader(ItemLoad, handle, sizeof(wireCount) + sizeof(TPlayerItem) * count);
            peer->Encode(&wireCount, sizeof(wireCount));
            if (count)
                peer->Encode(&items[0], sizeof(TPlayerItem) * count);

            dbsql::sprintf(query, "SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%ld AND lValue<>0",
                    GetTablePostfix(), static_cast<long>(player->id));
            CDBManager::instance().ReturnQuery(query, QID_QUEST, peer->GetHandle(), makeQueryInfo());

            dbsql::sprintf(query, "SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%ld",
                    GetTablePostfix(), static_cast<long>(player->id));
            CDBManager::instance().ReturnQuery(query, QID_AFFECT, peer->GetHandle(), makeQueryInfo());
        }
        else
        {
            dbsql::sprintf(query,
                    "SELECT id,`window`+0,pos,count,vnum,socket0,socket1,socket2,attrtype0,attrvalue0,attrtype1,attrvalue1,"
                    "attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 "
                    "FROM item%s WHERE owner_id=%ld AND `window` < 'SAFEBOX'",
                    GetTablePostfix(), static_cast<long>(player->id));
            CDBManager::instance().ReturnQuery(query, QID_ITEM, peer->GetHandle(), makeQueryInfo());

            dbsql::sprintf(query, "SELECT dwPID, szName, szState, lValue FROM quest%s WHERE dwPID=%ld",
                    GetTablePostfix(), static_cast<long>(player->id));
            CDBManager::instance().ReturnQuery(query, QID_QUEST, peer->GetHandle(), makeQueryInfo());

            dbsql::sprintf(query, "SELECT dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost FROM affect%s WHERE dwPID=%ld",
                    GetTablePostfix(), static_cast<long>(player->id));
            CDBManager::instance().ReturnQuery(query, QID_AFFECT, peer->GetHandle(), makeQueryInfo());
        }
        return;
    }

    sys_log(0, "[PLAYER_LOAD] Load from PlayerDB pid[%d]", packet->player_id);
    char query[8192];
    query[0] = '\0';
    dbsql::snprintf(query, sizeof(query),
            "SELECT id,name,job,voice,dir,x,y,z,map_index,exit_x,exit_y,exit_map_index,hp,mp,stamina,random_hp,random_sp,playtime,"
            "gold,level,level_step,st,ht,dx,iq,exp,stat_point,skill_point,sub_skill_point,stat_reset_count,part_base,part_hair,"
            "skill_level,quickslot,skill_group,alignment,mobile,horse_level,horse_riding,horse_hp,horse_hp_droptime,horse_stamina,"
            "UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(last_play),horse_skill_point FROM player%s WHERE id=%lu AND account_id=%lu",
            GetTablePostfix(), static_cast<unsigned long>(packet->player_id),
            static_cast<unsigned long>(packet->account_id));

    CDBManager::instance().ReturnQuery(query, QID_PLAYER, peer->GetHandle(), makeQueryInfo());

    dbsql::snprintf(query, sizeof(query),
            "SELECT id,`window`+0,pos,count,vnum,socket0,socket1,socket2,attrtype0,attrvalue0,attrtype1,attrvalue1,"
            "attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6  "
            "FROM item%s WHERE owner_id=%ld AND `window` < 'SAFEBOX'",
            GetTablePostfix(), static_cast<long>(packet->player_id));
    CDBManager::instance().ReturnQuery(query, QID_ITEM, peer->GetHandle(), makeQueryInfo());

    dbsql::snprintf(query, sizeof(query), "SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%ld",
            GetTablePostfix(), static_cast<long>(packet->player_id));
    CDBManager::instance().ReturnQuery(query, QID_QUEST, peer->GetHandle(), makeQueryInfo());

    dbsql::snprintf(query, sizeof(query), "SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%ld",
            GetTablePostfix(), static_cast<long>(packet->player_id));
    CDBManager::instance().ReturnQuery(query, QID_AFFECT, peer->GetHandle(), makeQueryInfo());
}

void CClientManager::InsertLogoutPlayer(int pid)
{
    std::map<int, tag_LogoutPlayerInfo*>::iterator it = m_map_logout.find(pid);
    if (it != m_map_logout.end())
    {
        if (g_log)
            sys_log(0, "LOGOUT : Update player time pid(%d)", pid);

        it->second->time = time(NULL);
        return;
    }

    tag_LogoutPlayerInfo* entry = new tag_LogoutPlayerInfo;
    entry->pid = pid;
    entry->time = time(NULL);
    m_map_logout.insert(std::make_pair(pid, entry));

    if (g_log)
        sys_log(0, "LOGOUT : Insert player pid(%d)", pid);
}

void CClientManager::DeleteLogoutPlayer(int pid)
{
    std::map<int, tag_LogoutPlayerInfo*>::iterator it = m_map_logout.find(pid);
    if (it == m_map_logout.end())
        return;

    tag_LogoutPlayerInfo* entry = it->second;
    if (g_log)
        sys_log(0, "LOGIN : Delete Player pid(%d)", entry->pid);

    m_map_logout.erase(it);
    delete entry;
}

void CClientManager::UpdateLogoutPlayer()
{
    std::map<int, tag_LogoutPlayerInfo*>::iterator it = m_map_logout.begin();
    time_t now = time(NULL);

    while (it != m_map_logout.end())
    {
        tag_LogoutPlayerInfo* entry = (it++)->second;
        if (entry->time < now - g_iLogoutSeconds)
        {
            FlushItemCacheSet(entry->pid);
            FlushPlayerCacheSet(entry->pid);
            DeleteLogoutPlayer(entry->pid);
        }
    }
}

void CClientManager::FlushPlayerCacheSet(int pid)
{
    TPlayerTableCacheMap::iterator it = m_map_playerCache.find(pid);
    if (it == m_map_playerCache.end())
        return;

    CPlayerTableCache* entry = it->second;
    entry->Flush();
    m_map_playerCache.erase(it);
    delete entry;
}

void CClientManager::QUERY_HIGHSCORE_REGISTER(CPeer* peer, TPacketGDHighscore* data)
{
    const auto board = dbsql::EscapeField(data->szBoard);
    if (!board)
        return;

    const auto query = fmt::sprintf("SELECT value FROM highscore%s WHERE board='%s' AND pid = %u",
                                    GetTablePostfix(), *board, data->dwPID);
    sys_log(0, "HEADER_GD_HIGHSCORE_REGISTER: PID %u", data->dwPID);

    ClientHandleInfo* info = new ClientHandleInfo(0);
    strncpy(info->login, data->szBoard, sizeof(info->login) - 1);
    info->login[sizeof(info->login) - 1] = '\0';
    info->account_id = static_cast<DWORD>(data->lValue);
    info->player_id = data->dwPID;
    info->account_index = data->cDir > 0;

    CDBManager::instance().ReturnQuery(query.c_str(), QID_HIGHSCORE_REGISTER, peer->GetHandle(), info);
}

void CClientManager::RESULT_HIGHSCORE_REGISTER(CPeer*, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    std::unique_ptr<ClientHandleInfo> info(static_cast<ClientHandleInfo*>(queryInfo->pvData));

    char board[21];
    strncpy(board, info->login, sizeof(board) - 1);
    board[sizeof(board) - 1] = '\0';
    const auto escapedBoard = dbsql::EscapeField(board);
    if (!escapedBoard)
        return;
    const int value = static_cast<int>(info->account_id);

    const char* format = "INSERT INTO highscore%s VALUES('%s', %lu, %d)";
    SQLResult* result = message->Get();
    if (result->iNumRows > 0)
    {
        if (!result->pSQLResult)
            return;

        MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
        if (row && row[0])
        {
            const int current = atoi(row[0]);
            if (info->account_index ? current >= value : current <= value)
                return;

            format = "REPLACE INTO highscore%s VALUES('%s', %lu, %d)";
        }
    }

    const auto query = fmt::sprintf(format, GetTablePostfix(), *escapedBoard, info->player_id, value);
    CDBManager::instance().AsyncQuery(query.c_str());
}

void CClientManager::__QUERY_PLAYER_DELETE(CPeer* peer, DWORD handle, TPlayerDeletePacket* packet)
{
    if (!packet->login[0] || !packet->player_id || packet->account_index >= PLAYER_PER_ACCOUNT)
        return;

    CLoginData* login = GetLoginDataByLogin(packet->login);
    if (login && login->GetConnectedPeerHandle() == peer->GetHandle())
    {
        const TAccountTable& account = login->GetRef();
        bool accepted = g_stLocale == "sjis" || m_bChinaEventServer;
        if (!accepted)
        {
            const size_t length = strlen(account.social_id);
            if (length >= 7 && !strncmp(packet->private_code, account.social_id + length - 7, 7))
            {
                accepted = true;
                CPlayerTableCache* cache = GetPlayerCache(packet->player_id);
                if (cache)
                {
                    const TPlayerTable* player = cache->Get();
                    if (player->level >= m_iPlayerDeleteLevelLimit)
                    {
                        sys_log(0, "PLAYER_DELETE FAILED LEVEL %lu >= DELETE LIMIT %d",
                                static_cast<unsigned long>(player->level), m_iPlayerDeleteLevelLimit);
                        accepted = false;
                    }
                }
            }
            else
            {
                sys_log(0, "PLAYER_DELETE FAILED len(%d)", static_cast<int>(length));
            }
        }

        if (accepted)
        {
            char query[512];
            const unsigned int slot = packet->account_index + 1;
            dbsql::snprintf(query, sizeof(query),
                    "SELECT p.id, p.level FROM player_index%s AS i JOIN player%s AS p ON p.id=i.pid%u "
                    "WHERE i.id=%lu AND p.account_id=i.id AND p.id=%lu",
                    GetTablePostfix(), GetTablePostfix(), slot, static_cast<unsigned long>(account.id),
                    static_cast<unsigned long>(packet->player_id));
            ClientHandleInfo* info = new ClientHandleInfo(handle, packet->player_id);
            info->account_id = account.id;
            info->account_index = packet->account_index;

            sys_log(0, "PLAYER_DELETE TRY: %s %d pid%d", packet->login, packet->player_id, slot);
            CDBManager::instance().ReturnQuery(query, QID_PLAYER_DELETE, peer->GetHandle(), info);
            return;
        }
    }

    peer->EncodeHeader(PlayerDeleteFailed, handle, 1);
    peer->EncodeBYTE(packet->account_index);
}

void CClientManager::__RESULT_PLAYER_DELETE(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    ClientHandleInfo* info = static_cast<ClientHandleInfo*>(queryInfo->pvData);
    SQLResult* result = message->Get();
    CLoginData* login = GetLoginDataByAID(info->account_id);
    if (!login || login->GetConnectedPeerHandle() != peer->GetHandle() ||
        message->uiSQLErrno != 0 || !result || !result->pSQLResult || !result->iNumRows)
    {
        sys_log(0, "PLAYER_DELETE FAIL NO ROW");
        peer->EncodeHeader(PlayerDeleteFailed, info->dwHandle, 1);
        peer->EncodeBYTE(info->account_index);
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
    DWORD playerID = atoi(row[0]);
    int level = atoi(row[1]);
    if (level >= m_iPlayerDeleteLevelLimit && !m_bChinaEventServer)
    {
        sys_log(0, "PLAYER_DELETE FAILED LEVEL %lu >= DELETE LIMIT %d",
                static_cast<unsigned long>(level), m_iPlayerDeleteLevelLimit);
        peer->EncodeHeader(PlayerDeleteFailed, info->dwHandle, 1);
        peer->EncodeBYTE(info->account_index);
        return;
    }

    char query[8193];
    dbsql::snprintf(query, sizeof(query),
            "INSERT INTO player%s_deleted SELECT * FROM player%s WHERE id=%lu AND account_id=%lu",
            GetTablePostfix(), GetTablePostfix(), info->player_id, info->account_id);
    std::unique_ptr<SQLMsg> backup(CDBManager::instance().DirectQuery(query));
    if (!backup || backup->uiSQLErrno != 0 || !backup->Get() || backup->Get()->iAffectedRows < 1)
    {
        sys_log(0, "PLAYER_DELETE FAILED %lu CANNOT INSERT TO player%s_deleted", playerID, GetTablePostfix());
        peer->EncodeHeader(PlayerDeleteFailed, info->dwHandle, 1);
        peer->EncodeBYTE(info->account_index);
        return;
    }

    const unsigned int slot = info->account_index + 1;
    dbsql::snprintf(query, 8192, "UPDATE player_index%s SET pid%u=0 WHERE id=%lu AND pid%u=%lu",
            GetTablePostfix(), slot, info->account_id, slot, info->player_id);
    std::unique_ptr<SQLMsg> index(CDBManager::instance().DirectQuery(query));
    if (!index || index->uiSQLErrno != 0 || !index->Get() || index->Get()->iAffectedRows < 1)
    {
        sys_log(0, "PLAYER_DELETE FAIL WHEN UPDATE account table");
        peer->EncodeHeader(PlayerDeleteFailed, info->dwHandle, 1);
        peer->EncodeBYTE(info->account_index);
        return;
    }

    login->GetRef().players[info->account_index] = {};

    sys_log(0, "PLAYER_DELETE SUCCESS %lu", playerID);
    char accountIndex[16];
    dbsql::sprintf(accountIndex, "player_id%d", m_iPlayerIDStart + info->account_index);

    CPlayerTableCache* cache = GetPlayerCache(info->player_id);
    if (cache)
    {
        m_map_playerCache.erase(info->player_id);
        delete cache;
    }

    TItemCacheSet* items = GetItemCacheSet(info->player_id);
    if (items)
    {
        TItemCacheSet::iterator it = items->begin();
        while (it != items->end())
            DeleteItemCache((*it++)->Get()->id);

        items->clear();
        delete items;
        m_map_pkItemCacheSetPtr.erase(info->player_id);
    }

    const long pid = static_cast<long>(info->player_id);
    dbsql::snprintf(query, sizeof(query), "DELETE FROM player%s WHERE id=%lu AND account_id=%lu",
            GetTablePostfix(), info->player_id, info->account_id);
    delete CDBManager::instance().DirectQuery(query);
    dbsql::snprintf(query, 8192, "DELETE FROM item%s WHERE owner_id=%ld AND `window` < 'SAFEBOX'", GetTablePostfix(), pid);
    delete CDBManager::instance().DirectQuery(query);
    dbsql::snprintf(query, 8192, "DELETE FROM quest%s WHERE dwPID=%ld", GetTablePostfix(), pid);
    CDBManager::instance().AsyncQuery(query);
    dbsql::snprintf(query, 8192, "DELETE FROM affect%s WHERE dwPID=%ld", GetTablePostfix(), pid);
    CDBManager::instance().AsyncQuery(query);
    dbsql::snprintf(query, 8192, "DELETE FROM guild_member%s WHERE pid=%ld", GetTablePostfix(), pid);
    CDBManager::instance().AsyncQuery(query);
    dbsql::snprintf(query, 8192, "DELETE FROM myshop_pricelist%s WHERE owner_id=%ld", GetTablePostfix(), pid);
    CDBManager::instance().AsyncQuery(query);

    peer->EncodeHeader(PlayerDeleteSuccess, info->dwHandle, 1);
    peer->EncodeBYTE(info->account_index);
}

void CClientManager::__QUERY_PLAYER_CREATE(CPeer* peer, DWORD handle, TPlayerCreatePacket* packet)
{
    const TPlayerTable& player = packet->player_table;
    const auto name = dbsql::EscapeField(player.name);
    CLoginData* login = GetLoginDataByAID(packet->account_id);
    if (!login || login->GetConnectedPeerHandle() != peer->GetHandle() ||
        packet->account_index >= PLAYER_PER_ACCOUNT || !name)
    {
        peer->EncodeHeader(PlayerCreateFailed, handle, 0);
        return;
    }

    char query[8193];
    const unsigned int slot = packet->account_index + 1;
    const long accountID = static_cast<long>(packet->account_id);

    dbsql::snprintf(query, 8192, "SELECT pid%u FROM player_index%s WHERE id=%ld",
            slot, GetTablePostfix(), accountID);
    std::unique_ptr<SQLMsg> playerIndex(CDBManager::instance().DirectQuery(query));
    SQLResult* result = playerIndex->Get();
    if (!result->iNumRows || !result->pSQLResult)
    {
        peer->EncodeHeader(PlayerCreateFailed, handle, 0);
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
    if (row[0])
    {
        int existingID = atoi(row[0]);
        if (existingID > 0)
        {
            peer->EncodeHeader(PlayerCreateAlready, handle, 0);
            sys_log(0, "ALREADY EXIST AccountChrIdx %d ID %d", packet->account_index, existingID);
            return;
        }
    }

    const char* nameQuery = g_stLocale == "sjis"
            ? "SELECT COUNT(*) as count FROM player%s WHERE name='%s' collate sjis_japanese_ci"
            : "SELECT COUNT(*) as count FROM player%s WHERE name='%s'";
    dbsql::snprintf(query, 8192, nameQuery, GetTablePostfix(), *name);
    std::unique_ptr<SQLMsg> nameCount(CDBManager::instance().DirectQuery(query));
    result = nameCount->Get();
    if (!result->iNumRows || !result->pSQLResult)
    {
        peer->EncodeHeader(PlayerCreateFailed, handle, 0);
        return;
    }

    row = mysql_fetch_row(result->pSQLResult);
    if (row[0][0] != '0')
    {
        sys_log(0, "ALREADY EXIST name %s, row[0] %s query %s", player.name, row[0], query);
        peer->EncodeHeader(PlayerCreateAlready, handle, 0);
        return;
    }

    const auto insertQuery = fmt::sprintf(
            "INSERT INTO player%s (id, account_id, name, level, st, ht, dx, iq, job, voice, dir, x, y, z, "
            "hp, mp, random_hp, random_sp, stat_point, stamina, part_base, part_main, part_hair, gold, playtime, "
            "skill_level, quickslot) VALUES(0, %ld, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %ld, %ld, %ld, "
            "%d, %d, %d, %d, %d, %d, %d, %d, 0, %ld, 0, %s, %s)",
            GetTablePostfix(), accountID, *name, player.level, player.st, player.ht, player.dx,
            player.iq, player.job, player.voice, player.dir, static_cast<long>(player.x),
            static_cast<long>(player.y), static_cast<long>(player.z), player.hp,
            player.sp, player.sRandomHP, player.sRandomSP, player.stat_point, player.stamina,
            player.part_base, player.part_base, static_cast<long>(player.gold),
            BlobLiteral(player.skills, sizeof(player.skills)),
            BlobLiteral(player.quickslot, sizeof(player.quickslot)));
    sys_log(0, "PlayerCreate accountid %d name %s level %d gold %d, st %d ht %d job %d",
            static_cast<int>(packet->account_id), player.name, player.level, player.gold, player.st, player.ht, player.job);

    std::unique_ptr<SQLMsg> inserted(CDBManager::instance().DirectQuery(insertQuery.c_str()));
    if (g_test_server)
        sys_log(0, "Create_Player queryLen[%d]", static_cast<int>(insertQuery.size()));

    result = inserted->Get();
    if (result->iAffectedRows < 1)
    {
        peer->EncodeHeader(PlayerCreateAlready, handle, 0);
        sys_log(0, "ALREADY EXIST3 query: %s AffectedRows %d", insertQuery.c_str(), result->iAffectedRows);
        return;
    }

    const int playerID = result->iInsertID;
    dbsql::snprintf(query, sizeof(query),
            "UPDATE player_index%s SET pid%u=%d WHERE id=%ld AND (pid%u=0 OR pid%u IS NULL)",
            GetTablePostfix(), slot, playerID, accountID, slot, slot);
    std::unique_ptr<SQLMsg> index(CDBManager::instance().DirectQuery(query));
    if (!index || index->uiSQLErrno != 0 || !index->Get() || index->Get()->iAffectedRows < 1)
    {
        sys_err("QUERY_ERROR: %s", query);
        dbsql::snprintf(query, sizeof(query), "DELETE FROM player%s WHERE id=%d AND account_id=%ld",
                GetTablePostfix(), playerID, accountID);
        delete CDBManager::instance().DirectQuery(query);
        peer->EncodeHeader(PlayerCreateFailed, handle, 0);
        return;
    }

    TPacketDGCreateSuccess response;
    memset(&response, 0, sizeof(response));
    response.bAccountCharacterIndex = packet->account_index;
    response.player.dwID = playerID;
    strncpy(response.player.szName, player.name, sizeof(response.player.szName) - 1);
    response.player.szName[sizeof(response.player.szName) - 1] = '\0';
    response.player.byJob = dbinput::IntegerOrZero<decltype(response.player.byJob)>(player.job);
    response.player.byLevel = 1;
    response.player.byST = dbinput::IntegerOrZero<decltype(response.player.byST)>(player.st);
    response.player.byHT = dbinput::IntegerOrZero<decltype(response.player.byHT)>(player.ht);
    response.player.byDX = dbinput::IntegerOrZero<decltype(response.player.byDX)>(player.dx);
    response.player.byIQ = dbinput::IntegerOrZero<decltype(response.player.byIQ)>(player.iq);
    response.player.wMainPart = player.part_base;
    response.player.x = player.x;
    response.player.y = player.y;

    login->GetRef().players[packet->account_index] = response.player;
    peer->EncodeHeader(PlayerCreateSuccess, handle, sizeof(response));
    peer->Encode(&response, sizeof(response));
    sys_log(0, "7 name %s job %d", response.player.szName, response.player.byJob);
}
