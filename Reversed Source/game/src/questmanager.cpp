#include "target.h"
#include "desc_manager.h"
#include "item_manager.h"
#include "item.h"
#include "xmas_event.h"
#include "party.h"
#include <stdint.h>
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
#include "questevent.h"
#include "char_manager.h"
#include "../../libthecore/include/log.h"
#include <cassert>
#include "config.h"
#include <fstream>
#include <cstdlib>
#include "locale.hpp"
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

void CQuestManager::Reload()
{
    for (auto& entry : m_mapPC) {
        if (entry.second.IsRunning()) {
            sys_err("QUEST reload deferred while a quest is suspended for PID %u", entry.first);
            return;
        }
    }
    Destroy();
    m_mapNPC.clear();
    m_mapNPCNameID.clear();
    m_hmQuestName.clear();
    m_mapQuestNameByIndex.clear();
    m_mapTimerID.clear();
    m_hmQuestStartScript.clear();
    m_mapEventName.clear();
    if (!Initialize()) {
        sys_err("QUEST reload failed");
        return;
    }
    for (const DWORD vnum : m_registeredNPCVnum)
        m_mapNPC[vnum].Set(vnum, std::to_string(vnum));
}

bool CQuestManager::Initialize()
{
    if (g_bAuthServer)
        return true;
    if (!InitializeLua())
        return false;

    m_pSelectedDungeon = nullptr;
    const char* events[] = {
        "click", "kill", "timer", "levelup", "login", "logout", "button", "info",
        "chat", "in", "out", "use", "server_timer", "enter", "leave", "letter",
        "take", "target", "party_kill", "unmount"
    };
    for (unsigned index = 0; index < sizeof(events) / sizeof(events[0]); ++index)
        m_mapEventName.emplace(events[index], index);

    m_bNoSend = false;
    m_iCurrentSkin = 1;

    std::ifstream input(g_stQuestDir + "/questnpc.txt");
    if (!input.is_open())
        sys_err("QUEST Cannot open 'questnpc.txt'");
    else
        sys_log(0, "QUEST can open 'questnpc.txt' (%s)", g_stQuestDir.c_str());

    unsigned vnum;
    int line = 0;
    while (input >> vnum) {
        ++line;
        std::string name;
        std::getline(input, name);
        const auto first = name.find_first_not_of(" \t\r\n\f\v");
        if (first == std::string::npos) {
            sys_err("QUEST questnpc.txt:%d:npc name error", line);
            continue;
        }
        const auto last = name.find_last_not_of(" \t\r\n\f\v");
        name = name.substr(first, last - first + 1);
        if (std::atoi(name.c_str()))
            continue;

        if (test_server)
            sys_log(0, "QUEST reading script of %s(%u)", name.c_str(), vnum);
        m_mapNPC[vnum].Set(vnum, name);
        m_mapNPCNameID[name] = vnum;
    }
    m_mapNPC[0].Set(0, "notarget");

    SetEventFlag("guild_withdraw_delay", g_iUseLocale ? 1 : 3);
    SetEventFlag("guild_disband_delay", g_iUseLocale ? 1 : 7);
    return true;
}
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
// 081a4d00
bool CQuestManager::CanEndQuestAtState(const std::string&, const std::string&)
{
    return false;
}

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
// 081a5240 rounds with rint, then converts with a signed 32-bit FISTP.
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
    const lua_Number rounded = rint(lua_tonumber(L, -1));
    const int v = rounded >= INT_MIN && rounded <= INT_MAX
        ? static_cast<int>(rounded) : INT_MIN;
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
CItem* CQuestManager::GetCurrentItem()
{
    return m_pCurrentCharacter ? m_pCurrentCharacter->GetQuestItemPtr() : nullptr;
}

void CQuestManager::SetCurrentItem(CItem* item)
{
    if (m_pCurrentCharacter)
        m_pCurrentCharacter->SetQuestItemPtr(item);
}

void CQuestManager::ClearCurrentItem()
{
    if (m_pCurrentCharacter)
        m_pCurrentCharacter->ClearQuestItemPtr();
}

CDungeon* CQuestManager::GetCurrentDungeon()
{
    return m_pCurrentCharacter ? m_pCurrentCharacter->GetDungeonForce() : m_pSelectedDungeon;
}

