#include "char.h"
#include "dungeon.h"
#include "party.h"
#include "questmanager.h"
#include "questpc.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"

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

bool ReadInt(lua_State* state, int argument, int& value)
{
    if (!lua_isnumber(state, argument))
        return false;
    const double number = std::trunc(lua_tonumber(state, argument));
    if (!(number >= INT_MIN && number <= INT_MAX))
        return false;
    value = static_cast<int>(number);
    return true;
}

bool ReadDword(lua_State* state, int argument, DWORD& value)
{
    if (!lua_isnumber(state, argument))
        return false;
    const double number = lua_tonumber(state, argument);
    if (!(number >= -9223372036854775808.0 && number < 9223372036854775808.0))
        value = 0;
    else
        value = static_cast<std::uint32_t>(static_cast<std::int64_t>(number));
    return true;
}

int DungeonIntValue(lua_Number value)
{
    return value > -2147483649.0 && value < 2147483648.0 ? static_cast<int>(value) : INT_MIN;
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
    int x, y;
    if (lua_gettop(state) >= 2 && ReadInt(state, 1, x) && ReadInt(state, 2, y)) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->JumpAll(dungeon->GetMapIndex(), x, y);
    }
    return 0;
}

int dungeon_new_jump_all(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    int map, x, y;
    if (!ReadInt(state, 1, map) || !ReadInt(state, 2, x) || !ReadInt(state, 3, y)) {
        manager.QuestError("dungeon_new_jump_all", 0x105, "not enough argument");
        return 0;
    }
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!character)
        return 0;
    CDungeon* dungeon = CDungeonManager::instance().Create(map);
    if (dungeon)
        dungeon->JumpAll(character->GetMapIndex(), x, y);
    else
        manager.QuestError("dungeon_new_jump_all", 0x10f, "cannot create dungeon %d", map);
    return 0;
}

int dungeon_new_jump(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    int map, x, y;
    if (lua_gettop(state) < 3) {
        manager.QuestError("dungeon_new_jump", 0xe7, "not enough argument");
        return 0;
    }
    if (!ReadInt(state, 1, map) || !ReadInt(state, 2, x) || !ReadInt(state, 3, y)) {
        manager.QuestError("dungeon_new_jump", 0xed, "wrong argument");
        return 0;
    }
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!character)
        return 0;
    CDungeon* dungeon = CDungeonManager::instance().Create(map);
    if (dungeon)
        character->WarpSet(dungeon->GetMapIndex(), x, y);
    else
        manager.QuestError("dungeon_new_jump", 0xf7, "cannot create dungeon %d", map);
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
    lua_pushnumber(state, dungeon ? dungeon->GetMapIndex() : 0);
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
    CQuestManager& manager = CQuestManager::instance();
    if (!lua_isstring(state, 1) || !lua_isnumber(state, 2)) {
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
    CQuestManager& manager = CQuestManager::instance();
    const bool validName = lua_isstring(state, 1);
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
    DWORD rawMap = 0;
    ReadDword(state, 1, rawMap);
    const auto map = static_cast<std::int32_t>(rawMap);
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

int dungeon_set_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    if (!manager.GetCurrentPC() || !lua_isstring(state, 1))
        return 0;
    const std::string flag = manager.GetCurrentQuestName() + "." + lua_tostring(state, 1);
    const int value = DungeonIntValue(std::rint(lua_tonumber(state, 2)));
    CDungeon* dungeon = manager.GetCurrentDungeon();
    if (!dungeon)
        return 0;
    dungeon->ForEachMember([&](CHARACTER* character) {
        sys_log(0, "Dungeon ForEachMember %s", character->GetName());
        if (character->GetDesc()) {
            if (PC* pc = manager.GetPCForce(character->GetPlayerID()))
                pc->SetFlag(flag, value, false);
        }
    });
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
    DWORD vid;
    if (lua_isstring(state, 1) && ReadDword(state, 2, vid)) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SetUnique(lua_tostring(state, 1), vid);
    }
    return 0;
}

