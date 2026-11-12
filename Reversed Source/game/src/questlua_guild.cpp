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
    char buffer[4097] = {};
    if (CGuild* guild = CurrentGuild())
        CGuildManager::instance().GetAroundRankString(guild->GetID(), buffer);
    lua_pushstring(state, buffer);
    return 1;
}

int guild_name(lua_State* state)
{
    if (!lua_isnumber(state, 1))
    {
        CQuestManager::instance().QuestError("guild_name", 0x60, "invalid argument");
        return 0;
    }

    CGuild* guild = CGuildManager::instance().FindGuild(GuildNumber(state, 1));
    lua_pushstring(state, guild ? guild->GetName() : "");
    return 1;
}

int guild_level(lua_State* state)
{
    luaL_checknumber(state, 1);
    CGuild* guild = CGuildManager::instance().FindGuild(GuildNumber(state, 1));
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
    if (ch && ch->GetGuild())
        ch->GetGuild()->GuildWarEntryAccept(GuildNumber(state, 1), ch);
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
    CGuild* guild = lua_isnumber(state, 1)
        ? CGuildManager::instance().FindGuild(GuildNumber(state, 1)) : nullptr;
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

int guild_get_warp_war_list(lua_State* state)
{
    lua_newtable(state);
    CGuildManager& guilds = CGuildManager::instance();
    int index = 1;
    guilds.for_each_war([&](DWORD first, DWORD second) {
        CGuild* guild = guilds.FindGuild(first);
        if (guild && guild->GetGuildWarType(second) != 0 && guild->GetGuildWarState(second) == 6)
        {
            lua_newtable(state);
            lua_pushnumber(state, first);
            lua_rawseti(state, -2, 1);
            lua_pushnumber(state, second);
            lua_rawseti(state, -2, 2);
            lua_rawseti(state, -2, index++);
        }
    });
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
        sys_log(0, "con.size(): %lu %lu %lu handi %ld", static_cast<DWORD>(data.dwID),
            static_cast<DWORD>(data.dwGuildFrom), static_cast<DWORD>(data.dwGuildTo), static_cast<long>(data.lHandicap));
        const bool fromStronger = data.lPowerFrom > data.lPowerTo;
        lua_pushnumber(state, data.dwID);
        lua_rawseti(state, -2, 1);
        lua_pushnumber(state, fromStronger ? data.dwGuildFrom : data.dwGuildTo);
        lua_rawseti(state, -2, 2);
        lua_pushnumber(state, fromStronger ? data.dwGuildTo : data.dwGuildFrom);
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
        if (ch && ch->GetDesc())
            bet = CGuildManager::instance().IsBet(GuildNumber(state, 1), ch->GetDesc()->GetAccountTable().login);
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
