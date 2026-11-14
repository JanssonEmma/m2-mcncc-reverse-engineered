#include "guild.h"
#include "../../libthecore/include/log.h"

#include "buffer_manager.h"
#include "char.h"
#include "config.h"
#include "desc.h"
#include "guild_manager.h"
#include "locale.hpp"
#include "locale_service.h"
#include "packet.h"
#include "p2p.h"
#include "questmanager.h"
#include "utils.h"
#include "war_map.h"
#include "sectree_manager.h"
#include "desc_client.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {
const char warStarted[] = "\074\261\346\265\345\076\040\261\346\265\345\300\374\301\337\277\241\264\302\040\273\347\263\311\277\241\040\265\373\270\245\040\300\314\300\315\300\314\040\276\370\275\300\264\317\264\331\056";

#pragma pack(push, 1)
struct GuildWarMapIndexPacket {
    BYTE header;
    std::uint32_t first;
    std::uint32_t second;
    std::int32_t map;
};
#pragma pack(pop)
static_assert(sizeof(GuildWarMapIndexPacket) == 13, "Guild war map peer packet");
}

const TGuildWarInfo& GuildWar_GetTypeInfo(unsigned type)
{
    return (LC_IsCIBN() ? CHN_aGuildWarInfo : KOR_aGuildWarInfo)[type];
}

unsigned GuildWar_GetTypeMapIndex(unsigned type)
{
    return GuildWar_GetTypeInfo(type).lMapIndex;
}

bool GuildWar_IsWarMap(unsigned type)
{
    return type == 0 || SECTREE_MANAGER::instance().GetMapRegion(GuildWar_GetTypeMapIndex(type));
}

namespace
{
void SendWarRequest(BYTE type, BYTE state, DWORD from, DWORD to, int price = 0, int score = 0)
{
    const TPacketGuildWar packet = {
        type, state, static_cast<std::uint32_t>(from), static_cast<std::uint32_t>(to), price, score
    };
    db_clientdesc->DBPacket(0x1a, 0, &packet, sizeof(packet));
}
}

int CGuild::GetWarPrice(BYTE type)
{
    return type < 3 ? GuildWar_GetTypeInfo(type).iWarPrice : 0;
}

bool CGuild::CanStartWar(BYTE type)
{
    if (type >= 3)
        return false;
    if (!test_server && !quest::CQuestManager::instance().GetEventFlag("guild_war_test") && GetMemberCount() < 8)
        return false;
    return GetLadderPoint() > 0;
}

