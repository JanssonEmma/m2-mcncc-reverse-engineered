#include "char.h"
#include "dungeon.h"
#include "party.h"
#include "questmanager.h"
#include "questpc.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"

#include <cfenv>
#include <climits>
#include <cmath>
#include <cstdint>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
CDungeon* CurrentDungeon()
{
    return CQuestManager::instance().GetCurrentDungeon();
}

bool ReadIntValue(lua_Number input, int& value)
{
    const double number = std::trunc(input);
    if (!(number >= INT_MIN && number <= INT_MAX))
        return false;
    value = static_cast<int>(number);
    return true;
}

bool ReadInt(lua_State* state, int argument, int& value)
{
    return lua_isnumber(state, argument) && ReadIntValue(lua_tonumber(state, argument), value);
}

std::uint32_t DungeonDwordValue(lua_Number number)
{
    if (!(number >= -9223372036854775808.0 && number < 9223372036854775808.0)) {
        std::feraiseexcept(FE_INVALID);
        return 0;
    }
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(number));
}

int DungeonIntValue(lua_Number value)
{
    if (value > -2147483649.0 && value < 2147483648.0)
        return static_cast<int>(value);
    std::feraiseexcept(FE_INVALID);
    return INT_MIN;
}

bool HasCountArguments(lua_State* state)
{
    return lua_isnumber(state, 1) && lua_isnumber(state, 2);
}
}

int dungeon_exit(lua_State*)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr())
        character->ExitToSavedLocation();
    return 0;
}

int dungeon_exit_all(lua_State*)
{
    if (CDungeon* dungeon = CurrentDungeon())
        dungeon->ExitAll();
    return 0;
}

int dungeon_get_kill_stone_count(lua_State* state)
{
    CDungeon* dungeon = HasCountArguments(state) ? CurrentDungeon() : nullptr;
    lua_pushnumber(state, dungeon ? dungeon->GetKillStoneCount() : 0);
    return 1;
}

int dungeon_get_kill_mob_count(lua_State* state)
{
    CDungeon* dungeon = HasCountArguments(state) ? CurrentDungeon() : nullptr;
    lua_pushnumber(state, dungeon ? dungeon->GetKillMobCount() : 0);
    return 1;
}

int dungeon_is_use_potion(lua_State* state)
{
    CDungeon* dungeon = HasCountArguments(state) ? CurrentDungeon() : nullptr;
    lua_pushboolean(state, !dungeon || dungeon->IsUsePotion());
    return 1;
}

int dungeon_revived(lua_State* state)
{
    CDungeon* dungeon = HasCountArguments(state) ? CurrentDungeon() : nullptr;
    lua_pushboolean(state, !dungeon || dungeon->IsUseRevive());
    return 1;
}

int dungeon_jump_all(lua_State* state)
{
    if (lua_gettop(state) >= 2 && lua_isnumber(state, 1) && lua_isnumber(state, 2)) {
        if (CDungeon* dungeon = CurrentDungeon()) {
            int x, y;
            if (ReadIntValue(lua_tonumber(state, 2), y)) {
                const lua_Number xValue = lua_tonumber(state, 1);
                const auto map = static_cast<std::int32_t>(dungeon->GetMapIndex());
                if (ReadIntValue(xValue, x))
                    dungeon->JumpAll(map, x, y);
            }
        }
    }
    return 0;
}

int dungeon_new_jump_all(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (lua_gettop(state) < 3) {
        manager.QuestError("dungeon_new_jump_all", 0x105, "not enough argument");
        return 0;
    }

    int map;
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3)
        || !ReadIntValue(lua_tonumber(state, 1), map)) {
        manager.QuestError("dungeon_new_jump_all", 0x105, "not enough argument");
        return 0;
    }

    CDungeonManager& dungeons = CDungeonManager::instance();
    CDungeon* dungeon = dungeons.Create(map);
    if (!dungeon) {
        manager.QuestError("dungeon_new_jump_all", 0x10f, "cannot create dungeon %d", map);
        return 0;
    }

    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!character) {
        dungeons.Destroy(dungeon);
        return 0;
    }

    int x, y;
    if (!ReadIntValue(lua_tonumber(state, 3), y) || !ReadIntValue(lua_tonumber(state, 2), x)) {
        dungeons.Destroy(dungeon);
        manager.QuestError("dungeon_new_jump_all", 0x105, "not enough argument");
        return 0;
    }

    dungeon->JumpAll(character->GetMapIndex(), x, y);
    return 0;
}

