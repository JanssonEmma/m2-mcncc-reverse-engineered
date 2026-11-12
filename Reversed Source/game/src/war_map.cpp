#include "war_map.h"

#include "buffer_manager.h"
#include "char.h"
#include "item.h"
#include "desc.h"
#include "guild.h"
#include "guild_manager.h"
#include "locale_service.h"
#include "../../libthecore/include/log.h"
#include <cassert>
#include <memory>
#include "packet.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "sectree_manager.h"
#include "affect.h"
#include "char_manager.h"
#include "config.h"
#include "locale.hpp"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <cassert>

namespace {
struct WarTimeoutInfo
{
    DWORD id;
    CWarMap* map;
};

struct WarEndInfo
{
    int step;
    CWarMap* map;
};

std::int32_t WarMapDelay(std::uint32_t seconds)
{
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * seconds);
}

long war_begin_event(LPEVENT event)
{
    static_cast<WarTimeoutInfo*>(event->info)->map->CheckWarEnd();
    return WarMapDelay(10);
}

long war_end_event(LPEVENT event)
{
    auto* info = static_cast<WarEndInfo*>(event->info);
    if (info->step == 0)
    {
        info->step = 1;
        info->map->ExitAll();
        return WarMapDelay(5);
    }

    info->map->SetEndEvent(nullptr);
    CWarMapManager::instance().DestroyWarMap(info->map);
    return 0;
}

long war_timeout_event(LPEVENT event)
{
    static_cast<WarTimeoutInfo*>(event->info)->map->Timeout();
    return 0;
}

const char observerMode[] = "\260\374\300\374\040\270\360\265\345\267\316\040\261\346\265\345\300\374\277\241\040\302\374\260\241\307\317\274\314\275\300\264\317\264\331\056";
const char observerExit[] = "\300\332\275\305\300\273\040\274\261\305\303\307\317\275\303\270\351\040\271\333\300\270\267\316\040\263\252\260\245\040\274\366\040\300\326\264\302\040\074\260\374\266\367\040\301\276\267\341\076\040\271\366\306\260\300\314\040\263\252\277\311\264\317\264\331\056";
const char opponentsMissing[] = "\261\346\265\345\300\374\277\241\040\302\374\260\241\307\321\040\273\363\264\353\271\346\040\261\346\265\345\277\370\300\314\040\276\306\271\253\265\265\040\276\370\275\300\264\317\264\331\056";
const char timeoutWarning[] = "\061\272\320\040\300\314\263\273\277\241\040\276\306\271\253\265\265\040\301\242\274\323\307\317\301\366\040\276\312\300\270\270\351\040\261\346\265\345\300\374\300\314\040\300\332\265\277\040\301\276\267\341\265\313\264\317\264\331\056";
const char earlyDraw[] = "\261\346\265\345\300\374\300\314\040\300\317\302\357\040\301\276\267\341\265\307\276\356\040\271\253\275\302\272\316\267\316\040\306\307\301\244\040\265\307\276\372\275\300\264\317\264\331\056\040\050\065\272\320\300\314\040\301\366\263\252\301\366\040\276\312\300\275\051";
}

void CWarMap::STeamData::Initialize()
{
    iMemberCount = 0;
    iUsePotionPrice = 0;
    iScore = 0;
    dwID = 0;
    pkGuild = nullptr;
    pkChrFlag = nullptr;
    pkChrFlagBase = nullptr;
    set_pidJoiner.clear();
}

CWarMap::CWarMap(long mapIndex, const TGuildWarInfo& war, SWarMapInfo* map,
        DWORD first, DWORD second)
    : m_kMapInfo(*map), m_bEnded(false), m_pkBeginEvent(nullptr), m_pkTimeoutEvent(nullptr),
      m_pkEndEvent(nullptr), m_pkResetFlagEvent(nullptr), m_iObserverCount(0),
      m_dwStartTime(0), m_bTimeout(0), m_WarInfo(war)
{
    m_kMapInfo.lMapIndex = mapIndex;
    CGuildManager& guilds = CGuildManager::instance();
    m_TeamData[0].Initialize();
    m_TeamData[0].dwID = first;
    m_TeamData[0].pkGuild = guilds.TouchGuild(first);
    m_TeamData[1].Initialize();
    m_TeamData[1].dwID = second;
    m_TeamData[1].pkGuild = guilds.TouchGuild(second);

    void* data = nullptr;
    const DWORD infoId = __AllocEventInfo(sizeof(WarTimeoutInfo), &data);
    static_cast<WarTimeoutInfo*>(data)->map = this;
    SetBeginEvent(event_create_ex(war_begin_event, infoId, WarMapDelay(60),
        const_cast<char*>("war_map.cpp"), 0x59));
    m_dwStartTime = get_dword_time();

    if (m_kMapInfo.bType == 1)
    {
        AddFlagBase(0, 0, 0);
        AddFlagBase(1, 0, 0);
        AddFlag(0, 0, 0);
        AddFlag(1, 0, 0);
    }
}

