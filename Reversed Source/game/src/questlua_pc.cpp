#include "config.h"
#include "db.h"
#include "questpc.h"
#include "../../libthecore/include/log.h"
#include <cmath>
#include "char_manager.h"
#include "guild_manager.h"
#include "war_map.h"

extern int g_nPortalLimitTime;
#include "item_manager.h"
#include "log.h"
#include <algorithm>
#include "locale.hpp"
#include "p2p.h"
#include "polymorph.h"
#include "desc_client.h"
#include <cstdlib>
#include <memory>
#include "../../libthecore/include/utils.h"
#include "mining.h"
#include "char.h"
#include "guild.h"
#include "item.h"
#include "marriage.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include <climits>
#include <cstdint>
#include <cstring>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
CHARACTER& CurrentPC()
{
    return *CQuestManager::instance().GetCurrentCharacterPtr();
}

int PCTruncate(long double value)
{
    if (!(value > -2147483649.0 && value < 2147483648.0))
        return INT_MIN;
    return static_cast<int>(value);
}

int PCSignedNumber(lua_State* state, int index)
{
    return PCTruncate(lua_tonumber(state, index));
}

BYTE PCShortByte(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    return value > -32769.0 && value < 32768.0
        ? static_cast<BYTE>(static_cast<std::int16_t>(value)) : 0;
}

std::string PCFlagName(const std::string& quest, const char* flag)
{
    return quest + "." + flag;
}

std::uint32_t PCUnsignedNumber(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value >= -9223372036854775808.0 && value < 9223372036854775808.0))
        return 0;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
}

std::int32_t LocalCell(long global, int base)
{
    const std::uint32_t delta = static_cast<std::uint32_t>(global) - static_cast<std::uint32_t>(base);
    std::int32_t signedDelta;
    std::memcpy(&signedDelta, &delta, sizeof(signedDelta));
    return signedDelta / 100;
}

}

int pc_get_level(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetLevel());
    return 1;
}

int pc_get_next_exp(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetNextExp());
    return 1;
}

int pc_get_exp(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetExp());
    return 1;
}

int pc_get_job(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetJob());
    return 1;
}

int pc_get_race(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetRaceNum());
    return 1;
}

int pc_get_hp(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetHP());
    return 1;
}

int pc_get_max_hp(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetMaxHP());
    return 1;
}

int pc_get_sp(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetSP());
    return 1;
}

int pc_get_max_sp(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetMaxSP());
    return 1;
}

int pc_get_money(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetGold());
    return 1;
}

int pc_get_real_alignment(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetRealAlignment() / 10);
    return 1;
}

int pc_get_alignment(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetAlignment() / 10);
    return 1;
}

int pc_get_vid(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, static_cast<DWORD>(ch.GetVID()));
    return 1;
}

int pc_get_playtime(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, static_cast<std::int32_t>(ch.GetRealPoint(POINT_PLAYTIME)));
    return 1;
}

int pc_get_leadership(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetLeadershipSkillLevel());
    return 1;
}

int pc_get_x(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, static_cast<std::int32_t>(ch.GetX()) / 100);
    return 1;
}

int pc_get_y(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, static_cast<std::int32_t>(ch.GetY()) / 100);
    return 1;
}

int pc_get_current_map_index(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, static_cast<std::int32_t>(ch.GetMapIndex()));
    return 1;
}

int pc_get_empire(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetEmpire());
    return 1;
}

int pc_get_skillgroup(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetSkillGroup());
    return 1;
}

int pc_get_horse_hp(lua_State* state)
{
    CHARACTER& character = CurrentPC();
    lua_pushnumber(state, character.GetHorseLevel() ? character.GetHorseHealth() : 0);
    return 1;
}

int pc_get_horse_stamina(lua_State* state)
{
    CHARACTER& character = CurrentPC();
    lua_pushnumber(state, character.GetHorseLevel() ? character.GetHorseStamina() : 0);
    return 1;
}

int pc_get_horse_level(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetHorseLevel());
    return 1;
}

int pc_get_gm_level(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushnumber(state, ch.GetGMLevel());
    return 1;
}

int pc_hasguild(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushboolean(state, ch.GetGuild() != nullptr);
    return 1;
}

int pc_in_dungeon(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushboolean(state, ch.GetDungeon() != nullptr);
    return 1;
}

int pc_is_polymorphed(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushboolean(state, ch.IsPolymorphed());
    return 1;
}

int pc_is_mount(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushboolean(state, ch.GetMountVnum() != 0);
    return 1;
}

int pc_is_horse_alive(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushboolean(state, ch.GetHorseLevel() != 0 && ch.GetHorseHealth() > 0);
    return 1;
}

int pc_is_gm(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    lua_pushboolean(state, ch.GetGMLevel() > 2);
    return 1;
}

int pc_get_name(lua_State* state)
{
    lua_pushstring(state, CurrentPC().GetName());
    return 1;
}

int pc_get_weapon(lua_State* state)
{
    CItem* weapon = CurrentPC().GetWear(4);
    lua_pushnumber(state, weapon ? weapon->GetVnum() : 0);
    return 1;
}

int pc_get_armor(lua_State* state)
{
    CItem* armor = CurrentPC().GetWear(0);
    lua_pushnumber(state, armor ? armor->GetVnum() : 0);
    return 1;
}

int pc_get_local_x(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    const SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(ch.GetMapIndex());
    const long coordinate = ch.GetX();
    lua_pushnumber(state, LocalCell(coordinate, map ? map->m_setting.iBaseX : 0));
    return 1;
}

int pc_get_local_y(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    const SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(ch.GetMapIndex());
    const long coordinate = ch.GetY();
    lua_pushnumber(state, LocalCell(coordinate, map ? map->m_setting.iBaseY : 0));
    return 1;
}

int pc_getguild(lua_State* state)
{
    CGuild* guild = CurrentPC().GetGuild();
    lua_pushnumber(state, guild ? guild->GetID() : 0);
    return 1;
}

int pc_isguildmaster(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    CGuild* guild = ch.GetGuild();
    lua_pushboolean(state, guild && guild->GetMasterPID() == ch.GetPlayerID());
    return 1;
}

int pc_get_part(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    lua_pushnumber(state, lua_isnumber(state, 1) && character
        ? character->GetPart(static_cast<BYTE>(PCSignedNumber(state, 1))) : 0);
    return 1;
}

int pc_get_skill_level(lua_State* state)
{
    int level = 0;
    if (lua_isnumber(state, 1))
        level = CurrentPC().GetSkillLevel(PCUnsignedNumber(state, 1));
    else
        CQuestManager::instance().QuestError("pc_get_skill_level", 0x58c, "invalid argument");
    lua_pushnumber(state, level);
    return 1;
}

int pc_get_sex(lua_State* state)
{
    lua_pushnumber(state, GET_SEX(&CurrentPC()));
    return 1;
}

int pc_is_married(lua_State* state)
{
    const DWORD playerId = CurrentPC().GetPlayerID();
    lua_pushboolean(state, marriage::CManager::instance().IsMarried(playerId));
    return 1;
}

