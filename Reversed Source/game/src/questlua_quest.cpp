#include "questmanager.h"
#include "questpc.h"
#include "config.h"
#include "../../libthecore/include/log.h"

#include <cmath>
#include <cstdint>

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

bool ReadRoundedValue(lua_State* state, int& value)
{
    if (!lua_isnumber(state, -1))
        return false;
    const double rounded = std::rint(lua_tonumber(state, -1));
    value = rounded >= -2147483648.0 && rounded < 2147483648.0
        ? static_cast<std::int32_t>(rounded) : INT32_MIN;
    return true;
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
    PC* pc = RunningPC();
    if (pc && lua_isstring(state, -1))
        pc->SetCurrentQuestTitle(lua_tostring(state, -1));
    return 0;
}

int quest_set_another_title(lua_State* state)
{
    PC* pc = CQuestManager::instance().GetCurrentPC();
    if (pc && lua_isstring(state, 1) && lua_isstring(state, 2)) {
        const std::string title = lua_tostring(state, 2);
        const std::string name = lua_tostring(state, 1);
        pc->SetQuestTitle(name, title);
    }
    return 0;
}

int quest_set_clock_name(lua_State* state)
{
    PC* pc = RunningPC();
    if (pc && lua_isstring(state, -1))
        pc->SetCurrentQuestClockName(lua_tostring(state, -1));
    return 0;
}

int quest_set_clock_value(lua_State* state)
{
    PC* pc = RunningPC();
    int value;
    if (pc && ReadRoundedValue(state, value))
        pc->SetCurrentQuestClockValue(value);
    return 0;
}

int quest_set_counter_name(lua_State* state)
{
    PC* pc = RunningPC();
    if (pc && lua_isstring(state, -1))
        pc->SetCurrentQuestCounterName(lua_tostring(state, -1));
    return 0;
}

int quest_set_counter_value(lua_State* state)
{
    PC* pc = RunningPC();
    int value;
    if (pc && ReadRoundedValue(state, value))
        pc->SetCurrentQuestCounterValue(value);
    return 0;
}

int quest_set_icon_file(lua_State* state)
{
    PC* pc = RunningPC();
    if (pc && lua_isstring(state, -1))
        pc->SetCurrentQuestIconFile(lua_tostring(state, -1));
    return 0;
}

int quest_setstate(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    const char* next = lua_tostring(state, -1);
    if (!next) {
        manager.QuestError("quest_setstate", 0x70, "state name is empty");
        return 0;
    }

    QuestState* running = manager.GetCurrentState();
    if (!running || running->co != state)
        return luaL_error(state, "running thread != current thread???");

    if (PC* pc = manager.GetCurrentPC()) {
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