CWarMap::~CWarMap()
{
    event_cancel(&m_pkBeginEvent);
    event_cancel(&m_pkEndEvent);
    event_cancel(&m_pkTimeoutEvent);
    event_cancel(&m_pkResetFlagEvent);

    sys_log(0, "WarMap::~WarMap : map index %ld", m_kMapInfo.lMapIndex);
    for (auto it = m_set_pkChr.begin(); it != m_set_pkChr.end();)
    {
        CHARACTER* ch = *it++;
        if (ch->GetDesc())
        {
            sys_log(0, "WarMap::~WarMap : disconnecting %s", ch->GetName());
            ch->Disconnect();
        }
    }
    event_cancel(&m_pkTimeoutEvent);
}

long CWarMap::GetMapIndex()
{
    return m_kMapInfo.lMapIndex;
}

void CWarMap::AddFlagBase(BYTE index, DWORD x, DWORD y)
{
    if (m_bEnded)
        return;
    if (index >= 2) {
        assert(index < 2);
        std::abort();
    }
    STeamData& team = m_TeamData[index];
    if (team.pkChrFlagBase)
        return;
    if (static_cast<std::uint32_t>(x) == 0)
    {
        x = m_kMapInfo.posStart[index].x;
        y = m_kMapInfo.posStart[index].y;
    }

    team.pkChrFlagBase = CHARACTER_MANAGER::instance().SpawnMob(20038, m_kMapInfo.lMapIndex,
        static_cast<std::int32_t>(x), static_cast<std::int32_t>(y), 0, false, -1, true);
    sys_log(0, "WarMap::AddFlagBase %u %p id %lu", static_cast<unsigned int>(index),
        static_cast<void*>(team.pkChrFlagBase), team.dwID);
    if (!team.pkChrFlagBase)
        return;
    team.pkChrFlagBase->SetPoint(POINT_STAT, team.dwID);
    team.pkChrFlagBase->SetWarMap(this);
}

void CWarMap::ExitAll()
{
    for (CHARACTER* ch : m_set_pkChr)
        if (ch->GetDesc())
            ch->ExitToSavedLocation();
}

bool CWarMap::SetEnded()
{
    sys_log(0, "WarMap::SetEnded %ld", m_kMapInfo.lMapIndex);
    if (m_pkEndEvent)
        return false;

    for (STeamData& team : m_TeamData)
    {
        if (team.pkChrFlag)
        {
            CHARACTER_MANAGER::instance().DestroyCharacter(team.pkChrFlag);
            team.pkChrFlag = nullptr;
        }
        if (team.pkChrFlagBase)
        {
            CHARACTER_MANAGER::instance().DestroyCharacter(team.pkChrFlagBase);
            team.pkChrFlagBase = nullptr;
        }
    }
    event_cancel(&m_pkResetFlagEvent);
    m_bEnded = true;
    void* data = nullptr;
    const DWORD infoId = __AllocEventInfo(sizeof(WarEndInfo), &data);
    auto* info = static_cast<WarEndInfo*>(data);
    info->step = 0;
    info->map = this;
    SetEndEvent(event_create_ex(war_end_event, infoId, WarMapDelay(10),
        const_cast<char*>("war_map.cpp"), 0x2db));
    return true;
}

void CWarMap::SetEndEvent(LPEVENT event)
{
    m_pkEndEvent = event;
}

void CWarMap::Draw()
{
    CGuildManager::instance().RequestWarOver(m_TeamData[0].dwID, m_TeamData[1].dwID, 0, 0);
}

