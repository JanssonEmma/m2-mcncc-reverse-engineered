// Forked map configuration and combat counters from the 2009 questlua_forked.cpp.
#include "questlua_forked.h"
#include "config.h"
#include "char.h"
#include "p2p.h"
#include "questmanager.h"
#include "../../libthecore/include/utils.h"
#include <cmath>
#include <limits>
#include <cstdint>
#include <map>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}
#include "../../libthecore/include/log.h"
#include <cstdio>
#include <cstring>
#include <set>
#include <string>
#include <vector>

extern std::string g_stLocal;

namespace {
struct ForkedSungziMapInfo
{
    int mapIndex;
    int startPosition[4][2];
    std::string mapName;
    int bossVnum;
};

struct ForkedPassMapInfo
{
    int mapIndex[4];
    int startPosition[4][2];
    std::string mapName[4];
};

std::vector<ForkedSungziMapInfo> sungziMaps;
std::vector<ForkedPassMapInfo> passMaps;
std::set<int> forkedMapIndices;
int empireKills[4];
int empireMobKills[4];
int bossKills;
std::map<std::uint32_t, std::uint32_t> g_map_deadcount;

int AddForkedKillCount(int& count, int increment)
{
    count = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(count) + static_cast<std::uint32_t>(increment));
    return count;
}

const ForkedSungziMapInfo* ActiveSungzi()
{
    const int index = g_iForkedVecIndex[0];
    if (index < 0 || static_cast<std::size_t>(index) >= sungziMaps.size())
        return nullptr;
    return &sungziMaps[index];
}
}

int g_bRegenFlag;

void InitForked()
{
    g_bRegenFlag = 0;
    for (int empire = 0; empire < 4; ++empire) {
        empireKills[empire] = 0;
        empireMobKills[empire] = 0;
    }
    bossKills = 0;
}

int LoadForked()
{
    InitForked();
    const std::string path = "locale/" + g_stLocal + "/forkedmapindex.txt";
    std::FILE* file = std::fopen(path.c_str(), "r");
    if (!file) {
        std::fprintf(stderr, "[INIT_FORKED] Do not open file (%s)\n", path.c_str());
        return 1;
    }

    std::fprintf(stderr, "[INIT_FORKED] Do open file (%s)\n", path.c_str());
    char line[256];
    while (std::fgets(line, sizeof(line), file)) {
        if (std::strncmp(line, "sungzi:", 7) == 0) {
            ForkedSungziMapInfo info = {};
            char name[128];
            if (std::sscanf(line + 7, "%d %d %d %d %d %d %d %127s %d",
                    &info.mapIndex,
                    &info.startPosition[1][0], &info.startPosition[1][1],
                    &info.startPosition[2][0], &info.startPosition[2][1],
                    &info.startPosition[3][0], &info.startPosition[3][1],
                    name, &info.bossVnum) != 9)
                continue;

            info.mapName = name;
            sungziMaps.push_back(info);
            forkedMapIndices.insert(info.mapIndex);
        } else if (std::strncmp(line, "pass:", 5) == 0) {
            ForkedPassMapInfo info = {};
            char name[3][128];
            if (std::sscanf(line + 5, "%d %d %d %127s %d %d %d %127s %d %d %d %127s",
                    &info.mapIndex[1], &info.startPosition[1][0], &info.startPosition[1][1], name[0],
                    &info.mapIndex[2], &info.startPosition[2][0], &info.startPosition[2][1], name[1],
                    &info.mapIndex[3], &info.startPosition[3][0], &info.startPosition[3][1], name[2]) != 12)
                continue;

            for (int empire = 1; empire <= 3; ++empire) {
                info.mapName[empire] = name[empire - 1];
                forkedMapIndices.insert(info.mapIndex[empire]);
            }
            passMaps.push_back(info);
        } else if (line[0] == '#') {
            std::fprintf(stderr, "%s", line);
        }
    }
    std::fclose(file);
    return 1;
}