int pc_is_engaged(lua_State* state)
{
    const DWORD playerId = CurrentPC().GetPlayerID();
    lua_pushboolean(state, marriage::CManager::instance().IsEngaged(playerId));
    return 1;
}

int pc_is_engaged_or_married(lua_State* state)
{
    const DWORD playerId = CurrentPC().GetPlayerID();
    lua_pushboolean(state, marriage::CManager::instance().IsEngagedOrMarried(playerId));
    return 1;
}

int pc_get_premium_remain_sec(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!lua_isnumber(state, 1))
    {
        CQuestManager::instance().QuestError("pc_get_premium_remain_sec", 0x716, "wrong premium index (is not number)");
        return 0;
    }
    const int index = PCSignedNumber(state, 1);
    if (static_cast<unsigned int>(index) >= 7)
    {
        CQuestManager::instance().QuestError("pc_get_premium_remain_sec", 0x727, "wrong premium index %d", index);
        return 0;
    }
    lua_pushnumber(state, character ? character->GetPremiumRemainSeconds(static_cast<BYTE>(index)) : 0);
    return 1;
}

int pc_change_sp(lua_State* state)
{
    bool changed = false;
    if (!lua_isnumber(state, 1))
        CQuestManager::instance().QuestError("pc_change_sp", 0x2d6, "invalid argument");
    else
    {
        const int amount = PCSignedNumber(state, 1);
        if (amount != 0) {
            CHARACTER& ch = CurrentPC();
            changed = amount > 0 || ch.GetSP() >= -static_cast<std::int64_t>(amount);
            if (changed)
                ch.PointChange(POINT_SP, amount, false, false);
        }
    }
    lua_pushboolean(state, changed);
    return 1;
}

int pc_change_money(lua_State* state)
{
    const int amount = PCSignedNumber(state, -1);
    CHARACTER& ch = CurrentPC();
    const std::int64_t total = static_cast<std::int64_t>(ch.GetGold()) + amount;
    if (total < 0 || total > INT_MAX)
        CQuestManager::instance().QuestError("pc_change_money", 0x343,
            "QUEST wrong ChangeGold %d (now %d)", amount, ch.GetGold());
    else
    {
        DBManager::instance().SendMoneyLog(4, ch.GetPlayerID(), amount);
        ch.PointChange(POINT_GOLD, amount, true, false);
    }
    return 0;
}

int pc_change_alignment(lua_State* state)
{
    const long double amount = 10.0L * static_cast<long double>(lua_tonumber(state, 1));
    const int delta = PCTruncate(amount);
    CurrentPC().UpdateAlignment(delta);
    return 0;
}

int pc_get_flag(lua_State* state)
{
    auto& quests = CQuestManager::instance();
    if (!lua_isstring(state, -1)) {
        quests.QuestError("pc_get_flag", 0x37f, "QUEST wrong get flag");
        return 0;
    }
    const char* flag = lua_tostring(state, -1);
    lua_pushnumber(state, quests.GetCurrentPC()->GetFlag(flag));
    return 1;
}

int pc_set_flag(lua_State* state)
{
    auto& quests = CQuestManager::instance();
    if (!lua_isstring(state, 1) || !lua_isnumber(state, 2)) {
        quests.QuestError("pc_set_flag", 0x3a6, "QUEST wrong set flag");
        return 0;
    }
    const char* key = lua_tostring(state, 1);
    PC* pc = quests.GetCurrentPC();
    const int value = PCTruncate(std::rint(lua_tonumber(state, 2)));
    pc->SetFlag(key, value, false);
    return 0;
}

int pc_get_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isstring(state, -1))
    {
        manager.QuestError("pc_get_quest_flag", 0x391, "QUEST wrong get flag");
        return 0;
    }

    const char* flag = lua_tostring(state, -1);
    PC& pc = *manager.GetCurrentPC();
    lua_pushnumber(state, pc.GetFlag(PCFlagName(pc.GetCurrentQuestName(), flag)));
    if (test_server)
        sys_log(0, "GetQF ( %s . %s )", pc.GetCurrentQuestName().c_str(), flag);
    return 1;
}

int pc_set_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isstring(state, 1) || !lua_isnumber(state, 2))
        manager.QuestError("pc_set_quest_flag", 0x3b8, "QUEST wrong set flag");
    else
    {
        const char* key = lua_tostring(state, 1);
        PC& pc = *manager.GetCurrentPC();
        const int value = PCTruncate(std::rint(lua_tonumber(state, 2)));
        pc.SetFlag(PCFlagName(pc.GetCurrentQuestName(), key), value, false);
    }
    return 0;
}

int pc_del_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isstring(state, 1))
    {
        manager.QuestError("pc_del_quest_flag", 0x3c8, "argument error");
        return 0;
    }

    const char* key = lua_tostring(state, 1);
    PC& pc = *manager.GetCurrentPC();
    pc.DeleteFlag(PCFlagName(pc.GetCurrentQuestName(), key));
    return 1;
}

int pc_get_another_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isstring(state, 1) || !lua_isstring(state, 2))
    {
        manager.QuestError("pc_get_another_quest_flag", 0x366, "QUEST wrong get flag");
        return 0;
    }

    const char* flag = lua_tostring(state, 2);
    const char* quest = lua_tostring(state, 1);
    PC* pc = manager.GetCurrentPC();
    if (!pc)
        return 0;
    lua_pushnumber(state, pc->GetFlag(PCFlagName(quest, flag)));
    return 1;
}

int pc_set_another_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isstring(state, 1) || !lua_isstring(state, 2) || !lua_isnumber(state, 3))
        manager.QuestError("pc_set_another_quest_flag", 0x352, "QUEST wrong set flag");
    else
    {
        const char* quest = lua_tostring(state, 1);
        const char* flag = lua_tostring(state, 2);
        PC* pc = manager.GetCurrentPC();
        const int value = PCTruncate(std::rint(lua_tonumber(state, 3)));
        pc->SetFlag(PCFlagName(quest, flag), value, false);
    }
    return 0;
}

int pc_set_part(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (lua_isnumber(state, 1) && lua_isnumber(state, 2) && character)
    {
        CHARACTER& ch = *character;
        const BYTE part = static_cast<BYTE>(PCSignedNumber(state, 1));
        const WORD value = static_cast<WORD>(PCSignedNumber(state, 2));
        ch.SetPart(part, value);
        ch.UpdatePacket();
    }
    return 0;
}

int pc_set_skillgroup(lua_State* state)
{
    if (!lua_isnumber(state, 1))
        CQuestManager::instance().QuestError("pc_set_skillgroup", 0x447, "QUEST wrong skillgroup number");
    else
    {
        CHARACTER& ch = CurrentPC();
        const auto rounded = std::rint(lua_tonumber(state, 1));
        const BYTE group = rounded >= -32768.0 && rounded <= 32767.0
            ? static_cast<BYTE>(static_cast<short>(rounded)) : 0;
        ch.SetSkillGroup(group);
    }
    return 0;
}

int pc_save_exit_location(lua_State*)
{
    CurrentPC().SaveExitLocation();
    return 1;
}

int pc_warp_exit(lua_State*)
{
    CurrentPC().ExitToSavedLocation();
    return 0;
}