int dungeon_new_jump(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (lua_gettop(state) < 3) {
        manager.QuestError("dungeon_new_jump", 0xe7, "not enough argument");
        return 0;
    }

    int map;
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3)
        || !ReadIntValue(lua_tonumber(state, 1), map)) {
        manager.QuestError("dungeon_new_jump", 0xed, "wrong argument");
        return 0;
    }

    CDungeonManager& dungeons = CDungeonManager::instance();
    CDungeon* dungeon = dungeons.Create(map);
    if (!dungeon) {
        manager.QuestError("dungeon_new_jump", 0xf7, "cannot create dungeon %d", map);
        return 0;
    }

    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!character) {
        dungeons.Destroy(dungeon);
        return 0;
    }

    int x, y;
    if (!ReadIntValue(lua_tonumber(state, 3), y) || !ReadIntValue(lua_tonumber(state, 2), x)) {
        dungeons.Destroy(dungeon);
        manager.QuestError("dungeon_new_jump", 0xed, "wrong argument");
        return 0;
    }

    character->WarpSet(dungeon->GetMapIndex(), x, y);
    return 0;
}

int dungeon_regen_file(lua_State* state)
{
    if (!lua_isstring(state, 1)) {
        CQuestManager::instance().QuestError("dungeon_regen_file", 0x74, "wrong filename");
    } else if (CDungeon* dungeon = CurrentDungeon()) {
        dungeon->SpawnRegen(lua_tostring(state, 1), true);
    }
    return 0;
}

int dungeon_set_regen_file(lua_State* state)
{
    if (!lua_isstring(state, 1)) {
        CQuestManager::instance().QuestError("dungeon_set_regen_file", 0x85, "wrong filename");
    } else if (CDungeon* dungeon = CurrentDungeon()) {
        dungeon->SpawnRegen(lua_tostring(state, 1), false);
    }
    return 0;
}

int dungeon_clear_regen(lua_State*)
{
    if (CDungeon* dungeon = CurrentDungeon())
        dungeon->ClearRegen();
    return 0;
}

int dungeon_set_exit_all_at_eliminate(lua_State* state)
{
    if (!lua_isnumber(state, 1)) {
        CQuestManager::instance().QuestError("dungeon_set_exit_all_at_eliminate", 0xa6, "wrong time");
    } else if (CDungeon* dungeon = CurrentDungeon()) {
        dungeon->SetExitAllAtEliminate(DungeonIntValue(lua_tonumber(state, 1)));
    }
    return 0;
}

int dungeon_set_warp_at_eliminate(lua_State* state)
{
    constexpr const char* errors[] = {"wrong time", "wrong map index", "wrong X", "wrong Y"};
    constexpr int lines[] = {0xb7, 0xbd, 0xc3, 0xc9};
    CQuestManager& manager = CQuestManager::instance();
    for (int index = 0; index < 4; ++index) {
        if (!lua_isnumber(state, index + 1)) {
            manager.QuestError("dungeon_set_warp_at_eliminate", lines[index], errors[index]);
            return 0;
        }
    }
    const char* filename = lua_gettop(state) >= 5 ? lua_tostring(state, 5) : nullptr;
    if (CDungeon* dungeon = manager.GetCurrentDungeon()) {
        const int y = DungeonIntValue(lua_tonumber(state, 4));
        const int x = DungeonIntValue(lua_tonumber(state, 3));
        const int map = DungeonIntValue(lua_tonumber(state, 2));
        const int delay = DungeonIntValue(lua_tonumber(state, 1));
        dungeon->SetWarpAtEliminate(delay, map, x, y, filename);
    } else {
        manager.QuestError("dungeon_set_warp_at_eliminate", 0xde, "cannot find dungeon");
    }
    return 0;
}