int IsForkedMapIndex(int mapIndex)
{
    const bool found = forkedMapIndices.find(mapIndex) != forkedMapIndices.end();
    if (test_server)
        sys_log(0, found ? "[IS_FORKED] %d " : "[IS_NOT_FORKED] %d ", mapIndex);
    return found;
}

int IsSungziMapIndex(int mapIndex)
{
    if (test_server && sungziMaps.empty())
        sys_log(0, "[IS_NOT_SUNGZI] %d ", mapIndex);
    for (const auto& info : sungziMaps) {
        if (info.mapIndex == mapIndex) {
            if (test_server)
                sys_log(0, "[IS_SUNGZI] %d ", mapIndex);
            return 1;
        }
    }
    return 0;
}

int GetSungziStartX(BYTE empire)
{
    const auto* info = ActiveSungzi();
    const int x = info && empire < 4 ? info->startPosition[empire][0] : 0;
    if (test_server)
        sys_log(0, "sungzi pos x %lu ", static_cast<unsigned long>(x));
    return x;
}

int GetSungziStartY(BYTE empire)
{
    const auto* info = ActiveSungzi();
    const int y = info && empire < 4 ? info->startPosition[empire][1] : 0;
    if (test_server)
        sys_log(0, "sungzi pos y %lu ", static_cast<unsigned long>(y));
    return y;
}

int GetSungziMapIndex()
{
    const auto* info = ActiveSungzi();
    return info ? info->mapIndex : 0;
}

int GetSungziBossVnum()
{
    const auto* info = ActiveSungzi();
    const int vnum = info ? info->bossVnum : 0;
    if (test_server)
        sys_log(0, "sungzi boss vnum %lu", static_cast<unsigned long>(vnum));
    return vnum;
}

int inc_boss_kill_count()
{
    return AddForkedKillCount(bossKills, 1);
}

int get_boss_kill_count()
{
    return bossKills;
}

int get_kill_count_per_empire(int empire)
{
    return empire >= 0 && empire < 4 ? empireKills[empire] : -1;
}

int set_kill_count_per_empire(int empire, int kills)
{
    if (empire < 0 || empire >= 4)
        return -1;
    return empireKills[empire] = kills;
}

int inc_kill_count_per_empire(int empire, int kills)
{
    if (empire < 0 || empire >= 4)
        return -1;
    return AddForkedKillCount(empireKills[empire], kills);
}

// Original questlua_forked.cpp globals and map assignment (080a4ad0).
int g_iForkedVecIndex[2];

int SetForkedMap(int pass, int sungzi)
{
    g_iForkedVecIndex[0]=sungzi;
    g_iForkedVecIndex[1]=pass;
    return 1;
}

int GetPassMapIndex(BYTE empire)
{
    const int index = g_iForkedVecIndex[1];
    if (empire >= 4 || index < 0 || static_cast<std::size_t>(index) >= passMaps.size())
        return 0;
    return passMaps[index].mapIndex[empire];
}

const char* GetSungziMapPath()
{
    const auto* info = ActiveSungzi();
    if (!info)
        return nullptr;
    static std::string path;
    path = "./locale/" + g_stLocal + "/map/" + info->mapName + "/";
    return path.c_str();
}

int get_mob_kill_count_per_empire(int empire)
{
    return empire >= 0 && empire < 4 ? empireMobKills[empire] : -1;
}

int inc_mob_kill_count_per_empire(int empire)
{
    return empire >= 0 && empire < 4 ? AddForkedKillCount(empireMobKills[empire], 1) : -1;
}

void init_dead_count(int pid)
{
    g_map_deadcount[static_cast<std::uint32_t>(pid)] =
        quest::CQuestManager::instance().GetEventFlag("threeway_war_dead_count");
}

void init_all_dead_count()
{
    g_map_deadcount.clear();
}

int get_dead_count(int pid)
{
    const auto entry = g_map_deadcount.find(static_cast<std::uint32_t>(pid));
    return entry == g_map_deadcount.end() ? 0 : static_cast<int>(entry->second);
}