int pc_change_sex(lua_State* state)
{
    lua_pushnumber(state, CurrentPC().ChangeSex() ? 1 : 0);
    return 1;
}

int pc_remove_polymorph(lua_State*)
{
    CHARACTER& ch = CurrentPC();
    ch.RemoveAffect(220);
    ch.SetPolymorph(0, false);
    return 0;
}

int pc_polymorph(lua_State* state)
{
    auto& character = CurrentPC();
    const std::uint32_t race = PCUnsignedNumber(state, 1);
    const int duration = PCSignedNumber(state, 2);
    character.AddAffect(220, POINT_POLYMORPH, static_cast<std::int32_t>(race), 35,
        duration, 0, true, false);
    return 0;
}

int pc_mount(lua_State* state)
{
    if (!lua_isnumber(state, 1))
        return 0;
    int duration = lua_isnumber(state, 2) ? PCSignedNumber(state, 2) : 60;
    const std::uint32_t vnum = PCUnsignedNumber(state, 1);
    if (duration < 0)
        duration = 60;
    CHARACTER& ch = CurrentPC();
    ch.RemoveAffect(221);
    if (vnum)
        ch.AddAffect(221, POINT_MOUNT, static_cast<std::int32_t>(vnum), 0, duration, 0, true, false);
    return 0;
}

int pc_unmount(lua_State*)
{
    CurrentPC().RemoveAffect(221);
    return 0;
}

int pc_revive_horse(lua_State*)
{
    CurrentPC().ReviveHorse();
    return 1;
}

namespace {
struct LocalWarpDiagnostics
{
    const char* function;
    const char* missingMap;
    const char* missingCoordinates;
    int mapArgumentLine, coordinateArgumentLine, mapLine, xLine, yLine;
};

bool ReadLocalWarp(lua_State* state, const LocalWarpDiagnostics& diagnostics,
        long& map, long& x, long& y, const TMapRegion*& region)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isnumber(state, 1))
    {
        manager.QuestError(diagnostics.function, diagnostics.mapArgumentLine, "%s", diagnostics.missingMap);
        return false;
    }
    if (!lua_isnumber(state, 2) || !lua_isnumber(state, 3))
    {
        manager.QuestError(diagnostics.function, diagnostics.coordinateArgumentLine, "%s", diagnostics.missingCoordinates);
        return false;
    }
    map = PCSignedNumber(state, 1);
    region = SECTREE_MANAGER::instance().GetMapRegion(map);
    if (!region)
    {
        manager.QuestError(diagnostics.function, diagnostics.mapLine, "invalid map index %ld", map);
        return false;
    }
    x = PCSignedNumber(state, 2);
    y = PCSignedNumber(state, 3);
    const long width = static_cast<long>(region->ex) - region->sx;
    const long height = static_cast<long>(region->ey) - region->sy;
    if (x > width)
    {
        manager.QuestError(diagnostics.function, diagnostics.xLine, "x coordinate overflow max: %ld input: %ld", width, x);
        return false;
    }
    if (y > height)
    {
        manager.QuestError(diagnostics.function, diagnostics.yLine, "y coordinate overflow max: %ld input: %ld", height, y);
        return false;
    }
    return true;
}
}

int pc_warp(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
        return 0;
    CHARACTER& ch = CurrentPC();
    const int map = lua_isnumber(state, 3) ? PCSignedNumber(state, 3) : 0;
    if (!ch.IsHack(true, true, g_nPortalLimitTime))
    {
        if (test_server) {
            const int y = PCSignedNumber(state, 2);
            const int x = PCSignedNumber(state, 1);
            ch.ChatPacket(1, "pc_warp %d %d %d", x, y, map);
        }
        const int y = PCSignedNumber(state, 2);
        const int x = PCSignedNumber(state, 1);
        ch.WarpSet(x, y, map);
    }
    return 0;
}

int pc_warp_local(lua_State* state)
{
    const LocalWarpDiagnostics diagnostics = {"pc_warp_local", "no map index argument",
        "no coodinate argument", 0xb2, 0xb8, 0xc1, 0xca, 0xd0};
    long map, x, y;
    const TMapRegion* region;
    if (ReadLocalWarp(state, diagnostics, map, x, y, region))
        CurrentPC().WarpSet(x + region->sx, y + region->sy, 0);
    return 0;
}

int pc_set_warp_location(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isnumber(state, 1))
        manager.QuestError("pc_set_warp_location", 0x51, "wrong map index");
    else if (!lua_isnumber(state, 2) || !lua_isnumber(state, 3))
        manager.QuestError("pc_set_warp_location", 0x57, "wrong coodinate");
    else {
        const int y = PCSignedNumber(state, 3);
        const int x = PCSignedNumber(state, 2);
        const int map = PCSignedNumber(state, 1);
        CurrentPC().SetWarpLocation(map, x, y);
    }
    return 0;
}

int pc_set_warp_location_local(lua_State* state)
{
    const LocalWarpDiagnostics diagnostics = {"pc_set_warp_location_local", "wrong map index",
        "wrong coodinate", 0x65, 0x6b, 0x74, 0x7d, 0x83};
    long map, x, y;
    const TMapRegion* region;
    if (ReadLocalWarp(state, diagnostics, map, x, y, region))
        CurrentPC().SetWarpLocation(map, x, y);
    return 0;
}

int pc_get_start_location(lua_State* state)
{
    CHARACTER& character = CurrentPC();
    const auto startIndex = [&character]() {
        const BYTE empire = character.GetEmpire();
        return empire < 4 ? empire : 0;
    };
    lua_pushnumber(state, static_cast<std::int32_t>(g_start_map[startIndex()]));
    lua_pushnumber(state, static_cast<std::uint32_t>(g_start_position[startIndex()][0]) / 100);
    lua_pushnumber(state, static_cast<std::uint32_t>(g_start_position[startIndex()][1]) / 100);
    return 3;
}

int pc_get_war_map(lua_State* state)
{
    CWarMap* map = CurrentPC().GetWarMap();
    lua_pushnumber(state, map ? static_cast<std::int32_t>(map->GetMapIndex()) : 0);
    return 1;
}

int pc_warp_to_guild_war_observer_position(lua_State* state)
{
    luaL_checknumber(state, 1);
    luaL_checknumber(state, 2);
    const DWORD first = PCUnsignedNumber(state, 1);
    const DWORD second = PCUnsignedNumber(state, 2);
    CGuildManager& guilds = CGuildManager::instance();
    CGuild* guild = guilds.FindGuild(first);
    if (!guild || !guilds.FindGuild(second))
        return luaL_error(state, "no such guild with id %d %d", static_cast<int>(first), static_cast<int>(second));

    const long map = guild->GetGuildWarMapIndex(second);
    PIXEL_POSITION position;
    if (!CWarMapManager::instance().GetStartPosition(map, 2, position))
        return luaL_error(state, "not under warp guild war between guild %d %d", static_cast<int>(first), static_cast<int>(second));

    CHARACTER& ch = CurrentPC();
    if (!ch.IsHack(true, true, g_nPortalLimitTime))
    {
        ch.SetQuestFlag("war.is_war_member", 0);
        ch.SaveExitLocation();
        ch.WarpSet(position.x, position.y, map);
    }
    return 0;
}

