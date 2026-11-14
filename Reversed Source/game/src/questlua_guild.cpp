#include "char.h"
#include "desc.h"
#include "desc_client.h"
#include "guild.h"
#include "guild_manager.h"
#include "questmanager.h"
#include "../../libthecore/include/log.h"
#include <cstdint>
#include <cstring>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
std::uint32_t GuildNumber(lua_State* state, int argument)
{
    const lua_Number value = lua_tonumber(state, argument);
    if (!(value >= -9223372036854775808.0 && value < 9223372036854775808.0))
        return 0;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
}

CGuild* CurrentGuild()
{
    CHARACTER* ch = CQuestManager::instance().GetCurrentCharacterPtr();
    return ch ? ch->GetGuild() : nullptr;
}

}

int guild_get_rank(lua_State* state)
{
    CGuild* guild = CurrentGuild();
    lua_pushnumber(state, guild ? CGuildManager::instance().GetRank(guild) : -1);
    return 1;
}

int guild_get_ladder_point(lua_State* state)
{
    CGuild* guild = CurrentGuild();
    lua_pushnumber(state, guild ? guild->GetLadderPoint() : -1);
    return 1;
}

int guild_high_ranking_string(lua_State* state)
{
    CGuild* guild = CurrentGuild();
    char buffer[4097];
    CGuildManager::instance().GetHighRankString(guild ? guild->GetID() : 0, buffer);
    lua_pushstring(state, buffer);
    return 1;
}

int guild_around_ranking_string(lua_State* state)
{
    if (CGuild* guild = CurrentGuild()) {
        char buffer[4097];
        CGuildManager::instance().GetAroundRankString(guild->GetID(), buffer);
        lua_pushstring(state, buffer);
    } else {
        lua_pushstring(state, "");
    }
    return 1;
}

int guild_name(lua_State* state)
{
    if (!lua_isnumber(state, 1))
    {
        CQuestManager::instance().QuestError("guild_name", 0x60, "invalid argument");
        return 0;
    }

    const DWORD guildId = GuildNumber(state, 1);
    CGuild* guild = CGuildManager::instance().FindGuild(guildId);
    lua_pushstring(state, guild ? guild->GetName() : "");
    return 1;
}

int guild_level(lua_State* state)
{
    luaL_checknumber(state, 1);
    const DWORD guildId = GuildNumber(state, 1);
    CGuild* guild = CGuildManager::instance().FindGuild(guildId);
    lua_pushnumber(state, guild ? guild->GetLevel() : 0);
    return 1;
}

int guild_is_war(lua_State* state)
{
    if (!lua_isnumber(state, 1))
    {
        CQuestManager::instance().QuestError("guild_is_war", 0x4e, "invalid argument");
        return 0;
    }
    CGuild* guild = CurrentGuild();
    lua_pushboolean(state, guild && guild->UnderWar(GuildNumber(state, 1)));
    return 1;
}

int guild_war_enter(lua_State* state)
{
    if (!lua_isnumber(state, 1))
    {
        CQuestManager::instance().QuestError("guild_war_enter", 0x80, "invalid argument");
        return 0;
    }
    CHARACTER* ch = CQuestManager::instance().GetCurrentCharacterPtr();
    if (ch && ch->GetGuild()) {
        const DWORD opponent = GuildNumber(state, 1);
        if (CGuild* guild = ch->GetGuild())
            guild->GuildWarEntryAccept(opponent, ch);
    }
    return 0;
}

int guild_get_any_war(lua_State* state)
{
    CGuild* guild = CurrentGuild();
    lua_pushnumber(state, guild ? guild->UnderAnyWar(3) : 0);
    return 1;
}

int guild_get_name(lua_State* state)
{
    CGuild* guild = nullptr;
    if (lua_isnumber(state, 1)) {
        const DWORD guildId = GuildNumber(state, 1);
        guild = CGuildManager::instance().FindGuild(guildId);
    }
    lua_pushstring(state, guild ? guild->GetName() : "");
    return 1;
}

