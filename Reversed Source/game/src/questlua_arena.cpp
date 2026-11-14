#include "arena.h"
#include "char.h"
#include "char_manager.h"
#include "questmanager.h"
#include "../../libthecore/include/log.h"
#include <cfenv>
#include <climits>
#include <cstdint>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
int ArenaNumber(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value > -2147483649.0 && value < 2147483648.0)) {
        std::feraiseexcept(FE_INVALID);
        return INT_MIN;
    }
    return static_cast<int>(value);
}
}

int arena_start_duel(lua_State* state)
{
    CHARACTER* first = CQuestManager::instance().GetCurrentCharacterPtr();
    const char* name = lua_tostring(state, 1);
    CHARACTER* second = name ? CHARACTER_MANAGER::instance().FindPC(name) : nullptr;
    const int sets = ArenaNumber(state, 2);
    int result = 0;
    if (first && second)
    {
        for (CHARACTER* player : {first, second})
            if (player->IsHorseRiding())
            {
                player->StopRiding();
                player->HorseSummon(false, false);
            }
        CArenaManager& manager = CArenaManager::instance();
        const auto firstMap = first->GetMapIndex();
        const auto firstPlayerID = first->GetPlayerID();
        const auto secondIsMember = [second]() {
            const auto map = second->GetMapIndex();
            const auto playerID = second->GetPlayerID();
            return CArenaManager::instance().IsMember(map, playerID);
        };
        if (manager.IsMember(firstMap, firstPlayerID) || secondIsMember())
            result = 2;
        else
            result = CArenaManager::instance().StartDuel(first, second, sets) ? 1 : 3;
    }
    lua_pushnumber(state, result);
    return 1;
}

int arena_add_map(lua_State* state)
{
    const int map = ArenaNumber(state, 1);
    const int ax = ArenaNumber(state, 2);
    const int ay = ArenaNumber(state, 3);
    const int bx = ArenaNumber(state, 4);
    const int by = ArenaNumber(state, 5);
    const bool added = CArenaManager::instance().AddArena(static_cast<std::uint32_t>(map), ax, ay, bx, by);
    sys_log(0, added ? "Add Arena Map:%d startA(%d,%d) startB(%d,%d)"
                     : "Failed to load arena map info(map:%d AX:%d AY:%d BX:%d BY:%d",
        map, ax, ay, bx, by);
    return 1;
}

int arena_get_duel_list(lua_State* state)
{
    CArenaManager::instance().GetDuelList(state);
    return 1;
}

int arena_add_observer(lua_State* state)
{
    const auto map = static_cast<std::uint32_t>(ArenaNumber(state, 1));
    const int x = ArenaNumber(state, 2);
    const int y = ArenaNumber(state, 3);
    CHARACTER* observer = CQuestManager::instance().GetCurrentCharacterPtr();
    CArenaManager::instance().AddObserver(observer, map, x, y);
    return 1;
}

int arena_is_in_arena(lua_State* state)
{
    const lua_Number value = lua_tonumber(state, 1);
    std::uint32_t pid = 0;
    if (value >= -9223372036854775808.0 && value < 9223372036854775808.0)
        pid = static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
    else
        std::feraiseexcept(FE_INVALID);
    CHARACTER* ch = CHARACTER_MANAGER::instance().FindByPID(pid);
    bool result = !ch;
    if (ch && (!ch->GetArena() || ch->GetArenaObserverMode()))
    {
        const auto playerID = ch->GetPlayerID();
        const auto map = ch->GetMapIndex();
        result = CArenaManager::instance().IsMember(map, playerID) == 1;
    }
    lua_pushnumber(state, result);
    return 1;
}

void RegisterArenaFunctionTable()
{
    luaL_reg functions[] = {
        {"start_duel", arena_start_duel},
        {"add_map", arena_add_map},
        {"get_duel_list", arena_get_duel_list},
        {"add_observer", arena_add_observer},
        {"is_in_arena", arena_is_in_arena},
        {nullptr, nullptr},
    };
    CQuestManager::instance().AddLuaFunctionTable("arena", functions);
}
}
