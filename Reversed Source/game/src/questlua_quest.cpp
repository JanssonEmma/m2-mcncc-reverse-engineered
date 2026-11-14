#include "questmanager.h"
#include "questpc.h"
#include "config.h"
#include "../../libthecore/include/log.h"

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <string>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
PC* RunningPC()
{
    PC* pc = CQuestManager::instance().GetCurrentPC();
    return pc && pc->GetRunningQuestState() ? pc : nullptr;
}

std::int32_t ConvertRoundedValue(double rounded)
{
    if (rounded >= -2147483648.0 && rounded < 2147483648.0)
        return static_cast<std::int32_t>(rounded);
    std::feraiseexcept(FE_INVALID);
    return INT32_MIN;
}
}

int quest_start(lua_State*)
{
    if (PC* pc = RunningPC())
        pc->SetCurrentQuestStartFlag();
    return 0;
}

int quest_done(lua_State*)
{
    if (PC* pc = RunningPC())
        pc->SetCurrentQuestDoneFlag();
    return 0;
}

int quest_set_title(lua_State* state)
{
    if (!lua_isstring(state, -1))
        return 0;
    const std::string value = lua_tostring(state, -1);
    if (PC* pc = RunningPC())
        pc->SetCurrentQuestTitle(value);
    return 0;
}

int quest_set_another_title(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isstring(state, 2)) {
        const std::string title = lua_tostring(state, 2);
        const std::string name = lua_tostring(state, 1);
        if (PC* pc = CQuestManager::instance().GetCurrentPC())
            pc->SetQuestTitle(name, title);
    }
    return 0;
}

int quest_set_clock_name(lua_State* state)
{
    if (!lua_isstring(state, -1))
        return 0;
    const std::string value = lua_tostring(state, -1);
    if (PC* pc = RunningPC())
        pc->SetCurrentQuestClockName(value);
    return 0;
}

int quest_set_clock_value(lua_State* state)
{
    if (!lua_isnumber(state, -1))
        return 0;
    const double rounded = std::rint(lua_tonumber(state, -1));
    PC* pc = RunningPC();
    const auto value = ConvertRoundedValue(rounded);
    if (pc)
        pc->SetCurrentQuestClockValue(value);
    return 0;
}

int quest_set_counter_name(lua_State* state)
{
    if (!lua_isstring(state, -1))
        return 0;
    const std::string value = lua_tostring(state, -1);
    if (PC* pc = RunningPC())
        pc->SetCurrentQuestCounterName(value);
    return 0;
}

int quest_set_counter_value(lua_State* state)
{
    if (!lua_isnumber(state, -1))
        return 0;
    const double rounded = std::rint(lua_tonumber(state, -1));
    PC* pc = RunningPC();
    const auto value = ConvertRoundedValue(rounded);
    if (pc)
        pc->SetCurrentQuestCounterValue(value);
    return 0;
}

int quest_set_icon_file(lua_State* state)
{
    if (!lua_isstring(state, -1))
        return 0;
    const std::string value = lua_tostring(state, -1);
    if (PC* pc = RunningPC())
        pc->SetCurrentQuestIconFile(value);
    return 0;
}

int quest_setstate(lua_State* state)
{
    const char* next = lua_tostring(state, -1);
    CQuestManager& manager = CQuestManager::instance();
    if (!next) {
        manager.QuestError("quest_setstate", 0x70, "state name is empty");
        return 0;
    }

    QuestState* running = manager.GetCurrentState();
    PC* pc = manager.GetCurrentPC();
    if (!running || running->co != state)
        return luaL_error(state, "running thread != current thread???");

    if (pc) {
        const std::string nextState = lua_tostring(state, -1);
        if (test_server)
            sys_log(0, "questlua->setstate( %s , %s )", pc->GetCurrentQuestName().c_str(), nextState.c_str());
        running->st = manager.GetQuestStateIndex(pc->GetCurrentQuestName(), nextState);
        pc->SetCurrentQuestStateName(nextState);
    }
    return 0;
}

int quest_coroutine_yield(lua_State* state)
{
    return lua_yield(state, lua_gettop(state));
}

int quest_no_send(lua_State*)
{
    CQuestManager::instance().SetNoSend();
    return 0;
}

int quest_get_current_quest_index(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    PC* pc = manager.GetCurrentPC();
    const unsigned index = pc ? manager.GetQuestIndexByName(pc->GetCurrentQuestName()) : 0;
    lua_pushnumber(state, static_cast<int>(index));
    return 1;
}

// 0x08169940, table at 0x0822bee0.
void RegisterQuestFunctionTable()
{
    luaL_reg functions[] = {
        {"setstate", quest_setstate},
        {"set_state", quest_setstate},
        {"yield", quest_coroutine_yield},
        {"set_title", quest_set_title},
        {"set_title2", quest_set_another_title},
        {"set_clock_name", quest_set_clock_name},
        {"set_clock_value", quest_set_clock_value},
        {"set_counter_name", quest_set_counter_name},
        {"set_counter_value", quest_set_counter_value},
        {"set_icon", quest_set_icon_file},
        {"start", quest_start},
        {"done", quest_done},
        {"getcurrentquestindex", quest_get_current_quest_index},
        {"no_send", quest_no_send},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("q", functions);
}
}