int pc_select_pid(lua_State* state)
{
    auto& quests = CQuestManager::instance();
    CHARACTER* previous = quests.GetCurrentCharacterPtr();
    CHARACTER* selected = CHARACTER_MANAGER::instance().FindByPID(PCUnsignedNumber(state, 1));
    DWORD oldPID = 0;
    if (selected) {
        quests.GetPC(selected->GetPlayerID());
        if (previous)
            oldPID = previous->GetPlayerID();
    }
    lua_pushnumber(state, oldPID);
    return 1;
}

int pc_select_vid(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* previous = manager.GetCurrentCharacterPtr();
    CHARACTER* selected = CHARACTER_MANAGER::instance().Find(PCUnsignedNumber(state, 1));
    DWORD oldVID = 0;
    if (selected)
    {
        manager.GetPC(selected->GetPlayerID());
        if (previous)
            oldVID = static_cast<DWORD>(previous->GetVID());
    }
    lua_pushnumber(state, oldVID);
    return 1;
}

int pc_send_block_mode(lua_State* state)
{
    const lua_Number value = lua_tonumber(state, 1);
    const BYTE mode = value > -32769.0 && value < 32768.0 ? static_cast<BYTE>(static_cast<short>(value)) : 0;
    CurrentPC().SetBlockModeForce(mode);
    return 1;
}

int pc_is_clear_skill_group(lua_State* state)
{
    lua_pushboolean(state, CurrentPC().GetQuestFlag("skill_group_clear.clear") == 1);
    return 1;
}

int pc_give_gold(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isnumber(state, 1))
        manager.QuestError("pc_give_gold", 0x138, "QUEST : wrong argument");
    else
    {
        const int amount = PCSignedNumber(state, 1);
        if (amount > 0)
        {
            CHARACTER& ch = CurrentPC();
            DBManager::instance().SendMoneyLog(4, ch.GetPlayerID(), amount);
            ch.PointChange(POINT_GOLD, amount, true, false);
        }
        else
            manager.QuestError("pc_give_gold", 0x140, "QUEST : gold amount less then zero");
    }
    return 0;
}

namespace {
std::uint32_t PCRewardBits(long double value)
{
    if (!(value >= -9223372036854775808.0 && value < 9223372036854775808.0))
        return 0;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
}

void LogQuestReward(CHARACTER& ch, int kind, DWORD reward, int count, PC* player = nullptr)
{
    const int level = static_cast<BYTE>(ch.GetLevel());
    const DWORD playerId = ch.GetPlayerID();
    const auto& questName = player ? player->GetCurrentQuestName() : CQuestManager::instance().GetCurrentQuestName();
    const char* postfix = get_table_postfix();
    LogManager::instance().Query("INSERT INTO quest_reward_log%s VALUES('%s',%lu,%d,%d,%lu,%d,NOW())",
        postfix, questName.c_str(), playerId,
        level, kind, reward, count);
}

bool PCItemVnum(lua_State* state, int index, DWORD& vnum, bool reward = false)
{
    if (lua_isnumber(state, index))
    {
        vnum = reward ? static_cast<std::uint32_t>(PCSignedNumber(state, index))
                      : PCUnsignedNumber(state, index);
        return true;
    }
    const char* name = lua_tostring(state, index);
    return name && ITEM_MANAGER::instance().GetVnum(name, vnum);
}

int PCRewardCount(lua_State* state, int index)
{
    if (lua_isnumber(state, index) && lua_tonumber(state, index) > 0)
        return PCTruncate(std::rint(lua_tonumber(state, index)));
    return 1;
}
}

int pc_give_exp(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isnumber(state, 2))
    {
        CHARACTER& ch = CurrentPC();
        const auto loggedAmount = std::rint(lua_tonumber(state, 2));
        const char* label = lua_tostring(state, 1);
        sys_log(0, "QUEST [REWARD] %s give exp %s %d", ch.GetName(), label, PCTruncate(loggedAmount));
        const DWORD exp = PCRewardBits(std::rint(lua_tonumber(state, 2)));
        PC* player = CQuestManager::instance().GetCurrentPC();
        LogQuestReward(ch, 1, exp, 0);
        const char* rewardLabel = lua_tostring(state, 1);
        player->GiveExp(rewardLabel, exp);
    }
    return 0;
}

int pc_give_exp2(lua_State* state)
{
    if (lua_isnumber(state, 1))
    {
        CHARACTER& ch = CurrentPC();
        const auto rounded = std::rint(lua_tonumber(state, 1));
        sys_log(0, "QUEST [REWARD] %s give exp2 %d", ch.GetName(), PCTruncate(rounded));
        const std::uint32_t exp = PCRewardBits(std::rint(lua_tonumber(state, 1)));
        LogQuestReward(ch, 1, exp, 0);
        ch.PointChange(POINT_EXP, static_cast<std::int32_t>(exp), false, false);
    }
    return 0;
}

int pc_give_exp_perc(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* ch = manager.GetCurrentCharacterPtr();
    if (ch && lua_isstring(state, 1) && lua_isnumber(state, 2) && lua_isnumber(state, 3))
    {
        const int level = PCTruncate(std::rint(lua_tonumber(state, 2)));
        const lua_Number percent = lua_tonumber(state, 3);
        const char* label = lua_tostring(state, 1);
        sys_log(0, "QUEST [REWARD] %s give exp %s lev %d percent %g%%",
            ch->GetName(), label, level, percent);
        const int index = std::max(0, std::min(level, 98));
        const DWORD exp = PCRewardBits(static_cast<long double>(static_cast<std::uint32_t>(exp_table[index])) * percent / 100.0L);
        PC* player = manager.GetCurrentPC();
        LogQuestReward(*ch, 1, exp, 0);
        const char* rewardLabel = lua_tostring(state, 1);
        player->GiveExp(rewardLabel, exp);
    }
    return 0;
}

int pc_give_item(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    PC* player = manager.GetCurrentPC();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!lua_isstring(state, 1) || (!lua_isstring(state, 2) && !lua_isnumber(state, 2)))
    {
        manager.QuestError("pc_give_item", 0x1aa, "QUEST : wrong argument");
        return 0;
    }
    DWORD vnum;
    if (!PCItemVnum(state, 2, vnum, true))
    {
        manager.QuestError("pc_give_item", 0x1b4, "QUEST Make item call error : wrong item name : %s",
            lua_tostring(state, 1));
        return 0;
    }
    const int count = PCRewardCount(state, 3);
    if (count < 1)
    {
        manager.QuestError("pc_give_item", 0x1c0, "QUEST Make item call error : wrong item count : %g",
            lua_tonumber(state, 2));
        return 0;
    }

    if (!character || !player)
        return 0;
    CHARACTER& ch = *character;
    const char* characterName = ch.GetName();
    const char* itemLabel = lua_tostring(state, 2);
    sys_log(0, "QUEST [REWARD] item %s to %s", itemLabel, characterName);
    player->GiveItem(lua_tostring(state, 1), vnum, count);
    LogQuestReward(ch, 2, vnum, count, player);
    return 0;
}

