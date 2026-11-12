#include "stdafx.h"
#include "NumericInput.h"
#include "SqlFormat.h"
#include "SqlInput.h"
#include "GuildManager.h"
#include "Main.h"
#include "ClientManager.h"
#include "QID.h"
#include "Config.h"

#include <cmath>
#include <cstdint>
#include <memory>

extern std::string g_stLocale;

namespace
{
    const int GUILD_RANK_MAX_NUM = 20;
    const BYTE GUILD_LADDER_HEADER = 101;
    const BYTE GUILD_MONEY_HEADER = 103;
    const BYTE GUILD_WITHDRAW_HEADER = 104;
    const BYTE GUILD_WAR_HEADER = 97;
    const BYTE GUILD_WAR_SCORE_HEADER = 98;
    const BYTE WAR_TYPE_FIELD = 0;
    const BYTE WAR_TYPE_BATTLE = 1;
    const BYTE WAR_STATE_DECLARE = 1;
    const BYTE WAR_STATE_ON_WAR = 6;
    const BYTE WAR_STATE_END = 7;
    const BYTE WAR_STATE_WAIT_START = 4;
    const BYTE WAR_STATE_RESERVE = 9;
    const BYTE RESERVE_ADD_HEADER = 106;
    const BYTE RESERVE_DEL_HEADER = 107;
    const BYTE RESERVE_BET_HEADER = 108;

    const int c_aiScoreByLevel[21] =
    {
        500, 500, 1000, 2000, 3000, 4000, 6000,
        8000, 10000, 12000, 15000, 18000, 21000, 24000,
        28000, 32000, 36000, 40000, 45000, 50000, 55000,
    };

    const int c_aiScoreByRanking[21] =
    {
        0, 55000, 50000, 45000, 40000, 36000, 32000,
        28000, 24000, 21000, 18000, 15000, 12000, 10000,
        8000, 6000, 4000, 3000, 2000, 1000, 500,
    };

#pragma pack(push, 1)
    struct GuildLadderPacket
    {
        std::uint32_t dwGuild;
        std::int32_t lLadderPoint;
        std::int32_t lWin;
        std::int32_t lDraw;
        std::int32_t lLoss;
    };


#pragma pack(pop)
    struct FSendPeerWar
    {
        FSendPeerWar(BYTE bType, BYTE bWar, DWORD GID1, DWORD GID2)
        {
            if (number_ex(0, 1, const_cast<char*>("GuildManager.cpp"), 22))
                std::swap(GID1, GID2);

            memset(&p, 0, sizeof(TPacketGuildWar));

            p.bWar = bWar;
            p.bType = bType;
            p.dwGuildFrom = GID1;
            p.dwGuildTo = GID2;
        }

        void operator() (CPeer* peer)
        {
            if (peer->GetChannel() == 0)
                return;

            peer->EncodeHeader(GUILD_WAR_HEADER, 0, sizeof(TPacketGuildWar));
            peer->Encode(&p, sizeof(TPacketGuildWar));
        }

        TPacketGuildWar p;
    };

    struct FSendGuildWarScore
    {
        FSendGuildWarScore(DWORD guild_gain, DWORD dwOppGID, int iScore, int iBetScore)
        {
            pck.dwGuildGainPoint = guild_gain;
            pck.dwGuildOpponent = dwOppGID;
            pck.lScore = iScore;
            pck.lBetScore = iBetScore;
        }

        void operator() (CPeer* peer)
        {
            if (peer->GetChannel() == 0)
                return;

            peer->EncodeHeader(GUILD_WAR_SCORE_HEADER, 0, sizeof(pck));
            peer->Encode(&pck, sizeof(pck));
        }

        TPacketGuildWarScore pck;
    };
}

CGuildManager::CGuildManager()
{
}

CGuildManager::~CGuildManager()
{
    while (!m_pqOnWar.empty())
    {
        if (!m_pqOnWar.top().second->bEnd)
            delete m_pqOnWar.top().second;

        m_pqOnWar.pop();
    }
}

TGuild & CGuildManager::TouchGuild(DWORD GID)
{
    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(GID);

    if (it != m_map_kGuild.end())
        return it->second;

    TGuild info;
    info.ladder_point = 0;
    info.win = 0;
    info.draw = 0;
    info.loss = 0;
    info.gold = 0;
    // The original insertion leaves the name and level indeterminate.
    m_map_kGuild.insert(std::map<DWORD, TGuild>::value_type(GID, info));
    return m_map_kGuild[GID];
}

void CGuildManager::ParseResult(SQLResult * pRes)
{
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(pRes->pSQLResult)))
    {
        DWORD GID = strtoul(row[0], NULL, 10);

        TGuild & r_info = TouchGuild(GID);

        strncpy(r_info.szName, row[1], 12);
        r_info.szName[12] = '\0';
        r_info.ladder_point = atoi(row[2]);
        r_info.win = atoi(row[3]);
        r_info.draw = atoi(row[4]);
        r_info.loss = atoi(row[5]);
        r_info.gold = atoi(row[6]);
        r_info.level = atoi(row[7]);

        sys_log(0,
                "GuildWar: %-24s ladder %-5d win %-3d draw %-3d loss %-3d",
                r_info.szName,
                r_info.ladder_point,
                r_info.win,
                r_info.draw,
                r_info.loss);
    }
}

void CGuildManager::Initialize()
{
    char szQuery[1025];
    dbsql::snprintf(szQuery, 1024, "SELECT id, name, ladder_point, win, draw, loss, gold, level FROM guild%s", GetTablePostfix());
    std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

    if (pmsg->Get()->iNumRows)
        ParseResult(pmsg->Get());

    char str[128 + 1];

    if (!CConfig::instance().GetValue("POLY_POWER", str, 128))
        *str = '\0';

    if (!polyPower.Analyze(str))
        sys_err("cannot set power poly: %s", str);

    if (!CConfig::instance().GetValue("POLY_HANDICAP", str, 128))
        *str = '\0';

    if (!polyHandicap.Analyze(str))
        sys_err("cannot set handicap poly: %s", str);

    QueryRanking();
}

void CGuildManager::Load(DWORD dwGuildID)
{
    char szQuery[1025];

    dbsql::snprintf(szQuery, 1024, "SELECT id, name, ladder_point, win, draw, loss, gold, level FROM guild%s WHERE id=%lu", GetTablePostfix(), dwGuildID);
    std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

    if (pmsg->Get()->iNumRows)
        ParseResult(pmsg->Get());
}

