#include "char.h"
#include "questmanager.h"
#include "../../libthecore/include/log.h"
#include <algorithm>
#include <cmath>
#include <limits>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
LPCHARACTER CurrentHorseCharacter()
{
    return CQuestManager::instance().GetCurrentCharacterPtr();
}

void SetQuestHorseLevel(LPCHARACTER character, int level)
{
    character->SetHorseLevel(level);
    character->ComputePoints();
    character->SkillLevelPacket();
}

int HorsePercentage(int current, int maximum)
{
    return maximum > 0 ? std::max(0, std::min(current * 100 / maximum, 100)) : 0;
}
}

int horse_is_riding(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    const bool riding = character && character->IsHorseRiding();
    sys_log(1, "is_riding %d", static_cast<int>(riding));
    lua_pushnumber(state, riding ? 1 : 0);
    return 1;
}

int horse_ride(lua_State*)
{
    LPCHARACTER character = CurrentHorseCharacter();
    sys_log(1, "horse_ride: %p", static_cast<void*>(character));
    if (character)
        character->StartRiding();
    return 0;
}

int horse_unride(lua_State*)
{
    if (LPCHARACTER character = CurrentHorseCharacter())
        character->StopRiding();
    return 0;
}

int horse_summon(lua_State*)
{
    if (LPCHARACTER character = CurrentHorseCharacter())
        character->HorseSummon(true, false);
    return 0;
}

int horse_unsummon(lua_State*)
{
    if (LPCHARACTER character = CurrentHorseCharacter())
        character->HorseSummon(false, false);
    return 0;
}

int horse_is_mine(lua_State* state)
{
    auto& questManager = CQuestManager::instance();
    LPCHARACTER character = questManager.GetCurrentCharacterPtr();
    LPCHARACTER horse = questManager.GetCurrentNPCCharacterPtr();
    lua_pushboolean(state, horse && horse->GetRider() == character);
    return 1;
}

int horse_set_level(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    if (character && lua_isnumber(state, 1)) {
        const lua_Number value = std::trunc(lua_tonumber(state, 1));
        const int level = value >= 0 && value <= std::numeric_limits<int>::max()
            ? std::min(static_cast<int>(value), static_cast<int>(HORSE_MAX_LEVEL)) : 0;
        SetQuestHorseLevel(character, level);
    }
    return 0;
}

int horse_get_level(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    lua_pushnumber(state, character ? character->GetHorseLevel() : 0);
    return 1;
}

int horse_advance(lua_State*)
{
    if (LPCHARACTER character = CurrentHorseCharacter()) {
        const int level = character->GetHorseLevel();
        if (level < HORSE_MAX_LEVEL)
            SetQuestHorseLevel(character, level + 1);
    }
    return 0;
}

int horse_get_health(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    lua_pushnumber(state, character && character->GetHorseLevel() ? character->GetHorseHealth() : 0);
    return 1;
}

int horse_get_health_pct(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    const int percent = character
        ? HorsePercentage(character->GetHorseHealth(), character->GetHorseMaxHealth()) : 0;
    sys_log(1, "horse.get_health_pct %d", percent);
    lua_pushnumber(state, character && character->GetHorseLevel() ? percent : 0);
    return 1;
}

int horse_get_stamina(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    lua_pushnumber(state, character && character->GetHorseLevel() ? character->GetHorseStamina() : 0);
    return 1;
}

int horse_get_stamina_pct(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    const int percent = character
        ? HorsePercentage(character->GetHorseStamina(), character->GetHorseMaxStamina()) : 0;
    sys_log(1, "horse.get_stamina_pct %d", percent);
    lua_pushnumber(state, character && character->GetHorseLevel() ? percent : 0);
    return 1;
}

int horse_get_grade(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    lua_pushnumber(state, character ? character->GetHorseGrade() : 0);
    return 1;
}

int horse_is_dead(lua_State* state)
{
    LPCHARACTER character = CurrentHorseCharacter();
    lua_pushboolean(state, character && character->GetHorseHealth() <= 0);
    return 1;
}

int horse_revive(lua_State*)
{
    LPCHARACTER character = CurrentHorseCharacter();
    if (character && character->GetHorseLevel() && character->GetHorseHealth() <= 0)
        character->ReviveHorse();
    return 0;
}

int horse_feed(lua_State*)
{
    LPCHARACTER character = CurrentHorseCharacter();
    if (character && character->GetHorseLevel() && character->GetHorseHealth() > 0)
        character->FeedHorse();
    return 0;
}

void RegisterHorseFunctionTable()
{
    luaL_reg functions[] = {
        {"is_mine", horse_is_mine},
        {"is_riding", horse_is_riding},
        {"ride", horse_ride},
        {"unride", horse_unride},
        {"summon", horse_summon},
        {"unsummon", horse_unsummon},
        {"advance", horse_advance},
        {"get_level", horse_get_level},
        {"set_level", horse_set_level},
        {"get_health", horse_get_health},
        {"get_health_pct", horse_get_health_pct},
        {"get_stamina", horse_get_stamina},
        {"get_stamina_pct", horse_get_stamina_pct},
        {"get_grade", horse_get_grade},
        {"is_dead", horse_is_dead},
        {"revive", horse_revive},
        {"feed", horse_feed},
        {nullptr, nullptr},
    };
    CQuestManager::instance().AddLuaFunctionTable("horse", functions);
}
}