int dec_dead_count(int pid)
{
    const auto entry = g_map_deadcount.find(static_cast<std::uint32_t>(pid));
    if (entry == g_map_deadcount.end()) {
        init_dead_count(pid);
        return 0;
    }
    return static_cast<int>(--entry->second);
}

namespace {
const ForkedPassMapInfo* ActivePass()
{
    const int index = g_iForkedVecIndex[1];
    if (index < 0 || static_cast<std::size_t>(index) >= passMaps.size())
        return nullptr;
    return &passMaps[index];
}

int ForkedStartPosition(BYTE empire, bool sungzi, int axis)
{
    if (empire >= 4)
        return 0;
    int position = 0;
    if (sungzi) {
        if (const auto* map = ActiveSungzi())
            position = map->startPosition[empire][axis];
        if (test_server)
            sys_log(0, axis == 0 ? "sungzi pos x %lu " : "sungzi pos y %lu ",
                    static_cast<unsigned long>(position));
    } else {
        if (const auto* map = ActivePass())
            position = map->startPosition[empire][axis];
        if (test_server)
            sys_log(0, axis == 0 ? "Pass StartX %d" : "Pass StartY %d", position);
    }
    return position;
}
}

int GetPassStartX(BYTE empire)
{
    return ForkedStartPosition(empire, false, 0);
}

int GetPassStartY(BYTE empire)
{
    return ForkedStartPosition(empire, false, 1);
}

const char* GetPassMapPath(BYTE empire)
{
    const auto* map = ActivePass();
    if (!map || empire >= 4)
        return nullptr;
    static std::string path;
    path = "./locale/" + g_stLocal + "/map/" + map->mapName[empire] + "/";
    return path.c_str();
}

int InitForkedMap()
{
    if (sungziMaps.empty() || passMaps.empty())
        return 0;

    char* const file = const_cast<char*>("questlua_forked.cpp");
    const int sungzi = number_ex(0, static_cast<int>(sungziMaps.size()) - 1, file, 0xf2);
    const int pass = number_ex(0, static_cast<int>(passMaps.size()) - 1, file, 0xf3);
    SetForkedMap(pass, sungzi);
    if (test_server)
        sys_log(0, "[INIT_FORKED_MAP] SungMapindex %d Pass %d %d %d ", GetSungziMapIndex(),
                GetPassMapIndex(1), GetPassMapIndex(2), GetPassMapIndex(3));
    const BYTE packet[] = {23, static_cast<BYTE>(pass), static_cast<BYTE>(sungzi)};
    P2P_MANAGER::instance().Send(packet, sizeof(packet));
    if (test_server)
        sys_log(0, "[INIT_FORKED_MAP] sungzi %d pass %d size %d",
                static_cast<int>(packet[2]), static_cast<int>(packet[1]), static_cast<int>(sizeof(packet)));
    return 0;
}

int GetVictoryEmpire()
{
    for (int empire = 1; empire <= 3; ++empire) {
        if (empireMobKills[empire] < 0)
            return empire;
    }
    return 0;
}