void CGuildManager::QueryRanking()
{
    char szQuery[257];
    dbsql::snprintf(szQuery, 256, "SELECT id,name,ladder_point FROM guild%s ORDER BY ladder_point DESC LIMIT 20", GetTablePostfix());

    CDBManager::instance().ReturnQuery(szQuery, QID_GUILD_RANKING, 0, 0);
}

int CGuildManager::GetRanking(DWORD dwGID)
{
    std::map<DWORD, int>::iterator it = map_kLadderPointRankingByGID.find(dwGID);

    if (it == map_kLadderPointRankingByGID.end())
        return GUILD_RANK_MAX_NUM;

    return std::max(0, std::min(it->second, GUILD_RANK_MAX_NUM));
}

void CGuildManager::ResultRanking(MYSQL_RES * pRes)
{
    if (!pRes)
        return;

    const int iLastLadderPoint = -1;
    int iRank = 0;

    map_kLadderPointRankingByGID.clear();

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(pRes)))
    {
        DWORD dwGID = static_cast<DWORD>(atoi(row[0]));
        int iLadderPoint = atoi(row[2]);

        if (iLadderPoint != iLastLadderPoint)
            ++iRank;

        map_kLadderPointRankingByGID.insert(std::make_pair(dwGID, iRank));
    }
}

int CGuildManager::GetLadderPoint(DWORD guildId)
{
    std::map<DWORD, TGuild>::iterator entry = m_map_kGuild.find(guildId);
    if (entry == m_map_kGuild.end())
        return 0;

    return entry->second.ladder_point;
}

void CGuildManager::BootReserveWar()
{
    const char * c_apszQuery[2] =
    {
        "SELECT id, guild1, guild2, UNIX_TIMESTAMP(time), type, warprice, initscore, bet_from, bet_to, power1, power2, handicap FROM guild_war_reservation WHERE started=1 AND winner=-1",
        "SELECT id, guild1, guild2, UNIX_TIMESTAMP(time), type, warprice, initscore, bet_from, bet_to, power1, power2, handicap FROM guild_war_reservation WHERE started=0"
    };

    for (int i = 0; i < 2; ++i)
    {
        std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(c_apszQuery[i]));

        if (pmsg->Get()->iNumRows <= 0)
            continue;

        MYSQL_ROW row;

        while ((row = mysql_fetch_row(pmsg->Get()->pSQLResult)))
        {
            int col = 0;

            TGuildWarReserve t;

            t.dwID = atoi(row[col++]);
            t.dwGuildFrom = atoi(row[col++]);
            t.dwGuildTo = atoi(row[col++]);
            t.dwTime = atoi(row[col++]);
            t.bType = dbinput::ReadInteger<decltype(t.bType)>(row[col++]);
            t.lWarPrice = atoi(row[col++]);
            t.lInitialScore = atoi(row[col++]);
            t.dwBetFrom = atoi(row[col++]);
            t.dwBetTo = atoi(row[col++]);
            t.lPowerFrom = atoi(row[col++]);
            t.lPowerTo = atoi(row[col++]);
            t.lHandicap = atoi(row[col++]);
            t.bStarted = 0;

            CGuildWarReserve * pkReserve = new CGuildWarReserve(t);

            char buf[513];
            snprintf(buf, 512, "GuildWar: BootReserveWar : step %d id %u GID1 %u GID2 %u", i, static_cast<unsigned>(t.dwID), static_cast<unsigned>(t.dwGuildFrom), static_cast<unsigned>(t.dwGuildTo));



            if (i == 0 || static_cast<std::int32_t>(t.dwTime - CClientManager::instance().GetCurrentTime()) < 0)
            {
                if (i == 0)
                    sys_log(0, "%s : DB was shutdowned while war is being.", buf);
                else
                    sys_log(0, "%s : left time lower than 5 minutes, will be canceled", buf);

                pkReserve->Draw();
                delete pkReserve;
            }
            else
            {
                sys_log(0, "%s : OK", buf);
                m_map_kWarReserve.insert(std::make_pair(t.dwID, pkReserve));
            }
        }
    }
}

CGuildWarReserve::CGuildWarReserve(TGuildWarReserve& rTable)
{
    thecore_memcpy(&m_data, &rTable, sizeof(TGuildWarReserve));
    m_iLastNoticeMin = -1;

    Initialize();
}

void CGuildWarReserve::Initialize()
{
    char szQuery[257];
    dbsql::snprintf(szQuery, 256, "SELECT login, guild, gold FROM guild_war_bet WHERE war_id=%lu", static_cast<unsigned long>(m_data.dwID));

    std::unique_ptr<SQLMsg> msgbet(CDBManager::instance().DirectQuery(szQuery));

    if (msgbet->Get()->iNumRows)
    {
        MYSQL_RES * res = msgbet->Get()->pSQLResult;
        MYSQL_ROW row;

        char szLogin[LOGIN_MAX_LEN+1];
        DWORD dwGuild;
        DWORD dwGold;

        while ((row = mysql_fetch_row(res)))
        {
            strncpy(szLogin, row[0], 30);
            szLogin[30] = '\0';
            dwGuild = static_cast<DWORD>(atoi(row[1]));
            dwGold = static_cast<DWORD>(atoi(row[2]));

            mapBet.insert(std::make_pair(szLogin, std::make_pair(dwGuild, dwGold)));
        }
    }
}

void CGuildWarReserve::Draw()
{
    char szQuery[1025];

    dbsql::snprintf(szQuery, 1024, "UPDATE guild_war_reservation SET started=1,winner=0 WHERE id=%lu", static_cast<unsigned long>(m_data.dwID));
    CDBManager::instance().AsyncQuery(szQuery);

    if (mapBet.empty())
        return;

    sys_log(0, "WAR_REWARD: Draw. war_id %lu", static_cast<unsigned long>(m_data.dwID));

    std::map<std::string, std::pair<DWORD, DWORD> >::iterator it = mapBet.begin();

    while (1)
    {
        int iLen = 0;
        int iRow = 0;

        iLen += dbsql::snprintf(szQuery, 512 - iLen, "INSERT INTO item_award (login, vnum, socket0, given_time) VALUES");

        while (it != mapBet.end())
        {
            const auto login = it->first.size() <= LOGIN_MAX_LEN ? dbsql::EscapeLiteral(it->first) : std::nullopt;
            if (!login)
            {
                ++it;
                continue;
            }
            if (iRow == 0)
                iLen += dbsql::snprintf(szQuery + iLen, 512 - iLen, "('%s', %d, %lu, NOW())",
                        *login, 50026, it->second.second);
            else
                iLen += dbsql::snprintf(szQuery + iLen, 512 - iLen, ",('%s', %d, %lu, NOW())",
                        *login, 50026, it->second.second);

            it++;

            if (iLen > 384)
                break;

            ++iRow;
        }

        if (iRow > 0)
        {
            sys_log(0, "WAR_REWARD: QUERY: %s", szQuery);
            CDBManager::instance().AsyncQuery(szQuery);
        }

        if (it == mapBet.end())
            break;
    }
}