void CGuild::RequestDeclareWar(DWORD opponent, BYTE type)
{
    if (opponent == GetID())
    {
        sys_log(0, "GuildWar.DeclareWar.DECLARE_WAR_SELF id(%d -> %d), type(%d)",
                static_cast<int>(GetID()), static_cast<int>(opponent), type);
        return;
    }
    if (type >= 3)
    {
        sys_log(0, "GuildWar.DeclareWar.UNKNOWN_WAR_TYPE id(%d -> %d), type(%d)",
                static_cast<int>(GetID()), static_cast<int>(opponent), type);
        return;
    }

    const auto war = m_EnemyGuild.find(opponent);
    const char* notice = nullptr;
    if (war == m_EnemyGuild.end())
    {
        if (GuildWar_IsWarMap(type))
        {
            SendWarRequest(type, 1, GetID(), opponent);
            sys_log(0, "GuildWar.DeclareWar id(%d -> %d), type(%d)",
                    static_cast<int>(GetID()), static_cast<int>(opponent), type);
            return;
        }
        _sys_err("RequestDeclareWar", 0x136, "GuildWar.DeclareWar.NOT_EXIST_MAP id(%d -> %d), type(%d), map(%d)",
                 static_cast<int>(GetID()), static_cast<int>(opponent), type, static_cast<int>(GuildWar_GetTypeMapIndex(type)));
    }
    else if (war->second.state == 1)
        notice = "\300\314\271\314 \274\261\300\374\306\367\260\355 \301\337\300\316 \261\346\265\345\300\324\264\317\264\331.";
    else if (war->second.state != 3)
    {
        _sys_err("RequestDeclareWar", 0x180, "GuildWar.DeclareWar.UNKNOWN_STATE[%d]: id(%d vs %d), type(%d), guild(%s:%lu)",
                 war->second.state, static_cast<int>(GetID()), static_cast<int>(opponent), type, GetName(), GetID());
        return;
    }
    else
    {
        const BYTE savedType = war->second.type;
        if (savedType == 0)
        {
            SendWarRequest(0, 6, GetID(), opponent);
            sys_log(0, "GuildWar.AcceptWar id(%d -> %d), type(%d)", static_cast<int>(GetID()), static_cast<int>(opponent), 0);
            return;
        }
        if (savedType < 3 && GuildWar_IsWarMap(savedType))
        {
            const TGuildWarInfo& info = GuildWar_GetTypeInfo(savedType);
            SendWarRequest(savedType, 4, GetID(), opponent, info.iWarPrice,
                           test_server ? info.iInitialScore / 10 : info.iInitialScore);
            sys_log(0, "GuildWar.WaitStartSendToDB id(%d vs %d), type(%d), bet(%d), map_index(%d)",
                    static_cast<int>(GetID()), static_cast<int>(opponent), savedType, info.iWarPrice,
                    static_cast<int>(info.lMapIndex));
            return;
        }
        _sys_err("RequestDeclareWar", 0x15e, "GuildWar.AcceptWar.NOT_EXIST_MAP id(%d -> %d), type(%d), map(%d)",
                 static_cast<int>(GetID()), static_cast<int>(opponent), type, static_cast<int>(GuildWar_GetTypeMapIndex(type)));
    }

    if (!notice)
    {
        map_allow_log();
        notice = "\300\374\300\357 \274\255\271\366\260\241 \277\255\267\301\300\326\301\366 \276\312\276\306 \261\346\265\345\300\374\300\273 \275\303\300\333\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    }
    NotifyGuildMaster(locale_find(notice, g_iUseLocale));
}

void CGuild::NotifyGuildMaster(const char* message)
{
    if (CHARACTER* master = GetMasterCharacter())
        master->ChatPacket(1, "%s", message);
}

void CGuild::RequestRefuseWar(DWORD opponent)
{
    if (opponent != GetID() && GetGuildWarState(opponent) == 3)
        SendWarRequest(0, 2, GetID(), opponent);
}

SGuildWar::SGuildWar(BYTE warType)
    : war_start_time(0), score(0), state(0), type(warType), map_index(0)
{
}

DWORD CGuild::GetWarStartTime(DWORD opponent)
{
    if (opponent == GetID())
        return 0;
    const auto war = m_EnemyGuild.find(opponent);
    return war != m_EnemyGuild.end() ? war->second.war_start_time : 0;
}

void CGuild::GuildWarPacket(DWORD opponent, BYTE type, BYTE state)
{
    const TPacketGCGuild packet = {0x4b, 14, 0x0f};
    const std::uint32_t ids[] = {static_cast<std::uint32_t>(GetID()), static_cast<std::uint32_t>(opponent)};
    BYTE data[10];
    std::memcpy(data, ids, sizeof(ids));
    data[8] = type;
    data[9] = state;
    for (CHARACTER* member : m_memberOnline)
    {
        if (state == 6)
            member->ChatPacket(1, locale_find(warStarted, g_iUseLocale));
        if (DESC* desc = member->GetDesc())
        {
            desc->BufferedPacket(&packet, sizeof(packet));
            desc->Packet(data, sizeof(data));
        }
    }
}

bool CGuild::DeclareWar(DWORD opponent, BYTE type, BYTE state)
{
    TGuildWar war(type);
    war.state = state;
    if (!m_EnemyGuild.emplace(opponent, war).second)
        return false;
    GuildWarPacket(opponent, type, state);
    return true;
}

void CGuild::RefuseWar(DWORD opponent)
{
    if (opponent == GetID())
        return;
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end() || (war->second.state != 1 && war->second.state != 3))
        return;
    const BYTE type = war->second.type;
    m_EnemyGuild.erase(war);
    GuildWarPacket(opponent, type, 7);
}

void CGuild::ReserveWar(DWORD opponent, BYTE type)
{
    if (opponent == GetID())
        return;
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end()) {
        TGuildWar reservation(type);
        reservation.state = 9;
        m_EnemyGuild.emplace(opponent, reservation);
    } else {
        war->second.state = 9;
    }
    sys_log(0, "Guild::ReserveWar %lu", static_cast<unsigned long>(static_cast<std::uint32_t>(opponent)));
}

bool CGuild::CheckStartWar(DWORD opponent)
{
    const auto war = m_EnemyGuild.find(opponent);
    return war != m_EnemyGuild.end() && war->second.state != 6;
}