int dungeon_purge_unique(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* key = lua_tostring(state, 1);
        sys_log(0, "QUEST_DUNGEON_PURGE_UNIQUE %s", key);
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->PurgeUnique(key);
    }
    return 0;
}

int dungeon_kill_unique(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* key = lua_tostring(state, 1);
        sys_log(0, "QUEST_DUNGEON_KILL_UNIQUE %s", key);
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->KillUnique(key);
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
    int grade;
    if (lua_isstring(state, 1) && ReadInt(state, 2, grade)) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->UniqueSetDefGrade(lua_tostring(state, 1), grade);
    }
    return 0;
}

int dungeon_unique_set_hp(lua_State* state)
{
    int hp;
    if (lua_isstring(state, 1) && ReadInt(state, 2, hp)) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->UniqueSetHP(lua_tostring(state, 1), hp);
    }
    return 0;
}

int dungeon_unique_set_maxhp(lua_State* state)
{
    int maxHP;
    if (lua_isstring(state, 1) && ReadInt(state, 2, maxHP)) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->UniqueSetMaxHP(lua_tostring(state, 1), maxHP);
    }
    return 0;
}

int dungeon_join(lua_State* state)
{
    int map;
    if (!ReadInt(state, 1, map))
        return 0;
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!character)
        return 0;
    CParty* party = character->GetParty();
    if (party && party->GetLeaderPID() != character->GetPlayerID())
        return 0;

    if (CDungeon* dungeon = CDungeonManager::instance().Create(map)) {
        if (party)
            dungeon->JoinParty(party);
        else
            dungeon->Join(character);
    }
    return 0;
}

int dungeon_set_dest(lua_State* state)
{
    int x, y;
    if (!ReadInt(state, 1, x) || !ReadInt(state, 2, y))
        return 0;
    CQuestManager& manager = CQuestManager::instance();
    CDungeon* dungeon = manager.GetCurrentDungeon();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    CParty* party = character ? character->GetParty() : nullptr;
    if (dungeon && party)
        dungeon->SendDestPositionToParty(party, x, y);
    return 0;
}

int dungeon_spawn(lua_State* state)
{
    DWORD vnum;
    if (ReadDword(state, 1, vnum) && lua_isstring(state, 2)) {
        sys_log(0, "QUEST_DUNGEON_SPAWN %d %s", static_cast<int>(vnum), lua_tostring(state, 2));
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->Spawn(vnum, lua_tostring(state, 2));
    }
    return 0;
}

int dungeon_spawn_mob(lua_State* state)
{
    DWORD vnum;
    int x, y;
    CQuestManager& manager = CQuestManager::instance();
    if (!ReadDword(state, 1, vnum) || !ReadInt(state, 2, x) || !ReadInt(state, 3, y)) {
        manager.QuestError("dungeon_spawn_mob", 0x26d, "invalid argument");
        return 0;
    }

    DWORD first = 0;
    if (CDungeon* dungeon = manager.GetCurrentDungeon()) {
        const float radius = static_cast<float>(lua_tonumber(state, 4));
        int count = 1;
        if (lua_isnumber(state, 5) && !ReadInt(state, 5, count))
            return 0;
        if (!count)
            count = 1;
        sys_log(0, "dungeon_spawn_mob %lu %d %d", vnum, x, y);
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
    DWORD vnum, count;
    int x, y;
    const float radius = static_cast<float>(lua_tonumber(state, 4)) * 100.0f;
    CQuestManager& manager = CQuestManager::instance();
    if (!ReadDword(state, 1, vnum) || !ReadInt(state, 2, x) || !ReadInt(state, 3, y)
        || x < INT_MIN / 100 || x > INT_MAX / 100 || y < INT_MIN / 100 || y > INT_MAX / 100
        || !lua_isnumber(state, 4) || !(radius > -INT_MAX / 2 && radius < INT_MAX / 2)
        || !ReadDword(state, 6, count)) {
        manager.QuestError("dungeon_spawn_group", 0x2ca, "invalid argument");
        return 0;
    }

    CHARACTER* leader = nullptr;
    if (count <= INT_MAX) {
        if (CDungeon* dungeon = manager.GetCurrentDungeon())
            leader = dungeon->SpawnGroup(vnum, x * 100, y * 100, radius,
                                         lua_toboolean(state, 5), static_cast<int>(count));
    }
    lua_pushnumber(state, leader ? static_cast<DWORD>(leader->GetVID()) : 0);
    return 1;
}

int dungeon_spawn_unique(lua_State* state)
{
    DWORD vnum;
    if (lua_isstring(state, 1) && ReadDword(state, 2, vnum) && lua_isstring(state, 3)) {
        sys_log(0, "QUEST_DUNGEON_SPAWN_UNIQUE %s %d %s",
                lua_tostring(state, 1), static_cast<int>(vnum), lua_tostring(state, 3));
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnUnique(lua_tostring(state, 1), vnum, lua_tostring(state, 3));
    }
    return 0;
}

int dungeon_spawn_stone_door(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isstring(state, 2)) {
        sys_log(0, "QUEST_DUNGEON_SPAWN_STONE_DOOR %s %s", lua_tostring(state, 1), lua_tostring(state, 2));
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnStoneDoor(lua_tostring(state, 1), lua_tostring(state, 2));
    }
    return 0;
}