void CQuestManager::SelectDungeon(CDungeon* dungeon)
{
    m_pSelectedDungeon = dungeon;
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

namespace quest {
DWORD CQuestManager::GetServerTimerArg() { return m_dwServerTimerArg; }
}

namespace quest {
// 081a3310
void CQuestManager::LevelUp(unsigned int pc) {
    PC* pPC=GetPC(pc);
    if (!pPC) {
        _sys_err("LevelUp",500,"QUEST LEVELUP_EVENT no such pc id : %d",pc);
        return;
    }
    if (!pPC->IsLoaded()) return;
    m_mapNPC[0].OnLevelUp(*pPC);
}
//081a4540, old signature unsigned int rather than DWORD.
void CQuestManager::Unmount(unsigned int pid) {
    PC* pc=GetPC(pid);
    if(!pc) {
        _sys_err("Unmount",0x308,"QUEST no such pc id : %d",pid);
        return;
    }
    if(!pc->IsLoaded()) return;
    m_mapNPC[0].OnUnmount(*pc);
}
// 081a72e0. Both ExecuteQuestScript overloads are static (DWARF).
bool CQuestManager::ExecuteQuestScript(PC& pc,DWORD index,int state,
    const char* code,int size,std::vector<AArgScript*>* chat,bool cache) {
    return ExecuteQuestScript(pc,instance().GetQuestNameByIndex(index),state,
        code,size,chat,cache);
}
// 081a7380
bool CQuestManager::ExecuteQuestScript(PC& pc,const std::string& name,int state,
    const char* code,int size,std::vector<AArgScript*>* chat,bool cache) {
    QuestState qs=instance().OpenState(name,state);
    if (chat) qs.chat_scripts.swap(*chat);
    if (cache) {
        lua_pushstring(qs.co,"__codecache");
        lua_gettable(qs.co,LUA_GLOBALSINDEX);
        // The 32-bit target uses the script address as a numeric cache key.
        lua_pushnumber(qs.co,static_cast<lua_Number>(reinterpret_cast<uintptr_t>(code)));
        lua_rawget(qs.co,-2);
        if (lua_type(qs.co,-1)==LUA_TNIL) {
            lua_pop(qs.co,1);
            luaL_loadbuffer(qs.co,code,size,name.c_str());
            lua_pushnumber(qs.co,static_cast<lua_Number>(reinterpret_cast<uintptr_t>(code)));
            lua_pushvalue(qs.co,-2);
            lua_rawset(qs.co,-4);
        }
        lua_remove(qs.co,-2);
    } else luaL_loadbuffer(qs.co,code,size,name.c_str());
    pc.SetQuest(name,qs);
    QuestState& running=*pc.GetRunningQuestState();
    if (instance().RunState(running)) return true;
    instance().CloseState(running);
    pc.EndRunning();
    return false;
}
}

namespace quest {
// 081a1ff0. GetPC is dereferenced directly in the original.
void CQuestManager::Confirm(unsigned int pid, EQuestConfirmType answer, unsigned int replyingPid)
{
    PC* pc = GetPC(pid);
    QuestState* state = pc->GetRunningQuestState();
    if (!state) {
        _sys_err("Confirm", 0xae, "no quest running for pc, cannot process input : %lu",
                 static_cast<DWORD>(pid));
        return;
    }
    if (state->suspend_state != 4 || (replyingPid && !pc->IsConfirmWait(replyingPid))) {
        const int line = state->suspend_state == 4 ? 0xba : 0xb4;
        _sys_err("Confirm", line, "not wait for a confirm : %lu %d",
                 static_cast<DWORD>(pid), static_cast<int>(state->suspend_state));
        return;
    }
    state->args = 1;
    pc->ClearConfirmWait();
    pc->SetSendDoneFlag();
    lua_pushnumber(state->co, static_cast<lua_Number>(answer));
    AddScript("[END_CONFIRM_WAIT]");
    SetSkinStyle(0);
    SendScript();
    if (!RunState(*pc->GetRunningQuestState())) {
        CloseState(*pc->GetRunningQuestState());
        pc->EndRunning();
    }
}

// 081a2490, 081a25f0 and 081a2750
void CQuestManager::EnterState(DWORD pid, DWORD index, int state)
{
    PC* pc = GetPC(pid);
    if (!pc)
        _sys_err("EnterState", 0x146, "QUEST no such pc id : %d", static_cast<int>(pid));
    else if (pc->IsLoaded())
        m_mapNPC[0].OnEnterState(*pc, index, state);
}

void CQuestManager::LeaveState(DWORD pid, DWORD index, int state)
{
    PC* pc = GetPC(pid);
    if (!pc)
        _sys_err("LeaveState", 0x154, "QUEST no such pc id : %d", static_cast<int>(pid));
    else if (pc->IsLoaded())
        m_mapNPC[0].OnLeaveState(*pc, index, state);
}

void CQuestManager::Letter(DWORD pid, DWORD index, int state)
{
    PC* pc = GetPC(pid);
    if (!pc)
        _sys_err("Letter", 0x162, "QUEST no such pc id : %d", static_cast<int>(pid));
    else if (pc->IsLoaded())
        m_mapNPC[0].OnLetter(*pc, index, state);
}
}

namespace quest {
// 081a2190
void CQuestManager::Input(unsigned int pid, const char* message)
{
    PC* pc = GetPC(pid);
    if (!pc) {
        _sys_err("Input", 0xd6, "no pc! : %lu", static_cast<DWORD>(pid));
        return;
    }
    QuestState* state = pc->GetRunningQuestState();
    if (!state) {
        _sys_err("Input", 0xdc, "no quest running for pc, cannot process input : %lu",
                 static_cast<DWORD>(pid));
        return;
    }
    if (state->suspend_state != 3) {
        _sys_err("Input", 0xe2, "not wait for a input : %lu %d",
                 static_cast<DWORD>(pid), static_cast<int>(state->suspend_state));
        return;
    }
    state->args = 1;
    pc->SetSendDoneFlag();
    lua_pushstring(state->co, message);
    if (!RunState(*pc->GetRunningQuestState())) {
        CloseState(*pc->GetRunningQuestState());
        pc->EndRunning();
    }
}

// 081a22a0
void CQuestManager::Select(unsigned int pid, unsigned int selection)
{
    PC* pc = GetPC(pid);
    QuestState* state = pc ? pc->GetRunningQuestState() : 0;
    if (!state || state->suspend_state != 2) {
        _sys_err("Select", 0x11d, "wrong QUEST_SELECT request! : %d", static_cast<int>(pid));
        return;
    }
    pc->SetSendDoneFlag();
    if (state->chat_scripts.empty()) {
        state->args = 1;
        lua_pushnumber(state->co, static_cast<lua_Number>(selection + 1u));
        if (RunState(*pc->GetRunningQuestState()))
            return;
        CloseState(*pc->GetRunningQuestState());
    } else {
        CloseState(*state);
        const std::vector<AArgScript*>& scripts = pc->GetRunningQuestState()->chat_scripts;
        if (selection < scripts.size()) {
            AArgScript* script = scripts[selection];
            ExecuteQuestScript(*pc, script->quest_index, script->state_index,
                               script->script.GetCode(), script->script.GetSize());
            return;
        }
        pc->SetSendDoneFlag();
        GotoEndState(*state);
    }
    pc->EndRunning();
}

bool CQuestManager::TakeItem(unsigned int pid, unsigned int npc, CItem* item)
{
    PC* player = GetPC(pid);
    if (!player) {
        _sys_err("TakeItem", 0x278, "QUEST USE_ITEM_EVENT no such pc id : %d", pid);
        return false;
    }
    if (!player->IsLoaded()) {
        if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid))
            character->ChatPacket(1, "%s", locale_find("\xc4\xf9\xbd\xba\xc6\xae\xb8\xa6 \xb7\xce\xb5\xe5\xc7\xcf\xb4\xc2 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xb8\xb8 \xb1\xe2\xb4\xd9\xb7\xc1 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.", g_iUseLocale));
        return false;
    }
    SetCurrentItem(item);
    return m_mapNPC[npc].OnTakeItem(*player);
}

