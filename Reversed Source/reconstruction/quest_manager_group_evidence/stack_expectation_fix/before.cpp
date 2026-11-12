// Whole original CQM state queries over actual supplied Lua5.0.3 C SDK; test owns real VM lifetime.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include "../../game/src/config.h"
extern "C" {
#include "../../liblua/include/lua.h"
}
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <fenv.h>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static quest::CQuestManager* manager;
static lua_State *primary,*alternate;
static int errorLine,errorCount,debugCount,debugValue,debugTop,mode,metaCalls;
static std::string errorQuest,errorState,debugQuest,debugState;
static int errorIndex;
extern "C" void quest_lua_error(int line,const char* quest,const char* state,int index){
 assert(line==939||line==957);++errorCount;errorLine=line;errorQuest=quest;errorState=state?state:"";errorIndex=index;
 // Diagnostic occurs with failed global value still on originalstack; following settop reloads ownerL.
 assert(lua_gettop(manager->L)==3);
 if(mode==1){manager->L=alternate;mode=0;}
}
extern "C" void quest_lua_debug(int value,int top,const char* quest,const char* state){
 ++debugCount;debugValue=value;debugTop=top;debugQuest=quest;debugState=state;assert(lua_gettop(manager->L)==top);
 if(mode==2){manager->L=alternate;mode=0;}
}
static void Prefix(lua_State* L,int count=2){lua_settop(L,0);lua_pushnumber(L,111);lua_pushstring(L,"sentinel");if(count==3)lua_pushnumber(L,222);assert(lua_gettop(L)==count);}
static void Top(lua_State* L){assert(lua_gettop(L)==2&&lua_tonumber(L,1)==111&&!std::strcmp(lua_tostring(L,2),"sentinel"));}
static void Reset(){manager->L=primary;Prefix(primary);Prefix(alternate);mode=0;errorLine=errorCount=debugCount=0;test_server=0;}
static void Number(lua_State* L,const char* key,double value){lua_pushstring(L,key);lua_pushnumber(L,value);lua_settable(L,-3);}
static void String(lua_State* L,int key,const char* value){lua_pushnumber(L,key);lua_pushstring(L,value);lua_settable(L,-3);}
static int IndexMetamethod(lua_State* L){
 ++metaCalls;assert(lua_gettop(L)==2);
 if(mode==3){assert(!std::strcmp(lua_tostring(L,2),"metastate"));lua_pushnumber(L,26);manager->L=alternate;return 1;}
 if(mode==4){assert(lua_tonumber(L,2)==77);lua_pushstring(L,"from-primary");manager->L=alternate;return 1;}
 lua_pushnumber(L,17);return 1;
}
static void MetaTable(lua_State* L,const char* name){lua_newtable(L);lua_newtable(L);lua_pushstring(L,"__index");lua_pushcfunction(L,IndexMetamethod);lua_settable(L,-3);lua_setmetatable(L,-2);lua_setglobal(L,name);}
static const std::string wrongQuest("wrong"),wrongState("x");
static int ProtectedName(lua_State* L){const char* value=manager->GetQuestStateName(wrongQuest,1);if(value)lua_pushstring(L,value);else lua_pushnil(L);return 1;}
static int ProtectedIndex(lua_State* L){lua_pushnumber(L,manager->GetQuestStateIndex(wrongQuest,wrongState));return 1;}
int main(){
 assert(sizeof(lua_Number)==8&&sizeof(int)==4);desc_test_allow_clock();quest::CQuestManager qm;manager=&qm;assert(!qm.L);primary=lua_open();alternate=lua_open();assert(primary&&alternate);manager->L=primary;
 // Real table values: reverse numeric keys, forward named values and convertible numeric strings.
 lua_newtable(primary);String(primary,0,"start");String(primary,-17,"negative");String(primary,INT_MAX,"highest");Number(primary,"start",0);Number(primary,"negative",-17);Number(primary,"fraction",3.5);Number(primary,"minus",-2.5);Number(primary,"up",1.25);Number(primary,"down",1.75);
 lua_pushstring(primary,"numeric-string");lua_pushstring(primary,"12.5");lua_settable(primary,-3);lua_pushnumber(primary,9);lua_pushboolean(primary,1);lua_settable(primary,-3);lua_pushnumber(primary,18);lua_pushnumber(primary,23);lua_settable(primary,-3);lua_setglobal(primary,"q");
 Reset();assert(!std::strcmp(qm.GetQuestStateName("q",0),"start"));Top(primary);const char* borrowed=qm.GetQuestStateName("q",-17);assert(!std::strcmp(borrowed,"negative")&&borrowed==qm.GetQuestStateName("q",-17));Top(primary);assert(!std::strcmp(qm.GetQuestStateName("q",INT_MAX),"highest"));Top(primary);
 assert(!qm.GetQuestStateName("q",999)&&!qm.GetQuestStateName("q",9));Top(primary);std::string numeric=qm.GetQuestStateName("q",18);assert(numeric=="23");Top(primary);assert(!errorCount&&!debugCount);
 const int oldRound=fegetround();assert(fesetround(FE_TONEAREST)==0);assert(qm.GetQuestStateIndex("q","fraction")==4&&qm.GetQuestStateIndex("q","minus")==-2&&qm.GetQuestStateIndex("q","numeric-string")==12);Top(primary);
 assert(fesetround(FE_UPWARD)==0);assert(qm.GetQuestStateIndex("q","up")==2);assert(fesetround(FE_DOWNWARD)==0);assert(qm.GetQuestStateIndex("q","down")==1);assert(fesetround(oldRound)==0);
 assert(qm.GetQuestStateIndex("q","absent")==0&&qm.GetQuestStateIndex("q","start")==0&&qm.GetQuestStateIndex("q","negative")==-17);Top(primary);assert(!errorCount);
 // Both std::string arguments enter Lua as original C strings, truncating at embeddedNUL.
 std::string binaryQuest("q\0ignored",9),binaryState("negative\0ignored",16);assert(qm.GetQuestStateIndex(binaryQuest,binaryState)==-17);assert(!std::strcmp(qm.GetQuestStateName(binaryQuest,0),"start"));Top(primary);
 Reset();assert(!std::strcmp(qm.GetQuestStateName("missing",-1),""));assert(errorCount==1&&errorLine==939&&errorQuest=="missing"&&errorIndex==-1);Top(primary);assert(qm.GetQuestStateIndex("missing","state")==0);assert(errorCount==2&&errorLine==957&&errorQuest=="missing"&&errorState=="state");Top(primary);
 // Missingnil diagnostics then restore on whichever manager.L the callback leaves active.
 Reset();Prefix(alternate,3);mode=1;assert(qm.GetQuestStateIndex("missing","state")==0&&manager->L==alternate&&mode==0);Top(alternate);assert(lua_gettop(primary)==3);Prefix(primary);
 Reset();Prefix(alternate,3);mode=1;assert(!std::strcmp(qm.GetQuestStateName("missing",4),"")&&manager->L==alternate&&mode==0);Top(alternate);assert(lua_gettop(primary)==3);Prefix(primary);
 // Debug logging runs after stack restoration, retaining old format's x/v argument inversion.
 Reset();test_server=1;assert(qm.GetQuestStateIndex("q","negative")==-17);assert(debugCount==1&&debugValue==-17&&debugTop==2&&debugQuest=="q"&&debugState=="negative");Top(primary);mode=2;assert(qm.GetQuestStateIndex("q","start")==0&&manager->L==alternate&&debugCount==2&&mode==0);Top(primary);Top(alternate);
 // Actual __index C closures exercise real Lua lookup dispatch, not a table/query mock.
 Reset();MetaTable(primary,"meta");metaCalls=0;assert(qm.GetQuestStateIndex("meta","any")==17&&metaCalls==1);Top(primary);
 // A Lua metamethod can change the manager ownerL. Later tonumber/tostring/settop reload it.
 Reset();lua_pushnumber(alternate,91);mode=3;assert(qm.GetQuestStateIndex("meta","metastate")==91&&manager->L==alternate&&metaCalls==2);Top(alternate);assert(lua_gettop(primary)==3);Prefix(primary);
 Reset();lua_pushstring(alternate,"from-alternate");mode=4;std::string changed=qm.GetQuestStateName("meta",77);assert(changed=="from-alternate"&&manager->L==alternate&&metaCalls==3);Top(alternate);assert(lua_gettop(primary)==3);Prefix(primary);
 // Non-table globals preserve actual Lua errors. Test-owned protected calls catch; app methods don't.
 Reset();lua_pushnumber(primary,5);lua_setglobal(primary,"wrong");lua_pushcfunction(primary,ProtectedName);int result=lua_pcall(primary,0,1,0);assert(result!=0&&lua_gettop(primary)==3&&!errorCount);lua_pop(primary,1);Top(primary);lua_pushcfunction(primary,ProtectedIndex);result=lua_pcall(primary,0,1,0);assert(result!=0&&lua_gettop(primary)==3&&!errorCount);lua_pop(primary,1);Top(primary);
 manager->L=0;lua_close(alternate);lua_close(primary);manager=0;primary=alternate=0;test_server=0;std::puts("whole CQM Lua state queries passed over supplied real Lua5.0.3 SDK; original5.0 revision/VM startup equivalence unclaimed");return 0;
}
