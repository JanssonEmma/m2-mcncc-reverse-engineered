// Full actual PC/CQM consumers over actual suppliedLuaSDK + ChatPacket/DESC/event graph.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#include "../../game/src/char.h"
#undef private
#undef protected
#include "../../game/src/config.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
extern "C" {
#include "../../liblua/include/lua.h"
}
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static quest::PC* pc;static int mode,metaCalls;static bool active;static LPBUFFER outA,outB;static CHARACTER* receiver;static DESC* alternate;
static std::vector<std::string> messages;static std::vector<int> destinations;
static unsigned Read2(const BYTE* b){return static_cast<unsigned>(b[0])|(static_cast<unsigned>(b[1])<<8);}
static int Meta(lua_State* L){
 ++metaCalls;assert(lua_gettop(L)==2);
 if(mode==1){assert(!std::strcmp(lua_tostring(L,2),"move-current"));pc->m_stCurQuest="changed";lua_pushnumber(L,26);return 1;}
 if(mode==2){assert(!std::strcmp(lua_tostring(L,2),"move-name"));pc->m_stCurQuest="other";lua_pushnumber(L,-2);return 1;}
 if(mode==3){assert(lua_tonumber(L,2)==17);pc->m_FlagMap["q.__status"]=99;lua_pushstring(L,"old-name");return 1;}
 lua_pushnil(L);return 1;
}
static void Number(lua_State* L,const char* key,int value){lua_pushstring(L,key);lua_pushnumber(L,value);lua_settable(L,-3);}
static void Name(lua_State* L,int index,const char* value){lua_pushnumber(L,index);lua_pushstring(L,value);lua_settable(L,-3);}
static void Prefix(lua_State* L){lua_settop(L,0);lua_pushnumber(L,111);lua_pushstring(L,"sentinel");}
static void Top(lua_State* L){assert(lua_gettop(L)==2&&lua_tonumber(L,1)==111&&!std::strcmp(lua_tostring(L,2),"sentinel"));}
static void Reset(quest::PC& owner,lua_State* L){pc=&owner;owner.m_FlagMap.clear();owner.m_FlagSaveMap.clear();owner.m_QuestStateChange.clear();owner.m_stCurQuest="q";mode=metaCalls=0;test_server=0;Prefix(L);}
static void* Copy(void* output,const void* input,size_t count){
 if(active&&(output==outA->write_point||output==outB->write_point)){
  const BYTE* b=static_cast<const BYTE*>(input);assert(count>=10&&b[0]==4&&Read2(b+1)==count&&b[3]==1&&b[count-1]==0);messages.push_back(std::string(reinterpret_cast<const char*>(b+9)));destinations.push_back(output==outA->write_point?0:1);void* answer=std::memcpy(output,input,count);
  if(mode==4&&messages.size()==1){pc->m_FlagMap["a"]=7;pc->m_FlagMap["aa"]=3;pc->m_FlagMap["b"]=-8;pc->m_FlagMap["0before"]=4;receiver->BindDesc(alternate);}
  return answer;
 }
 return std::memcpy(output,input,count);
}
static void Begin(CHARACTER& ch,DESC& first){active=false;messages.clear();destinations.clear();buffer_reset(outA);buffer_reset(outB);ch.BindDesc(&first);receiver=&ch;}
static void Message(unsigned i,const char* text,int dest=0){assert(i<messages.size()&&messages[i]==text&&destinations[i]==dest);}
static void Change(const quest::PC& owner,unsigned i,DWORD index,int previous,int next){assert(i<owner.m_QuestStateChange.size());const quest::PC::TQuestStateChangeInfo& row=owner.m_QuestStateChange[i];assert(row.quest_idx==index&&row.prev_state==previous&&row.next_state==next);}
int main(){
 desc_test_allow_clock();assert(sizeof(int)==4&&sizeof(DWORD)==4&&sizeof(lua_Number)==8);quest::CQuestManager manager;quest::PC owner;pc=&owner;lua_State* L=lua_open();assert(L);manager.L=L;
 lua_newtable(L);Number(L,"next",23);Number(L,"same",5);Number(L,"zero",0);Name(L,23,"next-name");Name(L,-17,"negative-name");lua_newtable(L);lua_pushstring(L,"__index");lua_pushcfunction(L,Meta);lua_settable(L,-3);lua_setmetatable(L,-2);lua_setglobal(L,"q");manager.m_hmQuestName["q"]=7;manager.m_hmQuestName["other"]=9;
 quest::QuestState running;running._clock_value=17;running._counter_value=-18;running.st=41;owner.m_RunningQuestState=&running;owner.m_iLastState=-83;
 Reset(owner,L);owner.SetCurrentQuestStateName(std::string("next"));Top(L);assert(owner.m_FlagMap["q.__status"]==23&&owner.m_FlagSaveMap["q.__status"]==23&&running.st==41&&owner.m_RunningQuestState==&running&&owner.m_QuestStateChange.empty());
 owner.SetCurrentQuestStateName(std::string("zero"));Top(L);assert(owner.m_FlagMap.find("q.__status")==owner.m_FlagMap.end()&&owner.m_FlagSaveMap["q.__status"]==0&&running.st==41);
 Reset(owner,L);mode=1;owner.SetCurrentQuestStateName(std::string("move-current"));Top(L);assert(metaCalls==1&&owner.m_stCurQuest=="changed"&&owner.m_FlagMap["changed.__status"]==26&&owner.m_FlagSaveMap["changed.__status"]==26&&owner.m_FlagMap.find("q.__status")==owner.m_FlagMap.end()&&running.st==41);
 // Query followed by actual integer queue overload, no direct state/flag assignment; same-state no-op.
 Reset(owner,L);owner.m_FlagMap["q.__status"]=5;owner.SetQuestState(std::string("q"),std::string("next"));Top(L);assert(owner.m_QuestStateChange.size()==1&&owner.m_FlagMap["q.__status"]==5&&owner.m_FlagSaveMap.empty()&&running.st==41);Change(owner,0,7,5,23);
 owner.SetQuestState(std::string("q"),std::string("same"));Top(L);assert(owner.m_QuestStateChange.size()==1);owner.SetQuestState(std::string("q"),std::string("absent"));Top(L);assert(owner.m_QuestStateChange.size()==2);Change(owner,1,7,5,0);
 Reset(owner,L);owner.m_FlagMap["other.__status"]=3;mode=2;owner.SetQuestState(owner.m_stCurQuest,std::string("move-name"));Top(L);assert(metaCalls==1&&owner.m_stCurQuest=="other"&&owner.m_QuestStateChange.size()==1&&owner.m_FlagMap["other.__status"]==3);Change(owner,0,9,3,-2);
 CHARACTER ch;DescOutputProbe first,second;LPFDWATCH watch=fdwatch_new(16);outA=buffer_new(0,16384);outB=buffer_new(0,16384);first.Attach(watch,5,outA);first.SetPacketState(5);second.Attach(watch,6,outB);second.SetPacketState(5);alternate=&second;thecore_memcpy=Copy;
 Reset(owner,L);Begin(ch,first);owner.SendFlagList(0);assert(messages.empty()); // empty map never dereferences nullreceiver.
 owner.m_FlagMap[".__status"]=4;owner.m_FlagMap["a"]=INT_MIN;owner.m_FlagMap["q.__status"]=-17;owner.m_FlagMap[std::string("x\0y",3)]=INT_MAX;active=true;owner.SendFlagList(&ch);active=false;Top(L);assert(messages.size()==4);Message(0,".__status 4");Message(1,"a -2147483648");Message(2,"q negative-name (-17)");Message(3,"x 2147483647");
 // Reverse lookup receives old17 but numericdisplay rereads99 after actualLua __index callback.
 Reset(owner,L);Begin(ch,first);owner.m_FlagMap["q.__status"]=17;mode=3;active=true;owner.SendFlagList(&ch);active=false;Top(L);assert(metaCalls==1&&messages.size()==1&&owner.m_FlagMap["q.__status"]==99);Message(0,"q old-name (99)");
 Reset(owner,L);Begin(ch,first);owner.m_FlagMap["a"]=1;owner.m_FlagMap["b"]=2;owner.m_FlagMap["q.__status"]=23;mode=4;active=true;owner.SendFlagList(&ch);active=false;Top(L);assert(messages.size()==4);Message(0,"a 1");Message(1,"aa 3",1);Message(2,"b -8",1);Message(3,"q next-name (23)",1);assert(owner.m_FlagMap["0before"]==4);
 active=false;thecore_memcpy=std::memcpy;ch.BindDesc(0);first.Attach(0,-1,0);second.Attach(0,-1,0);buffer_delete(outA);buffer_delete(outB);fdwatch_delete(watch);owner.m_RunningQuestState=0;manager.L=0;lua_close(L);test_server=0;std::puts("whole PC state consumers actual Lua/PC/network graph passed; borrowed-string invalidation/defaultstate copy unsafe domains excluded");return 0;
}
