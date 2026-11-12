#include "char.h"
#include "constants.h"
#include "questmanager.h"
#include "questpc.h"
#include "../../libthecore/include/log.h"

#include <cstdint>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
constexpr DWORD questAffect = 1000;
constexpr DWORD hairAffect = 514;
constexpr DWORD collectAffect = 515;

int ReturnLegacyValue(lua_State* state)
{
    // The old wrappers return the top argument as one result.
    if (!lua_gettop(state))
        lua_pushnil(state);
    return 1;
}

std::int32_t AffectSignedNumber(lua_State* state, int argument)
{
    const lua_Number value = lua_tonumber(state, argument);
    if (!(value > -2147483649.0 && value < 2147483648.0))
        return INT32_MIN;
    return static_cast<std::int32_t>(value);
}

BYTE AffectSlot(lua_State* state)
{
    const lua_Number value = lua_tonumber(state, 1);
    if (!(value > -32769.0 && value < 32768.0))
        return 0;
    return static_cast<BYTE>(static_cast<std::int16_t>(value));
}

int AddAffect(lua_State* state, DWORD type, bool usePoint, const char* function, int line)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3)) {
        _sys_err(function, line, "invalid argument");
        return 0;
    }
    const BYTE slot = AffectSlot(state);
    const char* label = usePoint ? "POINTON" : "APPLYON";
    const int limit = usePoint ? static_cast<int>(POINT_MAX_NUM) : static_cast<int>(MAX_APPLY_NUM);
    sys_log(0, "%s %d value %d", label, slot, AffectSignedNumber(state, 2));
    if (slot < 1 || slot >= limit) {
        _sys_err(function, line + 13, "%s is out of range : %d", usePoint ? "point" : "apply", slot);
        return 0;
    }

    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!character || (type == questAffect && character->FindAffect(type, slot)))
        return 0;
    const auto value = AffectSignedNumber(state, 2);
    const auto duration = AffectSignedNumber(state, 3);
    sys_log(0, "--%s %d value %d dur %d", label, slot, value, duration);
    const BYTE point = usePoint ? slot : aApplyInfo[slot].bPointType;
    character->AddAffect(type, point, value, 0, duration, 0, false, false);
    return 1;
}
}

int affect_add(lua_State* state)
{
    return AddAffect(state, questAffect, false, "affect_add", 0x12);
}

int affect_remove(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!character)
        return 0;

    DWORD type = lua_isnumber(state, 1)
        ? static_cast<std::uint32_t>(AffectSignedNumber(state, 1)) : 0;
    if (!type) {
        PC* pc = manager.GetCurrentPC();
        if (!pc)
            return 0;
        type = questAffect + pc->GetCurrentQuestIndex();
    }

    character->RemoveAffect(type);
    return ReturnLegacyValue(state);
}

int affect_remove_bad(lua_State* state)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr())
        character->RemoveBadAffect();
    return ReturnLegacyValue(state);
}

int affect_remove_good(lua_State* state)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr())
        character->RemoveGoodAffect();
    return ReturnLegacyValue(state);
}

int affect_add_hair(lua_State* state)
{
    return AddAffect(state, hairAffect, false, "affect_add_hair", 0x55);
}

int affect_remove_hair(lua_State* state)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr())
        character->RemoveAffect(hairAffect);
    return ReturnLegacyValue(state);
}

int affect_add_collect(lua_State* state)
{
    return AddAffect(state, collectAffect, false, "affect_add_collect", 0x7e);
}

int affect_add_collect_point(lua_State* state)
{
    return AddAffect(state, collectAffect, true, "affect_add_collect_point", 0xa0);
}

int affect_remove_collect(lua_State*)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr())
        character->RemoveAffect(collectAffect);
    return 0;
}

// 0x08069d80, table at 0x081f87e4.
void RegisterAffectFunctionTable()
{
    luaL_reg functions[] = {
        {"add", affect_add},
        {"remove", affect_remove},
        {"remove_bad", affect_remove_bad},
        {"remove_good", affect_remove_good},
        {"add_hair", affect_add_hair},
        {"remove_hair", affect_remove_hair},
        {"add_collect", affect_add_collect},
        {"add_collect_point", affect_add_collect_point},
        {"remove_collect", affect_remove_collect},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("affect", functions);
}
}