int dungeon_get_map_index(lua_State* state)
{
    CDungeon* dungeon = CurrentDungeon();
    if (!dungeon)
        CQuestManager::instance().QuestError("dungeon_get_map_index", 0x69, "no dungeon !!!");
    else
        sys_log(0, "Dungeon GetMapIndex %d", static_cast<int>(dungeon->GetMapIndex()));
    lua_pushnumber(state, dungeon ? static_cast<std::int32_t>(dungeon->GetMapIndex()) : 0);
    return 1;
}

int dungeon_check_eliminated(lua_State*)
{
    if (CDungeon* dungeon = CurrentDungeon())
        dungeon->CheckEliminated();
    return 0;
}

int dungeon_count_monster(lua_State* state)
{
    CDungeon* dungeon = CurrentDungeon();
    if (!dungeon)
        CQuestManager::instance().QuestError("dungeon_count_monster", 0x337, "not in a dungeon");
    lua_pushnumber(state, dungeon ? dungeon->CountMonster() : INT_MAX);
    return 1;
}

int dungeon_set_flag(lua_State* state)
{
    const bool valid = lua_isstring(state, 1) && lua_isnumber(state, 2);
    CQuestManager& manager = CQuestManager::instance();
    if (!valid) {
        manager.QuestError("dungeon_set_flag", 0x30, "wrong set flag");
    } else if (CDungeon* dungeon = manager.GetCurrentDungeon()) {
        const char* name = lua_tostring(state, 1);
        const int value = DungeonIntValue(lua_tonumber(state, 2));
        dungeon->SetFlag(name, value);
    } else {
        manager.QuestError("dungeon_set_flag", 0x3f, "no dungeon !!!");
    }
    return 0;
}

int dungeon_get_flag(lua_State* state)
{
    const bool validName = lua_isstring(state, 1);
    CQuestManager& manager = CQuestManager::instance();
    if (!validName)
        manager.QuestError("dungeon_get_flag", 0x49, "wrong get flag");
    CDungeon* dungeon = manager.GetCurrentDungeon();
    if (!dungeon)
        manager.QuestError("dungeon_get_flag", 0x56, "no dungeon !!!");
    lua_pushnumber(state, dungeon && validName ? dungeon->GetFlag(lua_tostring(state, 1)) : 0);
    return 1;
}

int dungeon_select(lua_State* state)
{
    const auto map = static_cast<std::int32_t>(DungeonDwordValue(lua_tonumber(state, 1)));
    CDungeon* dungeon = map ? CDungeonManager::instance().Find(map) : nullptr;
    CQuestManager::instance().SelectDungeon(dungeon);
    lua_pushboolean(state, dungeon != nullptr);
    return 1;
}

int dungeon_notice(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->Notice(lua_tostring(state, 1));
    }
    return 0;
}

struct FSetQuestFlag
{
    std::string flagname;
    int value;

    void operator()(CHARACTER* character)
    {
        sys_log(0, "Dungeon ForEachMember %s", character->GetName());
        if (character->GetDesc()) {
            if (PC* pc = CQuestManager::instance().GetPCForce(character->GetPlayerID()))
                pc->SetFlag(flagname, value, false);
        }
    }
};

int dungeon_set_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!manager.GetCurrentPC() || !lua_isstring(state, 1))
        return 0;
    std::string flag = manager.GetCurrentQuestName();
    flag += ".";
    flag += lua_tostring(state, 1);
    const int value = DungeonIntValue(std::rint(lua_tonumber(state, 2)));
    CDungeon* dungeon = manager.GetCurrentDungeon();
    if (!dungeon)
        return 0;
    dungeon->ForEachMember(FSetQuestFlag{flag, value});
    return 0;
}

int dungeon_purge(lua_State*)
{
    if (CDungeon* dungeon = CurrentDungeon())
        dungeon->Purge();
    return 0;
}

int dungeon_kill_all(lua_State*)
{
    if (CDungeon* dungeon = CurrentDungeon())
        dungeon->KillAll();
    return 0;
}

int dungeon_exit_all_to_start_position(lua_State*)
{
    if (CDungeon* dungeon = CurrentDungeon())
        dungeon->ExitAllToStartPosition();
    return 0;
}