void CGuildManager::GuildWarWin(DWORD GID)
{
    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(GID);

    if (it == m_map_kGuild.end())
        return;

    ++it->second.win;

    char buf[1024];
    dbsql::snprintf(buf, sizeof(buf), "UPDATE guild%s SET win=%d WHERE id=%lu", GetTablePostfix(), it->second.win, GID);
    CDBManager::instance().AsyncQuery(buf);
}

void CGuildManager::GuildWarLose(DWORD GID)
{
    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(GID);

    if (it == m_map_kGuild.end())
        return;

    ++it->second.loss;

    char buf[1024];
    dbsql::snprintf(buf, sizeof(buf), "UPDATE guild%s SET loss=%d WHERE id=%lu", GetTablePostfix(), it->second.loss, GID);
    CDBManager::instance().AsyncQuery(buf);
}

void CGuildManager::GuildWarDraw(DWORD GID)
{
    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(GID);

    if (it == m_map_kGuild.end())
        return;

    ++it->second.draw;

    char buf[1024];
    dbsql::snprintf(buf, sizeof(buf), "UPDATE guild%s SET draw=%d WHERE id=%lu", GetTablePostfix(), it->second.draw, GID);
    CDBManager::instance().AsyncQuery(buf);
}

bool CGuildManager::IsHalfWinLadderPoint(DWORD dwGuildWinner, DWORD dwGuildLoser)
{
    DWORD GID1 = dwGuildWinner;
    DWORD GID2 = dwGuildLoser;

    if (GID1 > GID2)
        std::swap(GID1, GID2);

    std::map<DWORD, time_t>::iterator it = m_mapGuildWarEndTime[GID1].find(GID2);

    if (it != m_mapGuildWarEndTime[GID1].end() &&
            it->second + (12 * 60 * 60) > CClientManager::instance().GetCurrentTime())
        return true;

    return false;
}

void CGuildManager::ProcessDraw(DWORD dwGuildID1, DWORD dwGuildID2)
{
    sys_log(0, "GuildWar: \tThe war between %d and %d is ended in draw", static_cast<int>(dwGuildID1), static_cast<int>(dwGuildID2));

    GuildWarDraw(dwGuildID1);
    GuildWarDraw(dwGuildID2);
    ChangeLadderPoint(dwGuildID1, 0);
    ChangeLadderPoint(dwGuildID2, 0);

    QueryRanking();
}

void CGuildManager::ProcessWinLose(DWORD dwGuildWinner, DWORD dwGuildLoser)
{
    GuildWarWin(dwGuildWinner);
    GuildWarLose(dwGuildLoser);
    sys_log(0, "GuildWar: \tWinner : %d Loser : %d", static_cast<int>(dwGuildWinner), static_cast<int>(dwGuildLoser));

    int iPoint = GetLadderPoint(dwGuildLoser);
    int gain = (int)(iPoint * 0.05);
    int loss = (int)(iPoint * 0.07);

    if (IsHalfWinLadderPoint(dwGuildWinner, dwGuildLoser))
        gain /= 2;

    sys_log(0, "GuildWar: \tgain : %d loss : %d", gain, loss);

    ChangeLadderPoint(dwGuildWinner, gain);
    ChangeLadderPoint(dwGuildLoser, -loss);

    QueryRanking();
}

void CGuildManager::ChangeLadderPoint(DWORD GID, int change)
{
    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(GID);

    if (it == m_map_kGuild.end())
        return;

    TGuild & r = it->second;

    r.ladder_point += change;

    if (r.ladder_point < 0)
        r.ladder_point = 0;

    sys_log(0, "GuildManager::ChangeLadderPoint %lu %d", GID, r.ladder_point);

    char buf[1024];
    dbsql::snprintf(buf, sizeof(buf), "UPDATE guild%s SET ladder_point=%d WHERE id = %lu", GetTablePostfix(), r.ladder_point, GID);
    CDBManager::instance().AsyncQuery(buf);



    GuildLadderPacket p;

    p.dwGuild = GID;
    p.lLadderPoint = r.ladder_point;
    p.lWin = r.win;
    p.lDraw = r.draw;
    p.lLoss = r.loss;

    CClientManager::instance().ForwardPacket(GUILD_LADDER_HEADER, &p, sizeof(GuildLadderPacket));
}

void CGuildManager::UseSkill(DWORD GID, DWORD dwSkillVnum, DWORD dwCooltime)
{

    sys_log(0, "UseSkill(gid=%d, skill=%d) CoolTime(%d:%d)", static_cast<int>(GID), static_cast<int>(dwSkillVnum), static_cast<int>(dwCooltime),
            static_cast<int>(CClientManager::instance().GetCurrentTime() + dwCooltime));
    m_pqSkill.push(std::make_pair(CClientManager::instance().GetCurrentTime() + dwCooltime, TGuildSkillUsed(GID, dwSkillVnum)));

}

void CGuildManager::MoneyChange(DWORD dwGuild, DWORD dwGold)
{
    sys_log(0, "GuildManager::MoneyChange %d %d", static_cast<int>(dwGuild), static_cast<int>(dwGold));

    TPacketDGGuildMoneyChange p;
    p.dwGuild = dwGuild;
    p.lTotalGold = dwGold;
    CClientManager::instance().ForwardPacket(GUILD_MONEY_HEADER, &p, sizeof(p));

    char buf[1024];
    dbsql::snprintf(buf, sizeof(buf), "UPDATE guild%s SET gold=%lu WHERE id = %lu", GetTablePostfix(), dwGold, dwGuild);
    CDBManager::instance().AsyncQuery(buf);
}

void CGuildManager::DepositMoney(DWORD dwGuild, long lGold)
{
    if (lGold <= 0)
        return;

    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(dwGuild);

    if (it == m_map_kGuild.end())
    {
        sys_err("No guild by id %lu", dwGuild);
        return;
    }

    it->second.gold += lGold;
    sys_log(0, "GUILD: %lu Deposit %lu Total %d", dwGuild, static_cast<unsigned long>(lGold), it->second.gold);

    MoneyChange(dwGuild, it->second.gold);
}

