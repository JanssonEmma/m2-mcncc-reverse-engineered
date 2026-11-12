#include "guild_manager.h"
#include "war_map.h"
#include "config.h"
#include "db.h"
#include <cstdlib>
#include <memory>
#include "char_manager.h"
#include "desc_client.h"
#include "locale.hpp"
#include "utils.h"
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <string>
#include "guild.h"
#include "questmanager.h"
#include "buffer_manager.h"
#include "char.h"
#include "desc.h"
#include "packet.h"
#include "../../common/length.h"
#include <cstdint>
#include "lzo_manager.h"
#include "crc32.h"
#include "../../libthecore/include/log.h"

namespace {
bool GuildRankOrder(const CGuild* first, const CGuild* second)
{
    if (first->GetLadderPoint() != second->GetLadderPoint())
        return first->GetLadderPoint() < second->GetLadderPoint();
    if (first->GetGuildWarWinCount() != second->GetGuildWarWinCount())
        return first->GetGuildWarWinCount() < second->GetGuildWarWinCount();
    if (first->GetGuildWarLossCount() != second->GetGuildWarLossCount())
        return first->GetGuildWarLossCount() < second->GetGuildWarLossCount();
    return std::strcmp(first->GetName(), second->GetName()) > 0;
}

std::vector<CGuild*> SortedGuilds(const std::map<DWORD, CGuild*>& guilds)
{
    std::vector<CGuild*> sorted;
    for (const auto& entry : guilds)
        if (entry.second)
            sorted.push_back(entry.second);
    std::sort(sorted.begin(), sorted.end(), GuildRankOrder);
    return sorted;
}

void AppendGuildRank(std::string& text, CGuild* guild, DWORD selected, int rank)
{
    const bool highlight = guild->GetID() == selected;
    const bool separator = !text.empty();
    if (highlight)
        text += "[COLOR r;255|g;255|b;0]";
    if (separator)
        text += "[ENTER]";
    char row[128];
    std::snprintf(row, sizeof(row), "%3d | %-12s | %-8d | %4d | %4d | %4d",
        rank, guild->GetName(), guild->GetLadderPoint(), guild->GetGuildWarWinCount(),
        guild->GetGuildWarDrawCount(), guild->GetGuildWarLossCount());
    text += row;
    if (highlight)
        text += "[/COLOR]";
}
}

void CGuildManager::GetHighRankString(DWORD guild, char* buffer)
{
    const auto sorted = SortedGuilds(m_mapGuild);
    std::string text;
    for (auto it = sorted.rbegin(); it != sorted.rend() && it - sorted.rbegin() < 8; ++it)
    {
        CGuild* entry = *it;
        if (entry->GetLadderPoint() < 1)
            break;
        AppendGuildRank(text, entry, guild, GetRank(entry));
    }
    std::memcpy(buffer, text.c_str(), text.size() + 1);
}

void CGuildManager::GetAroundRankString(DWORD guild, char* buffer)
{
    const auto sorted = SortedGuilds(m_mapGuild);
    const auto found = std::find_if(sorted.begin(), sorted.end(),
        [guild](CGuild* entry) { return entry->GetID() == guild; });
    const auto center = found - sorted.begin();
    std::string text;
    for (int offset = -3; offset <= 3; ++offset)
    {
        const auto index = center - offset;
        if (index >= 0 && index < static_cast<decltype(index)>(sorted.size()))
        {
            CGuild* entry = sorted[index];
            AppendGuildRank(text, entry, guild, GetRank(entry));
        }
    }
    std::memcpy(buffer, text.c_str(), text.size() + 1);
}

int CGuildManager::GetRank(CGuild* guild)
{
    const int points = guild->GetLadderPoint();
    int rank = 1;
    for (const auto& entry : m_mapGuild)
        if (entry.second->GetLadderPoint() > points)
            ++rank;
    return rank;
}

void CGuildManager::SkillRecharge()
{
    for (const auto& guild : m_mapGuild)
        guild.second->SkillRecharge();
}

