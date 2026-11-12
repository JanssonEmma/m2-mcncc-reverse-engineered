#include <ctime>
#include <climits>
#include <cstdio>
#include <fenv.h>
#include <math.h>
#include "char.h"
#include "locale_service.h"
#include "../../libthecore/include/utils.h"
#include <sstream>
#include "config.h"
#include "../../libthecore/include/log.h"
// Original questlua.cpp closed methods only; full VM/startup/RunState remains pending.
#include "questmanager.h"
extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}
namespace quest {
//0805cd80: originalNRVOdirectretstorage. Defaultcounters remain indeterminate; runtime not forced.
QuestState CQuestManager::OpenState(const std::string& quest_name,int state_index) {
    (void)quest_name;
    QuestState qs;
    qs.args=0;
    qs.st=state_index;
    qs.co=lua_newthread(L);
    qs.ico=luaL_ref(L,LUA_REGISTRYINDEX);
    return qs;
}
//0805d000: CURRENT ownerL registry, co used only as guard; no coroutine/VM close or fieldreset.
void CQuestManager::CloseState(QuestState& qs) {
    if(qs.co) {
        luaL_unref(L,LUA_REGISTRYINDEX,qs.ico);
        qs.co=0;
    }
}
//0805cd10: qs unused; only script append and complete actualSendScript, no stateclose.
void CQuestManager::GotoEndState(QuestState& qs) {
    (void)qs;
    AddScript(std::string("[DONE]"));
    SendScript();
}
}

// Complete original questlua.cpp yield/script bridges; RunState and Confirm remain separate.
namespace quest {
//0805cc90: write suspendBEFOREscriptappend/SendScript; no Lua stack mutation.
void CQuestManager::GotoPauseState(QuestState& qs){qs.suspend_state=1;AddScript(std::string("[NEXT]"));SendScript();}
//0805cc10
void CQuestManager::GotoInputState(QuestState& qs){qs.suspend_state=3;AddScript(std::string("[INPUT]"));SendScript();}
//0805cb90
void CQuestManager::GotoSelectItemState(QuestState& qs){qs.suspend_state=5;AddScript(std::string("[SELECT_ITEM]"));SendScript();}
//0805c1d0: full table traversal, invalidentry diagnostics AND normal complete script path.
void CQuestManager::GotoSelectState(QuestState& qs){
    lua_checkstack(qs.co,1);
    int n=luaL_getn(qs.co,-1);
    qs.args=static_cast<short>(n);
    std::ostringstream os;
    os<<"[QUESTION ";
    for(int i=1;i<=n;++i){
        lua_rawgeti(qs.co,-1,i);
        if(lua_isstring(qs.co,-1)){
            if(i!=1)os<<"|";
            os<<i<<";"<<lua_tostring(qs.co,-1);
        }else{
            const char* type=lua_typename(qs.co,-1); // Original -1 typecode bug, NOTlua_type repair.
            CQuestManager::instance().QuestError("GotoSelectState",2021,"SELECT wrong data %s",type);
            CQuestManager::instance().QuestError("GotoSelectState",2022,"here");
        }
        lua_pop(qs.co,1);
    }
    os<<"]";
    AddScript(os.str());
    qs.suspend_state=2;
    if(test_server)sys_log(0,"%s",m_strScript.c_str());
    SendScript();
}
}

namespace quest {
// Original 0805a5e0. The final global assignment honors Lua metamethods.
void CQuestManager::AddLuaFunctionTable(const char* name, luaL_reg* functions) {
    lua_newtable(L);
    for (; functions->name; ++functions) {
        lua_pushstring(L, functions->name);
        lua_pushcfunction(L, functions->func);
        lua_rawset(L, -3);
    }
    lua_setglobal(L, name);
}

// Original 0805a690. Lua's number/string predicates include coercible values.
void CQuestManager::BuildStateIndexToName(const char* quest_name) {
    const int top = lua_gettop(L);
    lua_getglobal(L, quest_name);
    if (lua_isnil(L, -1)) {
        CQuestManager::instance().QuestError("BuildStateIndexToName", 1740,
                                            "QUEST wrong quest state file for quest %s", quest_name);
    } else {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            if (lua_isstring(L, -2) && lua_isnumber(L, -1)) {
                lua_pushvalue(L, -2);
                lua_rawset(L, -4);
            } else {
                lua_pop(L, 1);
            }
        }
    }
    lua_settop(L, top);
}
}