int dungeon_set_unique(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isnumber(state, 2)) {
        if (CDungeon* dungeon = CurrentDungeon()) {
            const DWORD vid = DungeonDwordValue(lua_tonumber(state, 2));
            dungeon->SetUnique(lua_tostring(state, 1), vid);
        }
    }
    return 0;
}

int dungeon_purge_unique(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* key = lua_tostring(state, 1);
        sys_log(0, "QUEST_DUNGEON_PURGE_UNIQUE %s", key);
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->PurgeUnique(lua_tostring(state, 1));
    }
    return 0;
}

int dungeon_kill_unique(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* key = lua_tostring(state, 1);
        sys_log(0, "QUEST_DUNGEON_KILL_UNIQUE %s", key);
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->KillUnique(lua_tostring(state, 1));
    }
    return 0;
}

int dungeon_is_unique_dead(lua_State* state)
{
    CDungeon* dungeon = lua_isstring(state, 1) ? CurrentDungeon() : nullptr;
    lua_pushboolean(state, dungeon && dungeon->IsUniqueDead(lua_tostring(state, 1)));
    return 1;
}

int dungeon_unique_get_hp_perc(lua_State* state)
{
    CDungeon* dungeon = lua_isstring(state, 1) ? CurrentDungeon() : nullptr;
    lua_pushnumber(state, dungeon ? dungeon->GetUniqueHpPerc(lua_tostring(state, 1)) : 0);
    return 1;
}

int dungeon_unique_set_def_grade(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isnumber(state, 2)) {
        if (CDungeon* dungeon = CurrentDungeon()) {
            int grade;
            if (ReadIntValue(lua_tonumber(state, 2), grade))
                dungeon->UniqueSetDefGrade(lua_tostring(state, 1), grade);
        }
    }
    return 0;
}

int dungeon_unique_set_hp(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isnumber(state, 2)) {
        if (CDungeon* dungeon = CurrentDungeon()) {
            int hp;
            if (ReadIntValue(lua_tonumber(state, 2), hp))
                dungeon->UniqueSetHP(lua_tostring(state, 1), hp);
        }
    }
    return 0;
}

int dungeon_unique_set_maxhp(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isnumber(state, 2)) {
        if (CDungeon* dungeon = CurrentDungeon()) {
            int maxHP;
            if (ReadIntValue(lua_tonumber(state, 2), maxHP))
                dungeon->UniqueSetMaxHP(lua_tostring(state, 1), maxHP);
        }
    }
    return 0;
}

int dungeon_join(lua_State* state)
{
    int map;
    if (lua_gettop(state) < 1 || !ReadInt(state, 1, map))
        return 0;

    CDungeon* dungeon = CDungeonManager::instance().Create(map);
    if (!dungeon)
        return 0;

    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!character) {
        CDungeonManager::instance().Destroy(dungeon);
        return 0;
    }

    if (CParty* party = character->GetParty()) {
        const auto leaderId = static_cast<std::uint32_t>(party->GetLeaderPID());
        const auto playerId = static_cast<std::uint32_t>(character->GetPlayerID());
        if (leaderId == playerId) {
            dungeon->JoinParty(character->GetParty());
            return 0;
        }
        if (character->GetParty())
            return 0;
    }
    dungeon->Join(character);
    return 0;
}

int dungeon_set_dest(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
        return 0;

    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    CParty* party = character ? character->GetParty() : nullptr;
    CDungeon* dungeon = manager.GetCurrentDungeon();
    int x, y;
    if (dungeon && party && ReadIntValue(lua_tonumber(state, 2), y) && ReadIntValue(lua_tonumber(state, 1), x))
        dungeon->SendDestPositionToParty(party, x, y);
    return 0;
}

int dungeon_spawn(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isstring(state, 2))
        return 0;

    const char* loggedArea = lua_tostring(state, 2);
    const int loggedVnum = DungeonIntValue(lua_tonumber(state, 1));
    sys_log(0, "QUEST_DUNGEON_SPAWN %d %s", loggedVnum, loggedArea);

    if (CDungeon* dungeon = CurrentDungeon()) {
        const char* area = lua_tostring(state, 2);
        const DWORD vnum = DungeonDwordValue(lua_tonumber(state, 1));
        dungeon->Spawn(vnum, area);
    }
    return 0;
}