namespace quest {
namespace {
LPCHARACTER CurrentForkedCharacter()
{
    return CQuestManager::instance().GetCurrentCharacterPtr();
}

BYTE CurrentForkedEmpire()
{
    LPCHARACTER character = CurrentForkedCharacter();
    return character ? character->GetEmpire() : 0;
}

int ForkedLuaNumber(lua_State* state)
{
    const auto value = std::trunc(lua_tonumber(state, 1));
    return value >= std::numeric_limits<int>::min() && value <= std::numeric_limits<int>::max()
        ? static_cast<int>(value) : std::numeric_limits<int>::min();
}

int PushForkedNumber(lua_State* state, int value)
{
    lua_pushnumber(state, value);
    return 1;
}

// These legacy mutators return the top argument without pushing a result.
int ForkedLegacyResult(lua_State* state)
{
    if (!lua_gettop(state))
        lua_pushnil(state);
    return 1;
}

int ForkedDeadLimit()
{
    return CQuestManager::instance().GetEventFlag("threeway_war_dead_count");
}

void LogForkedDeadCount(LPCHARACTER character, const char* operation)
{
    if (test_server)
        character->ChatPacket(1, "%s DeadCount %d %d", operation, character->m_iKillCount, ForkedDeadLimit());
}

int PushForkedPassPath(lua_State* state, int empire)
{
    const char* path = GetPassMapPath(static_cast<BYTE>(empire));
    lua_pushstring(state, path);
    sys_log(0, "[PASS_PATH] Empire %d Path  %s", empire, path ? path : "");
    return 1;
}
}

int forked_dec_dead_count(lua_State*)
{
    if (LPCHARACTER character = CurrentForkedCharacter())
        AddForkedKillCount(character->m_iKillCount, -1);
    return 0;
}

int forked_clear_dead_count(lua_State*)
{
    if (LPCHARACTER character = CurrentForkedCharacter()) {
        character->ClearKillCount();
        LogForkedDeadCount(character, "clear");
    }
    return 0;
}

int forked_set_dead_count(lua_State*)
{
    if (LPCHARACTER character = CurrentForkedCharacter()) {
        character->m_iKillCount = ForkedDeadLimit();
        LogForkedDeadCount(character, "set");
    }
    return 0;
}

int forked_get_dead_count(lua_State* state)
{
    LPCHARACTER character = CurrentForkedCharacter();
    if (character)
        LogForkedDeadCount(character, "get");
    lua_pushnumber(state, character ? character->m_iKillCount : 0);
    if (character)
        LogForkedDeadCount(character, "get");
    return 1;
}

int forked_init_kill_count_per_empire(lua_State*)
{
    set_kill_count_per_empire(CurrentForkedEmpire(), 0);
    return 0;
}

int forked_inc_kill_count_per_empire(lua_State*)
{
    inc_kill_count_per_empire(CurrentForkedEmpire(), 1);
    return 0;
}

int forked_get_kill_count_per_empire(lua_State* state)
{
    return PushForkedNumber(state, get_kill_count_per_empire(CurrentForkedEmpire()));
}

int forked_init_mob_kill_count_per_empire(lua_State* state)
{
    const BYTE empire = CurrentForkedEmpire();
    if (empire < 4)
        empireMobKills[empire] = 0;
    return ForkedLegacyResult(state);
}

int forked_inc_mob_kill_count_per_empire(lua_State* state)
{
    inc_mob_kill_count_per_empire(CurrentForkedEmpire());
    return ForkedLegacyResult(state);
}

int forked_get_mob_kill_count_per_empire(lua_State* state)
{
    return PushForkedNumber(state, get_mob_kill_count_per_empire(CurrentForkedEmpire()));
}

int forked_get_boss_kill_count(lua_State* state)
{
    return PushForkedNumber(state, get_boss_kill_count());
}

int forked_inc_boss_kill_count(lua_State* state)
{
    inc_boss_kill_count();
    return ForkedLegacyResult(state);
}

int forked_init_boss_kill_count(lua_State* state)
{
    bossKills = 0;
    return ForkedLegacyResult(state);
}

int forked_init(lua_State* state)
{
    InitForked();
    InitForkedMap();
    return ForkedLegacyResult(state);
}

int forked_start_pos_x(lua_State* state)
{
    return PushForkedNumber(state, ForkedStartPosition(CurrentForkedEmpire(), true, 0));
}

int forked_start_pos_y(lua_State* state)
{
    return PushForkedNumber(state, ForkedStartPosition(CurrentForkedEmpire(), true, 1));
}

int forked_get_sungzi_boss_vnum(lua_State* state)
{
    return PushForkedNumber(state, GetSungziBossVnum());
}

int forked_pass_start_pos_x(lua_State* state)
{
    return PushForkedNumber(state, GetPassStartX(CurrentForkedEmpire()));
}

int forked_pass_start_pos_y(lua_State* state)
{
    return PushForkedNumber(state, GetPassStartY(CurrentForkedEmpire()));
}

int forked_sungzi_mapindex(lua_State* state)
{
    const int map = GetSungziMapIndex();
    lua_pushnumber(state, map);
    if (test_server)
        sys_log(0, "forked_sungzi_map_index_by_empire %d", map);
    return 1;
}

int forked_pass_mapindex(lua_State* state)
{
    return PushForkedNumber(state, GetPassMapIndex(CurrentForkedEmpire()));
}

int forked_pass_mapindex_by_empire(lua_State* state)
{
    const int map = GetPassMapIndex(static_cast<BYTE>(ForkedLuaNumber(state)));
    lua_pushnumber(state, map);
    if (test_server)
        sys_log(0, "forked_pass_map_index_by_empire %d", map);
    return 1;
}

int forked_get_sungzi_path(lua_State* state)
{
    const char* path = GetSungziMapPath();
    lua_pushstring(state, path);
    sys_log(0, "[SUNGZI_PATH] Sungzi %s", path ? path : "");
    return 1;
}

int forked_get_pass_path_my_empire(lua_State* state)
{
    return PushForkedPassPath(state, CurrentForkedEmpire());
}

int forked_get_pass_path_by_empire(lua_State* state)
{
    return PushForkedPassPath(state, ForkedLuaNumber(state));
}

int forked_is_forked_mapindex(lua_State* state)
{
    return PushForkedNumber(state, IsForkedMapIndex(ForkedLuaNumber(state)));
}

int forked_is_sungzi_mapindex(lua_State* state)
{
    return PushForkedNumber(state, IsSungziMapIndex(ForkedLuaNumber(state)));
}

int forked_get_victory_empire(lua_State* state)
{
    return PushForkedNumber(state, GetVictoryEmpire());
}

int forked_get_level_limit(lua_State* state)
{
    const int level = CQuestManager::instance().GetEventFlag("threeway_war_level");
    return PushForkedNumber(state, level ? level : 30);
}

void RegisterForkedFunctionTable()
{
    luaL_reg functions[] = {
        {"decdeadcount", forked_dec_dead_count},
        {"cleardeadcount", forked_clear_dead_count},
        {"setdeadcount", forked_set_dead_count},
        {"getdeadcount", forked_get_dead_count},
        {"initkillcount", forked_init_kill_count_per_empire},
        {"inckillcount", forked_inc_kill_count_per_empire},
        {"getkillcount", forked_get_kill_count_per_empire},
        {"initmobkillcount", forked_init_mob_kill_count_per_empire},
        {"incmobkillcount", forked_inc_mob_kill_count_per_empire},
        {"getmobkillcount", forked_get_mob_kill_count_per_empire},
        {"getbosskillcount", forked_get_boss_kill_count},
        {"incbosskillcount", forked_inc_boss_kill_count},
        {"initforked", forked_init},
        {"getsungziposx", forked_start_pos_x},
        {"getsungziposy", forked_start_pos_y},
        {"getsungzibossvnum", forked_get_sungzi_boss_vnum},
        {"getpassstartposx", forked_pass_start_pos_x},
        {"getpassstartposy", forked_pass_start_pos_y},
        {"getsungzimapindex", forked_sungzi_mapindex},
        {"getpassmapindex", forked_pass_mapindex},
        {"getpassmapindexbyempire", forked_pass_mapindex_by_empire},
        {"getsungzipath", forked_get_sungzi_path},
        {"getpasspath", forked_get_pass_path_my_empire},
        {"getpasspathbyempire", forked_get_pass_path_by_empire},
        {"isforkedmapindex", forked_is_forked_mapindex},
        {"issungzimapindex", forked_is_sungzi_mapindex},
        {"getvictoryempire", forked_get_victory_empire},
        {"getlevellimit", forked_get_level_limit},
        {nullptr, nullptr},
    };
    CQuestManager::instance().AddLuaFunctionTable("forked", functions);
}
}