int CGuildManager::GetDisbandDelay()
{
    return quest::CQuestManager::instance().GetEventFlag("guild_disband_delay") * (test_server ? 60 : 86400);
}

int CGuildManager::GetWithdrawDelay()
{
    return quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay") * (test_server ? 60 : 86400);
}

DWORD CGuildManager::CreateGuild(TGuildCreateParameter& parameter)
{
    if (!parameter.master)
        return 0;
    if (!check_name(parameter.name)) {
        parameter.master->ChatPacket(1, "%s", locale_find("\074\261\346\265\345\076\040\261\346\265\345\040\300\314\270\247\300\314\040\300\373\307\325\307\317\301\366\040\276\312\275\300\264\317\264\331\056", g_iUseLocale));
        return 0;
    }

    DBManager& db = DBManager::instance();
    char escaped[2 * sizeof(parameter.name) + 1]{};
    db.EscapeString(escaped, parameter.name, std::strlen(parameter.name));
    std::unique_ptr<SQLMsg> message(db.DirectQuery("SELECT COUNT(*) FROM guild%s WHERE name = '%s'",
                                                get_table_postfix(), escaped));
    SQLResult* result = message ? message->Get() : nullptr;
    MYSQL_ROW row = result && result->iNumRows && result->pSQLResult ? mysql_fetch_row(result->pSQLResult) : nullptr;
    if (!row || !row[0]) {
        parameter.master->ChatPacket(1, "%s", locale_find("\074\261\346\265\345\076\040\261\346\265\345\270\246\040\273\375\274\272\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return 0;
    }
    if (row[0][0] != '0') {
        parameter.master->ChatPacket(1, "%s", locale_find("\074\261\346\265\345\076\040\300\314\271\314\040\260\260\300\272\040\300\314\270\247\300\307\040\261\346\265\345\260\241\040\300\326\275\300\264\317\264\331\056", g_iUseLocale));
        return 0;
    }
    std::unique_ptr<CGuild> guild(new CGuild(parameter));
    const DWORD id = guild->GetID();
    if (!id || !m_mapGuild.emplace(id, guild.get()).second)
        return 0;
    guild.release();
    return id;
}

CGuild* CGuildManager::FindGuildByName(const std::string& name)
{
    for (const auto& entry : m_mapGuild) {
        if (name == entry.second->GetName())
            return entry.second;
    }
    return nullptr;
}

CGuild* CGuildManager::FindGuild(DWORD guildID)
{
    TGuildMap::iterator guild = m_mapGuild.find(guildID);
    return guild == m_mapGuild.end() ? 0 : guild->second;
}

void CGuildManager::Link(DWORD pid, CGuild* guild)
{
    if (m_map_pkGuildByPID.find(pid) == m_map_pkGuildByPID.end())
        m_map_pkGuildByPID.insert(std::make_pair(pid, guild));
}

void CGuildManager::Unlink(DWORD pid)
{
    m_map_pkGuildByPID.erase(pid);
}

CGuild* CGuildManager::GetLinkedGuild(DWORD pid)
{
    TGuildMap::iterator guild = m_map_pkGuildByPID.find(pid);
    return guild == m_map_pkGuildByPID.end() ? 0 : guild->second;
}

void SendNotice(const char* text);

namespace {
const char declarationNotice[] = "\045\163\040\261\346\265\345\260\241\040\045\163\040\261\346\265\345\277\241\040\274\261\300\374\306\367\260\355\270\246\040\307\317\277\264\275\300\264\317\264\331\041";
const char refusalNotice[] = "\074\261\346\265\345\076\040\045\163\040\261\346\265\345\260\241\040\261\346\265\345\300\374\300\273\040\260\305\272\316\307\317\277\264\275\300\264\317\264\331\056";
const char waitingNotice[] = "\045\163\040\261\346\265\345\277\315\040\045\163\040\261\346\265\345\260\241\040\300\341\275\303\040\310\304\040\300\374\300\357\300\273\040\275\303\300\333\307\325\264\317\264\331\041";
const char startNotice[] = "\045\163\040\261\346\265\345\277\315\040\045\163\040\261\346\265\345\260\241\040\300\374\300\357\300\273\040\275\303\300\333\307\317\277\264\275\300\264\317\264\331\041";
const char drawNotice[] = "\045\163\040\261\346\265\345\277\315\040\045\163\040\261\346\265\345\040\273\347\300\314\300\307\040\300\374\300\357\300\314\040\271\253\275\302\272\316\267\316\040\263\241\263\265\275\300\264\317\264\331\056";
const char victoryNotice[] = "\045\163\040\261\346\265\345\260\241\040\045\163\040\261\346\265\345\277\315\300\307\040\300\374\300\357\277\241\274\255\040\275\302\270\256\040\307\337\275\300\264\317\264\331\056";
const char cancelMasterNotice[] = "\074\261\346\265\345\076\040\261\346\265\345\300\374\300\314\040\303\353\274\322\040\265\307\276\372\275\300\264\317\264\331\056";
const char cancelNotice[] = "\045\163\040\261\346\265\345\277\315\040\045\163\040\261\346\265\345\040\273\347\300\314\300\307\040\300\374\300\357\300\314\040\303\353\274\322\265\307\276\372\275\300\264\317\264\331\056";

void NoticeWar(const char* key, CGuild* first, CGuild* second)
{
    if (!first || !second)
        return;
    char text[256];
    std::snprintf(text, sizeof(text), locale_find(key, g_iUseLocale), first->GetName(), second->GetName());
    SendNotice(text);
}

std::pair<DWORD, DWORD> WarPair(DWORD first, DWORD second)
{
    return first < second ? std::make_pair(first, second) : std::make_pair(second, first);
}

void SendWarListUpdate(const std::pair<DWORD, DWORD>& guilds, BYTE subheader)
{
    const TPacketGCGuild packet = {0x4b, 12, subheader};
    const std::uint32_t first = guilds.first;
    const std::uint32_t second = guilds.second;
    CHARACTER_MANAGER::instance().for_each_pc([&](CHARACTER* ch) {
        if (DESC* desc = ch->GetDesc())
        {
            desc->BufferedPacket(&packet, sizeof(packet));
            desc->BufferedPacket(&first, sizeof(first));
            desc->Packet(&second, sizeof(second));
        }
    });
}
}

void CGuildManager::DeclareWar(DWORD first, DWORD second, BYTE type)
{
    if (first == second)
        return;
    CGuild* left = FindGuild(first);
    CGuild* right = FindGuild(second);
    if (left && right && left->DeclareWar(second, type, 1) && right->DeclareWar(first, type, 3) && !LC_IsGermany())
        NoticeWar(declarationNotice, left, right);
}

void CGuildManager::RefuseWar(DWORD first, DWORD second)
{
    CGuild* left = FindGuild(first);
    CGuild* right = FindGuild(second);
    if (left && right)
    {
        if (CHARACTER* master = CHARACTER_MANAGER::instance().FindByPID(right->GetMasterPID()))
            master->ChatPacket(1, locale_find(refusalNotice, g_iUseLocale), left->GetName());
    }
    if (left)
        left->RefuseWar(second);
    if (right)
        right->RefuseWar(first);
}

void CGuildManager::ReserveWar(DWORD first, DWORD second, BYTE type)
{
    sys_log(0, "GuildManager::ReserveWar %lu %lu", first, second);
    CGuild* left = FindGuild(first);
    CGuild* right = FindGuild(second);
    if (left && right)
    {
        left->ReserveWar(second, type);
        right->ReserveWar(first, type);
    }
}

void CGuildManager::WaitStartWar(DWORD first, DWORD second)
{
    CGuild* left = FindGuild(first);
    CGuild* right = FindGuild(second);
    if (!left || !right)
    {
        sys_log(0, "GuildWar: CGuildManager::WaitStartWar(%d,%d) - something wrong in arg. there is no guild like that.",
            static_cast<int>(first), static_cast<int>(second));
        return;
    }
    if (left->WaitStartWar(second) || right->WaitStartWar(first))
        NoticeWar(waitingNotice, left, right);
}

void CGuildManager::StartWar(DWORD first, DWORD second)
{
    CGuild* left = FindGuild(first);
    CGuild* right = FindGuild(second);
    if (!left || !right || !left->CheckStartWar(second) || !right->CheckStartWar(first))
        return;

    left->StartWar(second);
    right->StartWar(first);
    NoticeWar(startNotice, left, right);
    const auto pair = WarPair(first, second);
    SendWarListUpdate(pair, 0x11);
    m_GuildWar.insert(pair);
}

// 0809c850
void CGuildManager::StopAllGuildWar()
{
    for (const auto& war : m_GuildWar) {
        CGuild* first = TouchGuild(war.first);
        CGuild* second = TouchGuild(war.second);
        first->EndWar(war.second);
        second->EndWar(war.first);
    }
    m_GuildWar.clear();
}

void SendGuildWarOverNotice(CGuild* first, CGuild* second, bool draw)
{
    NoticeWar(draw ? drawNotice : victoryNotice, first, second);
}

bool CGuildManager::EndWar(DWORD first, DWORD second)
{
    const auto pair = WarPair(first, second);
    CGuild* left = FindGuild(pair.first);
    CGuild* right = FindGuild(pair.second);
    const auto war = m_GuildWar.find(pair);
    if (war == m_GuildWar.end())
    {
        sys_log(0, "EndWar(%d,%d) - EndWar request but guild is not in list",
            static_cast<int>(pair.first), static_cast<int>(pair.second));
        return false;
    }

    if (left && right && left->GetGuildWarType(pair.second) == 0)
    {
        const int leftScore = left->GetWarScoreAgainstTo(pair.second);
        const int rightScore = right->GetWarScoreAgainstTo(pair.first);
        SendGuildWarOverNotice(left, right, leftScore == rightScore);
    }
    if (left)
        left->EndWar(pair.second);
    if (right)
        right->EndWar(pair.first);
    m_GuildWarEndTime[pair] = get_global_time();
    SendWarListUpdate(pair, 0x12);
    m_GuildWar.erase(war);
    return true;
}

void CGuildManager::WarOver(DWORD first, DWORD second, bool draw)
{
    const auto pair = WarPair(first, second);
    if (m_GuildWar.find(pair) == m_GuildWar.end())
        return;
    SendGuildWarOverNotice(FindGuild(first), FindGuild(second), draw);
    EndWar(pair.first, pair.second);
}

void CGuildManager::CancelWar(DWORD first, DWORD second)
{
    if (!EndWar(first, second))
        return;

    CGuild* left = FindGuild(first);
    CGuild* right = FindGuild(second);
    CHARACTER_MANAGER& chars = CHARACTER_MANAGER::instance();
    for (CGuild* guild : {left, right})
        if (guild)
            if (CHARACTER* master = chars.FindByPID(guild->GetMasterPID()))
                master->ChatPacket(1, locale_find(cancelMasterNotice, g_iUseLocale));
    NoticeWar(cancelNotice, left, right);
}

void CGuildManager::SendGuildNames(CHARACTER* ch)
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    TPacketGCGuild packet;
    packet.header = 0x4b;
    packet.size = static_cast<WORD>(sizeof(packet) +
        (sizeof(std::uint32_t) + GUILD_NAME_MAX_LEN) * m_mapGuild.size());
    packet.subheader = 0x10;
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    for (TGuildMap::const_iterator guild = m_mapGuild.begin(); guild != m_mapGuild.end(); ++guild)
    {
        const std::uint32_t id = guild->second->GetID();
        buffer.write(&id, sizeof(id));
        buffer.write(guild->second->GetName(), GUILD_NAME_MAX_LEN);
    }
    desc->Packet(buffer.read_peek(), buffer.size());
}

void CGuildManager::SendGuildWar(CHARACTER* ch)
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    TPacketGCGuild packet;
    packet.header = 0x4b;
    packet.size = static_cast<WORD>(sizeof(packet) +
        2 * sizeof(std::uint32_t) * m_GuildWar.size());
    packet.subheader = 0x11;
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    for (TGuildWarContainer::const_iterator war = m_GuildWar.begin(); war != m_GuildWar.end(); ++war)
    {
        const std::uint32_t first = war->first;
        const std::uint32_t second = war->second;
        buffer.write(&first, sizeof(first));
        buffer.write(&second, sizeof(second));
    }
    desc->Packet(buffer.read_peek(), buffer.size());
}