int dungeon_spawn_mob(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3)) {
        manager.QuestError("dungeon_spawn_mob", 0x26d, "invalid argument");
        return 0;
    }

    DWORD first = 0;
    if (CDungeon* dungeon = manager.GetCurrentDungeon()) {
        const DWORD vnum = DungeonDwordValue(lua_tonumber(state, 1));
        const double xValue = std::trunc(lua_tonumber(state, 2));
        const double yValue = std::trunc(lua_tonumber(state, 3));
        if (!(xValue >= INT_MIN && xValue <= INT_MAX && yValue >= INT_MIN && yValue <= INT_MAX)) {
            manager.QuestError("dungeon_spawn_mob", 0x26d, "invalid argument");
            return 0;
        }
        const int x = static_cast<int>(xValue);
        const int y = static_cast<int>(yValue);
        const float radius = lua_isnumber(state, 4) ? static_cast<float>(lua_tonumber(state, 4)) : 0.0f;
        const double countValue = lua_isnumber(state, 5) ? std::trunc(lua_tonumber(state, 5)) : 1.0;
        if (!(countValue >= INT_MIN && countValue <= INT_MAX))
            return 0;
        sys_log(0, "dungeon_spawn_mob %lu %d %d", vnum, x, y);
        const int count = countValue == 0 ? 1 : static_cast<int>(countValue);
        for (int index = 0; index < count; ++index) {
            std::int64_t nx = x, ny = y;
            if (radius >= 1.0f && radius <= INT_MAX / 100) {
                const int angleStep = number_ex(0, 999, const_cast<char*>("questlua_dungeon.cpp"), 0x28d);
                constexpr float pi = 3.1415927410125732f;
                const float angle = static_cast<float>(angleStep * 2.0L * pi / 1000.0L);
                const int step = number_ex(0, 999, const_cast<char*>("questlua_dungeon.cpp"), 0x28e);
                const float distance = static_cast<float>(step * static_cast<long double>(radius) / 1000.0L);
                nx += static_cast<int>(static_cast<long double>(std::cos(angle)) * distance);
                ny += static_cast<int>(static_cast<long double>(std::sin(angle)) * distance);
            }
            if (nx < INT_MIN / 100 || nx > INT_MAX / 100 || ny < INT_MIN / 100 || ny > INT_MAX / 100)
                continue;
            CHARACTER* character = dungeon->SpawnMob(vnum, static_cast<int>(nx), static_cast<int>(ny));
            if (character && !first)
                first = character->GetVID();
        }
    }
    lua_pushnumber(state, first);
    return 1;
}

int dungeon_spawn_group(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3)
        || !lua_isnumber(state, 4) || !lua_isnumber(state, 6)) {
        manager.QuestError("dungeon_spawn_group", 0x2ca, "invalid argument");
        return 0;
    }

    CDungeon* dungeon = manager.GetCurrentDungeon();
    if (!dungeon) {
        lua_pushnumber(state, 0);
        return 1;
    }
    const DWORD vnum = DungeonDwordValue(lua_tonumber(state, 1));
    const double x = std::trunc(lua_tonumber(state, 2));
    const double y = std::trunc(lua_tonumber(state, 3));
    const float radius = static_cast<float>(lua_tonumber(state, 4)) * 100.0f;
    const bool aggressive = lua_toboolean(state, 5) != 0;
    const auto count = DungeonDwordValue(lua_tonumber(state, 6));
    if (!(x >= INT_MIN / 100 && x <= INT_MAX / 100 && y >= INT_MIN / 100 && y <= INT_MAX / 100
          && radius > -INT_MAX / 2 && radius < INT_MAX / 2)) {
        manager.QuestError("dungeon_spawn_group", 0x2ca, "invalid argument");
        return 0;
    }

    CHARACTER* leader = nullptr;
    if (count <= INT_MAX)
        leader = dungeon->SpawnGroup(vnum, static_cast<int>(x) * 100, static_cast<int>(y) * 100,
                                     radius, aggressive, static_cast<int>(count));
    lua_pushnumber(state, leader ? static_cast<DWORD>(leader->GetVID()) : 0);
    return 1;
}