void CGuildManager::WithdrawMoney(CPeer* peer, DWORD dwGuild, long lGold)
{
    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(dwGuild);

    if (it == m_map_kGuild.end())
    {
        sys_err("No guild by id %lu", dwGuild);
        return;
    }

    if (it->second.gold >= lGold)
    {
        it->second.gold -= lGold;
        sys_log(0, "GUILD: %lu Withdraw %d Total %d", dwGuild, static_cast<int>(lGold), it->second.gold);

        TPacketDGGuildMoneyWithdraw p;
        p.dwGuild = dwGuild;
        p.lChangeGold = lGold;

        peer->EncodeHeader(GUILD_WITHDRAW_HEADER, 0, sizeof(p));
        peer->Encode(&p, sizeof(p));
    }
}

void CGuildManager::WithdrawMoneyReply(DWORD dwGuild, BYTE bGiveSuccess, long lGold)
{
    std::map<DWORD, TGuild>::iterator it = m_map_kGuild.find(dwGuild);

    if (it == m_map_kGuild.end())
        return;

    sys_log(0, "GuildManager::WithdrawMoneyReply : guild %lu success %d gold %d", dwGuild, bGiveSuccess, static_cast<int>(lGold));

    if (!bGiveSuccess)
        it->second.gold += lGold;
    else
        MoneyChange(dwGuild, it->second.gold);
}

void CGuildManager::Update()
{
    ProcessReserveWar();

    time_t now = CClientManager::instance().GetCurrentTime();

    if (!m_pqOnWar.empty())
    {

        while (!m_pqOnWar.empty() && (m_pqOnWar.top().first <= now || (m_pqOnWar.top().second && m_pqOnWar.top().second->bEnd)))
        {
            TGuildWarPQElement * e = m_pqOnWar.top().second;

            m_pqOnWar.pop();

            if (e)
            {
                if (!e->bEnd)
                    WarEnd(e->GID[0], e->GID[1], false);

                delete e;
            }
        }
    }

    while (!m_pqSkill.empty() && m_pqSkill.top().first <= now)
    {
        const TGuildSkillUsed& s = m_pqSkill.top().second;
        CClientManager::instance().SendGuildSkillUsable(s.GID, s.dwSkillVnum, true);
        m_pqSkill.pop();
    }

    while (!m_pqWaitStart.empty() && m_pqWaitStart.top().first <= now)
    {
        const TGuildWaitStartInfo & ws = m_pqWaitStart.top().second;
        m_pqWaitStart.pop();

        StartWar(ws.bType, ws.GID[0], ws.GID[1], ws.pkReserve);

        if (ws.lInitialScore)
        {
            UpdateScore(ws.GID[0], ws.GID[1], ws.lInitialScore, 0);
            UpdateScore(ws.GID[1], ws.GID[0], ws.lInitialScore, 0);
        }

        TPacketGuildWar p;

        p.bType     = ws.bType;
        p.bWar      = WAR_STATE_ON_WAR;
        p.dwGuildFrom   = ws.GID[0];
        p.dwGuildTo = ws.GID[1];

        CClientManager::instance().ForwardPacket(GUILD_WAR_HEADER, &p, sizeof(p));
        sys_log(0, "GuildWar: GUILD sending start of wait start war %d %d", static_cast<int>(ws.GID[0]), static_cast<int>(ws.GID[1]));
    }
}

void CGuildManager::OnSetup(CPeer* peer)
{
    for (std::map<DWORD, std::map<DWORD, TGuildWarInfo> >::iterator it_cont = m_WarMap.begin(); it_cont != m_WarMap.end(); ++it_cont)
        for (std::map<DWORD, TGuildWarInfo>::iterator it = it_cont->second.begin(); it != it_cont->second.end(); ++it)
        {
            DWORD g1 = it_cont->first;
            DWORD g2 = it->first;
            TGuildWarPQElement* p = it->second.pElement;

            if (!p || p->bEnd)
                continue;

            FSendPeerWar(p->bType, WAR_STATE_ON_WAR, g1, g2) (peer);

        }

    for (std::set<TGuildDeclareInfo>::iterator it = m_DeclareMap.begin(); it != m_DeclareMap.end(); ++it)
    {
        FSendPeerWar(it->bType, WAR_STATE_DECLARE, it->dwGuildID[0], it->dwGuildID[1]) (peer);
    }

    for (std::map<DWORD, CGuildWarReserve*>::iterator it = m_map_kWarReserve.begin(); it != m_map_kWarReserve.end(); ++it)
    {
        it->second->OnSetup(peer);
    }
}

void CGuildManager::RemoveWar(DWORD GID1, DWORD GID2)
{
    sys_log(0, "GuildWar: RemoveWar(%d, %d)", static_cast<int>(GID1), static_cast<int>(GID2));

    if (GID1 > GID2)
        std::swap(GID2, GID1);

    std::map<DWORD, TGuildWarInfo>::iterator it = m_WarMap[GID1].find(GID2);

    if (it == m_WarMap[GID1].end())
    {
        if (m_WarMap[GID1].empty())
            m_WarMap.erase(GID1);

        return;
    }

    if (it->second.pElement)
        it->second.pElement->bEnd = true;

    m_mapGuildWarEndTime[GID1][GID2] = CClientManager::instance().GetCurrentTime();

    m_WarMap[GID1].erase(it);

    if (m_WarMap[GID1].empty())
        m_WarMap.erase(GID1);
}

void CGuildManager::WarEnd(DWORD GID1, DWORD GID2, bool bForceDraw)
{
    if (GID1 > GID2)
        std::swap(GID2, GID1);

    sys_log(0, "GuildWar: WarEnd %d %d", static_cast<int>(GID1), static_cast<int>(GID2));

    std::map<DWORD, TGuildWarInfo>::iterator itWarMap = m_WarMap[GID1].find(GID2);

    if (itWarMap == m_WarMap[GID1].end())
    {
        sys_err("GuildWar: war not exist or already ended. [1]");
        return;
    }

    TGuildWarInfo gwi = itWarMap->second;
    TGuildWarPQElement * pData = gwi.pElement;

    if (!pData || pData->bEnd)
    {
        sys_err("GuildWar: war not exist or already ended. [2]");
        return;
    }

    DWORD win_guild = pData->GID[0];
    DWORD lose_guild = pData->GID[1];

    bool bDraw = false;

    if (!bForceDraw)
    {
        if (pData->iScore[0] > pData->iScore[1])
        {
            win_guild = pData->GID[0];
            lose_guild = pData->GID[1];
        }
        else if (pData->iScore[1] > pData->iScore[0])
        {
            win_guild = pData->GID[1];
            lose_guild = pData->GID[0];
        }
        else
            bDraw = true;
    }
    else
        bDraw = true;

    if (bDraw)
        ProcessDraw(win_guild, lose_guild);
    else
        ProcessWinLose(win_guild, lose_guild);

    CClientManager::instance().for_each_peer(FSendPeerWar(0, WAR_STATE_END, GID1, GID2));

    RemoveWar(GID1, GID2);
}

