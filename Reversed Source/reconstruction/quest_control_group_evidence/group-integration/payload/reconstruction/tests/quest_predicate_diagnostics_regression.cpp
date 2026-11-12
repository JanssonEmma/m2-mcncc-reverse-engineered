// Actual CQM start predicate and diagnostics over complete suppliedSDK/PC/CHAR/DESC graph.
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
#include "../../liblua/include/lauxlib.h"
}
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static quest::CQuestManager* manager;static lua_State *A,*B;static int mode,alerts,errors,metaCalls;static std::string lastAlert,loggedQuest,loggedState,loggedEvent;
static quest::PC *afterLookupPC,*afterErrorPC;static quest::QuestState* changedState;static CHARACTER* afterErrorCharacter;
static LPBUFFER outA,outB;static bool active;static std::vector<std::string> messages;static std::vector<int> destinations;
static unsigned Read2(const BYTE* p){return static_cast<unsigned>(p[0])|(static_cast<unsigned>(p[1])<<8);}
static void Prefix(lua_State* L,bool value){lua_settop(L,0);lua_pushnumber(L,111);lua_pushboolean(L,value?1:0);assert(lua_gettop(L)==2);}
static void Top(lua_State* L,bool value){assert(lua_gettop(L)==2&&lua_tonumber(L,1)==111&&lua_type(L,2)==LUA_TBOOLEAN&&(lua_toboolean(L,2)!=0)==value);}
static int Alert(lua_State* L){assert(lua_gettop(L)==1);const char* text=lua_tostring(L,1);assert(text);lastAlert=text;++alerts;if(mode==2){manager->L=B;mode=0;}return 0;}
static int Swap(lua_State* L){assert(lua_gettop(L)==0);manager->L=B;mode=0;lua_pushboolean(L,0);return 1;}
static int DiagnosticMeta(lua_State* L){
 assert(lua_gettop(L)==2&&lua_tonumber(L,2)==17);++metaCalls;assert(mode==11);
 manager->m_pCurrentPC=afterLookupPC;manager->m_CurrentRunningState=changedState;manager->m_iRunningEventIndex=9;manager->m_mapEventName.clear();manager->m_mapEventName["aa9"]=9;manager->m_mapEventName["zz9"]=9;
 lua_getglobal(L,"rooted_state_name");assert(lua_type(L,-1)==LUA_TSTRING);return 1;
}
extern "C" void quest_predicate_diagnostic_error(const char* quest,const char* state,const char* event){
 assert(quest&&state&&event);++errors;loggedQuest=quest;loggedState=state;loggedEvent=event;
 if(mode==11){manager->m_pCurrentPC=afterErrorPC;manager->m_pCurrentCharacter=afterErrorCharacter;manager->m_mapEventName.clear();manager->m_mapEventName["replaced"]=31;test_server=1;}
 else if(mode==12){manager->m_pCurrentPC=0;manager->m_pCurrentCharacter=0;test_server=1;}
}
static void* Copy(void* output,const void* input,size_t count){
 if(active&&(output==outA->write_point||output==outB->write_point)){
  const BYTE* data=static_cast<const BYTE*>(input);assert(count>=10&&data[0]==4&&Read2(data+1)==count&&data[3]==3&&data[count-1]==0);messages.push_back(std::string(reinterpret_cast<const char*>(data+9)));destinations.push_back(output==outA->write_point?0:1);return std::memcpy(output,input,count);
 }
 return std::memcpy(output,input,count);
}
static void Script(unsigned index,const std::string& source){assert(!source.empty());manager->m_hmQuestStartScript[index]=std::vector<char>(source.begin(),source.end());}
static void Predicate(unsigned index,const std::string& source,bool prefix,bool expected){manager->L=A;Prefix(A,prefix);Prefix(B,!prefix);alerts=0;lastAlert.clear();mode=0;Script(index,source);assert(manager->CanStartQuest(index)==expected);Top(A,prefix);assert(manager->L==A);}
static void ResetDiagnostic(quest::PC& owner,quest::QuestState& state,CHARACTER* character,bool server){manager->L=A;Prefix(A,false);owner.m_stCurQuest="q";manager->m_pCurrentPC=&owner;manager->m_CurrentRunningState=&state;manager->m_pCurrentCharacter=character;manager->m_iRunningEventIndex=7;manager->m_mapEventName.clear();manager->m_bError=true;mode=0;errors=metaCalls=0;loggedQuest.clear();loggedState.clear();loggedEvent.clear();messages.clear();destinations.clear();buffer_reset(outA);buffer_reset(outB);test_server=server?1:0;}
int main(){
 desc_test_allow_clock();assert(sizeof(int)==4&&sizeof(unsigned int)==4&&sizeof(lua_Number)==8);quest::CQuestManager qm;manager=&qm;
 // Missing hashkey permits start without touching evenNULL L or inserting a script.
 assert(!qm.L&&qm.CanStartQuest(0)&&qm.CanStartQuest(UINT_MAX)&&qm.m_hmQuestStartScript.empty());A=lua_open();B=lua_open();assert(A&&B);qm.L=A;lua_pushcfunction(A,Alert);lua_setglobal(A,"_ALERT");lua_pushcfunction(A,Swap);lua_setglobal(A,"swap");
 const char* sources[]={"return true","return false","return nil","return 0","return ''","return 1, false","return false, 1"};const bool values[]={true,false,false,true,true,false,true};
 for(unsigned p=0;p<2;++p)for(unsigned i=0;i<7;++i){Predicate(i,sources[i],p!=0,values[i]);assert(!alerts);}
 // Nonempty no-return chunks expose the old callerprefix top, not an invented defaultresult.
 Predicate(100,"local x=1",false,false);assert(!alerts);Predicate(100,"-- no results",true,true);assert(!alerts);
 // Actual aux_do/callalert consumes errors; app ignores status and tests remainingprefix truthiness.
 for(unsigned p=0;p<2;++p){Predicate(101,"return (",p!=0,p!=0);assert(alerts==1&&lastAlert.find("StartScript")!=std::string::npos);Predicate(101,"return missing.value",p!=0,p!=0);assert(alerts==1&&lastAlert.find("StartScript")!=std::string::npos);}
 std::string binary="return false";binary.push_back(0);binary+="return true";Predicate(UINT_MAX,binary,true,true);assert(alerts==1); // Fullbyte length; strlen-truncation would returnfalse.
 // Entire scriptbytes may change safely only after their load phase; these callbacks changeL only.
 Script(102,"return swap()");qm.L=A;Prefix(A,false);Prefix(B,false);lua_pushboolean(B,1);mode=1;assert(qm.CanStartQuest(102)&&qm.L==B&&mode==0);Top(B,false);assert(lua_gettop(A)==3&&lua_toboolean(A,-1)==0);Prefix(A,false);
 Script(103,"return missing.value");qm.L=A;Prefix(A,true);Prefix(B,true);lua_pushboolean(B,0);mode=2;alerts=0;assert(!qm.CanStartQuest(103)&&qm.L==B&&mode==0&&alerts==1);Top(A,true);Top(B,true);
 // Real Lua table for diagnostic reverse names; borrowednames remain rooted across externalcallbacks.
 qm.L=A;lua_pushstring(A,"borrowed-old");lua_setglobal(A,"rooted_state_name");lua_newtable(A);lua_pushnumber(A,0);lua_pushstring(A,"start");lua_settable(A,-3);lua_newtable(A);lua_pushstring(A,"__index");lua_pushcfunction(A,DiagnosticMeta);lua_settable(A,-3);lua_setmetatable(A,-2);lua_setglobal(A,"q");
 quest::PC owner,changedOwner,lastOwner;changedOwner.m_stCurQuest="after-lookup";lastOwner.m_stCurQuest="after-error";afterLookupPC=&changedOwner;afterErrorPC=&lastOwner;quest::QuestState state,nextState;state._clock_value=1;state._counter_value=2;state.st=0;nextState._clock_value=3;nextState._counter_value=4;nextState.st=99;changedState=&nextState;
 CHARACTER firstCharacter,secondCharacter;afterErrorCharacter=&secondCharacter;DescOutputProbe firstDesc,secondDesc;LPFDWATCH watch=fdwatch_new(16);outA=buffer_new(0,16384);outB=buffer_new(0,16384);firstDesc.Attach(watch,5,outA);firstDesc.SetPacketState(5);secondDesc.Attach(watch,6,outB);secondDesc.SetPacketState(5);firstCharacter.BindDesc(&firstDesc);secondCharacter.BindDesc(&secondDesc);thecore_memcpy=Copy;
 ResetDiagnostic(owner,state,&firstCharacter,false);qm.m_mapEventName["aaa"]=7;qm.m_mapEventName["zzz"]=7;active=true;qm.WriteRunningStateToSyserr();active=false;Top(A,false);assert(errors==1&&loggedQuest=="q"&&loggedState=="start"&&loggedEvent=="aaa"&&messages.empty()&&qm.m_bError);
 ResetDiagnostic(owner,state,&firstCharacter,true);active=true;qm.WriteRunningStateToSyserr();active=false;Top(A,false);assert(errors==1&&loggedEvent.empty()&&messages.size()==1&&messages[0]=="LUA_ERROR: quest q.start "&&destinations[0]==0&&qm.m_bError);
 ResetDiagnostic(owner,state,0,true);active=true;qm.WriteRunningStateToSyserr();active=false;assert(errors==1&&messages.empty()&&qm.m_bError);Top(A,false);
 // Oldlookup17 -> rootedoldname; lookupcallbackchangesPC/state/event then errorcallbackchangesPC/receiver/map.
 state.st=17;ResetDiagnostic(owner,state,&firstCharacter,false);mode=11;active=true;qm.WriteRunningStateToSyserr();active=false;Top(A,false);assert(metaCalls==1&&errors==1&&loggedQuest=="after-lookup"&&loggedState=="borrowed-old"&&loggedEvent=="aa9");assert(messages.size()==1&&messages[0]=="LUA_ERROR: quest after-error.borrowed-old aa9"&&destinations[0]==1&&qm.m_pCurrentPC==&lastOwner&&qm.m_CurrentRunningState==&nextState&&state.st==17&&nextState.st==99&&qm.m_bError);
 // Posterror nullreceiver skipsclient and avoids another currentPC lookup, even ifcallbacknullsPC.
 state.st=0;ResetDiagnostic(owner,state,&firstCharacter,true);mode=12;active=true;qm.WriteRunningStateToSyserr();active=false;assert(errors==1&&messages.empty()&&!qm.m_pCurrentPC&&!qm.m_pCurrentCharacter&&qm.m_bError);Top(A,false);
 active=false;thecore_memcpy=std::memcpy;firstCharacter.BindDesc(0);secondCharacter.BindDesc(0);firstDesc.Attach(0,-1,0);secondDesc.Attach(0,-1,0);buffer_delete(outA);buffer_delete(outB);fdwatch_delete(watch);qm.L=0;qm.m_pCurrentPC=0;qm.m_CurrentRunningState=0;qm.m_pCurrentCharacter=0;lua_close(B);lua_close(A);test_server=0;std::puts("whole CanStartQuest/WriteRunningStateToSyserr actual SDK/PC/network passed; emptyvector and invalidborrower domains excluded");return 0;
}
