#include "stdafx.h"
#include "MoneyLog.h"
#include "ClientManager.h"
#include "Peer.h"

#include <cstdint>

CMoneyLog::CMoneyLog()
{
}

CMoneyLog::~CMoneyLog()
{
}

void CMoneyLog::Save()
{
	CPeer* peer = CClientManager::instance().GetAnyPeer();
	if (!peer)
		return;
	for (BYTE bType = 0; bType < MONEY_LOG_TYPE_MAX_NUM; ++bType)
	{
		std::map<DWORD, int>::const_iterator it;
		for (it = m_MoneyLogContainer[bType].begin(); it != m_MoneyLogContainer[bType].end(); ++it)
		{
			TPacketMoneyLog p;
			p.type = bType;
			p.vnum = it->first;
			p.gold = it->second;
			peer->EncodeHeader(HEADER_DG_MONEY_LOG, 0, sizeof(p));
			peer->Encode(&p, sizeof(p));
		}
		m_MoneyLogContainer[bType].clear();
	}
}

void CMoneyLog::AddLog(BYTE bType, DWORD dwVnum, int iGold)
{
	int& total = m_MoneyLogContainer[bType][dwVnum];
	const auto bits = static_cast<std::uint32_t>(total) + static_cast<std::uint32_t>(iGold);
	total = bits <= 0x7fffffffu ? static_cast<int>(bits)
	                            : -1 - static_cast<int>(0xffffffffu - bits);
}