bool CGuild::WaitStartWar(DWORD opponent)
{
    const DWORD self = GetID();
    if (opponent == self)
    {
        sys_log(0, "GuildWar.WaitStartWar.DECLARE_WAR_SELF id(%lu -> %lu)", self, opponent);
        return false;
    }
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end())
    {
        sys_log(0, "GuildWar.WaitStartWar.UNKNOWN_ENEMY id(%lu -> %lu)", self, opponent);
        return false;
    }
    if (war->second.state == 4)
    {
        sys_log(0, "GuildWar.WaitStartWar.UNKNOWN_STATE id(%lu -> %lu), state(%d)", self, opponent, 4);
        return false;
    }
    war->second.state = 4;
    CGuildManager& guilds = CGuildManager::instance();
    CGuild* enemy = guilds.FindGuild(opponent);
    if (!enemy)
    {
        sys_log(0, "GuildWar.WaitStartWar.NOT_EXIST_GUILD id(%lu -> %lu)", self, opponent);
        return false;
    }

    const BYTE type = war->second.type;
    if (type == 0)
    {
        sys_log(0, "GuildWar.WaitStartWar.FIELD_TYPE id(%lu -> %lu)", self, opponent);
        return true;
    }
    if (type >= 3)
        return false;
    const TGuildWarInfo& info = GuildWar_GetTypeInfo(type);
    sys_log(0, "GuildWar.WaitStartWar.CheckWarServer id(%lu -> %lu), type(%d), map(%d)",
        self, opponent, static_cast<int>(type), static_cast<int>(info.lMapIndex));
    if (!map_allow_find(info.lMapIndex))
    {
        sys_log(0, "GuildWar.WaitStartWar.SKIP_WAR_MAP id(%lu -> %lu)", self, opponent);
        return true;
    }

    const DWORD first = std::min(self, opponent);
    const DWORD second = std::max(self, opponent);
    const long index = CWarMapManager::instance().CreateWarMap(info, first, second);
    if (!index)
    {
        _sys_err("WaitStartWar", 0x1fd, "GuildWar.WaitStartWar.CREATE_WARMAP_ERROR id(%lu vs %lu), type(%u), map(%d)",
            first, second, static_cast<unsigned>(type), static_cast<int>(info.lMapIndex));
        guilds.RequestEndWar(self, opponent);
        return false;
    }

    sys_log(0, "GuildWar.WaitStartWar.CreateMap id(%lu vs %lu), type(%u), map(%d) -> map_inst(%lu)",
        first, second, static_cast<unsigned>(type), static_cast<int>(info.lMapIndex), static_cast<unsigned long>(index));
    SetGuildWarMapIndex(opponent, index);
    enemy->SetGuildWarMapIndex(self, index);
    const GuildWarMapIndexPacket packet = {0x0f, static_cast<std::uint32_t>(first),
        static_cast<std::uint32_t>(second), static_cast<std::int32_t>(index)};
    P2P_MANAGER::instance().Send(&packet, sizeof(packet));
    return true;
}

void CGuild::StartWar(DWORD opponent)
{
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end() || war->second.state == 6)
        return;
    war->second.state = 6;
    war->second.war_start_time = static_cast<std::uint32_t>(get_global_time());
    GuildWarPacket(opponent, war->second.type, 6);
    if (war->second.type)
        GuildWarEntryAsk(opponent);
}