void CWarMap::IncMember(CHARACTER* ch)
{
    if (!ch->GetDesc())
        return;

    sys_log(0, "WarMap::IncMember");
    CGuild* guild = ch->GetGuild();
    const DWORD id = guild ? guild->GetID() : 0;
    bool member = ch->GetQuestFlag("war.is_war_member") > 0;
    BYTE index;
    if (member && !GetTeamIndex(id, index))
    {
        ch->SetQuestFlag("war.is_war_member", 0);
        member = false;
    }

    if (member)
    {
        m_TeamData[index].AppendMember(ch);
        event_cancel(&m_pkTimeoutEvent);
        sys_log(0, "WarMap +m %lu(cur:%d, acc:%d) vs %lu(cur:%d, acc:%d)",
            m_TeamData[0].dwID, m_TeamData[0].GetCurJointerCount(), m_TeamData[0].GetAccumulatedJoinerCount(),
            m_TeamData[1].dwID, m_TeamData[1].GetCurJointerCount(), m_TeamData[1].GetAccumulatedJoinerCount());
    }
    else
    {
        sys_log(0, "WarMap +o %d", ++m_iObserverCount);
        ch->SetObserverMode(true);
        ch->ChatPacket(1, locale_find(observerMode, g_iUseLocale));
        ch->ChatPacket(1, locale_find(observerExit, g_iUseLocale));
    }
    UpdateUserCount();
    m_set_pkChr.insert(ch);
    DESC* desc = ch->GetDesc();
    SendWarPacket(desc);
    SendScorePacket(0, desc);
    SendScorePacket(1, desc);
}

void CWarMap::DecMember(CHARACTER* ch)
{
    if (!ch->GetDesc())
        return;

    sys_log(0, "WarMap::DecMember");
    if (ch->IsObserverMode())
    {
        sys_log(0, "WarMap -o %d", --m_iObserverCount);
        ch->SetObserverMode(false);
    }
    else
    {
        CGuild* guild = ch->GetGuild();
        const DWORD id = guild ? guild->GetID() : 0;
        BYTE index;
        if (GetTeamIndex(id, index))
            m_TeamData[index].RemoveMember(ch);
        if (m_kMapInfo.bType == 1)
        {
            const CAffect* flag = ch->FindAffect(222, 0);
            if (flag)
            {
                if (GetTeamIndex(flag->lApplyValue, index))
                    AddFlag(index, ch->GetX(), ch->GetY());
                ch->RemoveAffect(222);
            }
        }
        sys_log(0, "WarMap -m %lu(cur:%d, acc:%d) vs %lu(cur:%d, acc:%d)",
            m_TeamData[0].dwID, m_TeamData[0].GetCurJointerCount(), m_TeamData[0].GetAccumulatedJoinerCount(),
            m_TeamData[1].dwID, m_TeamData[1].GetCurJointerCount(), m_TeamData[1].GetAccumulatedJoinerCount());
        CheckWarEnd();
        ch->SetQuestFlag("war.is_war_member", 0);
    }

    UpdateUserCount();
    m_set_pkChr.erase(ch);
}

void CWarMap::AddFlag(BYTE index, DWORD x, DWORD y)
{
    if (m_bEnded)
        return;
    if (index >= 2) {
        assert(index < 2);
        std::abort();
    }
    STeamData& team = m_TeamData[index];
    if (team.pkChrFlag)
        return;
    if (static_cast<std::uint32_t>(x) == 0)
    {
        x = m_kMapInfo.posStart[index].x;
        y = m_kMapInfo.posStart[index].y;
    }

    team.pkChrFlag = CHARACTER_MANAGER::instance().SpawnMob(20035 + index, m_kMapInfo.lMapIndex,
        static_cast<std::int32_t>(x), static_cast<std::int32_t>(y), 0, false, -1, true);
    sys_log(0, "WarMap::AddFlag %u %p id %lu", static_cast<unsigned int>(index),
        static_cast<void*>(team.pkChrFlag), team.dwID);
    if (!team.pkChrFlag)
        return;
    team.pkChrFlag->SetPoint(POINT_STAT, team.dwID);
    team.pkChrFlag->SetWarMap(this);
}

void CWarMap::Notice(const char* text)
{
    for (CHARACTER* ch : m_set_pkChr)
        ch->ChatPacket(2, "%s", text);
}