int guild_war_bet(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3))
    {
        CQuestManager::instance().QuestError("guild_war_bet", 0xaf, "invalid argument");
        return 0;
    }
    CHARACTER* ch = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!ch || !ch->GetDesc())
        return 0;

    TPacketGDGuildWarBet packet = {};
    packet.dwWarID = GuildNumber(state, 1);
    std::strncpy(packet.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(packet.szLogin) - 1);
    packet.dwGuild = GuildNumber(state, 2);
    packet.dwGold = GuildNumber(state, 3);
    sys_log(0, "GUILD_WAR_BET: %s login %s war_id %lu guild %lu gold %lu", ch->GetName(), packet.szLogin,
        static_cast<DWORD>(packet.dwWarID), static_cast<DWORD>(packet.dwGuild), static_cast<DWORD>(packet.dwGold));
    db_clientdesc->DBPacket(0x39, 0, &packet, sizeof(packet));
    return 0;
}

struct FBuildLuaGuildWarList
{
    lua_State* L;
    int m_count;

    void operator()(DWORD first, DWORD second)
    {
        CGuild* guild = CGuildManager::instance().FindGuild(first);
        if (!guild || guild->GetGuildWarType(second) == 0 || guild->GetGuildWarState(second) != 6)
            return;

        lua_newtable(L);
        lua_pushnumber(L, first);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, second);
        lua_rawseti(L, -2, 2);
        lua_rawseti(L, -2, m_count++);
    }
};

int guild_get_warp_war_list(lua_State* state)
{
    FBuildLuaGuildWarList build{state, 1};
    lua_newtable(state);
    CGuildManager::instance().for_each_war(build);
    return 1;
}

int guild_get_reserve_war_table(lua_State* state)
{
    const auto& reserves = CGuildManager::instance().GetReserveWarRef();
    sys_log(0, "con.size(): %d", static_cast<int>(reserves.size()));
    lua_newtable(state);
    int row = 0;
    for (const CGuildWarReserveForGame* entry : reserves)
    {
        const auto& data = entry->data;
        if (data.bType != 1)
            continue;

        lua_newtable(state);
        const auto handicap = static_cast<long>(data.lHandicap);
        const auto guildTo = static_cast<DWORD>(data.dwGuildTo);
        const auto guildFrom = static_cast<DWORD>(data.dwGuildFrom);
        const auto reserveId = static_cast<DWORD>(data.dwID);
        sys_log(0, "con.size(): %lu %lu %lu handi %ld", reserveId, guildFrom, guildTo, handicap);
        lua_pushnumber(state, data.dwID);
        lua_rawseti(state, -2, 1);
        lua_pushnumber(state, data.lPowerFrom > data.lPowerTo ? data.dwGuildFrom : data.dwGuildTo);
        lua_rawseti(state, -2, 2);
        lua_pushnumber(state, data.lPowerFrom > data.lPowerTo ? data.dwGuildTo : data.dwGuildFrom);
        lua_rawseti(state, -2, 3);
        lua_pushnumber(state, data.lHandicap);
        lua_rawseti(state, -2, 4);
        lua_rawseti(state, -2, ++row);
    }
    return 1;
}

int guild_is_bet(lua_State* state)
{
    bool bet = true;
    if (!lua_isnumber(state, 1))
        CQuestManager::instance().QuestError("guild_is_bet", 199, "invalid argument");
    else
    {
        CHARACTER* ch = CQuestManager::instance().GetCurrentCharacterPtr();
        if (auto* desc = ch ? ch->GetDesc() : nullptr) {
            const char* login = desc->GetAccountTable().login;
            const DWORD reserveId = GuildNumber(state, 1);
            bet = CGuildManager::instance().IsBet(reserveId, login);
        }
    }
    lua_pushboolean(state, bet);
    return 1;
}

int guild_get_member_count(lua_State* state)
{
    CGuild* guild = CurrentGuild();
    lua_pushnumber(state, guild ? guild->GetMemberCount() : 0);
    return 1;
}

void RegisterGuildFunctionTable()
{
    luaL_reg functions[] = {
        {"get_rank", guild_get_rank},
        {"get_ladder_point", guild_get_ladder_point},
        {"high_ranking_string", guild_high_ranking_string},
        {"around_ranking_string", guild_around_ranking_string},
        {"name", guild_name},
        {"level", guild_level},
        {"is_war", guild_is_war},
        {"war_enter", guild_war_enter},
        {"get_any_war", guild_get_any_war},
        {"get_reserve_war_table", guild_get_reserve_war_table},
        {"get_name", guild_get_name},
        {"war_bet", guild_war_bet},
        {"is_bet", guild_is_bet},
        {"get_warp_war_list", guild_get_warp_war_list},
        {"get_member_count", guild_get_member_count},
        {nullptr, nullptr},
    };
    CQuestManager::instance().AddLuaFunctionTable("guild", functions);
}
}