void CGuild::GuildWarEntryAccept(DWORD opponent, CHARACTER* ch)
{
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end() || war->second.type == 0)
        return;

    if (war->second.state != 6)
    {
        ch->ChatPacket(1, locale_find("\300\314\271\314\040\300\374\300\357\300\314\040\263\241\263\265\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }

    const long mapIndex = war->second.map_index;
    PIXEL_POSITION start;
    if (mapIndex && CWarMapManager::instance().GetStartPosition(mapIndex, opponent <= GetID(), start))
    {
        ch->SetQuestFlag("war.is_war_member", 1);
        ch->SaveExitLocation();
        ch->WarpSet(start.x, start.y, war->second.map_index);
    }
}

void CGuild::GuildWarEntryAsk(DWORD opponent)
{
    const auto war = m_EnemyGuild.find(opponent);
    const int self = static_cast<int>(GetID());
    const int enemy = static_cast<int>(opponent);
    if (war == m_EnemyGuild.end())
    {
        _sys_err("GuildWarEntryAsk", 0x2a3, "GuildWar.GuildWarEntryAsk.UNKNOWN_ENEMY(%d)", enemy);
        return;
    }
    const long map = war->second.map_index;
    sys_log(0, "GuildWar.GuildWarEntryAsk id(%d vs %d), map(%ld)", self, enemy, map);
    if (!war->second.map_index)
    {
        _sys_err("GuildWarEntryAsk", 0x2ac, "GuildWar.GuildWarEntryAsk.NOT_EXIST_MAP id(%d vs %d)", self, enemy);
        return;
    }
    PIXEL_POSITION position = {};
    if (!CWarMapManager::instance().GetStartPosition(war->second.map_index, opponent <= GetID(), position))
    {
        _sys_err("GuildWarEntryAsk", 0x2b3, "GuildWar.GuildWarEntryAsk.START_POSITION_ERROR id(%d vs %d), pos(%d, %d)",
            self, enemy, static_cast<int>(position.x), static_cast<int>(position.y));
        return;
    }
    sys_log(0, "GuildWar.GuildWarEntryAsk.OnlineMemberCount(%d)", static_cast<int>(m_memberOnline.size()));
    auto& quests = quest::CQuestManager::instance();
    for (CHARACTER* member : m_memberOnline)
    {
        const DWORD pid = member->GetPlayerID();
        const unsigned int index = quests.GetQuestIndexByName("guild_war_join");
        if (!index)
        {
            _sys_err("GuildWarEntryAsk", 0x2c9,
                "GuildWar.GuildWarEntryAsk.SendLetterToMember.QUEST_ERROR pid(%d), quest_name('guild_war_join.quest')",
                static_cast<int>(pid));
            return;
        }
        sys_log(0, "GuildWar.GuildWarEntryAsk.SendLetterToMember pid(%d), qid(%d)",
            static_cast<int>(pid), static_cast<int>(index));
        quests.Letter(pid, index, 0);
    }
}

void CGuild::EndWar(DWORD opponent)
{
    if (opponent == GetID())
        return;
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end())
        return;

    if (CWarMap* map = CWarMapManager::instance().Find(war->second.map_index))
        map->SetEnded();
    GuildWarPacket(opponent, war->second.type, 7);
    m_EnemyGuild.erase(war);
    if (!UnderAnyWar(3))
    {
        for (CHARACTER* member : m_memberOnline)
        {
            for (DWORD affect = 0x98; affect <= 0x9d; ++affect)
                member->RemoveAffect(affect);
            member->RemoveBadAffect();
        }
    }
}

// 080af080
DWORD CGuild::GetGuildWarMapIndex(DWORD opponent)
{
    const auto war = m_EnemyGuild.find(opponent);
    return war == m_EnemyGuild.end() ? 0 : war->second.map_index;
}

void CGuild::SetGuildWarMapIndex(DWORD opponent, long mapIndex)
{
    std::map<DWORD, TGuildWar>::iterator war = m_EnemyGuild.find(opponent);
    if (war != m_EnemyGuild.end()) {
        war->second.map_index = static_cast<std::uint32_t>(mapIndex);
        const int opponentId = static_cast<std::int32_t>(opponent);
        sys_log(0, "GuildWar.SetGuildWarMapIndex id(%d -> %d), map(%d)",
                static_cast<std::int32_t>(m_data.guild_id), opponentId, static_cast<std::int32_t>(mapIndex));
    }
}

//080add20. Independent actual read-only map leaf; ctor uncertainty is separate.
DWORD CGuild::UnderAnyWar(BYTE type) {
    for(std::map<DWORD,TGuildWar>::iterator it=m_EnemyGuild.begin();it!=m_EnemyGuild.end();++it)
        if((type>2 || it->second.type==type) && it->second.state==6)
            return it->first;
    return 0;
}

// 080adcb0
int CGuild::GetGuildWarType(DWORD opponent)
{
    const auto war = m_EnemyGuild.find(opponent);
    return war == m_EnemyGuild.end() ? 0 : war->second.type;
}

int CGuild::GetGuildWarState(DWORD opponent)
{
    if (opponent == m_data.guild_id)
        return 0;

    const auto war = m_EnemyGuild.find(opponent);
    return war == m_EnemyGuild.end() ? 0 : static_cast<int>(war->second.state);
}

bool CGuild::UnderWar(DWORD opponent)
{
    if (opponent == m_data.guild_id)
        return false;
    std::map<DWORD, TGuildWar>::iterator it = m_EnemyGuild.find(opponent);
    return it != m_EnemyGuild.end() && it->second.state == 6;
}

void CGuild::SetLadderPoint(int point)
{
    if (m_data.ladder_point != point) {
        char message[256];
        std::snprintf(message, sizeof(message), locale_find("\074\261\346\265\345\076\040\267\241\264\365\040\301\241\274\366\260\241\040\045\144\040\301\241\300\314\040\265\307\276\372\275\300\264\317\264\331", g_iUseLocale), point);
        for (CHARACTER* member : m_memberOnline)
            member->ChatPacket(1, "%s", message);
    }
    m_data.ladder_point = point;
}

void CGuild::ChangeLadderPoint(int amount)
{
    const TPacketGuildLadderPoint packet = {static_cast<std::uint32_t>(GetID()), amount};
    if (db_clientdesc)
        db_clientdesc->DBPacket(46, 0, &packet, sizeof(packet));
}

void CGuild::SetWarScoreAgainstTo(DWORD opponent, int score)
{
    const DWORD self = m_data.guild_id;
    sys_log(0, "GuildWarScore Set %lu from %lu %d", self, opponent, score);
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end())
        return;

    war->second.score = static_cast<std::uint32_t>(score);
    CGuildManager& manager = CGuildManager::instance();
    if (war->second.type == 0)
    {
        const TPacketGCGuild packet = {0x4b, 0x10, 0x13};
        const std::uint32_t guild = self;
        const std::uint32_t other = opponent;
        const std::int32_t points = score;
        TEMP_BUFFER buffer;
        buffer.write(&packet, sizeof(packet));
        buffer.write(&guild, sizeof(guild));
        buffer.write(&other, sizeof(other));
        buffer.write(&points, sizeof(points));
        const int size = buffer.size();
        const void* data = buffer.read_peek();
        Packet(data, size);
        if (CGuild* enemy = manager.TouchGuild(opponent)) {
            const int enemySize = buffer.size();
            const void* enemyData = buffer.read_peek();
            enemy->Packet(enemyData, enemySize);
        }
    }
    else
    {
        CGuild* enemy = manager.TouchGuild(opponent);
        if (CWarMap* map = CWarMapManager::instance().Find(war->second.map_index))
            map->UpdateScore(self, score, opponent, enemy->GetWarScoreAgainstTo(self));
    }
}