namespace quest {

// The original i386 casts use a truncating signed 32-bit x87 conversion.
static int LuaTruncateInt(lua_Number value) {
    if (!(value > -2147483649.0 && value < 2147483648.0)) {
        feraiseexcept(FE_INVALID);
        return INT_MIN;
    }
    return static_cast<int>(value);
}

// 08052b40
void combine_lua_string(lua_State* state, std::ostringstream& output) {
    const int count = lua_gettop(state);
    for (int index = 1; index <= count; ++index) {
        if (lua_isstring(state, index)) {
            output << lua_tostring(state, index);
        } else if (lua_isnumber(state, index)) {
            char text[20];
            std::sprintf(text, "%.14g\n", lua_tonumber(state, index));
            output << text;
        }
    }
}

// 08053e70
int _get_locale(lua_State* state) {
    lua_pushstring(state, g_stLocale.c_str());
    return 1;
}

// 08053eb0
int _number(lua_State* state) {
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2)) {
        lua_pushnumber(state, 0);
        return 1;
    }
    const int last = LuaTruncateInt(lua_tonumber(state, 2));
    const int first = LuaTruncateInt(lua_tonumber(state, 1));
    lua_pushnumber(state, number_ex(first, last, const_cast<char*>("questlua.cpp"), 315));
    return 1;
}

// 08055b70
int _say(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string script = output.str() + "[ENTER]";
    CQuestManager::Instance().AddScript(script);
    return 0;
}

// 08055df0
int _raw_script(lua_State* state) {
    if (test_server)
        sys_log(0, "_raw_script : %s ", lua_tostring(state, -1));
    if (lua_isstring(state, -1)) {
        const std::string script(lua_tostring(state, -1));
        CQuestManager::Instance().AddScript(script);
    } else {
        const char* name = CQuestManager::instance().GetCurrentQuestName().c_str();
        CQuestManager::instance().QuestError("_raw_script", 706,
                                             "QUEST wrong argument: questname: %s", name);
    }
    return 0;
}

// 08056270
int _set_skin(lua_State* state) {
    if (lua_isnumber(state, -1)) {
        const double rounded = rint(lua_tonumber(state, -1));
        CQuestManager& manager = CQuestManager::Instance();
        manager.SetSkinStyle(LuaTruncateInt(rounded));
    } else {
        CQuestManager::instance().QuestError("_set_skin", 739, "QUEST wrong skin index");
    }
    return 0;
}

// 08056490
int _get_server_timer_arg(lua_State* state) {
    lua_pushnumber(state, CQuestManager::instance().GetServerTimerArg());
    return 1;
}

// 08057ac0
int _getnpcid(lua_State* state) {
    const char* name = lua_tostring(state, -1);
    CQuestManager& manager = CQuestManager::instance();
    lua_pushnumber(state, manager.FindNPCIDByName(std::string(name)));
    return 1;
}

// 08057b90
int _is_test_server(lua_State* state) {
    lua_pushboolean(state, test_server);
    return 1;
}

// 0805a4b0
int _is_allow_map(lua_State* state) {
    if (test_server) {
        CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
        const bool allowed = map_allow_find(LuaTruncateInt(lua_tonumber(state, 1)));
        const int map = LuaTruncateInt(lua_tonumber(state, 1));
        character->ChatPacket(1, "allow map %d %d", map, static_cast<int>(allowed));
    }
    const bool allowed = map_allow_find(LuaTruncateInt(lua_tonumber(state, 1)));
    lua_pushnumber(state, allowed);
    return 1;
}


// 08055fb0
int _left_image(lua_State* state) {
    if (lua_isstring(state, -1)) {
        const std::string image(lua_tostring(state, -1));
        const std::string script = "[LEFTIMAGE src;" + image + "]";
        CQuestManager::Instance().AddScript(script);
    }
    return 0;
}

// 08056110
int _top_image(lua_State* state) {
    if (lua_isstring(state, -1)) {
        const std::string image(lua_tostring(state, -1));
        const std::string script = "[TOPIMAGE src;" + image + "]";
        CQuestManager::Instance().AddScript(script);
    }
    return 0;
}

// 08057120
int _time_to_str(lua_State* state) {
    const std::time_t instant = static_cast<std::time_t>(LuaTruncateInt(lua_tonumber(state, -1)));
    lua_pushstring(state, std::asctime(std::gmtime(&instant)));
    return 1;
}

// 08057180
int _chat(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(0, "%s", message.c_str());
    return 0;
}

// 080573d0
int _cmdchat(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(5, "%s", message.c_str());
    return 0;
}

// 08057620
int _syschat(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(1, "%s", message.c_str());
    return 0;
}

// 08057870
int _notice(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(2, "%s", message.c_str());
    return 0;
}

}
