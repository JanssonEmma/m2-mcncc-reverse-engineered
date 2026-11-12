#include <climits>
#include <dirent.h>
#include <cstdarg>
#include <cstdio>
#include "char.h"
extern "C" {
#include "../../liblua/include/lauxlib.h"
}
#include "char.h"
#include "desc.h"
#include "buffer_manager.h"
#include "packet.h"
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

// Complete original bounded questmanager.cpp script owner methods.
namespace quest {
//081a4ee0: stringclear before skinreset; other flags/context unchanged.
void CQuestManager::ClearScript() { m_strScript.clear();m_iCurrentSkin=1; }
//081a4f30: full std::string append, including embeddedNUL and selfalias.
void CQuestManager::AddScript(const std::string& str) { m_strScript.append(str); }
//081a4f50: full suppression/packet/livecallback sequence, no source narrowing.
void CQuestManager::SendScript() {
    if(m_bNoSend) {m_bNoSend=false;ClearScript();return;}
    if(m_strScript=="[DONE]" || m_strScript=="[NEXT]") {
        if(m_pCurrentPC && !m_pCurrentPC->GetAndResetDoneFlag() && m_strScript=="[DONE]" && m_iCurrentSkin==1 && !m_bError) {ClearScript();return;}
        m_iCurrentSkin=0;
    }
    struct packet_script packet;
    packet.header=45;
    packet.skin=static_cast<BYTE>(m_iCurrentSkin);
    packet.src_size=static_cast<WORD>(m_strScript.size());
    TEMP_BUFFER buf;
    packet.size=static_cast<WORD>(packet.src_size+sizeof(packet));
    buf.write(&packet,sizeof(packet));
    int payloadLength=static_cast<int>(m_strScript.size());
    char* payload=&m_strScript[0];
    buf.write(payload,payloadLength);
    int bytes=buf.size();
    const void* data=buf.read_peek();
    GetCurrentCharacterPtr()->GetDesc()->Packet(data,bytes);
    if(test_server) {
        int debugBytes=buf.size();
        sys_log(0,"m_strScript %s size %d",m_strScript.c_str(),debugBytes);
    }
    ClearScript();
}
}


// Complete original questmanager.cpp diagnostic body; doesNOT set m_bError.
namespace quest {
//081a7bb0: bounded nativevsnprintf then actualerror sink, freshserver/currentCHAR, cachedCHARtwomessages.
void CQuestManager::QuestError(const char* func,int line,const char* fmt,...){
    char szMsg[4096];
    va_list args;va_start(args,fmt);
    std::vsnprintf(szMsg,sizeof(szMsg),fmt,args);
    va_end(args);
    _sys_err(func,line,"%s",szMsg);
    if(test_server){
        CHARACTER* ch=GetCurrentCharacterPtr();
        if(ch){ch->ChatPacket(3,"error occor on [%s:%d]",func,line);ch->ChatPacket(3,"%s",szMsg);}
    }
}
}


// Whole original questmanager.cpp predicate and diagnostic; actual LuaSDK/query/CHAR graph dependencies.
namespace quest {
//081a4c20: missing entry permits start without touching L. Status is intentionally ignored.
bool CQuestManager::CanStartQuest(unsigned int quest_index) {
    __gnu_cxx::hash_map<unsigned int,std::vector<char>,__gnu_cxx::hash<unsigned int>,std::equal_to<unsigned int>,std::allocator<std::vector<char> > >::iterator it=m_hmQuestStartScript.find(quest_index);
    if(it==m_hmQuestStartScript.end()) return true;
    int x=lua_gettop(L);
    lua_dobuffer(L,&it->second[0],it->second.size(),"StartScript");
    int bStart=lua_toboolean(L,-1);
    lua_settop(L,x);
    return bStart!=0;
}
//081a7a20: old state/index before reverse lookup; current quest/context reread after Lua/log callbacks.
void CQuestManager::WriteRunningStateToSyserr() {
    QuestState* current=m_CurrentRunningState;
    const char* state_name=GetQuestStateName(GetCurrentQuestName(),current->st);
    std::string event_index_name="";
    TEventNameMap::iterator it;
    for(it=m_mapEventName.begin();it!=m_mapEventName.end();++it) {
        if(it->second==m_iRunningEventIndex) {event_index_name=it->first;break;}
    }
    const char* quest_name=GetCurrentQuestName().c_str();
    _sys_err("WriteRunningStateToSyserr",1437,"LUA_ERROR: quest %s.%s %s",quest_name,state_name,event_index_name.c_str());
    if(m_pCurrentCharacter && test_server) {
        const char* live_quest_name=GetCurrentQuestName().c_str();
        m_pCurrentCharacter->ChatPacket(3,"LUA_ERROR: quest %s.%s %s",live_quest_name,state_name,event_index_name.c_str());
    }
}
}

// Whole original CQM range selector081a5400, unsignedpredicate includes negativefallback.
namespace quest {
void CQuestManager::SetSkinStyle(int style){
    if(static_cast<unsigned int>(style)<6U)m_iCurrentSkin=style;
    else m_iCurrentSkin=1;
}
}

// Complete original CQM owning lifecycle. Compiler retains complete member/base teardown.
namespace quest {
//081a0720: actual currentL close; setNULL ONLYafter all SDKfinalizers return. No registry/context repair.
void CQuestManager::Destroy(){if(L){lua_close(L);L=0;}}
//D2 0819fa20/D1 0819fe70/D0 081a02c0. All full owner fields and singleton base destruct afterward.
CQuestManager::~CQuestManager(){Destroy();}
}

namespace quest {
// Whole original methods 081a5700 / 081a5660; append inside namespace quest.
LPCHARACTER CQuestManager::GetCurrentNPCCharacterPtr() {
    if(m_pCurrentCharacter) return m_pCurrentCharacter->GetQuestNPC();
    return 0;
}
unsigned int CQuestManager::GetCurrentNPCRace() {
    if(GetCurrentNPCCharacterPtr()) return GetCurrentNPCCharacterPtr()->GetRaceNum();
    return 0;
}

}

namespace quest {
void CQuestManager::RegisterNPCVnum(DWORD vnum)
{
    if (m_registeredNPCVnum.find(vnum) != m_registeredNPCVnum.end())
        return;

    m_registeredNPCVnum.insert(vnum);
    char path[256];
    for (std::set<std::string>::const_iterator objectDir = g_setQuestObjectDir.begin();
         objectDir != g_setQuestObjectDir.end(); ++objectDir) {
        ::snprintf(path, sizeof(path) - 1, "%s/%lu", objectDir->c_str(), vnum);
        sys_log(0, "%s", path);
        DIR* directory = opendir(path);
        if (!directory)
            continue;

        closedir(directory);
        ::snprintf(path, sizeof(path) - 1, "%lu", vnum);
        sys_log(0, "%s", path);
        m_mapNPC[vnum].Set(vnum, std::string(path));
    }
}

unsigned int CQuestManager::LoadTimerScript(const std::string& name)
{
    const std::map<std::string, unsigned int>::iterator found = m_mapTimerID.find(name);
    if (found != m_mapTimerID.end())
        return found->second;

    const unsigned int newId = UINT_MAX - static_cast<unsigned int>(m_mapTimerID.size());
    m_mapNPC[newId].Set(newId, name);
    m_mapTimerID.insert(std::make_pair(name, newId));
    return newId;
}
}
