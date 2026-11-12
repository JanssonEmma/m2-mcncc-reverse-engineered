#include "stdafx.h"
#include "PrivManager.h"
#include "ClientManager.h"
#include <limits>

const int CHARACTER_GOOD_PRIV_DURATION = 2*60*60;
const int CHARACTER_BAD_PRIV_DURATION = 60*60;

CPrivManager::CPrivManager()
{
	for (int type = 0; type < MAX_PRIV_NUM; ++type)
	{
		for (int empire = 0; empire < EMPIRE_MAX_NUM; ++empire)
			m_aaPrivEmpire[type][empire] = 0;
	}
}

CPrivManager::~CPrivManager()
{
}

void CPrivManager::Update()
{
	time_t now = CClientManager::instance().GetCurrentTime();

	while (!m_pqPrivGuild.empty() && m_pqPrivGuild.top().first <= now)
	{
		TPrivGuildData* p = m_pqPrivGuild.top().second;
		m_pqPrivGuild.pop();
		if (!p)
			continue;

		auto& guilds = m_aPrivGuild[p->type];
		const auto it = guilds.find(p->guild_id);
		if (it != guilds.end() && it->second == p)
		{
			guilds.erase(it);
			if (p->value != 0 && !p->bRemoved)
				SendChangeGuildPriv(p->guild_id, p->type, 0, 0);
		}

		delete p;
	}

	while (!m_pqPrivEmpire.empty() && m_pqPrivEmpire.top().first <= now)
	{
		TPrivEmpireData* p = m_pqPrivEmpire.top().second;
		m_pqPrivEmpire.pop();
		if (!p)
			continue;

		auto& current = m_aaPrivEmpire[p->type][p->empire];
		if (current == p)
		{
			current = nullptr;
			if (p->value != 0 && !p->bRemoved)
				SendChangeEmpirePriv(p->empire, p->type, 0, 0);
		}

		delete p;
	}

	while (!m_pqPrivChar.empty() && m_pqPrivChar.top().first <= now)
	{
		TPrivCharData* p = m_pqPrivChar.top().second;
		m_pqPrivChar.pop();
		if (!p)
			continue;

		if (!p->bRemoved)
		{
			SendChangeCharPriv(p->pid, p->type, 0);
			std::map<DWORD, TPrivCharData*>::iterator it = m_aPrivChar[p->type].find(p->pid);
			if (it != m_aPrivChar[p->type].end())
				m_aPrivChar[p->type].erase(it);
		}
		delete p;
	}
}

void CPrivManager::AddCharPriv(DWORD pid, BYTE type, int value)
{
	if (type >= sizeof(m_aPrivChar) / sizeof(m_aPrivChar[0]))
		return;

	std::map<DWORD, TPrivCharData*>::iterator it = m_aPrivChar[type].find(pid);

	if (it != m_aPrivChar[type].end())
		return;

	if (!value)
		return;

	time_t now = CClientManager::instance().GetCurrentTime();
	TPrivCharData* p = new TPrivCharData(type, value, pid);

	int iDuration = CHARACTER_BAD_PRIV_DURATION;

	if (value > 0)
		iDuration = CHARACTER_GOOD_PRIV_DURATION;

	m_pqPrivChar.push(std::make_pair(now + iDuration, p));
	m_aPrivChar[type].insert(std::make_pair(pid, p));

	sys_log(0, "AddCharPriv %d %d %d", static_cast<int>(pid), type, value);
	SendChangeCharPriv(pid, type, value);
}

void CPrivManager::AddGuildPriv(DWORD guild_id, BYTE type, int value, time_t duration_sec)
{
	if (type >= sizeof(m_aPrivGuild) / sizeof(m_aPrivGuild[0]))
		return;

	std::map<DWORD, TPrivGuildData*>::iterator it = m_aPrivGuild[type].find(guild_id);

	time_t now = CClientManager::instance().GetCurrentTime();
	if (duration_sec < 0 || now > std::numeric_limits<time_t>::max() - duration_sec)
		return;
	time_t end = now + duration_sec;
	TPrivGuildData * p = new TPrivGuildData(type, value, guild_id, end);
	m_pqPrivGuild.push(std::make_pair(end, p));

	if (it != m_aPrivGuild[type].end())
		it->second = p;
	else
		m_aPrivGuild[type].insert(std::make_pair(guild_id, p));

	SendChangeGuildPriv(guild_id, type, value, end);

	sys_log(0, "Guild Priv guild(%d) type(%d) value(%d) duration_sec(%d)", static_cast<int>(guild_id), type, value, static_cast<int>(duration_sec));
}