void CWarMap::CheckWarEnd()
{
    if (m_bEnded)
        return;

    if (m_TeamData[0].iMemberCount != 0 && m_TeamData[1].iMemberCount != 0)
    {
        CheckScore();
        return;
    }

    if (m_bTimeout || m_pkTimeoutEvent)
        return;
    Notice(locale_find(opponentsMissing, g_iUseLocale));
    Notice(locale_find(timeoutWarning, g_iUseLocale));
    sys_log(0, "CheckWarEnd: Timeout begin %lu vs %lu", m_TeamData[0].dwID, m_TeamData[1].dwID);
    void* data = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(WarTimeoutInfo), &data);
    static_cast<WarTimeoutInfo*>(data)->map = this;
    SetTimeoutEvent(event_create_ex(war_timeout_event, id, WarMapDelay(60),
        const_cast<char*>("war_map.cpp"), 0x1bd));
}

int CWarMap::GetRewardGold(BYTE winner)
{
    const auto potionReward = [](int percent, int total) {
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(percent) * total) / 100;
    };
    const auto warPrice = static_cast<std::uint32_t>(m_WarInfo.iWarPrice);
    return static_cast<std::int32_t>(warPrice +
        potionReward(m_WarInfo.iWinnerPotionRewardPctToWinner, m_TeamData[winner].iUsePotionPrice) +
        potionReward(m_WarInfo.iLoserPotionRewardPctToWinner, m_TeamData[winner ? 0 : 1].iUsePotionPrice));
}

DWORD CWarMap::GetWinnerGuild()
{
    const int first = m_TeamData[0].iScore;
    const int second = m_TeamData[1].iScore;
    if (first == second)
        return 0;
    return m_TeamData[first > second ? 0 : 1].dwID;
}

void CWarMap::UpdateScore(DWORD first, int firstScore, DWORD second, int secondScore)
{
    BYTE index;
    if (GetTeamIndex(first, index) && m_TeamData[index].iScore != firstScore)
    {
        m_TeamData[index].iScore = firstScore;
        SendScorePacket(index);
    }
    if (GetTeamIndex(second, index) && m_TeamData[index].iScore != secondScore)
    {
        m_TeamData[index].iScore = secondScore;
        SendScorePacket(index);
    }
    CheckScore();
}

bool CWarMap::CheckScore()
{
    if (m_bEnded)
        return true;
    if (static_cast<std::uint32_t>(get_dword_time() - m_dwStartTime) < 30000)
        return false;

    const int first = m_TeamData[0].iScore;
    const int second = m_TeamData[1].iScore;
    const int endScore = test_server ? m_WarInfo.iEndScore / 10 : m_WarInfo.iEndScore;
    if (first == second || (first < endScore && second < endScore))
        return false;

    const BYTE index = first >= endScore ? 0 : 1;
    const DWORD winner = m_TeamData[index].dwID;
    const DWORD loser = m_TeamData[index ? 0 : 1].dwID;
    const int reward = GetRewardGold(index);
    sys_log(0, "WarMap::CheckScore end score %d guild1 %lu score guild2 %d %lu score %d winner %lu reward %d",
        endScore, m_TeamData[0].dwID, first, m_TeamData[1].dwID, second, winner, reward);
    CGuildManager::instance().RequestWarOver(winner, loser, winner, reward);
    return true;
}

void CWarMap::Timeout()
{
    m_pkTimeoutEvent = nullptr;
    if (m_bTimeout || m_bEnded)
        return;

    DWORD winner = 0;
    DWORD loser = 0;
    int reward = 0;
    if (static_cast<std::uint32_t>(get_dword_time() - m_dwStartTime) < 300000)
        Notice(locale_find(earlyDraw, g_iUseLocale));
    else if (m_TeamData[0].iMemberCount && m_TeamData[1].iMemberCount)
    {
        winner = GetWinnerGuild();
        BYTE index;
        if (GetTeamIndex(winner, index))
        {
            loser = m_TeamData[index ? 0 : 1].dwID;
            reward = GetRewardGold(index);
        }
        _sys_err("Timeout", 0x1ff,
            "WarMap: member count is not zero, guild1 %lu %d guild2 %lu %d, winner %lu",
            m_TeamData[0].dwID, m_TeamData[0].iMemberCount, m_TeamData[1].dwID, m_TeamData[1].iMemberCount, winner);
    }
    else
    {
        const BYTE index = m_TeamData[0].iMemberCount ? 0 : 1;
        winner = m_TeamData[index].dwID;
        loser = m_TeamData[index ? 0 : 1].dwID;
        reward = GetRewardGold(index);
    }

    sys_log(0, "WarMap: Timeout %lu %lu winner %lu loser %lu reward %d map %ld",
        m_TeamData[0].dwID, m_TeamData[1].dwID, winner, loser, reward, m_kMapInfo.lMapIndex);
    CGuildManager::instance().RequestWarOver(winner ? winner : m_TeamData[0].dwID,
        winner ? loser : m_TeamData[1].dwID, winner, reward);
    m_bTimeout = 1;
}