bool CQuestManager::UseItem(unsigned int pid, CItem* item, bool receiveAll)
{
    PC* player = GetPC(pid);
    if (!player) {
        _sys_err("UseItem", 0x292, "QUEST USE_ITEM_EVENT no such pc id : %d", pid);
        return false;
    }
    if (!player->IsLoaded()) {
        if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid))
            character->ChatPacket(1, "%s", locale_find("\304\371\275\272\306\256\270\246\040\267\316\265\345\307\317\264\302\040\301\337\300\324\264\317\264\331\056\040\300\341\275\303\270\270\040\261\342\264\331\267\301\040\301\326\275\312\275\303\277\300\056", g_iUseLocale));
        return false;
    }
    SetCurrentItem(item);
    return m_mapNPC[item->GetVnum()].OnUseItem(*player, receiveAll);
}

bool CQuestManager::Click(unsigned int pid, CHARACTER* target)
{
    PC* player = GetPC(pid);
    if (!player) {
        _sys_err("Click", 0x2f6, "QUEST CLICK_EVENT no such pc id : %d", pid);
        return false;
    }
    if (!player->IsLoaded()) {
        if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid))
            character->ChatPacket(1, "%s", locale_find("\304\371\275\272\306\256\270\246\040\267\316\265\345\307\317\264\302\040\301\337\300\324\264\317\264\331\056\040\300\341\275\303\270\270\040\261\342\264\331\267\301\040\301\326\275\312\275\303\277\300\056", g_iUseLocale));
        return false;
    }

    TargetInfo* info = CTargetManager::instance().GetTargetInfo(pid, TARGET_TYPE_VID, target->GetVID());
    if (test_server)
        sys_log(0, "CQuestManager::Click(pid=%d, npc_name=%s) - target_info(%p)",
                pid, target->GetName(), static_cast<void*>(info));
    if (info) {
        bool result = false;
        if (m_mapNPC[0].OnTarget(*player, info->dwQuestIndex, info->szTargetName, "click", result))
            return result;
    }

    const unsigned int race = target->GetRaceNum();
    if (target->GetCharType() == CHAR_TYPE_PC)
        return false;

    auto found = m_mapNPC.find(race);
    if (found == m_mapNPC.end()) {
        _sys_err("Click", 0x2d8,
                 "CQuestManager::Click(pid=%d, target_npc_name=%s) - NOT EXIST NPC RACE VNUM[%d]",
                 pid, target->GetName(), race);
        return false;
    }
    NPC& npc = found->second;
    if (npc.HasChat()) {
        if (test_server)
            sys_log(0, "CQuestManager::Click->OnChat");
        if (npc.OnChat(*player))
            return true;
        if (test_server)
            sys_log(0, "CQuestManager::Click->OnChat Failed");
    }
    return npc.OnClick(*player);
}