CGuild* CGuildManager::TouchGuild(DWORD guildId)
{
    TGuildMap::iterator it = m_mapGuild.find(guildId);
    if (it != m_mapGuild.end())
        return it->second;

    CGuild* guild = new CGuild(guildId);
    m_mapGuild.insert(std::make_pair(guildId, guild));
    const TPacketGCGuild packet = {0x4b, 0x14, 0x10};
    const std::uint32_t id = guildId;
    CHARACTER_MANAGER::instance().for_each_pc([&](CHARACTER* ch) {
        if (DESC* desc = ch->GetDesc())
        {
            desc->BufferedPacket(&packet, sizeof(packet));
            desc->BufferedPacket(&id, sizeof(id));
            desc->Packet(guild->GetName(), GUILD_NAME_MAX_LEN);
        }
    });
    return guild;
}

void CGuildManager::LoginMember(CHARACTER* ch)
{
    TGuildMap::iterator guild = m_map_pkGuildByPID.find(ch->GetPlayerID());
    if (guild != m_map_pkGuildByPID.end())
        guild->second->LoginMember(ch);
}

void CGuildManager::RequestCancelWar(DWORD first, DWORD second)
{
    sys_log(0, "RequestCancelWar %d %d", static_cast<int>(first), static_cast<int>(second));
    const TPacketGuildWar packet = {0, 5, static_cast<std::uint32_t>(first),
        static_cast<std::uint32_t>(second), 0, 0};
    db_clientdesc->DBPacket(0x1a, 0, &packet, sizeof(packet));
}

