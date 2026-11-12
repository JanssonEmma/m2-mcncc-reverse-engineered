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
