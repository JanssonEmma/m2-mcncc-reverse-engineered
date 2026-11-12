#include "char.h"
#include "desc_client.h"
#include "../../common/tables.h"
#include <cstring>
extern "C" {
#include "../../liblua/include/lua.h"
}
#include <math.h>
//Partial old quest manager behavior; Lua startup/lifecycle remain pending.
#include "questmanager.h"
#include "char_manager.h"
#include "../../libthecore/include/log.h"
#include <cassert>
#include "config.h"
#include <fstream>
#include <iterator>
#include <algorithm>
namespace quest {
//081a6bb0: missing names return0 without insertion.
int CQuestManager::GetEventFlag(const std::string& name) {
    std::map<std::string,int>::iterator it=m_mapEventFlag.find(name);
    return it==m_mapEventFlag.end()?0:it->second;
}
// 0819f0f0/C2 and0819f5e0/C1 audited: ONLY L scalar initialized.
// All other raw scalar/pointer fields stay indeterminate until real setup/API.
CQuestManager::CQuestManager():L(0) {}
// 081a4d40. Complete real value insertion/destruction; SetID only for new key.
PC* CQuestManager::GetPCForce(unsigned int pc) {
    std::map<unsigned int,PC>::iterator it=m_mapPC.find(pc);
    if(it!=m_mapPC.end()) return &it->second;
    PC* result=&m_mapPC[pc];
    result->SetID(pc);
    return result;
}
// 081a4e50. Missing/null PID preserves ALL existing current context and map.
PC* CQuestManager::GetPC(unsigned int pc) {
    CHARACTER* ch=CHARACTER_MANAGER::instance().FindByPID(pc);
    if(!ch) return 0;
    PC* result=GetPCForce(pc);
    m_pCurrentPC=result;
    m_pCurrentCharacter=ch;
    m_pSelectedDungeon=0;
    return result;
}

// Complete original questmanager.cpp naming/index queries.
// 081a1f00: no insertion or diagnostics on miss.
unsigned int CQuestManager::FindNPCIDByName(const std::string& name) {
    std::map<std::string,unsigned int>::iterator it=m_mapNPCNameID.find(name);
    return it==m_mapNPCNameID.end()?0:it->second;
}
// 081a5720: current PC must be valid; return original reference directly.
const std::string& CQuestManager::GetCurrentQuestName() {
    return m_pCurrentPC->GetCurrentQuestName();
}
// 081a5a90: actual SGI hash owner, signed stored int converted to uint32.
unsigned int CQuestManager::GetQuestIndexByName(const std::string& name) {
    __gnu_cxx::hash_map<std::string,int,stringhash,std::equal_to<std::string>,std::allocator<int> >::iterator it=m_hmQuestName.find(name);
    return it==m_hmQuestName.end()?0:static_cast<unsigned int>(it->second);
}
// 081a5b60: error -> assertion -> persistent empty reference if assertion returns.
const std::string& CQuestManager::GetQuestNameByIndex(unsigned int index) {
    std::map<unsigned int,std::string>::iterator it=m_mapQuestNameByIndex.find(index);
    if(it==m_mapQuestNameByIndex.end()) {
        _sys_err("GetQuestNameByIndex",1077,"cannot find quest name by index %u",index);
        assert(!"cannot find quest name by index");
        static std::string st="";
        return st;
    }
    return it->second;
}

// Complete original questmanager.cpp081a4690/081a5770.
// Required actual headers: questmanager.h, config.h, libthecore/log.h,
// <fstream>, <iterator>, <algorithm>, <cassert>.
void CQuestManager::LoadStartQuest(const std::string& name,unsigned int index) {
    for(std::set<std::string>::iterator it=g_setQuestObjectDir.begin();it!=g_setQuestObjectDir.end();++it) {
        std::string fullName=*it+"/begin_condition/"+name;
        std::ifstream input(fullName.c_str()); // Original ios::in8, not binary.
        if(input.is_open()) {
            sys_log(0,"QUEST loading begin condition for %s",name.c_str());
            std::istreambuf_iterator<char> first(input),last;
            std::copy(first,last,std::back_inserter(m_hmQuestStartScript[index]));
        }
    }
}
void CQuestManager::RegisterQuest(const std::string& name,unsigned int index) {
    assert(index>0);
    __gnu_cxx::hash_map<std::string,int,stringhash,std::equal_to<std::string>,std::allocator<int> >::iterator it=m_hmQuestName.find(name);
    if(it!=m_hmQuestName.end()) return;
    m_hmQuestName.insert(std::make_pair(name,index));
    LoadStartQuest(name,index);
    m_mapQuestNameByIndex.insert(std::make_pair(index,name));
    sys_log(0,"QUEST: Register %4u %s",index,name.c_str());
}

}