void CGuildManager::RecvWarOver(DWORD dwGuildWinner, DWORD dwGuildLoser, bool bDraw, long lWarPrice)
{
    sys_log(0, "GuildWar: RecvWarOver : winner %lu vs %lu draw? %d war_price %ld", dwGuildWinner, dwGuildLoser, bDraw ? 1 : 0, lWarPrice);

    DWORD GID1 = dwGuildWinner;
    DWORD GID2 = dwGuildLoser;

    if (GID1 > GID2)
        std::swap(GID1, GID2);

    std::map<DWORD, TGuildWarInfo>::iterator it = m_WarMap[GID1].find(GID2);

    if (it == m_WarMap[GID1].end())
        return;

    TGuildWarInfo & gw = it->second;

    if (bDraw)
    {

        DepositMoney(dwGuildWinner, lWarPrice / 2);
        DepositMoney(dwGuildLoser, lWarPrice / 2);
        ProcessDraw(dwGuildWinner, dwGuildLoser);
    }
    else
    {

        DepositMoney(dwGuildWinner, lWarPrice);
        ProcessWinLose(dwGuildWinner, dwGuildLoser);
    }

    if (gw.pkReserve)
    {
        if (bDraw || !gw.pElement)
            gw.pkReserve->Draw();
        else if (gw.pElement->bType == WAR_TYPE_BATTLE)
            gw.pkReserve->End(gw.pElement->iBetScore[0], gw.pElement->iBetScore[1]);
    }

    RemoveWar(GID1, GID2);
}

void CGuildManager::RecvWarEnd(DWORD GID1, DWORD GID2)
{
    sys_log(0, "GuildWar: RecvWarEnded : %lu vs %lu", GID1, GID2);
    WarEnd(GID1, GID2, true);
}

void CGuildManager::StartWar(BYTE bType, DWORD GID1, DWORD GID2, CGuildWarReserve * pkReserve)
{
    sys_log(0, "GuildWar: StartWar(%d,%d,%d)", bType, static_cast<int>(GID1), static_cast<int>(GID2));

    if (GID1 > GID2)
        std::swap(GID1, GID2);

    TGuildWarInfo & gw = m_WarMap[GID1][GID2];

    if (bType == WAR_TYPE_FIELD)
        gw.tEndTime = CClientManager::instance().GetCurrentTime() + 1800;
    else
        gw.tEndTime = CClientManager::instance().GetCurrentTime() + 172800;

    gw.pElement = new TGuildWarPQElement(bType, GID1, GID2);
    gw.pkReserve = pkReserve;

    m_pqOnWar.push(std::make_pair(gw.tEndTime, gw.pElement));
}

void CGuildManager::UpdateScore(DWORD dwGainGID, DWORD dwOppGID, int iScoreDelta, int iBetScoreDelta)
{
    DWORD GID1 = dwGainGID;
    DWORD GID2 = dwOppGID;

    if (GID1 > GID2)
        std::swap(GID1, GID2);

    std::map<DWORD, TGuildWarInfo>::iterator it = m_WarMap[GID1].find(GID2);

    if (it != m_WarMap[GID1].end())
    {
        TGuildWarPQElement * p = it->second.pElement;

        if (!p || p->bEnd)
        {
            sys_err("GuildWar: war not exist or already ended.");
            return;
        }

        int iNewScore = 0;
        int iNewBetScore = 0;

        if (p->GID[0] == dwGainGID)
        {
            p->iScore[0] += iScoreDelta;
            p->iBetScore[0] += iBetScoreDelta;

            iNewScore = p->iScore[0];
            iNewBetScore = p->iBetScore[0];
        }
        else
        {
            p->iScore[1] += iScoreDelta;
            p->iBetScore[1] += iBetScoreDelta;

            iNewScore = p->iScore[1];
            iNewBetScore = p->iBetScore[1];
        }

        sys_log(0, "GuildWar: SendGuildWarScore guild %lu wartype %u score_delta %d betscore_delta %d result %lu, %lu",
                dwGainGID, static_cast<unsigned>(p->bType), iScoreDelta, iBetScoreDelta,
                static_cast<unsigned long>(iNewScore), static_cast<unsigned long>(iNewBetScore));

        CClientManager::instance().for_each_peer(FSendGuildWarScore(dwGainGID, dwOppGID, iNewScore, iNewBetScore));
    }
}

void CGuildManager::AddDeclare(BYTE bType, DWORD guild_from, DWORD guild_to)
{
    TGuildDeclareInfo di(bType, guild_from, guild_to);

    if (m_DeclareMap.find(di) == m_DeclareMap.end())
        m_DeclareMap.insert(di);

    sys_log(0, "GuildWar: AddDeclare(Type:%d,from:%d,to:%d)", bType, static_cast<int>(guild_from), static_cast<int>(guild_to));
}

void CGuildManager::RemoveDeclare(DWORD guild_from, DWORD guild_to)
{
    std::set<TGuildDeclareInfo>::iterator it = m_DeclareMap.find(TGuildDeclareInfo(0, guild_from, guild_to));

    if (it != m_DeclareMap.end())
        m_DeclareMap.erase(it);

    it = m_DeclareMap.find(TGuildDeclareInfo(0,guild_to, guild_from));

    if (it != m_DeclareMap.end())
        m_DeclareMap.erase(it);

    sys_log(0, "GuildWar: RemoveDeclare(from:%d,to:%d)", static_cast<int>(guild_from), static_cast<int>(guild_to));
}

bool CGuildManager::TakeBetPrice(DWORD dwGuildTo, DWORD dwGuildFrom, long lWarPrice)
{
    std::map<DWORD, TGuild>::iterator it_from = m_map_kGuild.find(dwGuildFrom);
    std::map<DWORD, TGuild>::iterator it_to = m_map_kGuild.find(dwGuildTo);

    if (it_from == m_map_kGuild.end() || it_to == m_map_kGuild.end())
    {
        sys_log(0, "TakeBetPrice: guild not exist %lu %lu",
                dwGuildFrom, dwGuildTo);
        return false;
    }

    if (it_from->second.gold < lWarPrice || it_to->second.gold < lWarPrice)
    {
        sys_log(0, "TakeBetPrice: not enough gold %lu %d to %lu %d",
                dwGuildFrom, it_from->second.gold, dwGuildTo, it_to->second.gold);
        return false;
    }

    it_from->second.gold -= lWarPrice;
    it_to->second.gold -= lWarPrice;

    MoneyChange(dwGuildFrom, it_from->second.gold);
    MoneyChange(dwGuildTo, it_to->second.gold);
    return true;
}