int pc_give_or_drop_item(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!lua_isstring(state, 1) && !lua_isnumber(state, 1))
    {
        manager.QuestError("pc_give_or_drop_item", 0x1d9, "QUEST Make item call error : wrong argument");
        return 0;
    }
    DWORD vnum;
    if (!PCItemVnum(state, 1, vnum, true))
    {
        manager.QuestError("pc_give_or_drop_item", 0x1e5, "QUEST Make item call error : wrong item name : %s",
            lua_tostring(state, 1));
        return 0;
    }
    const int count = PCRewardCount(state, 2);
    if (count < 1)
    {
        manager.QuestError("pc_give_or_drop_item", 0x1ef, "QUEST Make item call error : wrong item count : %g",
            lua_tonumber(state, 2));
        return 0;
    }

    if (!character)
        return 0;
    CHARACTER& ch = *character;
    const char* characterName = ch.GetName();
    const char* itemLabel = lua_tostring(state, 1);
    sys_log(0, "QUEST [REWARD] item %s to %s", itemLabel, characterName);
    LogQuestReward(ch, 2, vnum, count);
    CItem* item = ch.AutoGiveItem(vnum, static_cast<BYTE>(count), -1, true);
    if (item && vnum >= 80003 && vnum <= 80007) {
        const DWORD itemId = item->GetID();
        auto& logger = LogManager::instance();
        const DWORD playerId = ch.GetPlayerID();
        logger.GoldBarLog(playerId, itemId, QUEST, "quest: give_item2");
    }
    return 0;
}

int pc_count_item(lua_State* state)
{
    DWORD vnum;
    int count = 0;
    if (lua_isnumber(state, -1)) {
        const DWORD itemVnum = PCUnsignedNumber(state, -1);
        count = CurrentPC().CountSpecifyItem(itemVnum);
    }
    else if (lua_isstring(state, -1))
    {
        const char* name = lua_tostring(state, 1);
        if (name && ITEM_MANAGER::instance().GetVnum(name, vnum))
            count = CurrentPC().CountSpecifyItem(vnum);
        else {
            const char* loggedName = lua_tostring(state, 1);
            CQuestManager::instance().QuestError("pc_count_item", 0x240,
                "QUEST count_item call error : wrong item name : %s", loggedName ? loggedName : "");
        }
    }
    lua_pushnumber(state, count);
    return 1;
}

int pc_remove_item(lua_State* state)
{
    const int arguments = lua_gettop(state);
    if (arguments != 1 && arguments != 2)
        return 0;
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isnumber(state, 1) && !lua_isstring(state, 1))
    {
        manager.QuestError("pc_remove_item", arguments == 1 ? 0x262 : 0x27b, "QUEST remove_item wrong argument");
        return 0;
    }
    DWORD vnum;
    if (!PCItemVnum(state, 1, vnum))
    {
        manager.QuestError("pc_remove_item", arguments == 1 ? 0x25c : 0x275,
            "QUEST remove_item call error : wrong item name : %s", lua_tostring(state, 1));
        return 0;
    }

    const DWORD count = arguments == 1 ? 1 : PCUnsignedNumber(state, 2);
    CHARACTER& ch = CurrentPC();
    const int playerId = static_cast<int>(ch.GetPlayerID());
    const char* characterName = ch.GetName();
    if (arguments == 1)
        sys_log(0, "QUEST remove a item vnum %d of %s[%d]", static_cast<int>(vnum),
            characterName, playerId);
    else
        sys_log(0, "QUEST remove items(vnum %d) count %d of %s[%d]", static_cast<int>(vnum),
            static_cast<int>(count), characterName, playerId);
    ch.RemoveSpecifyItem(vnum, count);
    return 0;
}

int pc_enough_inventory(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    const TItemTable* item = nullptr;
    if (lua_isnumber(state, 1)) {
        const DWORD itemVnum = PCUnsignedNumber(state, 1);
        item = ITEM_MANAGER::instance().GetTable(itemVnum);
    }
    lua_pushboolean(state, item && character && character->GetEmptyInventory(item->bSize) != -1);
    return 1;
}

namespace {
const char groupGold[] = "\265\267\040\045\144\040\263\311\300\273\040\310\271\265\346\307\337\275\300\264\317\264\331\056";
const char groupExperienceNotice[] = "\263\252\271\253\277\241\274\255\040\272\316\305\315\040\275\305\272\361\307\321\040\272\373\300\314\040\263\252\277\311\264\317\264\331\056";
const char groupExperienceAmount[] = "\045\144\300\307\040\260\346\307\350\304\241\270\246\040\310\271\265\346\307\337\275\300\264\317\264\331\056";

bool IsPositionScroll(CItem* item)
{
    return item && item->GetType() == 3 && item->GetSubType() == 1 &&
        (static_cast<std::int32_t>(item->GetValue(0)) == 1 ||
         static_cast<std::int32_t>(item->GetValue(0)) == 2);
}

const int teleportTowns[][2] = {
    {4743, 9548},
    {3235, 9086},
    {3531, 8829},
    {638, 1664},
    {1745, 1909},
    {1455, 2400},
    {9599, 2692},
    {8036, 2984},
    {8639, 2460},
    {4350, 2143},
};
}

int pc_has_master_skill(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    bool found = false;
    for (DWORD vnum = 0; vnum < 255; ++vnum)
        if (ch.GetSkillMasterType(vnum) > 0 && ch.GetSkillLevel(vnum) > 20)
        {
            found = true;
            break;
        }
    lua_pushboolean(state, found);
    return 1;
}

int pc_is_skill_book_no_delay(lua_State* state)
{
    lua_pushboolean(state, CurrentPC().FindAffect(513, 0) != nullptr);
    return 1;
}

int pc_remove_skill_book_no_delay(lua_State*)
{
    CurrentPC().RemoveAffect(513);
    return 0;
}

int pc_set_skill_level(lua_State* state)
{
    const auto vnum = PCUnsignedNumber(state, 1);
    const lua_Number value = lua_tonumber(state, 2);
    const BYTE level = value > -32769.0 && value < 32768.0
        ? static_cast<BYTE>(static_cast<short>(value)) : 0;
    CHARACTER& ch = CurrentPC();
    ch.SetSkillLevel(vnum, level);
    ch.SkillLevelPacket();
    return 1;
}

int pc_clear_one_skill(lua_State* state)
{
    const int vnum = PCSignedNumber(state, 1);
    sys_log(0, "%d skill clear", vnum);
    CHARACTER* ch = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!ch)
    {
        sys_log(0, "skill clear fail");
        lua_pushnumber(state, 0);
    }
    else
    {
        sys_log(0, "%d skill clear", vnum);
        ch->ResetOneSkill(static_cast<DWORD>(vnum));
    }
    return 0;
}

int pc_clear_skill(lua_State*)
{
    if (CHARACTER* ch = CQuestManager::instance().GetCurrentCharacterPtr())
        ch->ClearSkill();
    return 0;
}

int pc_clear_sub_skill(lua_State*)
{
    if (CHARACTER* ch = CQuestManager::instance().GetCurrentCharacterPtr())
        ch->ClearSubSkill();
    return 0;
}