// Complete original questmanager.cpp state-name/index lookups; actual Lua C SDK only.
// Required source includes: extern "C" { #include "../../liblua/include/lua.h" }, <math.h>.
namespace quest {
//081a5130: caller owns valid Lua state; return the raw borrowed Lua string after restoring stack.
const char* CQuestManager::GetQuestStateName(const std::string& quest_name,int state_index) {
    int x=lua_gettop(L);
    lua_getglobal(L,quest_name.c_str());
    if(lua_isnil(L,-1)) {
        _sys_err("GetQuestStateName",939,"QUEST wrong quest state file %s.%d",quest_name.c_str(),state_index);
        lua_settop(L,x);
        return "";
    }
    lua_pushnumber(L,state_index);
    lua_gettable(L,-2);
    const char* str=lua_tostring(L,-1);
    lua_settop(L,x);
    return str;
}
//081a5240: native rint followed by signed-int truncation; finite representable domain only in tests.
int CQuestManager::GetQuestStateIndex(const std::string& quest_name,const std::string& state_name) {
    int x=lua_gettop(L);
    lua_getglobal(L,quest_name.c_str());
    if(lua_isnil(L,-1)) {
        _sys_err("GetQuestStateIndex",957,"QUEST wrong quest state file %s.%s",quest_name.c_str(),state_name.c_str());
        lua_settop(L,x);
        return 0;
    }
    lua_pushstring(L,state_name.c_str());
    lua_gettable(L,-2);
    int v=static_cast<int>(rint(lua_tonumber(L,-1)));
    lua_settop(L,x);
    if(test_server)
        sys_log(0,"[QUESTMANAGER] GetQuestStateIndex x(%d) v(%d) %s %s",v,x,quest_name.c_str(),state_name.c_str());
    return v;
}
}

// Complete original bounded event-flag methods; SetEventFlag gameplay effects remain separate.
namespace quest {
//081a5c40: sorted LIVE traversal; value cached per row; advance AFTER actual ChatPacket.
void CQuestManager::SendEventFlagList(CHARACTER* ch) {
    std::map<std::string,int>::iterator it;
    for(it=m_mapEventFlag.begin();it!=m_mapEventFlag.end();++it) {
        const std::string& flagname=it->first;
        int value=it->second;
        if(!test_server && value==1 && flagname=="valentine_drop")
            ch->ChatPacket(1,"%s %d prob 800",flagname.c_str(),value);
        else if(!test_server && value==1 && flagname=="newyear_wonso")
            ch->ChatPacket(1,"%s %d prob 500",flagname.c_str(),value);
        else if(!test_server && value==1 && flagname=="newyear_fire")
            ch->ChatPacket(1,"%s %d prob 1000",flagname.c_str(),value);
        else ch->ChatPacket(1,"%s %d",flagname.c_str(),value);
    }
}
//081a5df0: full37-byte original packet, strncpy32 + explicit trailingNUL, no local flag mutation.
void CQuestManager::RequestSetEventFlag(const std::string& name,int value) {
    TPacketSetEventFlag p;
    std::strncpy(p.szFlagName,name.c_str(),32);
    p.szFlagName[32]='\0';
    p.lValue=value;
    db_clientdesc->DBPacket(55,0,&p,sizeof(p));
}
//081a6c00: originalfive selected flags and independent current-singleton queries in this order.
void CQuestManager::BroadcastEventFlagOnLogin(CHARACTER* ch) {
    int value;
    if((value=CQuestManager::instance().GetEventFlag("xmas_snow"))) ch->ChatPacket(5,"xmas_snow %d",value);
    if((value=CQuestManager::instance().GetEventFlag("xmas_boom"))) ch->ChatPacket(5,"xmas_boom %d",value);
    if((value=CQuestManager::instance().GetEventFlag("xmas_tree"))) ch->ChatPacket(5,"xmas_tree %d",value);
    if((value=CQuestManager::instance().GetEventFlag("day"))) ch->ChatPacket(5,"DayMode dark");
    if((value=CQuestManager::instance().GetEventFlag("newyear_boom"))) ch->ChatPacket(5,"newyear_boom %d",value);
}
}