bool CGuildManager::WaitStart(TPacketGuildWar * p)
{
    if (p->lWarPrice > 0)
        if (!TakeBetPrice(p->dwGuildFrom, p->dwGuildTo, p->lWarPrice))
            return false;

    DWORD dwCurTime = CClientManager::instance().GetCurrentTime();

    TGuildWaitStartInfo info(p->bType, p->dwGuildFrom, p->dwGuildTo, p->lWarPrice, p->lInitialScore, NULL);
    m_pqWaitStart.push(std::make_pair(dwCurTime + 5, info));

    sys_log(0,
            "GuildWar: WaitStart g1 %d g2 %d price %d start at %lu",
            static_cast<int>(p->dwGuildFrom),
            static_cast<int>(p->dwGuildTo),
            static_cast<int>(p->lWarPrice),
            dwCurTime + 5);

    return true;
}

void CGuildManager::CancelWar(DWORD GID1, DWORD GID2)
{
    RemoveDeclare(GID1, GID2);
    RemoveWar(GID1, GID2);
}

int GetAverageGuildMemberLevel(DWORD dwGID)
{
    char szQuery[8193];

    dbsql::snprintf(szQuery, sizeof(szQuery) - 1,
            "SELECT AVG(level) FROM guild_member%s, player%s AS p WHERE guild_id=%lu AND guild_member%s.pid=p.id",
            GetTablePostfix(), GetTablePostfix(), dwGID, GetTablePostfix());

    std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(szQuery));

    MYSQL_ROW row;
    row = mysql_fetch_row(msg->Get()->pSQLResult);

    return row[0] ? atoi(row[0]) : 0;
}

int GetGuildMemberCount(DWORD dwGID)
{
    char szQuery[8193];

    dbsql::snprintf(szQuery, sizeof(szQuery) - 1, "SELECT COUNT(*) FROM guild_member%s WHERE guild_id=%lu", GetTablePostfix(), dwGID);

    std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(szQuery));

    MYSQL_ROW row;
    row = mysql_fetch_row(msg->Get()->pSQLResult);

    return row[0] ? atoi(row[0]) : 0;
}

bool CGuildManager::ReserveWar(TPacketGuildWar * p)
{
    DWORD GID1 = p->dwGuildFrom;
    DWORD GID2 = p->dwGuildTo;

    if (GID1 > GID2)
        std::swap(GID1, GID2);

    if (p->lWarPrice > 0)
        if (!TakeBetPrice(GID1, GID2, p->lWarPrice))
            return false;

    TGuildWarReserve t;
    memset(&t, 0, sizeof(TGuildWarReserve));

    t.dwGuildFrom = GID1;
    t.dwGuildTo = GID2;
    t.dwTime = CClientManager::instance().GetCurrentTime() + 10;
    t.bType = p->bType;
    t.lWarPrice = p->lWarPrice;
    t.lInitialScore = p->lInitialScore;

    int lvp, rkp, alv, mc;

    TGuild & k1 = TouchGuild(GID1);

    lvp = c_aiScoreByLevel[std::min(20, k1.level)];
    rkp = c_aiScoreByRanking[GetRanking(GID1)];
    alv = GetAverageGuildMemberLevel(GID1);
    mc = GetGuildMemberCount(GID1);

    polyPower.SetVar("lvp", lvp);
    polyPower.SetVar("rkp", rkp);
    polyPower.SetVar("alv", alv);
    polyPower.SetVar("mc", mc);

    t.lPowerFrom = (long) polyPower.Eval();
    sys_log(0, "GuildWar: %lu lvp %d rkp %d alv %d mc %d power %ld", GID1, lvp, rkp, alv, mc, static_cast<long>(t.lPowerFrom));

    TGuild & k2 = TouchGuild(GID2);

    lvp = c_aiScoreByLevel[std::min(20, k2.level)];
    rkp = c_aiScoreByRanking[GetRanking(GID2)];
    alv = GetAverageGuildMemberLevel(GID2);
    mc = GetGuildMemberCount(GID2);

    polyPower.SetVar("lvp", lvp);
    polyPower.SetVar("rkp", rkp);
    polyPower.SetVar("alv", alv);
    polyPower.SetVar("mc", mc);

    t.lPowerTo = (long) polyPower.Eval();
    sys_log(0, "GuildWar: %lu lvp %d rkp %d alv %d mc %d power %ld", GID2, lvp, rkp, alv, mc, static_cast<long>(t.lPowerTo));

    if (t.lPowerTo > t.lPowerFrom)
    {
        polyHandicap.SetVar("pA", t.lPowerTo);
        polyHandicap.SetVar("pB", t.lPowerFrom);
    }
    else
    {
        polyHandicap.SetVar("pA", t.lPowerFrom);
        polyHandicap.SetVar("pB", t.lPowerTo);
    }

    t.lHandicap = (long) polyHandicap.Eval();
    sys_log(0, "GuildWar: handicap %ld", static_cast<long>(t.lHandicap));

    char szQuery[513];

    dbsql::snprintf(szQuery, sizeof(szQuery) - 1,
            "INSERT INTO guild_war_reservation (guild1, guild2, time, type, warprice, initscore, power1, power2, handicap) "
            "VALUES(%lu, %lu, DATE_ADD(NOW(), INTERVAL 180 SECOND), %u, %ld, %ld, %ld, %ld, %ld)",
            GID1, GID2, static_cast<unsigned>(p->bType), static_cast<long>(p->lWarPrice),
            static_cast<long>(p->lInitialScore), static_cast<long>(t.lPowerFrom), static_cast<long>(t.lPowerTo), static_cast<long>(t.lHandicap));

    std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

    if (pmsg->Get()->iAffectedRows <= 0 || pmsg->Get()->iInsertID <= 0)
    {
        sys_err("GuildWar: Cannot insert row");
        return false;
    }

    t.dwID = pmsg->Get()->iInsertID;

    m_map_kWarReserve.insert(std::make_pair(t.dwID, new CGuildWarReserve(t)));

    CClientManager::instance().ForwardPacket(RESERVE_ADD_HEADER, &t, sizeof(TGuildWarReserve));
    return true;
}