int pc_have_map_scroll(lua_State* state)
{
    bool found = false;
    if (lua_isstring(state, 1))
    {
        const char* mapName = lua_tostring(state, 1);
        const auto* region = SECTREE_MANAGER::instance().FindRegionByPartialName(mapName);
        if (region)
        {
            CHARACTER& ch = CurrentPC();
            for (BYTE cell = 0; cell < INVENTORY_MAX_NUM; ++cell)
            {
                CItem* item = ch.GetItem(cell);
                if (!IsPositionScroll(item))
                    continue;
                const long* sockets = item->GetSockets();
                const auto x = static_cast<std::int32_t>(sockets[0]);
                const auto y = static_cast<std::int32_t>(sockets[1]);
                if (region->sx <= x && region->sy <= y && x <= region->ex && y <= region->ey)
                {
                    found = true;
                    break;
                }
            }
        }
    }
    lua_pushboolean(state, found);
    return 1;
}

int pc_have_pos_scroll(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    bool found = false;
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
        CQuestManager::instance().QuestError("pc_have_pos_scroll", 0x4f9, "invalid x y position");
    else if (character)
    {
        CHARACTER& ch = *character;
        const auto x = static_cast<std::uint32_t>(PCSignedNumber(state, 1));
        const auto y = static_cast<std::uint32_t>(PCSignedNumber(state, 2));
        const auto radius = static_cast<float>(lua_tonumber(state, 3));
        for (BYTE cell = 0; cell < INVENTORY_MAX_NUM; ++cell)
        {
            CItem* item = ch.GetItem(cell);
            if (!IsPositionScroll(item))
                continue;
            const long* sockets = item->GetSockets();
            const auto dx = x - static_cast<std::uint32_t>(sockets[0]);
            const auto dy = y - static_cast<std::uint32_t>(sockets[1]);
            const auto distance = static_cast<std::int32_t>(dx * dx + dy * dy);
            if (distance < static_cast<double>(radius) * radius)
            {
                found = true;
                break;
            }
        }
    }
    lua_pushboolean(state, found);
    return 1;
}

int pc_get_equip_refine_level(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    const int cell = PCSignedNumber(state, 1);
    int level = 0;
    if (static_cast<unsigned int>(cell) < 19)
    {
        CItem* item = character ? character->GetWear(static_cast<BYTE>(cell)) : nullptr;
        if (item)
            level = item->GetRefineLevel();
    }
    else
        CQuestManager::instance().QuestError("pc_get_equip_refine_level", 0x528, "invalid wear position %d", cell);
    lua_pushnumber(state, level);
    return 1;
}

int pc_give_item_from_special_item_group(lua_State* state)
{
    luaL_checknumber(state, 1);
    CHARACTER& ch = CurrentPC();
    DWORD vnum = 0;
    DWORD count = 0;
    CItem* item = nullptr;
    ch.GiveItemFromSpecialItemGroup(PCUnsignedNumber(state, 1), vnum, count, item);
    if (!item)
    {
        if (vnum == 1)
        {
            const char* format = locale_find(groupGold, g_iUseLocale);
            ch.ChatPacket(1, format, static_cast<std::int32_t>(count));
        }
        else if (vnum == 2)
        {
            ch.ChatPacket(1, locale_find(groupExperienceNotice, g_iUseLocale));
            const char* format = locale_find(groupExperienceAmount, g_iUseLocale);
            ch.ChatPacket(1, format, static_cast<std::int32_t>(count));
        }
    }
    return 0;
}

int pc_teleport(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    long x;
    long y;
    if (lua_isnumber(state, 1))
    {
        const int town = PCSignedNumber(state, 1);
        if (static_cast<unsigned int>(town) >= sizeof(teleportTowns) / sizeof(teleportTowns[0]))
        {
            lua_pushnumber(state, 0);
            return 1;
        }
        x = teleportTowns[town][0] * 100;
        y = teleportTowns[town][1] * 100;
    }
    else
    {
        const char* name = lua_tostring(state, 1);
        CHARACTER_MANAGER& chars = CHARACTER_MANAGER::instance();
        CHARACTER* target = name ? chars.FindPC(name) : nullptr;
        if (!target)
        {
            CCI* peer = name ? P2P_MANAGER::instance().Find(name) : nullptr;
            if (!peer)
            {
                if (!name || !chars.FindPC(name))
                    ch.ChatPacket(1, "There is no one by that name");
            }
            else if (peer->bChannel == g_bChannel)
            {
                PIXEL_POSITION center;
                if (SECTREE_MANAGER::instance().GetCenterPositionOfMap(peer->bMapIndex, center))
                {
                    ch.ChatPacket(1, "You warp to ( %d , %d )", static_cast<int>(center.x), static_cast<int>(center.y));
                    ch.WarpSet(center.x, center.y, 0);
                    lua_pushnumber(state, 1);
                }
                else
                    ch.ChatPacket(1, "Cannot find map (index %d)", static_cast<int>(peer->bMapIndex));
            }
            else
                ch.ChatPacket(1, "Target is in %d channel (my channel %d)",
                    static_cast<int>(peer->bChannel), static_cast<int>(g_bChannel));
            lua_pushnumber(state, 0);
            return 1;
        }
        x = target->GetX() / 100 * 100;
        y = target->GetY() / 100 * 100;
    }

    ch.ChatPacket(1, "You warp to ( %d , %d )", static_cast<int>(x), static_cast<int>(y));
    ch.WarpSet(x, y, 0);
    ch.Stop();
    lua_pushnumber(state, 1);
    return 1;
}

int pc_reset_point(lua_State*)
{
    const int level = CurrentPC().GetLevel();
    CurrentPC().ResetPoint(level);
    return 0;
}

int pc_destroy_guild(lua_State*)
{
    CHARACTER& ch = CurrentPC();
    if (CGuild* guild = ch.GetGuild())
        guild->RequestDisband(ch.GetPlayerID());
    return 0;
}

int pc_remove_from_guild(lua_State*)
{
    CHARACTER& ch = CurrentPC();
    if (CGuild* guild = ch.GetGuild())
        guild->RequestRemoveMember(ch.GetPlayerID());
    return 0;
}

int pc_give_lotto(lua_State*)
{
    CQuestManager& manager = CQuestManager::instance();
    const DWORD pid = CurrentPC().GetPlayerID();
    sys_log(0, "TRY GIVE LOTTO TO pid %lu", pid);
    DWORD* reward = new DWORD[3];
    reward[0] = 50001;
    reward[1] = 1;
    reward[2] = manager.GetEventFlag("lotto_round");
    const DWORD sqlPlayerId = CurrentPC().GetPlayerID();
    const char* postfix = get_table_postfix();
    auto& database = DBManager::instance();
    const DWORD queryPlayerId = CurrentPC().GetPlayerID();
    database.ReturnQuery(3, queryPlayerId, reward,
        "INSERT INTO lotto_list VALUES(0, 'server%s', %lu, NOW())", postfix, sqlPlayerId);
    return 0;
}

