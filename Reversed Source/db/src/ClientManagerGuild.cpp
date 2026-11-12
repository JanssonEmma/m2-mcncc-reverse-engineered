#include "stdafx.h"
#include "NumericInput.h"
#include "SqlFormat.h"
#include "ClientManager.h"
#include "Main.h"
#include "DBManager.h"
#include "GuildManager.h"
#include <memory>

void CClientManager::GuildCreate(CPeer*, DWORD dwGuildID)
{
	const auto guildId = static_cast<std::uint32_t>(dwGuildID);
	ForwardPacket(HEADER_DG_GUILD_LOAD, &guildId, sizeof(guildId));

	CGuildManager::instance().Load(dwGuildID);
}

void CClientManager::GuildChangeGrade(CPeer*, TPacketGuild* p)
{
	ForwardPacket(HEADER_DG_GUILD_CHANGE_GRADE, p, sizeof(TPacketGuild));
}

void CClientManager::GuildAddMember(CPeer*, TPacketGDGuildAddMember * p)
{
	CGuildManager::instance().TouchGuild(p->dwGuild);

	char szQuery[512];

	dbsql::snprintf(szQuery, sizeof(szQuery),
			"INSERT INTO guild_member%s VALUES(%lu, %lu, %d, 0, 0)",
			GetTablePostfix(), p->dwPID, p->dwGuild, p->bGrade);

	std::unique_ptr<SQLMsg> pmsg_insert(CDBManager::instance().DirectQuery(szQuery));

	dbsql::snprintf(szQuery, sizeof(szQuery),
			"SELECT pid, grade, is_general, offer, level, job, name FROM guild_member%s, player%s WHERE guild_id = %lu and pid = id and pid = %lu", GetTablePostfix(), GetTablePostfix(), static_cast<unsigned long>(p->dwGuild), static_cast<unsigned long>(p->dwPID));

	std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

	if (pmsg->Get()->iNumRows == 0)
	{
		sys_err("Query failed when getting guild member data %s", pmsg->stQuery.c_str());
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

	if (!row[0] || !row[1])
		return;

	TPacketDGGuildMember dg;

	dg.dwGuild = p->dwGuild;
	dg.dwPID = strtoul(row[0], NULL, 10);
	dg.bGrade = dbinput::ReadInteger<decltype(dg.bGrade)>(row[1]);
	dg.isGeneral = dbinput::ReadInteger<decltype(dg.isGeneral)>(row[2]);
	dg.dwOffer = strtoul(row[3], NULL, 10);
	dg.bLevel = dbinput::ReadInteger<decltype(dg.bLevel)>(row[4]);
	dg.bJob = dbinput::ReadInteger<decltype(dg.bJob)>(row[5]);
	strncpy(dg.szName, row[6], sizeof(dg.szName) - 1);
	dg.szName[sizeof(dg.szName) - 1] = '\0';

	ForwardPacket(HEADER_DG_GUILD_ADD_MEMBER, &dg, sizeof(TPacketDGGuildMember));
}

void CClientManager::GuildRemoveMember(CPeer*, TPacketGuild* p)
{
	char szQuery[512];
	dbsql::snprintf(szQuery, sizeof(szQuery), "DELETE FROM guild_member%s WHERE pid=%lu and guild_id=%lu", GetTablePostfix(), static_cast<unsigned long>(p->dwInfo), static_cast<unsigned long>(p->dwGuild));
	CDBManager::instance().AsyncQuery(szQuery);

	dbsql::snprintf(szQuery, sizeof(szQuery), "REPLACE INTO quest%s (dwPID, szName, szState, lValue) VALUES(%lu, 'guild_manage', 'withdraw_time', %lu)", GetTablePostfix(), static_cast<unsigned long>(p->dwInfo), (DWORD) GetCurrentTime());
	CDBManager::instance().AsyncQuery(szQuery);

	ForwardPacket(HEADER_DG_GUILD_REMOVE_MEMBER, p, sizeof(TPacketGuild));
}

void CClientManager::GuildSkillUpdate(CPeer*, TPacketGuildSkillUpdate* p)
{
	ForwardPacket(HEADER_DG_GUILD_SKILL_UPDATE, p, sizeof(TPacketGuildSkillUpdate));
}

void CClientManager::GuildExpUpdate(CPeer* peer, TPacketGuildExpUpdate* p)
{
	ForwardPacket(HEADER_DG_GUILD_EXP_UPDATE, p, sizeof(TPacketGuildExpUpdate), 0, peer);
}

void CClientManager::GuildChangeMemberData(CPeer* peer, TPacketGuildChangeMemberData* p)
{
	ForwardPacket(HEADER_DG_GUILD_CHANGE_MEMBER_DATA, p, sizeof(TPacketGuildChangeMemberData), 0, peer);
}

void CClientManager::GuildDisband(CPeer*, TPacketGuild* p)
{
	char szQuery[512];

	dbsql::snprintf(szQuery, sizeof(szQuery), "DELETE FROM guild%s WHERE id=%lu", GetTablePostfix(), static_cast<unsigned long>(p->dwGuild));
	CDBManager::instance().AsyncQuery(szQuery);

	dbsql::snprintf(szQuery, sizeof(szQuery), "DELETE FROM guild_grade%s WHERE guild_id=%lu", GetTablePostfix(), static_cast<unsigned long>(p->dwGuild));
	CDBManager::instance().AsyncQuery(szQuery);

	dbsql::snprintf(szQuery, sizeof(szQuery), "REPLACE INTO quest%s (dwPID, szName, szState, lValue) SELECT pid, 'guild_manage', 'withdraw_time', %lu FROM guild_member%s WHERE guild_id = %lu", GetTablePostfix(), (DWORD) GetCurrentTime(), GetTablePostfix(), static_cast<unsigned long>(p->dwGuild));
	CDBManager::instance().AsyncQuery(szQuery);

	dbsql::snprintf(szQuery, sizeof(szQuery), "DELETE FROM guild_member%s WHERE guild_id=%lu", GetTablePostfix(), static_cast<unsigned long>(p->dwGuild));
	CDBManager::instance().AsyncQuery(szQuery);

	dbsql::snprintf(szQuery, sizeof(szQuery), "DELETE FROM guild_comment%s WHERE guild_id=%lu", GetTablePostfix(), static_cast<unsigned long>(p->dwGuild));
	CDBManager::instance().AsyncQuery(szQuery);

	ForwardPacket(HEADER_DG_GUILD_DISBAND, p, sizeof(TPacketGuild));
}

const char* __GetWarType(int type)
{
	switch (type)
	{
		case 0:
			return "\xc6\xd0\xbf\xd5";
		case 1:
			return "\xb8\xcd\xc0\xe5";
		case 2:
			return "\xbc\xf6\xc8\xa3";
		default:
			return "\xbe\xf8\xb4\xc2 \xb9\xf8\xc8\xa3";
	}
}

void CClientManager::GuildWar(CPeer*, TPacketGuildWar* p)
{
	switch (p->bWar)
	{
		case GUILD_WAR_SEND_DECLARE:
			CGuildManager::instance().AddDeclare(p->bType, p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_REFUSE:
			CGuildManager::instance().RemoveDeclare(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_WAIT_START:
			[[fallthrough]]; // Reserve the war after logging the wait state.
		case GUILD_WAR_RESERVE:
			CGuildManager::instance().RemoveDeclare(p->dwGuildFrom, p->dwGuildTo);

			if (!CGuildManager::instance().ReserveWar(p))
				p->bWar = GUILD_WAR_CANCEL;
			else
				p->bWar = GUILD_WAR_RESERVE;

			break;

		case GUILD_WAR_ON_WAR:
			CGuildManager::instance().RemoveDeclare(p->dwGuildFrom, p->dwGuildTo);
			CGuildManager::instance().StartWar(p->bType, p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_OVER:
			CGuildManager::instance().RecvWarOver(p->dwGuildFrom, p->dwGuildTo, p->bType, p->lWarPrice);
			break;

		case GUILD_WAR_END:
			CGuildManager::instance().RecvWarEnd(p->dwGuildFrom, p->dwGuildTo);
			return;

		case GUILD_WAR_CANCEL :
			CGuildManager::instance().CancelWar(p->dwGuildFrom, p->dwGuildTo);
			break;
	}

	ForwardPacket(HEADER_DG_GUILD_WAR, p, sizeof(TPacketGuildWar));
}

void CClientManager::GuildWarScore(CPeer*, TPacketGuildWarScore * p)
{
	CGuildManager::instance().UpdateScore(p->dwGuildGainPoint, p->dwGuildOpponent, p->lScore, p->lBetScore);
}

void CClientManager::GuildChangeLadderPoint(TPacketGuildLadderPoint* p)
{
	CGuildManager::instance().ChangeLadderPoint(p->dwGuild, p->lChange);
}

void CClientManager::GuildUseSkill(TPacketGuildUseSkill* p)
{
	CGuildManager::instance().UseSkill(p->dwGuild, p->dwSkillVnum, p->dwCooltime);
	SendGuildSkillUsable(p->dwGuild, p->dwSkillVnum, false);
}

void CClientManager::SendGuildSkillUsable(DWORD guild_id, DWORD dwSkillVnum, bool bUsable)
{
	TPacketGuildSkillUsableChange p;

	p.dwGuild = guild_id;
	p.dwSkillVnum = dwSkillVnum;
	p.bUsable = bUsable;

	ForwardPacket(HEADER_DG_GUILD_SKILL_USABLE_CHANGE, &p, sizeof(TPacketGuildSkillUsableChange));
}