void CPrivManager::AddEmpirePriv(BYTE empire, BYTE type, int value, time_t duration_sec)
{
	if (type >= sizeof(m_aaPrivEmpire) / sizeof(m_aaPrivEmpire[0]) ||
	    empire >= sizeof(m_aaPrivEmpire[0]) / sizeof(m_aaPrivEmpire[0][0]))
		return;

	if (duration_sec < 0)
		duration_sec = 0;

	time_t now = CClientManager::instance().GetCurrentTime();
	if (duration_sec < 0 || now > std::numeric_limits<time_t>::max() - duration_sec)
		return;
	time_t end = now + duration_sec;

	TPrivEmpireData * p = new TPrivEmpireData(type, value, empire, end);
	m_pqPrivEmpire.push(std::make_pair(end, p));
	m_aaPrivEmpire[type][empire] = p;

	SendChangeEmpirePriv(empire, type, value, end);

	sys_log(0, "Empire Priv empire(%d) type(%d) value(%d) duration_sec(%d)", empire, type, value, static_cast<int>(duration_sec));
}

struct FSendChangeGuildPriv
{
	FSendChangeGuildPriv(DWORD guild_id, BYTE type, int value, time_t end_time_sec)
	{
		p.guild_id = guild_id;
		p.type = type;
		p.value = value;
		p.bLog = 1;

		p.end_time_sec = end_time_sec;

	}

	void operator() (CPeer* peer)
	{
		peer->EncodeHeader(HEADER_DG_CHANGE_GUILD_PRIV, 0, sizeof(TPacketDGChangeGuildPriv));
		peer->Encode(&p, sizeof(TPacketDGChangeGuildPriv));
		p.bLog = 0;
	}

	TPacketDGChangeGuildPriv p;
};

struct FSendChangeEmpirePriv
{
	FSendChangeEmpirePriv(BYTE empire, BYTE type, int value, time_t end_time_sec)
	{
		p.empire = empire;
		p.type = type;
		p.value = value;
		p.bLog = 1;

		p.end_time_sec = end_time_sec;

	}

	void operator ()(CPeer* peer)
	{
		peer->EncodeHeader(HEADER_DG_CHANGE_EMPIRE_PRIV, 0, sizeof(TPacketDGChangeEmpirePriv));
		peer->Encode(&p, sizeof(TPacketDGChangeEmpirePriv));
		p.bLog = 0;
	}

	TPacketDGChangeEmpirePriv p;
};

struct FSendChangeCharPriv
{
	FSendChangeCharPriv(DWORD pid, BYTE type, int value)
	{
		p.pid = pid;
		p.type = type;
		p.value = value;
		p.bLog = 1;
	}
	void operator()(CPeer* peer)
	{
		peer->EncodeHeader(HEADER_DG_CHANGE_CHARACTER_PRIV, 0, sizeof(TPacketDGChangeCharacterPriv));
		peer->Encode(&p, sizeof(TPacketDGChangeCharacterPriv));
		p.bLog = 0;
	}
	TPacketDGChangeCharacterPriv p;
};

void CPrivManager::SendChangeGuildPriv(DWORD guild_id, BYTE type, int value, time_t end_time_sec)
{
	CClientManager::instance().for_each_peer(FSendChangeGuildPriv(guild_id, type, value, end_time_sec));
}

void CPrivManager::SendChangeEmpirePriv(BYTE empire, BYTE type, int value, time_t end_time_sec)
{
	CClientManager::instance().for_each_peer(FSendChangeEmpirePriv(empire, type, value, end_time_sec));
}

void CPrivManager::SendChangeCharPriv(DWORD pid, BYTE type, int value)
{
	CClientManager::instance().for_each_peer(FSendChangeCharPriv(pid, type, value));
}

void CPrivManager::SendPrivOnSetup(CPeer* peer)
{
	for (int i = 1; i < MAX_PRIV_NUM; ++i)
	{
		for (int e = 0; e < EMPIRE_MAX_NUM; ++e)
		{

			TPrivEmpireData* pPrivEmpireData = m_aaPrivEmpire[i][e];
			if (pPrivEmpireData)
			{
				FSendChangeEmpirePriv(static_cast<BYTE>(e), static_cast<BYTE>(i), pPrivEmpireData->value, pPrivEmpireData->end_time_sec)(peer);
			}

		}

		for (std::map<DWORD, TPrivGuildData*>::iterator it = m_aPrivGuild[i].begin(); it != m_aPrivGuild[i].end();++it)
		{

			FSendChangeGuildPriv(it->first, static_cast<BYTE>(i), it->second->value, it->second->end_time_sec)(peer);

		}
		for (std::map<DWORD, TPrivCharData*>::iterator it = m_aPrivChar[i].begin(); it != m_aPrivChar[i].end(); ++it)
		{
			FSendChangeCharPriv(it->first, static_cast<BYTE>(i), it->second->value)(peer);
		}
	}
}