int pc_give_polymorph_book(lua_State* state)
{
    if (!lua_isnumber(state, 1) && !lua_isnumber(state, 2) &&
        !lua_isnumber(state, 3) && !lua_isnumber(state, 4))
    {
        CQuestManager::instance().QuestError("pc_give_polymorph_book", 0x6f7,
            "Wrong Quest Function Arguments: pc_give_polymorph_book");
        return 0;
    }

    LPCHARACTER character = CQuestManager::instance().GetCurrentCharacterPtr();
    const BYTE levelLimit = PCShortByte(state, 4);
    const BYTE bookLevel = PCShortByte(state, 3);
    const DWORD practice = PCUnsignedNumber(state, 2);
    const DWORD mobVnum = PCUnsignedNumber(state, 1);
    CPolymorphUtils::instance().GiveBook(character,
        mobVnum, practice, bookLevel, levelLimit);
    return 1;
}

int pc_upgrade_polymorph_book(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    LPCHARACTER character = manager.GetCurrentCharacterPtr();
    LPITEM item = manager.GetCurrentItem();
    const bool upgraded = CPolymorphUtils::instance().BookUpgrade(character, item);
    lua_pushboolean(state, upgraded);
    return 1;
}

int pc_change_empire(lua_State* state)
{
    CHARACTER& character = CurrentPC();
    const BYTE empire = PCShortByte(state, 1);
    lua_pushnumber(state, character.ChangeEmpire(empire));
    return 1;
}

int pc_get_change_empire_count(lua_State* state)
{
    lua_pushnumber(state, CurrentPC().GetChangeEmpireCount());
    return 1;
}

int pc_set_change_empire_count(lua_State*)
{
    CurrentPC().SetChangeEmpireCount();
    return 0;
}

namespace {
int ChangePCName(lua_State* state)
{
    CHARACTER& ch = CurrentPC();
    if (!ch.GetNewName().empty())
        return 0;
    if (!lua_isstring(state, 1))
        return 1;
    const char* name = lua_tostring(state, 1);
    if (!check_name || !check_name(name))
        return 2;

    DBManager& db = DBManager::instance();
    const char* postfix = get_table_postfix();
    const std::size_t length = std::strlen(name);
    if (length > CHARACTER_NAME_MAX_LEN)
        return 2;
    char escaped[2 * CHARACTER_NAME_MAX_LEN + 1];
    db.EscapeString(escaped, name, static_cast<unsigned long>(length));
    std::unique_ptr<SQLMsg> existing(db.DirectQuery(
        "SELECT COUNT(*) FROM player%s WHERE name='%s'", postfix, escaped));
    SQLResult* result = existing ? existing->Get() : nullptr;
    if (result && result->iNumRows > 0 && result->pSQLResult)
    {
        const MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
        if (row && row[0] && std::atoi(row[0]) != 0)
            return 3;
    }

    const std::uint32_t pid = ch.GetPlayerID();
    db_clientdesc->DBPacketHeader(0x2a, 0, sizeof(pid));
    db_clientdesc->Packet(&pid, sizeof(pid));
    std::unique_ptr<SQLMsg> update(db.DirectQuery(
        "UPDATE player%s SET name='%s' WHERE id=%d", postfix, escaped, static_cast<int>(pid)));
    ch.SetNewName(name);
    return 4;
}
}

int pc_change_name(lua_State* state)
{
    lua_pushnumber(state, ChangePCName(state));
    return 1;
}

int pc_aggregate_monster(lua_State*)
{
    CurrentPC().AggregateMonster();
    return 0;
}

int pc_forget_my_attacker(lua_State*)
{
    CurrentPC().ForgetMyAttacker();
    return 0;
}

int pc_attract_ranger(lua_State*)
{
    CurrentPC().AttractRanger();
    return 0;
}

int pc_refine_equip(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
    {
        CQuestManager::instance().QuestError("pc_refine_equip", 0x53d, "invalid argument");
        lua_pushboolean(state, false);
        return 1;
    }

    if (!character) {
        lua_pushboolean(state, false);
        return 1;
    }
    CHARACTER& ch = *character;
    const BYTE slot = static_cast<BYTE>(PCSignedNumber(state, 1));
    const int levelLimit = PCSignedNumber(state, 2);
    const int chance = lua_isnumber(state, 3) ? PCSignedNumber(state, 3) : 100;
    CItem* original = ch.GetWear(slot);
    const TItemTable* proto = original ? original->GetProto() : nullptr;
    if (!proto || !proto->dwRefinedVnum || original->GetRefineLevel() > levelLimit ||
        (chance != 100 && number_ex(1, 100, const_cast<char*>("questlua_pc.cpp"), 0x559) > chance))
    {
        lua_pushboolean(state, false);
        return 1;
    }

    lua_pushboolean(state, true);
    ITEM_MANAGER& manager = ITEM_MANAGER::instance();
    CItem* replacement = manager.CreateItem(proto->dwRefinedVnum, 1, 0, false, -1);
    if (!replacement)
        return 1;

    const long* sockets = original->GetSockets();
    for (int i = 0; i < 3 && sockets[i] != 0; ++i)
        replacement->SetSocket(i, 1);
    int nextSocket = 0;
    for (int i = 0; i < 3; ++i)
        if (sockets[i] > 2 && sockets[i] != 0x7120)
            replacement->SetSocket(nextSocket++, sockets[i]);

    original->CopyAttributeTo(replacement);
    manager.RemoveItem(original, "REMOVE (REFINE SUCCESS)");
    replacement->EquipTo(&ch, slot);
    manager.FlushDelayedSave(replacement);
    const TItemTable* newProto = replacement->GetProto();
    LogManager::instance().ItemLog(ch.GetPlayerID(), ch.GetX(), ch.GetY(), replacement->GetID(),
        "REFINE SUCCESS (QUEST)", newProto ? newProto->szLocaleName : "");
    return 1;
}

int pc_mining(lua_State*)
{
    CurrentPC().mining(CQuestManager::instance().GetCurrentNPCCharacterPtr());
    return 0;
}

int pc_ore_refine(lua_State* state)
{
    bool success = false;
    if (lua_isnumber(state, 1))
    {
        const int cost = PCSignedNumber(state, 1);
        const int chance = PCSignedNumber(state, 2);
        const auto cell = static_cast<BYTE>(PCSignedNumber(state, 3));
        CQuestManager& manager = CQuestManager::instance();
        CHARACTER& ch = CurrentPC();
        CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
        CItem* ore = manager.GetCurrentItem();
        CItem* stone = ch.GetItem(cell);
        if (ore && stone)
            success = mining::OreRefine(&ch, npc, ore, cost, chance, stone);
    }
    lua_pushboolean(state, success);
    return 1;
}

int pc_diamond_refine(lua_State* state)
{
    bool success = false;
    if (lua_isnumber(state, 1))
    {
        const int cost = PCSignedNumber(state, 1);
        const int chance = PCSignedNumber(state, 2);
        CQuestManager& manager = CQuestManager::instance();
        CHARACTER& ch = CurrentPC();
        CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
        if (CItem* ore = manager.GetCurrentItem())
            success = mining::OreRefine(&ch, npc, ore, cost, chance, nullptr);
    }
    lua_pushboolean(state, success);
    return 1;
}