DWORD CWarMap::GetGuildOpponent(CHARACTER* character)
{
    BYTE team;
    CGuild* guild = character->GetGuild();
    return guild && GetTeamIndex(guild->GetID(), team) ? m_TeamData[1 - team].dwID : 0;
}

bool CWarMap::GetTeamIndex(DWORD guild, BYTE& index)
{
    if (m_TeamData[0].dwID == guild)
        index = 0;
    else if (m_TeamData[1].dwID == guild)
        index = 1;
    else
        return false;
    return true;
}

int CWarMap::STeamData::GetAccumulatedJoinerCount()
{
    return static_cast<int>(set_pidJoiner.size());
}

int CWarMap::STeamData::GetCurJointerCount()
{
    return iMemberCount;
}

void CWarMap::STeamData::AppendMember(CHARACTER* ch)
{
    set_pidJoiner.insert(ch->GetPlayerID());
    ++iMemberCount;
}

void CWarMap::STeamData::RemoveMember(CHARACTER*)
{
    --iMemberCount;
}

void CWarMap::UpdateUserCount()
{
    char observer[30];
    char users[30];
    std::snprintf(observer, sizeof(observer), "ObserverCount %d", m_iObserverCount);
    std::snprintf(users, sizeof(users), "WarUC %lu %d %lu %d %d",
        m_TeamData[0].dwID, m_TeamData[0].GetAccumulatedJoinerCount(),
        m_TeamData[1].dwID, m_TeamData[1].GetAccumulatedJoinerCount(), m_iObserverCount);
    for (CHARACTER* ch : m_set_pkChr)
    {
        ch->ChatPacket(5, "%s", observer);
        ch->ChatPacket(5, "%s", users);
    }
}

void CWarMap::Packet(const void* data, int size)
{
    for (CHARACTER* ch : m_set_pkChr)
        if (DESC* desc = ch->GetDesc())
            desc->Packet(data, size);
}

void CWarMap::SendWarPacket(DESC* desc)
{
    const TPacketGCGuild packet = {0x4b, 0x0e, 0x0f};
    const std::uint32_t self = m_TeamData[0].dwID;
    const std::uint32_t opponent = m_TeamData[1].dwID;
    BYTE data[10];
    std::memcpy(data, &self, sizeof(self));
    std::memcpy(data + sizeof(self), &opponent, sizeof(opponent));
    CGuildManager& manager = CGuildManager::instance();
    data[8] = static_cast<BYTE>(manager.TouchGuild(self)->GetGuildWarType(opponent));
    data[9] = static_cast<BYTE>(manager.TouchGuild(self)->GetGuildWarState(opponent));
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->Packet(data, sizeof(data));
}

void CWarMap::SendScorePacket(BYTE index, DESC* desc)
{
    const TPacketGCGuild packet = {0x4b, 0x10, 0x13};
    const std::uint32_t guild = m_TeamData[index].dwID;
    const std::uint32_t opponent = m_TeamData[index ? 0 : 1].dwID;
    const std::int32_t score = m_TeamData[index].iScore;
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(&guild, sizeof(guild));
    buffer.write(&opponent, sizeof(opponent));
    buffer.write(&score, sizeof(score));
    if (desc)
        desc->Packet(buffer.read_peek(), buffer.size());
    else
        Packet(buffer.read_peek(), buffer.size());
}

CWarMapManager::CWarMapManager() = default;

CWarMapManager::~CWarMapManager()
{
    for (const auto& entry : m_map_kWarMapInfo)
        delete entry.second;
}