void CGuildManager::ShowGuildWarList(CHARACTER* character)
{
    for (const auto& war : m_GuildWar)
    {
        CGuild* first = TouchGuild(war.first);
        CGuild* second = TouchGuild(war.second);
        if (first && second)
        {
            const auto elapsed = get_global_time() - first->GetWarStartTime(second->GetID());
            character->ChatPacket(2, "%s[%d] vs %s[%d] time %lu sec.",
                                  first->GetName(), static_cast<int>(first->GetID()),
                                  second->GetName(), static_cast<int>(second->GetID()),
                                  static_cast<unsigned long>(elapsed));
        }
    }
}

void CGuildManager::RequestEndWar(DWORD guild1, DWORD guild2)
{
    sys_log(0, "RequestEndWar %d %d", static_cast<int>(guild1), static_cast<int>(guild2));
    TPacketGuildWar packet = {0, 7, static_cast<std::uint32_t>(guild1),
        static_cast<std::uint32_t>(guild2), 0, 0};
    db_clientdesc->DBPacket(0x1a, 0, &packet, sizeof(packet));
}

void SendGuildWarScore(DWORD guild, DWORD opponent, int delta, int betDelta)
{
    const TPacketGuildWarScore packet = {
        static_cast<std::uint32_t>(guild), static_cast<std::uint32_t>(opponent), delta, betDelta
    };
    db_clientdesc->DBPacket(0x1b, 0, &packet, sizeof(packet));
    sys_log(0, "SendGuildWarScore %lu %lu %d", guild, opponent, delta);
}