int pc_learn_grand_master_skill(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!lua_isnumber(state, 1))
    {
        CQuestManager::instance().QuestError("pc_learn_grand_master_skill", 0x43, "wrong skill index");
        return 0;
    }
    const auto skill = static_cast<DWORD>(PCSignedNumber(state, 1));
    lua_pushboolean(state, character && character->LearnGrandMasterSkill(skill));
    return 1;
}

void RegisterPCFunctionTable()
{
    luaL_reg functions[] = {
        {"get_level", pc_get_level},
        {"get_next_exp", pc_get_next_exp},
        {"get_exp", pc_get_exp},
        {"get_job", pc_get_job},
        {"get_race", pc_get_race},
        {"change_sex", pc_change_sex},
        {"gethp", pc_get_hp},
        {"get_hp", pc_get_hp},
        {"getmaxhp", pc_get_max_hp},
        {"get_max_hp", pc_get_max_hp},
        {"getsp", pc_get_sp},
        {"get_sp", pc_get_sp},
        {"getmaxsp", pc_get_max_sp},
        {"get_max_sp", pc_get_max_sp},
        {"change_sp", pc_change_sp},
        {"getmoney", pc_get_money},
        {"get_money", pc_get_money},
        {"get_real_alignment", pc_get_real_alignment},
        {"get_alignment", pc_get_alignment},
        {"getweapon", pc_get_weapon},
        {"get_weapon", pc_get_weapon},
        {"getarmor", pc_get_armor},
        {"get_armor", pc_get_armor},
        {"getgold", pc_get_money},
        {"get_gold", pc_get_money},
        {"changegold", pc_change_money},
        {"changemoney", pc_change_money},
        {"changealignment", pc_change_alignment},
        {"change_gold", pc_change_money},
        {"change_money", pc_change_money},
        {"change_alignment", pc_change_alignment},
        {"getname", pc_get_name},
        {"get_name", pc_get_name},
        {"get_vid", pc_get_vid},
        {"getplaytime", pc_get_playtime},
        {"get_playtime", pc_get_playtime},
        {"getleadership", pc_get_leadership},
        {"get_leadership", pc_get_leadership},
        {"getqf", pc_get_quest_flag},
        {"setqf", pc_set_quest_flag},
        {"delqf", pc_del_quest_flag},
        {"getf", pc_get_another_quest_flag},
        {"setf", pc_set_another_quest_flag},
        {"get_x", pc_get_x},
        {"get_y", pc_get_y},
        {"getx", pc_get_x},
        {"gety", pc_get_y},
        {"get_local_x", pc_get_local_x},
        {"get_local_y", pc_get_local_y},
        {"getcurrentmapindex", pc_get_current_map_index},
        {"get_map_index", pc_get_current_map_index},
        {"give_exp", pc_give_exp},
        {"give_exp_perc", pc_give_exp_perc},
        {"give_exp2", pc_give_exp2},
        {"give_item", pc_give_item},
        {"give_item2", pc_give_or_drop_item},
        {"give_gold", pc_give_gold},
        {"count_item", pc_count_item},
        {"remove_item", pc_remove_item},
        {"countitem", pc_count_item},
        {"removeitem", pc_remove_item},
        {"reset_point", pc_reset_point},
        {"has_guild", pc_hasguild},
        {"hasguild", pc_hasguild},
        {"get_guild", pc_getguild},
        {"getguild", pc_getguild},
        {"isguildmaster", pc_isguildmaster},
        {"is_guild_master", pc_isguildmaster},
        {"destroy_guild", pc_destroy_guild},
        {"remove_from_guild", pc_remove_from_guild},
        {"in_dungeon", pc_in_dungeon},
        {"getempire", pc_get_empire},
        {"get_empire", pc_get_empire},
        {"get_skill_group", pc_get_skillgroup},
        {"set_skill_group", pc_set_skillgroup},
        {"warp", pc_warp},
        {"warp_local", pc_warp_local},
        {"warp_exit", pc_warp_exit},
        {"set_warp_location", pc_set_warp_location},
        {"set_warp_location_local", pc_set_warp_location_local},
        {"get_start_location", pc_get_start_location},
        {"has_master_skill", pc_has_master_skill},
        {"set_part", pc_set_part},
        {"get_part", pc_get_part},
        {"is_polymorphed", pc_is_polymorphed},
        {"remove_polymorph", pc_remove_polymorph},
        {"is_mount", pc_is_mount},
        {"polymorph", pc_polymorph},
        {"mount", pc_mount},
        {"unmount", pc_unmount},
        {"warp_to_guild_war_observer_position", pc_warp_to_guild_war_observer_position},
        {"give_item_from_special_item_group", pc_give_item_from_special_item_group},
        {"learn_grand_master_skill", pc_learn_grand_master_skill},
        {"is_skill_book_no_delay", pc_is_skill_book_no_delay},
        {"remove_skill_book_no_delay", pc_remove_skill_book_no_delay},
        {"enough_inventory", pc_enough_inventory},
        {"get_horse_level", pc_get_horse_level},
        {"is_horse_alive", pc_is_horse_alive},
        {"revive_horse", pc_revive_horse},
        {"have_pos_scroll", pc_have_pos_scroll},
        {"have_map_scroll", pc_have_map_scroll},
        {"get_war_map", pc_get_war_map},
        {"get_equip_refine_level", pc_get_equip_refine_level},
        {"refine_equip", pc_refine_equip},
        {"get_skill_level", pc_get_skill_level},
        {"give_lotto", pc_give_lotto},
        {"aggregate_monster", pc_aggregate_monster},
        {"forget_my_attacker", pc_forget_my_attacker},
        {"pc_attract_ranger", pc_attract_ranger},
        {"select", pc_select_vid},
        {"get_sex", pc_get_sex},
        {"is_married", pc_is_married},
        {"is_engaged", pc_is_engaged},
        {"is_engaged_or_married", pc_is_engaged_or_married},
        {"is_gm", pc_is_gm},
        {"get_gm_level", pc_get_gm_level},
        {"mining", pc_mining},
        {"ore_refine", pc_ore_refine},
        {"diamond_refine", pc_diamond_refine},
        {"clear_one_skill", pc_clear_one_skill},
        {"clear_skill", pc_clear_skill},
        {"clear_sub_skill", pc_clear_sub_skill},
        {"is_clear_skill_group", pc_is_clear_skill_group},
        {"save_exit_location", pc_save_exit_location},
        {"teleport", pc_teleport},
        {"set_skill_level", pc_set_skill_level},
        {"give_polymorph_book", pc_give_polymorph_book},
        {"upgrade_polymorph_book", pc_upgrade_polymorph_book},
        {"get_premium_remain_sec", pc_get_premium_remain_sec},
        {"send_block_mode", pc_send_block_mode},
        {"change_empire", pc_change_empire},
        {"get_change_empire_count", pc_get_change_empire_count},
        {"set_change_empire_count", pc_set_change_empire_count},
        {"change_name", pc_change_name},
        {nullptr, nullptr},
    };
    CQuestManager::instance().AddLuaFunctionTable("pc", functions);
}
}