int dungeon_spawn_unique(lua_State* state)
{
    if (!lua_isstring(state, 1) || !lua_isnumber(state, 2) || !lua_isstring(state, 3))
        return 0;

    const char* loggedArea = lua_tostring(state, 3);
    const int loggedVnum = DungeonIntValue(lua_tonumber(state, 2));
    const char* loggedKey = lua_tostring(state, 1);
    sys_log(0, "QUEST_DUNGEON_SPAWN_UNIQUE %s %d %s", loggedKey, loggedVnum, loggedArea);

    if (CDungeon* dungeon = CurrentDungeon()) {
        const char* area = lua_tostring(state, 3);
        const DWORD vnum = DungeonDwordValue(lua_tonumber(state, 2));
        const char* key = lua_tostring(state, 1);
        if (key && area)
            dungeon->SpawnUnique(key, vnum, area);
    }
    return 0;
}

int dungeon_spawn_stone_door(lua_State* state)
{
    if (!lua_isstring(state, 1) || !lua_isstring(state, 2))
        return 0;

    const char* loggedArea = lua_tostring(state, 2);
    const char* loggedKey = lua_tostring(state, 1);
    sys_log(0, "QUEST_DUNGEON_SPAWN_STONE_DOOR %s %s", loggedKey, loggedArea);

    if (CDungeon* dungeon = CurrentDungeon()) {
        const char* area = lua_tostring(state, 2);
        const char* key = lua_tostring(state, 1);
        dungeon->SpawnStoneDoor(key, area);
    }
    return 0;
}

int dungeon_spawn_wooden_door(lua_State* state)
{
    if (!lua_isstring(state, 1) || !lua_isstring(state, 2))
        return 0;

    const char* loggedArea = lua_tostring(state, 2);
    const char* loggedKey = lua_tostring(state, 1);
    sys_log(0, "QUEST_DUNGEON_SPAWN_WOODEN_DOOR %s %s", loggedKey, loggedArea);

    if (CDungeon* dungeon = CurrentDungeon()) {
        const char* area = lua_tostring(state, 2);
        const char* key = lua_tostring(state, 1);
        dungeon->SpawnWoodenDoor(key, area);
    }
    return 0;
}

int dungeon_spawn_name_mob(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2)
        || !lua_isnumber(state, 3) || !lua_isstring(state, 4))
        return 0;

    if (CDungeon* dungeon = CurrentDungeon()) {
        const DWORD vnum = DungeonDwordValue(lua_tonumber(state, 1));
        const double x = std::trunc(lua_tonumber(state, 2));
        const double y = std::trunc(lua_tonumber(state, 3));
        if (x >= INT_MIN && x <= INT_MAX && y >= INT_MIN && y <= INT_MAX)
            dungeon->SpawnNameMob(vnum, static_cast<int>(x), static_cast<int>(y), lua_tostring(state, 4));
    }
    return 0;
}

int dungeon_spawn_goto_mob(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2)
        || !lua_isnumber(state, 3) || !lua_isnumber(state, 4))
        return 0;

    const double fromX = std::trunc(lua_tonumber(state, 1));
    const double fromY = std::trunc(lua_tonumber(state, 2));
    const double toX = std::trunc(lua_tonumber(state, 3));
    const double toY = std::trunc(lua_tonumber(state, 4));
    if (fromX >= INT_MIN / 100 && fromX <= INT_MAX / 100
        && fromY >= INT_MIN / 100 && fromY <= INT_MAX / 100
        && toX >= INT_MIN && toX <= INT_MAX && toY >= INT_MIN && toY <= INT_MAX) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnGotoMob(static_cast<int>(fromX), static_cast<int>(fromY),
                                  static_cast<int>(toX), static_cast<int>(toY));
    }
    return 0;
}