void CGuildManager::ProcessReserveWar()
{
    DWORD dwCurTime = CClientManager::instance().GetCurrentTime();

    std::map<DWORD, CGuildWarReserve*>::iterator it = m_map_kWarReserve.begin();

    while (it != m_map_kWarReserve.end())
    {
        std::map<DWORD, CGuildWarReserve*>::iterator it2 = it++;

        CGuildWarReserve * pk = it2->second;
        TGuildWarReserve & r = pk->GetDataRef();

        if (!r.bStarted && r.dwTime - 1800 <= dwCurTime)
        {
            int iMin = static_cast<int>(std::ceil(static_cast<float>(static_cast<int>(r.dwTime - dwCurTime)) / 60.0f));

            TGuild & r_1 = m_map_kGuild[r.dwGuildFrom];
            TGuild & r_2 = m_map_kGuild[r.dwGuildTo];

            sys_log(0, "GuildWar: started GID1 %lu GID2 %lu %d time %d min %d", static_cast<unsigned long>(r.dwGuildFrom), static_cast<unsigned long>(r.dwGuildTo), r.bStarted, static_cast<int>(dwCurTime - r.dwTime), iMin);

            if (iMin <= 0)
            {
                char szQuery[129];
                dbsql::snprintf(szQuery, sizeof(szQuery) - 1, "UPDATE guild_war_reservation SET started=1 WHERE id=%lu", static_cast<unsigned long>(r.dwID));
                CDBManager::instance().AsyncQuery(szQuery);

                CClientManager::instance().ForwardPacket(RESERVE_DEL_HEADER, &r.dwID, sizeof(r.dwID));

                r.bStarted = true;

                TGuildWaitStartInfo info(r.bType, r.dwGuildFrom, r.dwGuildTo, r.lWarPrice, r.lInitialScore, pk);
                m_pqWaitStart.push(std::make_pair(dwCurTime + 5, info));

                TPacketGuildWar pck;

                pck.bType = r.bType;
                pck.bWar = WAR_STATE_WAIT_START;
                pck.dwGuildFrom = r.dwGuildFrom;
                pck.dwGuildTo = r.dwGuildTo;
                pck.lWarPrice = r.lWarPrice;
                pck.lInitialScore = r.lInitialScore;

                CClientManager::instance().ForwardPacket(GUILD_WAR_HEADER, &pck, sizeof(TPacketGuildWar));

            }
            else
            {
                if (iMin != pk->GetLastNoticeMin())
                {
                    pk->SetLastNoticeMin(iMin);

                    if (!g_stLocale.compare("euckr"))
                        CClientManager::instance().SendNotice("%s \261\346\265\345\277\315 %s \261\346\265\345\300\307 \300\374\300\357\300\314 \276\340 %d\272\320 \310\304 \275\303\300\333 \265\313\264\317\264\331!", r_1.szName, r_2.szName, iMin);
                    else if (!g_stLocale.compare("gb2312"))
                        CClientManager::instance().SendNotice("%s \260\357\273\341\272\315 %s \260\357\273\341\265\304\260\357\273\341\325\275\325\371\275\253\324\332 %d\267\326\326\323\272\363\277\252\312\274!", r_1.szName, r_2.szName, iMin);
                    else
                        CClientManager::instance().SendNotice("");
                }
            }
        }
    }
}

bool CGuildManager::Bet(DWORD dwID, const char * c_pszLogin, DWORD dwGold, DWORD dwGuild)
{
    const auto login = dbsql::ReadField(c_pszLogin, LOGIN_MAX_LEN + 1);
    if (!login)
        return false;
    const auto escapedLogin = dbsql::EscapeLiteral(*login);
    if (!escapedLogin)
        return false;

    std::map<DWORD, CGuildWarReserve*>::iterator it = m_map_kWarReserve.find(dwID);

    char szQuery[1025];

    if (it == m_map_kWarReserve.end())
    {
        sys_log(0, "WAR_RESERVE: Bet: cannot find reserve war by id %lu", dwID);
        dbsql::snprintf(szQuery, sizeof(szQuery) - 1, "INSERT INTO item_award (login, vnum, socket0, given_time) VALUES('%s', %d, %lu, NOW())",
                *escapedLogin, 50026, dwGold);
        CDBManager::instance().AsyncQuery(szQuery);
        return false;
    }

    if (!it->second->Bet(c_pszLogin, dwGold, dwGuild))
    {
        sys_log(0, "WAR_RESERVE: Bet: cannot bet id %lu, login %s, gold %lu, guild %lu", dwID, c_pszLogin, dwGold, dwGuild);
        dbsql::snprintf(szQuery, sizeof(szQuery) - 1, "INSERT INTO item_award (login, vnum, socket0, given_time) VALUES('%s', %d, %lu, NOW())",
                *escapedLogin, 50026, dwGold);
        CDBManager::instance().AsyncQuery(szQuery);
        return false;
    }

    return true;
}

void CGuildWarReserve::OnSetup(CPeer * peer)
{
    if (m_data.bStarted)
        return;

    FSendPeerWar(m_data.bType, WAR_STATE_RESERVE, m_data.dwGuildFrom, m_data.dwGuildTo) (peer);

    peer->EncodeHeader(RESERVE_ADD_HEADER, 0, sizeof(TGuildWarReserve));
    peer->Encode(&m_data, sizeof(TGuildWarReserve));

    TPacketGDGuildWarBet pckBet;
    pckBet.dwWarID = m_data.dwID;

    std::map<std::string, std::pair<DWORD, DWORD> >::iterator it = mapBet.begin();

    while (it != mapBet.end())
    {
        strncpy(pckBet.szLogin, it->first.c_str(), 30);
        pckBet.szLogin[30] = '\0';
        pckBet.dwGuild = it->second.first;
        pckBet.dwGold = it->second.second;

        peer->EncodeHeader(RESERVE_BET_HEADER, 0, sizeof(pckBet));
        peer->Encode(&pckBet, sizeof(pckBet));

        ++it;
    }
}

