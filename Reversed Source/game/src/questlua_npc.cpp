#include "char.h"
#include "char_manager.h"
#include "guild.h"
#include "questmanager.h"
#include "shop_manager.h"
#include "utils.h"
#include "../../libthecore/include/log.h"

#include <algorithm>
#include <cstdint>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
bool IsNear(CHARACTER* first, CHARACTER* second, double range)
{
    if (!first || !second)
        return false;
    const auto dx = static_cast<std::int32_t>(static_cast<std::uint32_t>(first->GetX()) -
                                              static_cast<std::uint32_t>(second->GetX()));
    const auto dy = static_cast<std::int32_t>(static_cast<std::uint32_t>(first->GetY()) -
                                              static_cast<std::uint32_t>(second->GetY()));
    return DISTANCE_APPROX(dx, dy) < 100.0L * static_cast<long double>(range);
}

CHARACTER* GetSanta()
{
    CHARACTER* npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
    return npc && !npc->GetDesc() && npc->GetRaceNum() == 20031 ? npc : nullptr;
}

int GetSantaCount(lua_State* state, BYTE point)
{
    CHARACTER* npc = GetSanta();
    lua_pushnumber(state, npc ? std::max(0L, npc->GetPoint(point)) : 0);
    return 1;
}

int DecSantaCount(BYTE point)
{
    if (CHARACTER* npc = GetSanta()) {
        const long count = npc->GetPoint(point);
        npc->SetPoint(point, count > 0 ? count - 1 : 0);
    }
    return 0;
}
}

int npc_open_shop(lua_State* state)
{
    int shop = 0;
    if (lua_gettop(state) == 1 && lua_isnumber(state, 1)) {
        const auto value = lua_tonumber(state, 1);
        shop = value > -2147483649.0 && value < 2147483648.0
            ? static_cast<std::int32_t>(value) : INT32_MIN;
    }

    CQuestManager& manager = CQuestManager::instance();
    if (manager.GetCurrentNPCCharacterPtr()) {
        CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
        CHARACTER* character = manager.GetCurrentCharacterPtr();
        if (npc && character)
            CShopManager::instance().StartShopping(character, npc, shop);
    }
    return 0;
}

int npc_is_pc(lua_State* state)
{
    CHARACTER* npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
    lua_pushboolean(state, npc && npc->GetDesc());
    return 1;
}

int npc_get_empire(lua_State* state)
{
    CHARACTER* npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
    lua_pushnumber(state, npc ? npc->GetEmpire() : 0);
    return 1;
}

int npc_get_race(lua_State* state)
{
    lua_pushnumber(state, CQuestManager::instance().GetCurrentNPCRace());
    return 1;
}

int npc_get_guild(lua_State* state)
{
    CHARACTER* npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
    CGuild* guild = npc ? npc->GetGuild() : nullptr;
    lua_pushnumber(state, guild ? guild->GetID() : 0);
    return 1;
}

// The original Santa uses these two point slots as gift counters.
int npc_get_remain_skill_book_count(lua_State* state) { return GetSantaCount(state, POINT_ATT_GRADE_BONUS); }
int npc_dec_remain_skill_book_count(lua_State*) { return DecSantaCount(POINT_ATT_GRADE_BONUS); }
int npc_get_remain_hairdye_count(lua_State* state) { return GetSantaCount(state, POINT_DEF_GRADE_BONUS); }
int npc_dec_remain_hairdye_count(lua_State*) { return DecSantaCount(POINT_DEF_GRADE_BONUS); }

int npc_is_quest(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
    lua_pushboolean(state, npc && manager.GetCurrentPC()
        && npc->GetQuestByVnum() == manager.GetQuestIndexByName(manager.GetCurrentQuestName()));
    return 1;
}

int npc_kill(lua_State*)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
    if (CHARACTER* character = manager.GetCurrentCharacterPtr())
        character->SetQuestNPCID(0);
    if (npc)
        npc->Dead(nullptr, false);
    return 0;
}

int npc_purge(lua_State*)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
    if (CHARACTER* character = manager.GetCurrentCharacterPtr())
        character->SetQuestNPCID(0);
    if (npc)
        CHARACTER_MANAGER::instance().DestroyCharacter(npc);
    return 0;
}

int npc_is_near(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    const double distance = lua_isnumber(state, 1) ? lua_tonumber(state, 1) : 10.0;
    lua_pushboolean(state, IsNear(manager.GetCurrentCharacterPtr(), manager.GetCurrentNPCCharacterPtr(), distance));
    return 1;
}

int npc_is_near_vid(lua_State* state)
{
    if (!lua_isnumber(state, 1)) {
        _sys_err("npc_is_near_vid", 0xb9, "invalid vid");
        lua_pushboolean(state, false);
        return 1;
    }
    const double value = lua_tonumber(state, 1);
    const auto vid = value >= -9223372036854775808.0 && value < 9223372036854775808.0
        ? static_cast<std::uint32_t>(static_cast<std::int64_t>(value)) : 0u;
    CHARACTER* target = CHARACTER_MANAGER::instance().Find(vid);
    CHARACTER* npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
    const double distance = lua_isnumber(state, 2) ? lua_tonumber(state, 2) : 10.0;
    lua_pushboolean(state, IsNear(target, npc, distance));
    return 1;
}

int npc_unlock(lua_State*)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
    if (npc && !npc->GetDesc() && character && npc->GetQuestNPCID() == character->GetPlayerID())
        npc->SetQuestNPCID(0);
    return 0;
}

int npc_lock(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* npc = manager.GetCurrentNPCCharacterPtr();
    bool locked = true;
    if (npc && !npc->GetDesc()) {
        CHARACTER* character = manager.GetCurrentCharacterPtr();
        if (!character) {
            locked = false;
        } else {
            const DWORD pid = character->GetPlayerID();
            const DWORD owner = npc->GetQuestNPCID();
            locked = !owner || owner == pid;
            if (locked)
                npc->SetQuestNPCID(pid);
        }
    }
    lua_pushboolean(state, locked);
    return 1;
}

// 0x08121310, table at 0x08223a4c.
void RegisterNPCFunctionTable()
{
    luaL_reg functions[] = {
        {"getrace", npc_get_race},
        {"get_race", npc_get_race},
        {"open_shop", npc_open_shop},
        {"get_empire", npc_get_empire},
        {"is_pc", npc_is_pc},
        {"is_quest", npc_is_quest},
        {"kill", npc_kill},
        {"purge", npc_purge},
        {"is_near", npc_is_near},
        {"is_near_vid", npc_is_near_vid},
        {"lock", npc_lock},
        {"unlock", npc_unlock},
        {"get_guild", npc_get_guild},
        {"get_remain_skill_book_count", npc_get_remain_skill_book_count},
        {"dec_remain_skill_book_count", npc_dec_remain_skill_book_count},
        {"get_remain_hairdye_count", npc_get_remain_hairdye_count},
        {"dec_remain_hairdye_count", npc_dec_remain_hairdye_count},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("npc", functions);
}
}