bool CWarMapManager::LoadWarMapInfo(const char*)
{
    const SWarMapInfo defaultMaps[] = {
        {0, 110, {{0x8fc0, 0x1450, 0}, {0xc47c, 0x5078, 0}, {0xb414, 0x2db4, 0}}},
        {1, 111, {{0xfb90, 0x1af4, 0}, {0x123cc, 0x4718, 0}, {0x110a8, 0x332c, 0}}},
    };
    const SWarMapInfo cibnMaps[] = {
        {0, 103, {{0xb284, 0x14500, 0}, {0x51a4, 0xd930, 0}, {0x95c6, 0xf9e8, 0}}},
        {1, 105, {{0x3458, 7000, 0}, {0x5d5c, 18000, 0}, {0x3458, 7000, 0}}},
    };

    const auto& maps = LC_IsCIBN() ? cibnMaps : defaultMaps;
    for (const SWarMapInfo& info : maps)
    {
        std::unique_ptr<SWarMapInfo> entry(new SWarMapInfo(info));
        if (m_map_kWarMapInfo.emplace(info.lMapIndex, entry.get()).second)
            entry.release();
    }
    return true;
}

bool CWarMapManager::GetStartPosition(long mapIndex, BYTE index, PIXEL_POSITION& position)
{
    assert(index < 3);
    SWarMapInfo* map = GetWarMapInfo(mapIndex);
    if (!map)
    {
        sys_log(0, "GetStartPosition FAILED [%d] WarMapInfoSize(%d)",
            static_cast<int>(mapIndex), static_cast<int>(m_map_kWarMapInfo.size()));
        for (const auto& entry : m_map_kWarMapInfo)
        {
            const PIXEL_POSITION& start = entry.second->posStart[index];
            sys_log(0, "WarMap[%d]=Pos(%d, %d)", static_cast<int>(entry.first),
                static_cast<int>(start.x), static_cast<int>(start.y));
        }
        return false;
    }

    position = map->posStart[index];
    return true;
}

long CWarMapManager::CreateWarMap(const TGuildWarInfo& war, DWORD first, DWORD second)
{
    SWarMapInfo* info = GetWarMapInfo(war.lMapIndex);
    if (!info)
    {
        _sys_err("CreateWarMap", 0x3fa, "GuildWar.CreateWarMap.NOT_FOUND_MAPINFO[%ld]", war.lMapIndex);
        return 0;
    }

    SECTREE_MANAGER& sectrees = SECTREE_MANAGER::instance();
    const long index = sectrees.CreatePrivateMap(war.lMapIndex);
    if (!index)
        return 0;

    try
    {
        const auto entry = m_mapWarMap.emplace(index, nullptr);
        if (!entry.second)
            return 0;
        try
        {
            entry.first->second = new CWarMap(index, war, info, first, second);
        }
        catch (...)
        {
            m_mapWarMap.erase(entry.first);
            throw;
        }
    }
    catch (...)
    {
        sectrees.DestroyPrivateMap(index);
        throw;
    }
    return index;
}

void CWarMapManager::DestroyWarMap(CWarMap* map)
{
    const long index = map->GetMapIndex();
    sys_log(0, "WarMap::DestroyWarMap : %ld", index);
    m_mapWarMap.erase(index);
    delete map;
    SECTREE_MANAGER::instance().DestroyPrivateMap(index);
}

void CWarMapManager::OnShutdown()
{
    for (auto it = m_mapWarMap.begin(); it != m_mapWarMap.end();)
        (it++)->second->Draw();
}

void CWarMap::OnKill(CHARACTER* killer, CHARACTER* victim)
{
    if (m_bEnded)
        return;

    CGuild* attacker = killer->GetGuild();
    CGuild* defender = victim->GetGuild();
    const DWORD attackerId = attacker ? attacker->GetID() : 0;
    const DWORD defenderId = defender ? defender->GetID() : 0;
    sys_log(0, "WarMap::OnKill %lu %lu", attackerId, defenderId);

    BYTE team;
    if (!GetTeamIndex(attackerId, team) || !GetTeamIndex(defenderId, team))
        return;

    if (m_kMapInfo.bType == 0) {
        SendGuildWarScore(attackerId, defenderId, 1, victim->GetLevel());
    } else if (m_kMapInfo.bType == 1) {
        if (CAffect* affect = victim->FindAffect(0xde)) {
            if (GetTeamIndex(affect->lApplyValue, team))
                AddFlag(team, victim->GetX(), victim->GetY());
            victim->RemoveAffect(0xde);
        }
    } else {
        _sys_err("OnKill", 0x30c, "unknown war map type %u index %ld",
                 static_cast<unsigned>(m_kMapInfo.bType), m_kMapInfo.lMapIndex);
    }
}