// 081a1f50
void CQuestManager::QuestButton(unsigned int pid, unsigned int index)
{
    PC* player = GetPC(pid);
    if (!player) {
        _sys_err("QuestButton", 0x25d, "QUEST CLICK_EVENT no such pc id : %u", pid);
    } else if (player->IsLoaded()) {
        m_mapNPC[0].OnButton(*player, index);
    } else if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid)) {
        character->ChatPacket(1, "%s", locale_find("\304\371\275\272\306\256\270\246\040\267\316\265\345\307\317\264\302\040\301\337\300\324\264\317\264\331\056\040\300\341\275\303\270\270\040\261\342\264\331\267\301\040\301\326\275\312\275\303\277\300\056", g_iUseLocale));
    }
}

void CQuestManager::QuestInfo(unsigned int pid, unsigned int index)
{
    PC* player = GetPC(pid);
    if (!player) {
        _sys_err("QuestInfo", 0x245, "QUEST INFO_EVENT no such pc id : %u", pid);
    } else if (player->IsLoaded()) {
        m_mapNPC[0].OnInfo(*player, index);
    } else if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid)) {
        character->ChatPacket(1, "%s", locale_find("\304\371\275\272\306\256\270\246\040\267\316\265\345\307\317\264\302\040\301\337\300\324\264\317\264\331\056\040\300\341\275\303\270\270\040\261\342\264\331\267\301\040\301\326\275\312\275\303\277\300\056", g_iUseLocale));
    }
}

void CQuestManager::SelectItem(unsigned int pid, unsigned int selection)
{
    PC* pc = GetPC(pid);
    QuestState* state = pc ? pc->GetRunningQuestState() : 0;
    if (!state || state->suspend_state != 5)
        return;
    pc->SetSendDoneFlag();
    state->args = 1;
    lua_pushnumber(state->co, static_cast<lua_Number>(selection));
    if (!RunState(*pc->GetRunningQuestState())) {
        CloseState(*pc->GetRunningQuestState());
        pc->EndRunning();
    }
}

// 081a23f0
void CQuestManager::Resume(unsigned int pid)
{
    PC* pc = GetPC(pid);
    QuestState* state = pc ? pc->GetRunningQuestState() : 0;
    if (!state || state->suspend_state != 1) {
        _sys_err("Resume", 0x137, "wrong QUEST_WAIT request! : %d", static_cast<int>(pid));
        return;
    }
    pc->SetSendDoneFlag();
    state->args = 0;
    if (!RunState(*state)) {
        CloseState(*pc->GetRunningQuestState());
        pc->EndRunning();
    }
}
}