int CGuild::GetWarScoreAgainstTo(DWORD opponent)
{
    const auto war = m_EnemyGuild.find(opponent);
    if (war == m_EnemyGuild.end())
    {
        sys_log(0, "GuildWarScore Get %lu from %lu No data", m_data.guild_id, opponent);
        return 0;
    }

    const int score = static_cast<std::int32_t>(war->second.score);
    sys_log(0, "GuildWarScore Get %lu from %lu %d", m_data.guild_id, opponent, score);
    return static_cast<std::int32_t>(war->second.score);
}

void CGuild::SendEnemyGuild(CHARACTER* ch)
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    const TPacketGCGuild warPacket = {0x4b, 0x0e, 0x0f};
    const TPacketGCGuild scorePacket = {0x4b, 0x10, 0x13};
    const std::uint32_t self = m_data.guild_id;
    for (const auto& enemy : m_EnemyGuild)
    {
        const std::uint32_t opponent = enemy.first;
        BYTE payload[10];
        std::memcpy(payload, &self, sizeof(self));
        std::memcpy(payload + sizeof(self), &opponent, sizeof(opponent));
        payload[8] = enemy.second.type;
        payload[9] = static_cast<BYTE>(enemy.second.state);
        desc->BufferedPacket(&warPacket, sizeof(warPacket));
        desc->Packet(payload, sizeof(payload));
        if (enemy.second.state != 6)
            continue;

        const std::int32_t score = GetWarScoreAgainstTo(opponent);
        desc->BufferedPacket(&scorePacket, sizeof(scorePacket));
        desc->BufferedPacket(&self, sizeof(self));
        desc->BufferedPacket(&opponent, sizeof(opponent));
        desc->Packet(&score, sizeof(score));

        CGuild* other = CGuildManager::instance().TouchGuild(opponent);
        const std::int32_t opponentScore = other->GetWarScoreAgainstTo(self);
        desc->BufferedPacket(&scorePacket, sizeof(scorePacket));
        desc->BufferedPacket(&opponent, sizeof(opponent));
        desc->BufferedPacket(&self, sizeof(self));
        desc->Packet(&opponentScore, sizeof(opponentScore));
    }
}