CWarMap* CWarMapManager::Find(long mapIndex)
{
    const auto found = m_mapWarMap.find(mapIndex);
    return found == m_mapWarMap.end() ? nullptr : found->second;
}

SWarMapInfo* CWarMapManager::GetWarMapInfo(long mapIndex)
{
    auto baseIndex = static_cast<std::int32_t>(mapIndex);
    if (baseIndex >= 10000)
        baseIndex /= 10000;
    const auto found = m_map_kWarMapInfo.find(baseIndex);
    return found == m_map_kWarMapInfo.end() ? nullptr : found->second;
}

bool CWarMapManager::IsWarMap(long mapIndex)
{
    return GetWarMapInfo(mapIndex) != nullptr;
}

// 0816a720
void CWarMap::UsePotion(CHARACTER* character, CItem* item)
{
    if (m_pkEndEvent || character->IsObserverMode())
        return;
    CGuild* guild = character->GetGuild();
    const TItemTable* proto = item->GetProto();
    if (!guild || !proto)
        return;
    BYTE index;
    if (GetTeamIndex(guild->GetID(), index)) {
        int& price = m_TeamData[index].iUsePotionPrice;
        price = static_cast<std::int32_t>(static_cast<std::uint32_t>(price) + proto->dwGold);
    }
}

namespace {
long war_reset_flag_event(LPEVENT event)
{
    CWarMap* map = static_cast<WarEndInfo*>(event->info)->map;
    map->AddFlag(0, 0, 0);
    map->AddFlag(1, 0, 0);
    map->SetResetFlagEvent(nullptr);
    return 0;
}
}

// 0816a600
DWORD CWarMap::GetGuildID(BYTE index)
{
    if (index >= 2) {
        assert(index < 2);
        std::abort();
    }
    return m_TeamData[index].dwID;
}

// 0816a680
BYTE CWarMap::GetType()
{
    return m_kMapInfo.bType;
}

// 0816a580
void CWarMap::SetBeginEvent(LPEVENT event)
{
    m_pkBeginEvent = event;
}

// 0816a5a0
void CWarMap::SetTimeoutEvent(LPEVENT event)
{
    m_pkTimeoutEvent = event;
}

CGuild* CWarMap::GetGuild(BYTE index)
{
    return m_TeamData[index].pkGuild;
}

void CWarMap::RemoveFlag(BYTE index)
{
    if (index >= 2) {
        assert(index < 2);
        std::abort();
    }
    CHARACTER*& flag = m_TeamData[index].pkChrFlag;
    if (flag) {
        sys_log(0, "WarMap::RemoveFlag %u %p", static_cast<unsigned>(index), static_cast<void*>(flag));
        flag->Dead(nullptr, true);
        flag = nullptr;
    }
}

bool CWarMap::IsFlagOnBase(BYTE index)
{
    if (index >= 2) {
        assert(index < 2);
        std::abort();
    }
    CHARACTER* flag = m_TeamData[index].pkChrFlag;
    return flag && flag->GetX() == m_kMapInfo.posStart[index].x &&
           flag->GetY() == m_kMapInfo.posStart[index].y;
}

void CWarMap::SetResetFlagEvent(LPEVENT event)
{
    m_pkResetFlagEvent = event;
}

void CWarMap::ResetFlag()
{
    if (m_kMapInfo.bType != 1 || m_pkResetFlagEvent || m_bEnded)
        return;

    for (CHARACTER* character : m_set_pkChr) {
        if (character->FindAffect(222))
            character->RemoveAffect(222);
    }
    RemoveFlag(0);
    RemoveFlag(1);

    void* data = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(WarEndInfo), &data);
    auto* info = static_cast<WarEndInfo*>(data);
    info->map = this;
    info->step = 0;
    SetResetFlagEvent(event_create_ex(war_reset_flag_event, id, WarMapDelay(5),
                                     const_cast<char*>("war_map.cpp"), 0x38d));
}
