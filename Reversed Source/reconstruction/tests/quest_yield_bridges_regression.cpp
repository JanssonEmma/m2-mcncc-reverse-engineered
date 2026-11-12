// Actual CQM/PC/script/network/Lua SDK; no RunState/Confirm/QuestError replacements.
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
static quest::CQuestManager* manager;static quest::QuestState* state;static CHARACTER *characterA,*characterB;
static LPBUFFER outA,outB;static bool active;static int mode,errors,debugs,scriptDest,expectedSuspend,scriptSkin;static std::string* inputText;
static std::vector<std::string> errorMessages,chatMessages;static std::vector<int> errorLines,chatDest;static std::vector<BYTE> script;
static unsigned Read2(const BYTE* b){return static_cast<unsigned>(b[0])|(static_cast<unsigned>(b[1])<<8);}
extern "C" void yield_error(const char* func,int line,const char* message){
 if(!active)return;++errors;assert(func&&message);errorMessages.push_back(message);errorLines.push_back(line);
 if(mode==1){assert(!std::strcmp(func,"Direct")&&line==42&&inputText);*inputText="after";test_server=1;manager->m_pCurrentCharacter=characterB;}
 if(mode==2&&errors==1){assert(!std::strcmp(func,"GotoSelectState")&&line==2021);state->args=-7;state->suspend_state=99;manager->m_strScript="callback|";}
}
extern "C" void yield_debug(const char* text){
 if(!active)return;++debugs;assert(text);if(mode==3){assert(state->args==3&&state->suspend_state==2);assert(std::string(text)=="[QUESTION 1;alpha|2;42|3;tail|;]]");state->args=-11;state->suspend_state=77;manager->m_strScript="debug-replacement";manager->m_pCurrentCharacter=characterB;}
}
static void* Copy(void* output,const void* input,size_t count){
 if(active&&(output==outA->write_point||output==outB->write_point)){
  const BYTE* b=static_cast<const BYTE*>(input);int dest=output==outA->write_point?0:1;
  if(b[0]==4){assert(count>=10&&Read2(b+1)==count&&b[3]==3&&b[count-1]==0);chatMessages.push_back(std::string(reinterpret_cast<const char*>(b+9)));chatDest.push_back(dest);}
  else {assert(b[0]==45&&count>=6&&Read2(b+1)==count&&Read2(b+4)==count-6&&b[3]==scriptSkin&&state->suspend_state==expectedSuspend);assert(script.empty());script.assign(b+6,b+count);scriptDest=dest;}
  void* result=std::memcpy(output,input,count);if(mode==1&&chatMessages.size()==1)manager->m_pCurrentCharacter=characterA;return result;
 }
 return std::memcpy(output,input,count);
}
static void Begin(){active=false;mode=errors=debugs=0;errorMessages.clear();errorLines.clear();chatMessages.clear();chatDest.clear();script.clear();scriptDest=-1;inputText=0;buffer_reset(outA);buffer_reset(outB);test_server=0;manager->m_bNoSend=false;manager->m_bError=false;manager->m_iCurrentSkin=2;manager->m_strScript.clear();manager->m_pCurrentCharacter=characterA;scriptSkin=2;}
static void Script(const std::string& expected,int dest=0){assert(script.size()==expected.size()&&!script.empty()&&!std::memcmp(&script[0],expected.data(),expected.size())&&scriptDest==dest);assert(manager->m_strScript.empty()&&manager->m_iCurrentSkin==1);}
static void ChoiceTable(lua_State* L,int kind){
 lua_settop(L,0);lua_pushnumber(L,111);lua_pushstring(L,"sentinel");lua_newtable(L);
 if(kind==0||kind==2){lua_pushstring(L,"alpha");lua_rawseti(L,-2,1);lua_pushnumber(L,42);lua_rawseti(L,-2,2);lua_pushstring(L,"tail|;]");lua_rawseti(L,-2,3);luaL_setn(L,-1,3);}
 else if(kind==1){lua_pushboolean(L,0);lua_rawseti(L,-2,1);lua_pushstring(L,"two");lua_rawseti(L,-2,2);luaL_setn(L,-1,2);}
 else {assert(kind==3);luaL_setn(L,-1,0);}
}
static void Stack(lua_State* L){assert(lua_gettop(L)==3&&lua_tonumber(L,1)==111&&!std::strcmp(lua_tostring(L,2),"sentinel")&&lua_istable(L,3));}
static void Kept(const quest::QuestState& qs,int reference){assert(qs.co&&qs.ico==reference&&qs.iIndex==23&&qs.bStart&&qs.st==41&&qs._clock_value==17&&qs._counter_value==-18&&qs._title=="title"&&qs.chat_scripts.size()==1&&!qs.chat_scripts[0]);}
int main(){
 desc_test_allow_clock();quest::CQuestManager qm;manager=&qm;quest::PC owner;quest::QuestState qs;state=&qs;qs._clock_value=17;qs._counter_value=-18;qs.iIndex=23;qs.bStart=true;qs.st=41;qs._title="title";qs.chat_scripts.push_back(0);lua_State* L=lua_open();assert(L);qm.L=L;qs.co=lua_newthread(L);qs.ico=luaL_ref(L,LUA_REGISTRYINDEX);const int reference=qs.ico;
 CHARACTER first,second;characterA=&first;characterB=&second;DescOutputProbe descA,descB;LPFDWATCH watch=fdwatch_new(16);outA=buffer_new(0,32768);outB=buffer_new(0,32768);descA.Attach(watch,5,outA);descA.SetPacketState(5);descB.Attach(watch,6,outB);descB.SetPacketState(5);first.BindDesc(&descA);second.BindDesc(&descB);qm.m_pCurrentPC=&owner;thecore_memcpy=Copy;
 typedef void(quest::CQuestManager::*Bridge)(quest::QuestState&);const Bridge bridges[]={&quest::CQuestManager::GotoPauseState,&quest::CQuestManager::GotoInputState,&quest::CQuestManager::GotoSelectItemState};const char* tokens[]={"[NEXT]","[INPUT]","[SELECT_ITEM]"};const int suspends[]={1,3,5};const std::string prefixes[]={"","prefix",std::string("p\0q",3)};
 for(unsigned b=0;b<3;++b)for(unsigned p=0;p<3;++p)for(unsigned drop=0;drop<2;++drop){Begin();qs.args=-99;qs.suspend_state=0;expectedSuspend=suspends[b];qm.m_strScript=prefixes[p];qm.m_bNoSend=drop!=0;owner.m_bShouldSendDone=true;scriptSkin=b==0&&p==0?0:2;active=true;(qm.*bridges[b])(qs);active=false;assert(qs.args==-99&&qs.suspend_state==suspends[b]&&!qm.m_bNoSend&&errors==0);Kept(qs,reference);if(drop){assert(script.empty()&&owner.m_bShouldSendDone&&qm.m_strScript.empty()&&qm.m_iCurrentSkin==1);}else{Script(prefixes[p]+tokens[b]);assert(owner.m_bShouldSendDone==!(b==0&&p==0));}}
 // Actual table/string conversion and original unescaped separators. Parenttable remains onco stack.
 Begin();ChoiceTable(qs.co,0);expectedSuspend=2;qs.args=-99;active=true;qm.GotoSelectState(qs);active=false;Stack(qs.co);assert(qs.args==3&&qs.suspend_state==2&&!errors&&!qm.m_bError);Script("[QUESTION 1;alpha|2;42|3;tail|;]]");Kept(qs,reference);
 Begin();ChoiceTable(qs.co,3);expectedSuspend=2;active=true;qm.GotoSelectState(qs);active=false;Stack(qs.co);assert(qs.args==0&&!errors);Script("[QUESTION ]");
 // Invalid firstentry logs twice thencontinues; lua_typename(co,-1) yields original'no value'.
 Begin();ChoiceTable(qs.co,1);expectedSuspend=2;mode=2;active=true;qm.GotoSelectState(qs);active=false;Stack(qs.co);assert(errors==2&&errorLines[0]==2021&&errorLines[1]==2022&&errorMessages[0]=="SELECT wrong data no value"&&errorMessages[1]=="here"&&qs.args==-7&&qs.suspend_state==2&&!qm.m_bError);Script("callback|[QUESTION |2;two]");
 // Debug occurs AFTERsuspend2 and BEFOREsend; callback changes remain live foractualnetwork output.
 Begin();ChoiceTable(qs.co,0);expectedSuspend=77;mode=3;test_server=1;active=true;qm.GotoSelectState(qs);active=false;Stack(qs.co);assert(debugs==1&&!errors&&qs.args==-11&&qs.suspend_state==77);Script("debug-replacement",1);
 // Whole QuestError formats BEFOREsink, thenreads freshserver/currentCHAR and cacheschfortwomessages.
 Begin();mode=1;qm.m_bError=true;std::string text="before";inputText=&text;active=true;qm.QuestError("Direct",42,"%s %d",text.c_str(),7);active=false;assert(errors==1&&errorMessages[0]=="before 7"&&text=="after"&&chatMessages.size()==2&&chatMessages[0]=="error occor on [Direct:42]"&&chatMessages[1]=="before 7"&&chatDest[0]==1&&chatDest[1]==1&&qm.m_pCurrentCharacter==&first&&qm.m_bError&&script.empty());
 Begin();test_server=1;qm.m_pCurrentCharacter=0;active=true;qm.QuestError("Direct",43,"nothing %d",9);active=false;assert(errors==1&&chatMessages.empty()&&!qm.m_bError);
 // Truncating native4096-byte diagnostics stayslogonly; oversized CHAR::ChatPacket unsafe domain excluded.
 Begin();std::string large(5000,'x');active=true;qm.QuestError("Direct",44,"%s",large.c_str());active=false;assert(errors==1&&errorMessages[0].size()==4095&&chatMessages.empty()&&!qm.m_bError);
 active=false;thecore_memcpy=std::memcpy;qm.CloseState(qs);assert(!qs.co);qm.L=0;lua_close(L);first.BindDesc(0);second.BindDesc(0);descA.Attach(0,-1,0);descB.Attach(0,-1,0);buffer_delete(outA);buffer_delete(outB);fdwatch_delete(watch);qm.m_pCurrentPC=0;qm.m_pCurrentCharacter=0;test_server=0;std::puts("whole quest yield bridges/QuestError actual Lua/script/network passed; Confirm/RunState remain fullfrontier");return 0;
}
