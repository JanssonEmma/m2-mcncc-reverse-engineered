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