int dungeon_spawn_wooden_door(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isstring(state, 2)) {
        sys_log(0, "QUEST_DUNGEON_SPAWN_WOODEN_DOOR %s %s", lua_tostring(state, 1), lua_tostring(state, 2));
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnWoodenDoor(lua_tostring(state, 1), lua_tostring(state, 2));
    }
    return 0;
}

int dungeon_spawn_name_mob(lua_State* state)
{
    DWORD vnum;
    int x, y;
    if (ReadDword(state, 1, vnum) && ReadInt(state, 2, x) && ReadInt(state, 3, y) && lua_isstring(state, 4)) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnNameMob(vnum, x, y, lua_tostring(state, 4));
    }
    return 0;
}

int dungeon_spawn_goto_mob(lua_State* state)
{
    int fromX, fromY, toX, toY;
    if (ReadInt(state, 1, fromX) && ReadInt(state, 2, fromY) &&
        ReadInt(state, 3, toX) && ReadInt(state, 4, toY) &&
        fromX >= INT_MIN / 100 && fromX <= INT_MAX / 100 && fromY >= INT_MIN / 100 && fromY <= INT_MAX / 100) {
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnGotoMob(fromX, fromY, toX, toY);
    }
    return 0;
}

int dungeon_spawn_move_unique(lua_State* state)
{
    DWORD vnum;
    if (lua_isstring(state, 1) && ReadDword(state, 2, vnum) &&
        lua_isstring(state, 3) && lua_isstring(state, 4)) {
        sys_log(0, "QUEST_DUNGEON_SPAWN_MOVE_UNIQUE %s %d %s %s",
                lua_tostring(state, 1), static_cast<int>(vnum), lua_tostring(state, 3), lua_tostring(state, 4));
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnMoveUnique(lua_tostring(state, 1), vnum, lua_tostring(state, 3), lua_tostring(state, 4));
    }
    return 0;
}

int dungeon_spawn_move_group(lua_State* state)
{
    DWORD vnum;
    if (ReadDword(state, 1, vnum) && lua_isstring(state, 2) && lua_isstring(state, 3)) {
        sys_log(0, "QUEST_DUNGEON_SPAWN_MOVE_GROUP %d %s %s",
                static_cast<int>(vnum), lua_tostring(state, 2), lua_tostring(state, 3));
        if (CDungeon* dungeon = CurrentDungeon())
            dungeon->SpawnMoveGroup(vnum, lua_tostring(state, 2), lua_tostring(state, 3), 1);
    }
    return 0;
}

// 0x08096920, verified subset in original relative order at 0x081fdc2c.
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