namespace quest {
// 081a2940, 081a2a90
void CQuestManager::Login(unsigned int pid, const char* quest)
{
    PC* pc = GetPC(pid);
    if (!pc)
        _sys_err("Login", 0x18b, "QUEST no such pc id : %d", static_cast<int>(pid));
    else if (pc->IsLoaded())
        m_mapNPC[0].OnLogin(*pc, quest);
}

void CQuestManager::Logout(unsigned int pid)
{
    PC* pc = GetPC(pid);
    if (!pc)
        _sys_err("Logout", 0x19b, "QUEST no such pc id : %d", static_cast<int>(pid));
    else if (pc->IsLoaded())
        m_mapNPC[0].OnLogout(*pc);
}

// 081a31d0
bool CQuestManager::Timer(unsigned int pid, unsigned int npc)
{
    PC* pc = GetPC(pid);
    if (!pc) {
        _sys_err("Timer", 0x1e1, "QUEST TIMER_EVENT no such pc id : %d", static_cast<int>(pid));
        return false;
    }
    return pc->IsLoaded() && m_mapNPC[npc].OnTimer(*pc);
}

// 081a80d0
void CQuestManager::SetServerTimerArg(DWORD arg)
{
    m_dwServerTimerArg = arg;
}

// 081a3090
void CQuestManager::AddServerTimer(const std::string& name, DWORD arg, LPEVENT event)
{
    sys_log(0, "XXX AddServerTimer %s %d %p", name.c_str(), static_cast<int>(arg), static_cast<void*>(event));
    if (!m_mapServerTimer.emplace(std::make_pair(name, arg), event).second) {
        _sys_err("AddServerTimer", 0x5bc, "already registered server timer name:%s arg:%lu", name.c_str(), arg);
        CancelServerTimerEvent(&event);
    }
}

void CQuestManager::ClearServerTimer(const std::string& name, DWORD arg)
{
    const auto found = m_mapServerTimer.find(std::make_pair(name, arg));
    if (found == m_mapServerTimer.end())
        return;

    CancelServerTimerEvent(&found->second);
    m_mapServerTimer.erase(found);
}

void CQuestManager::ClearServerTimerNotCancel(const std::string& name, DWORD arg)
{
    m_mapServerTimer.erase(std::make_pair(name, arg));
}

bool CQuestManager::ServerTimer(unsigned int npc, unsigned int arg)
{
    SetServerTimerArg(arg);
    sys_log(0, "XXX ServerTimer Call NPC %p", static_cast<void*>(GetPCForce(0)));
    m_pCurrentPC = GetPCForce(0);
    m_pCurrentCharacter = 0;
    m_pSelectedDungeon = 0;
    return m_mapNPC[npc].OnServerTimer(*m_pCurrentPC);
}
}

namespace quest {
void CQuestManager::AttrIn(unsigned int pid, CHARACTER* ch, int attr)
{
    PC* pc = GetPC(pid);
    if (!pc) {
        _sys_err("AttrIn", 519, "QUEST no such pc id : %d", pid);
        return;
    }
    const bool loaded = pc->IsLoaded();
    m_pCurrentPartyMember = ch;
    if (loaded)
        m_mapNPC[static_cast<unsigned int>(attr) + 100000u].OnAttrIn(*pc);
}

void CQuestManager::AttrOut(unsigned int pid, CHARACTER* ch, int attr)
{
    PC* pc = GetPC(pid);
    if (!pc) {
        _sys_err("AttrOut", 539, "QUEST no such pc id : %d", pid);
        return;
    }
    const bool loaded = pc->IsLoaded();
    m_pCurrentPartyMember = ch;
    if (loaded)
        m_mapNPC[static_cast<unsigned int>(attr) + 100000u].OnAttrOut(*pc);
}
}

namespace quest {
bool CQuestManager::GiveItemToPC(unsigned int pc, CHARACTER* victim)
{
    if (!victim->GetDesc()) return false;
    PC* player=GetPC(pc);
    if (!player || !player->IsLoaded()) return false;
    TargetInfo* target=CTargetManager::instance().GetTargetInfo(pc,TARGET_TYPE_VID,victim->GetVID());
    if (!target) return false;
    bool result;
    return m_mapNPC[0].OnTarget(*player,target->dwQuestIndex,target->szTargetName,"click",result);
}
}

namespace quest {
bool CQuestManager::Target(unsigned int pc, DWORD index, const char* name, const char* verb)
{
    PC* player=GetPC(pc);
    if (!player || !player->IsLoaded()) return false;
    bool result;
    return m_mapNPC[0].OnTarget(*player,index,name,verb,result);
}
}