void CGuildManager::Kill(CHARACTER* killer, CHARACTER* victim)
{
    if (!killer->GetDesc() || !victim->GetDesc())
        return;
    if (CWarMap* map = killer->GetWarMap()) {
        map->OnKill(killer, victim);
        return;
    }

    CGuild* attacker = killer->GetGuild();
    CGuild* defender = victim->GetGuild();
    if (attacker && defender && attacker->GetGuildWarType(defender->GetID()) == 0
        && attacker->UnderWar(defender->GetID())) {
        SendGuildWarScore(attacker->GetID(), defender->GetID(), victim->GetLevel(), 0);
    }
}

void CGuildManager::RequestWarOver(DWORD guild1, DWORD guild2, DWORD winner, long)
{
    CGuild* first = TouchGuild(guild1);
    CGuild* second = TouchGuild(guild2);
    if (first->GetGuildWarState(second->GetID()) != 6)
    {
        sys_log(0, "RequestWarOver : both guild was not in war %lu %lu", guild1, guild2);
        RequestEndWar(guild1, guild2);
        return;
    }

    const DWORD from = winner ? winner : guild1;
    const DWORD to = winner && winner != guild1 ? guild1 : guild2;
    TPacketGuildWar packet = {static_cast<BYTE>(winner == 0), 8,
        static_cast<std::uint32_t>(from), static_cast<std::uint32_t>(to), 0, 0};
    db_clientdesc->DBPacket(0x1a, 0, &packet, sizeof(packet));
    sys_log(0, "RequestWarOver : winner %lu loser %lu draw %u betprice %ld",
        from, to, static_cast<unsigned int>(packet.bType), static_cast<long>(packet.lWarPrice));
}