bool CGuildWarReserve::Bet(const char * pszLogin, DWORD dwGold, DWORD dwGuild)
{
    const auto login = dbsql::ReadField(pszLogin, LOGIN_MAX_LEN + 1);
    if (!login)
        return false;
    const auto escapedLogin = dbsql::EscapeLiteral(*login);
    if (!escapedLogin)
        return false;

    char szQuery[1025];

    if (m_data.dwGuildFrom != dwGuild && m_data.dwGuildTo != dwGuild)
    {
        sys_log(0, "GuildWarReserve::Bet: invalid guild id");
        return false;
    }

    if (m_data.bStarted)
    {
        sys_log(0, "GuildWarReserve::Bet: war is already started");
        return false;
    }

    if (mapBet.find(pszLogin) != mapBet.end())
    {
        sys_log(0, "GuildWarReserve::Bet: failed. already bet");
        return false;
    }

    dbsql::snprintf(szQuery, sizeof(szQuery) - 1,
            "INSERT INTO guild_war_bet (war_id, login, gold, guild) VALUES(%lu, '%s', %lu, %lu)",
            static_cast<unsigned long>(m_data.dwID), *escapedLogin, dwGold, dwGuild);

    std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

    if (pmsg->Get()->iAffectedRows <= 0)
    {
        sys_log(0, "GuildWarReserve::Bet: failed. cannot insert row to guild_war_bet table");
        return false;
    }

    if (m_data.dwGuildFrom == dwGuild)
        m_data.dwBetFrom += dwGold;
    else
        m_data.dwBetTo += dwGold;

    CClientManager::instance().ForwardPacket(RESERVE_ADD_HEADER, &m_data, sizeof(TGuildWarReserve));

    dbsql::snprintf(szQuery, sizeof(szQuery) - 1, "UPDATE guild_war_reservation SET bet_from=%lu,bet_to=%lu WHERE id=%lu",
            static_cast<unsigned long>(m_data.dwBetFrom), static_cast<unsigned long>(m_data.dwBetTo), static_cast<unsigned long>(m_data.dwID));

    CDBManager::instance().AsyncQuery(szQuery);

    sys_log(0, "GuildWarReserve::Bet: success. %s %lu war_id %lu bet %lu : %lu", pszLogin, dwGuild, static_cast<unsigned long>(m_data.dwID), static_cast<unsigned long>(m_data.dwBetFrom), static_cast<unsigned long>(m_data.dwBetTo));
    mapBet.insert(std::make_pair(pszLogin, std::make_pair(dwGuild, dwGold)));

    TPacketGDGuildWarBet pckBet;
    pckBet.dwWarID = m_data.dwID;
    strncpy(pckBet.szLogin, pszLogin, 30);
    pckBet.szLogin[30] = '\0';
    pckBet.dwGuild = dwGuild;
    pckBet.dwGold = dwGold;

    CClientManager::instance().ForwardPacket(RESERVE_BET_HEADER, &pckBet, sizeof(pckBet));
    return true;
}




void CGuildWarReserve::End(int iScoreFrom, int iScoreTo)
{
    DWORD dwWinner;

    sys_log(0, "WAR_REWARD: End: From %lu %d To %lu %d", static_cast<unsigned long>(m_data.dwGuildFrom), iScoreFrom, static_cast<unsigned long>(m_data.dwGuildTo), iScoreTo);

    if (m_data.lPowerFrom > m_data.lPowerTo)
    {
        if (m_data.lHandicap > iScoreFrom - iScoreTo)
        {
            sys_log(0, "WAR_REWARD: End: failed to overcome handicap, From is strong but To won");
            dwWinner = m_data.dwGuildTo;
        }
        else
        {
            sys_log(0, "WAR_REWARD: End: success to overcome handicap, From win!");
            dwWinner = m_data.dwGuildFrom;
        }
    }
    else
    {
        if (m_data.lHandicap > iScoreTo - iScoreFrom)
        {
            sys_log(0, "WAR_REWARD: End: failed to overcome handicap, To is strong but From won");
            dwWinner = m_data.dwGuildFrom;
        }
        else
        {
            sys_log(0, "WAR_REWARD: End: success to overcome handicap, To win!");
            dwWinner = m_data.dwGuildTo;
        }
    }

    char szQuery[1025];
    dbsql::snprintf(szQuery, sizeof(szQuery) - 1, "UPDATE guild_war_reservation SET started=1,winner=%lu,result1=%d,result2=%d WHERE id=%lu",
            dwWinner, iScoreFrom, iScoreTo, static_cast<unsigned long>(m_data.dwID));
    CDBManager::instance().AsyncQuery(szQuery);

    if (mapBet.empty())
        return;

    DWORD dwTotalBet = m_data.dwBetFrom + m_data.dwBetTo;
    DWORD dwWinnerBet = 0;

    if (dwWinner == m_data.dwGuildFrom)
        dwWinnerBet = m_data.dwBetFrom;
    else if (dwWinner == m_data.dwGuildTo)
        dwWinnerBet = m_data.dwBetTo;
    else
    {
        sys_err("WAR_REWARD: fatal error, winner does not exist!");
        return;
    }

    if (dwWinnerBet == 0)
    {
        sys_err("WAR_REWARD: total bet money on winner is zero");
        return;
    }

    sys_log(0, "WAR_REWARD: End: Total bet: %lu, Winner bet: %lu", dwTotalBet, dwWinnerBet);

    std::map<std::string, std::pair<DWORD, DWORD> >::iterator it = mapBet.begin();

    while (1)
    {
        int iLen = 0;
        int iRow = 0;

        iLen += dbsql::snprintf(szQuery, 512 - iLen, "INSERT INTO item_award (login, vnum, socket0, given_time) VALUES");

        while (it != mapBet.end())
        {
            if (it->second.first != dwWinner)
            {
                ++it;
                continue;
            }

            double ratio = (double) it->second.second / dwWinnerBet;

            sys_log(0, "WAR_REWARD: %s %lu ratio %f", it->first.c_str(), it->second.second, ratio);

            DWORD dwGold = (DWORD) (dwTotalBet * ratio * 0.9);

            const auto login = it->first.size() <= LOGIN_MAX_LEN ? dbsql::EscapeLiteral(it->first) : std::nullopt;
            if (!login)
            {
                ++it;
                continue;
            }
            if (iRow == 0)
                iLen += dbsql::snprintf(szQuery + iLen, 512 - iLen, "('%s', %d, %lu, NOW())",
                        *login, 50026, dwGold);
            else
                iLen += dbsql::snprintf(szQuery + iLen, 512 - iLen, ",('%s', %d, %lu, NOW())",
                        *login, 50026, dwGold);

            ++it;

            if (iLen > 384)
                break;

            ++iRow;
        }

        if (iRow > 0)
        {
            sys_log(0, "WAR_REWARD: query: %s", szQuery);
            CDBManager::instance().AsyncQuery(szQuery);
        }

        if (it == mapBet.end())
            break;
    }
}