namespace quest {
void CQuestManager::LogoutPC(CHARACTER* character)
{
    PC* player=GetPC(character->GetPlayerID());
    if (player && player->GetRunningQuestState()) {
        CloseState(*player->GetRunningQuestState());
        player->CancelRunning();
    }
    Logout(character->GetPlayerID());
    if (m_pCurrentPartyMember == character)
        m_pCurrentPartyMember = nullptr;
    if (character==m_pCurrentCharacter) {
        m_pCurrentCharacter=0;
        m_pCurrentPC=0;
    }
}

void CQuestManager::DisconnectPC(CHARACTER* character)
{
    m_mapPC.erase(character->GetPlayerID());
}
}

namespace quest {
void CQuestManager::SetEventFlag(const std::string& name, int value)
{
    int& flag = m_mapEventFlag[name];
    const int previous = flag;
    sys_log(0, "QUEST eventflag %s %d prev_value %d", name.c_str(), value, previous);
    flag = value;

    typedef int CHARACTER_MANAGER::*Rate;
    static const struct {
        const char* name;
        Rate field;
    } rates[] = {
        { "mob_item", &CHARACTER_MANAGER::m_iMobItemRate },
        { "mob_dam", &CHARACTER_MANAGER::m_iMobDamageRate },
        { "mob_gold", &CHARACTER_MANAGER::m_iMobGoldAmountRate },
        { "mob_gold_pct", &CHARACTER_MANAGER::m_iMobGoldDropRate },
        { "user_dam", &CHARACTER_MANAGER::m_iUserDamageRate },
        { "user_dam_buyer", &CHARACTER_MANAGER::m_iUserDamageRatePremium },
        { "mob_exp", &CHARACTER_MANAGER::m_iMobExpRate },
        { "mob_item_buyer", &CHARACTER_MANAGER::m_iMobItemRatePremium },
        { "mob_exp_buyer", &CHARACTER_MANAGER::m_iMobExpRatePremium },
        { "mob_gold_buyer", &CHARACTER_MANAGER::m_iMobGoldAmountRatePremium },
        { "mob_gold_pct_buyer", &CHARACTER_MANAGER::m_iMobGoldDropRatePremium },
    };
    for (const auto& rate : rates) {
        if (name == rate.name) {
            CHARACTER_MANAGER::instance().*rate.field = value;
            return;
        }
    }

    if (name == "crcdisconnect") {
        DESC_MANAGER::instance().SetDisconnectInvalidCRCMode(value != 0);
    } else if (name == "test") {
        test_server = value;
    } else if (name.compare(0, 5, "xmas_") == 0) {
        xmas::ProcessEventFlag(name, previous, value);
    } else if (name == "newyear_boom" || name == "day") {
        for (DESC* desc : DESC_MANAGER::instance().GetClientSet()) {
            if (CHARACTER* character = desc->GetCharacter()) {
                if (name == "newyear_boom")
                    character->ChatPacket(5, "newyear_boom %d", value);
                else
                    character->ChatPacket(5, value ? "DayMode dark" : "DayMode light");
            }
        }
        if (name == "day" && (previous == 0) != (value == 0))
            xmas::UpdateFireworkSellers(value != 0, "SetEventFlag", 0x4d8);
    } else if (name.compare(0, 15, "drop_char_stone") == 0) {
        DropEvent_CharStone_SetValue(name, value);
    }
}
}

namespace quest {
void CQuestManager::Kill(unsigned int pid, unsigned int npc)
{
    PC* pc = GetPC(pid);
    if (!pc) {
        _sys_err("Kill", 0x1c4, "QUEST: no such pc id : %d", static_cast<int>(pid));
        return;
    }
    if (!pc->IsLoaded() || m_mapNPC[npc].OnKill(*pc))
        return;

    CHARACTER* killer = m_pCurrentCharacter;
    CParty* party = killer->GetParty();
    CHARACTER* leader = party ? party->GetLeaderCharacter() : killer;
    if (leader) {
        m_pCurrentPartyMember = killer;
        PC* leaderPC = GetPC(leader->GetPlayerID());
        if (leaderPC && m_mapNPC[npc].OnPartyKill(*leaderPC))
            return;
        pc = GetPC(pid);
    }
    if (pc && !m_mapNPC[0].OnKill(*pc) && leader) {
        m_pCurrentPartyMember = killer;
        if (PC* leaderPC = GetPC(leader->GetPlayerID()))
            m_mapNPC[0].OnPartyKill(*leaderPC);
    }
}
}
