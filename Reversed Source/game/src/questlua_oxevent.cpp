#include "questmanager.h"
#include "OXEvent.h"
#include "config.h"
#include "locale_service.h"
#include <cstdio>
#include <cstdint>
extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
std::int32_t OXNumber(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value > -2147483649.0 && value < 2147483648.0))
        return INT32_MIN;
    return static_cast<std::int32_t>(value);
}
}

int oxevent_get_status(lua_State* state)
{
    lua_pushnumber(state, COXEventManager::instance().GetStatus());
    return 1;
}

int oxevent_open(lua_State* state)
{
    auto& manager = COXEventManager::instance();
    manager.ClearQuiz();
    char script[256];
    std::snprintf(script, 255, "%s/oxquiz.lua", LocaleService_GetBasePath().c_str());
    const bool loaded = lua_dofile(CQuestManager::instance().GetLuaState(), script) == 0;
    lua_pushnumber(state, loaded);
    if (loaded)
        manager.SetStatus(OXEVENT_OPEN);
    return 1;
}

// These OX bindings return one existing Lua stack value.
int oxevent_close(lua_State*)
{
    COXEventManager::instance().SetStatus(OXEVENT_CLOSE);
    return 1;
}

int oxevent_quiz(lua_State* state)
{
    if (lua_isnumber(state, 1) && lua_isnumber(state, 2)) {
        const auto duration = OXNumber(state, 2);
        const auto level = static_cast<BYTE>(OXNumber(state, 1));
        lua_pushnumber(state, COXEventManager::instance().Quiz(level, duration));
    }
    return 1;
}

int oxevent_get_attender(lua_State* state)
{
    lua_pushnumber(state, static_cast<std::int32_t>(COXEventManager::instance().GetAttenderCount()));
    return 1;
}

EVENTFUNC(end_oxevent)
{
    COXEventManager::instance().CloseEvent();
    return 0;
}

int oxevent_end_event(lua_State*)
{
    COXEventManager::instance().SetStatus(OXEVENT_FINISH);
    void* info;
    const DWORD id = __AllocEventInfo(4, &info);
    const auto delay = static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * 5u);
    event_create_ex(end_oxevent, id, delay, const_cast<char*>("questlua_oxevent.cpp"), 0x59);
    return 1;
}

int oxevent_end_event_force(lua_State*)
{
    auto& manager = COXEventManager::instance();
    manager.CloseEvent();
    manager.SetStatus(OXEVENT_FINISH);
    return 1;
}

int oxevent_give_item(lua_State* state)
{
    if (lua_isnumber(state, 1) && lua_isnumber(state, 2)) {
        const auto count = static_cast<BYTE>(OXNumber(state, 2));
        const auto vnum = static_cast<std::uint32_t>(OXNumber(state, 1));
        COXEventManager::instance().GiveItemToAttender(vnum, count);
    }
    return 1;
}

void RegisterOXEventFunctionTable()
{
    luaL_reg functions[] = {
        {"get_status", oxevent_get_status},
        {"open", oxevent_open},
        {"close", oxevent_close},
        {"quiz", oxevent_quiz},
        {"get_attender", oxevent_get_attender},
        {"end_event", oxevent_end_event},
        {"end_event_force", oxevent_end_event_force},
        {"give_item", oxevent_give_item},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("oxevent", functions);
}
}