void CGuildManager::P2PLoginMember(DWORD pid)
{
    TGuildMap::iterator guild = m_map_pkGuildByPID.find(pid);
    if (guild != m_map_pkGuildByPID.end())
        guild->second->P2PLoginMember(pid);
}

void CGuildManager::P2PLogoutMember(DWORD pid)
{
    TGuildMap::iterator guild = m_map_pkGuildByPID.find(pid);
    if (guild != m_map_pkGuildByPID.end())
        guild->second->P2PLogoutMember(pid);
}

CGuildManager::CGuildManager()
{
    m_pMarkManager = Guild::NewMarkManager();
}

void CGuildManager::LoadGuild(DWORD guildID)
{
    if (m_mapGuild.find(guildID) == m_mapGuild.end())
        m_mapGuild.emplace(guildID, new CGuild(guildID));
}

void CGuildManager::Initialize()
{
    if (g_bAuthServer)
        return;

    std::unique_ptr<SQLMsg> query(DBManager::instance().DirectQuery(
        "SELECT id FROM guild%s", get_table_postfix()));
    std::vector<int> guildIDs;
    if (query && query->Get() && query->Get()->pSQLResult) {
        SQLResult* result = query->Get();
        if (result->iNumRows > 0)
            guildIDs.reserve(result->iNumRows);
        while (MYSQL_ROW row = mysql_fetch_row(result->pSQLResult)) {
            if (!row[0])
                continue;
            const DWORD guild = std::strtoul(row[0], nullptr, 10);
            LoadGuild(guild);
            guildIDs.push_back(static_cast<int>(guild));
        }
    }

    if (m_pMarkManager) {
        m_pMarkManager->LoadMarkData("guild_mark.tga", guildIDs);
        m_pMarkManager->LoadSymbol("guild_symbol.tga");
    }
}

void CGuildManager::ReserveWarAdd(TGuildWarReserve* packet)
{
    CGuildWarReserveForGame* reserve;
    const DWORD id = packet->dwID;
    const auto found = m_map_kReserveWar.find(id);
    if (found != m_map_kReserveWar.end())
        reserve = found->second;
    else
    {
        std::unique_ptr<CGuildWarReserveForGame> entry(new CGuildWarReserveForGame);
        const auto inserted = m_map_kReserveWar.emplace(id, entry.get());
        try
        {
            m_vec_kReserveWar.push_back(entry.get());
        }
        catch (...)
        {
            m_map_kReserveWar.erase(inserted.first);
            throw;
        }
        reserve = entry.release();
    }
    reserve->data = *packet;
    sys_log(0, "ReserveWarAdd %lu gid1 %lu power %ld gid2 %lu power %ld handicap %ld",
        static_cast<DWORD>(packet->dwID), static_cast<DWORD>(packet->dwGuildFrom),
        static_cast<long>(packet->lPowerFrom), static_cast<DWORD>(packet->dwGuildTo),
        static_cast<long>(packet->lPowerTo), static_cast<long>(packet->lHandicap));
}