int dungeon_spawn_move_unique(lua_State* state)
{
    if (!lua_isstring(state, 1) || !lua_isnumber(state, 2)
        || !lua_isstring(state, 3) || !lua_isstring(state, 4))
        return 0;

    const char* loggedDestination = lua_tostring(state, 4);
    const char* loggedArea = lua_tostring(state, 3);
    const int loggedVnum = DungeonIntValue(lua_tonumber(state, 2));
    const char* loggedKey = lua_tostring(state, 1);
    sys_log(0, "QUEST_DUNGEON_SPAWN_MOVE_UNIQUE %s %d %s %s",
            loggedKey, loggedVnum, loggedArea, loggedDestination);

    if (CDungeon* dungeon = CurrentDungeon()) {
        const char* destination = lua_tostring(state, 4);
        const char* area = lua_tostring(state, 3);
        const DWORD vnum = DungeonDwordValue(lua_tonumber(state, 2));
        const char* key = lua_tostring(state, 1);
        if (key && area && destination)
            dungeon->SpawnMoveUnique(key, vnum, area, destination);
    }
    return 0;
}

int dungeon_spawn_move_group(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isstring(state, 2) || !lua_isstring(state, 3))
        return 0;

    const char* loggedDestination = lua_tostring(state, 3);
    const char* loggedArea = lua_tostring(state, 2);
    const int loggedVnum = DungeonIntValue(lua_tonumber(state, 1));
    sys_log(0, "QUEST_DUNGEON_SPAWN_MOVE_GROUP %d %s %s", loggedVnum, loggedArea, loggedDestination);

    if (CDungeon* dungeon = CurrentDungeon()) {
        const char* destination = lua_tostring(state, 3);
        const char* area = lua_tostring(state, 2);
        const DWORD vnum = DungeonDwordValue(lua_tonumber(state, 1));
        if (area && destination)
            dungeon->SpawnMoveGroup(vnum, area, destination, 1);
    }
    return 0;
}

// 0x08096920, all 45 entries in original order at 0x081fdc2c.
void RegisterDungeonFunctionTable()
{
    luaL_reg functions[] = {
        {"join", dungeon_join},
        {"exit", dungeon_exit},
        {"exit_all", dungeon_exit_all},
        {"purge", dungeon_purge},
        {"kill_all", dungeon_kill_all},
        {"spawn", dungeon_spawn},
        {"spawn_mob", dungeon_spawn_mob},
        {"spawn_name_mob", dungeon_spawn_name_mob},
        {"spawn_goto_mob", dungeon_spawn_goto_mob},
        {"spawn_group", dungeon_spawn_group},
        {"spawn_unique", dungeon_spawn_unique},
        {"spawn_move_unique", dungeon_spawn_move_unique},
        {"spawn_move_group", dungeon_spawn_move_group},
        {"spawn_stone_door", dungeon_spawn_stone_door},
        {"spawn_wooden_door", dungeon_spawn_wooden_door},
        {"purge_unique", dungeon_purge_unique},
        {"kill_unique", dungeon_kill_unique},
        {"is_unique_dead", dungeon_is_unique_dead},
        {"unique_get_hp_perc", dungeon_unique_get_hp_perc},
        {"unique_set_def_grade", dungeon_unique_set_def_grade},
        {"unique_set_hp", dungeon_unique_set_hp},
        {"unique_set_maxhp", dungeon_unique_set_maxhp},
        {"get_kill_stone_count", dungeon_get_kill_stone_count},
        {"get_kill_mob_count", dungeon_get_kill_mob_count},
        {"is_use_potion", dungeon_is_use_potion},
        {"revived", dungeon_revived},
        {"set_dest", dungeon_set_dest},
        {"jump_all", dungeon_jump_all},
        {"new_jump_all", dungeon_new_jump_all},
        {"new_jump", dungeon_new_jump},
        {"regen_file", dungeon_regen_file},
        {"set_regen_file", dungeon_set_regen_file},
        {"clear_regen", dungeon_clear_regen},
        {"set_exit_all_at_eliminate", dungeon_set_exit_all_at_eliminate},
        {"set_warp_at_eliminate", dungeon_set_warp_at_eliminate},
        {"get_map_index", dungeon_get_map_index},
        {"check_eliminated", dungeon_check_eliminated},
        {"exit_all_to_start_position", dungeon_exit_all_to_start_position},
        {"count_monster", dungeon_count_monster},
        {"setf", dungeon_set_flag},
        {"getf", dungeon_get_flag},
        {"set_unique", dungeon_set_unique},
        {"select", dungeon_select},
        {"notice", dungeon_notice},
        {"setqf", dungeon_set_quest_flag},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("d", functions);
}
}