void CGuildManager::ReserveWarDelete(DWORD id)
{
    sys_log(0, "ReserveWarDelete %lu", id);
    const auto found = m_map_kReserveWar.find(id);
    if (found == m_map_kReserveWar.end())
        return;
    const auto entry = std::find(m_vec_kReserveWar.begin(), m_vec_kReserveWar.end(), found->second);
    if (entry != m_vec_kReserveWar.end())
        m_vec_kReserveWar.erase(entry);
    delete found->second;
    m_map_kReserveWar.erase(found);
}

void CGuildManager::ReserveWarBet(TPacketGDGuildWarBet* packet)
{
    const auto found = m_map_kReserveWar.find(packet->dwWarID);
    if (found != m_map_kReserveWar.end())
        found->second->mapBet.emplace(packet->szLogin, std::make_pair(static_cast<DWORD>(packet->dwGuild), static_cast<DWORD>(packet->dwGold)));
}

bool CGuildManager::IsBet(DWORD id, const char* login)
{
    const auto found = m_map_kReserveWar.find(id);
    return found == m_map_kReserveWar.end() || found->second->mapBet.find(login) != found->second->mapBet.end();
}

std::vector<CGuildWarReserveForGame*>& CGuildManager::GetReserveWarRef()
{
    return m_vec_kReserveWar;
}

CGuildManager::~CGuildManager()
{
    for (CGuildWarReserveForGame* reserve : m_vec_kReserveWar)
        delete reserve;
    delete m_pMarkManager;
}

CGuildUtil::~CGuildUtil() {}

Guild::IImage* CGuildUtil::NewImage()
{
    return Guild::NewImage();
}

void CGuildUtil::DeleteImage(Guild::IImage* image)
{
    Guild::DeleteImage(image);
}

Guild::CRC CGuildUtil::CalculateCRC32(const void* data, unsigned int size)
{
    return get_crc32(static_cast<const char*>(data), size);
}

bool CGuildUtil::Compress(const void* source, unsigned int sourceSize,
                          void* destination, unsigned int& destinationSize)
{
    unsigned int compressedSize;
    const bool ok = LZOManager::instance().Compress(
        const_cast<BYTE*>(static_cast<const BYTE*>(source)), sourceSize,
        static_cast<BYTE*>(destination), &compressedSize);
    if (ok) {
        destinationSize = compressedSize;
        sys_log(0, "GuildUtil::Compress %d to %d", sourceSize, compressedSize);
    } else {
        _sys_err("Compress", 0x1ce, "cannot compress len %d", sourceSize);
    }
    return ok;
}

bool CGuildUtil::Uncompress(const void* source, unsigned int sourceSize,
                            void* destination, unsigned int& destinationSize)
{
    unsigned int uncompressedSize;
    const bool ok = LZOManager::instance().Decompress(
        const_cast<BYTE*>(static_cast<const BYTE*>(source)), sourceSize,
        static_cast<BYTE*>(destination), &uncompressedSize);
    if (ok) {
        destinationSize = uncompressedSize;
        sys_log(0, "GuildUtil::Uncompress %d to %d", sourceSize, uncompressedSize);
    } else {
        _sys_err("Uncompress", 0x1de, "cannot uncompress len %d", sourceSize);
    }
    return ok;
}

// 0809ae20
void CGuildManager::DisbandGuild(DWORD guildId)
{
    const auto guild = m_mapGuild.find(guildId);
    if (guild == m_mapGuild.end())
        return;

    guild->second->Disband();
    delete guild->second;
    m_mapGuild.erase(guild);
}
