// Actual LuaSDK registry ownership + full CQM script/PC/DESC/TEMP_BUFFER/event graph.
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
#include "../../game/src/buffer_manager.h"
#undef private
#undef protected
#include "../../game/src/packet.h"
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
static quest::CQuestManager* manager;static CHARACTER* alternateCharacter;static LPBUFFER pool,outA,outB,wanted;
static bool active;static int mode,debugLogs;static unsigned poolWrites,gcWrites;static size_t poolCount;
static std::vector<BYTE> frame;static std::string wantedPayload;static BYTE header[6];
static unsigned Read2(const BYTE* p){return static_cast<unsigned>(p[0])|(static_cast<unsigned>(p[1])<<8);}
static void Put2(BYTE* p,unsigned v){p[0]=static_cast<BYTE>(v);p[1]=static_cast<BYTE>(v>>8);}
static void* Copy(void* output,const void* input,size_t count){
 if(active&&output==pool->write_point){
  if(!poolWrites){assert(count==6&&!std::memcmp(input,header,6));if(mode==1)manager->m_strScript="longer-body";}
  else {assert(poolWrites==1&&count==wantedPayload.size()&&!std::memcmp(input,wantedPayload.data(),count));}
  const BYTE* first=static_cast<const BYTE*>(input);frame.insert(frame.end(),first,first+count);++poolWrites;void* answer=std::memcpy(output,input,count);
  if(mode==1&&poolWrites==2){manager->m_strScript="after-payload";manager->m_pCurrentCharacter=alternateCharacter;}
  return answer;
 }
 if(active&&(output==outA->write_point||output==outB->write_point)){
  assert(output==wanted->write_point&&poolWrites==2&&!gcWrites&&count==frame.size()&&!std::memcmp(input,&frame[0],count));assert(BUFFER_MANAGER::instance().buffer_stack.size()==poolCount-1);++gcWrites;void* answer=std::memcpy(output,input,count);
  if(mode==1){manager->m_strScript="after-gc";manager->m_iCurrentSkin=0x777;manager->m_bNoSend=true;test_server=1;}
  return answer;
 }
 return std::memcpy(output,input,count);
}
extern "C" void quest_script_debug(const char* text,int bytes){
 if(!active)return;++debugLogs;assert(mode==1&&!std::strcmp(text,"after-gc")&&bytes==17&&gcWrites==1&&BUFFER_MANAGER::instance().buffer_stack.size()==poolCount-1);manager->m_strScript="after-debug";manager->m_iCurrentSkin=77;
}
static void Begin(const std::string& script,int skin,LPBUFFER output){
 active=false;mode=debugLogs=0;poolWrites=gcWrites=0;frame.clear();buffer_reset(outA);buffer_reset(outB);wanted=output;wantedPayload=script;test_server=0;assert(BUFFER_MANAGER::instance_ptr()&&!BUFFER_MANAGER::instance().buffer_stack.empty());poolCount=BUFFER_MANAGER::instance().buffer_stack.size();pool=BUFFER_MANAGER::instance().buffer_stack.top();assert(!buffer_size(pool));header[0]=45;Put2(header+1,static_cast<WORD>(static_cast<WORD>(script.size())+6));header[3]=static_cast<BYTE>(skin);Put2(header+4,static_cast<WORD>(script.size()));
}
static void End(bool sent){active=false;assert(poolWrites==(sent?2u:0u)&&gcWrites==(sent?1u:0u));assert(BUFFER_MANAGER::instance().buffer_stack.size()==poolCount&&!buffer_size(pool));assert(manager->m_strScript.empty()&&manager->m_iCurrentSkin==1);if(sent){assert(buffer_size(wanted)==frame.size()&&frame.size()==6+wantedPayload.size());assert(Read2(&frame[1])==Read2(header+1)&&Read2(&frame[4])==Read2(header+4));}else assert(!buffer_size(outA)&&!buffer_size(outB));}
static void Prefix(lua_State* L){lua_settop(L,0);lua_pushnumber(L,111);lua_pushstring(L,"sentinel");}
static void Top(lua_State* L){assert(lua_gettop(L)==2&&lua_tonumber(L,1)==111&&!std::strcmp(lua_tostring(L,2),"sentinel"));}
static lua_State* Thread(lua_State* L,int& reference){lua_State* co=lua_newthread(L);assert(co);reference=luaL_ref(L,LUA_REGISTRYINDEX);assert(reference>0);return co;}
static bool IsThread(lua_State* L,int reference){lua_rawgeti(L,LUA_REGISTRYINDEX,reference);bool answer=lua_type(L,-1)==LUA_TTHREAD;lua_pop(L,1);return answer;}
static void Init(quest::QuestState& qs){qs._clock_value=17;qs._counter_value=-18;qs.ico=31;qs.args=-2;qs.suspend_state=9;qs.iIndex=23;qs.bStart=true;qs.st=41;qs._title="title";qs._clock_name="clock";qs._counter_name="counter";qs._icon_file="icon";qs.chat_scripts.push_back(0);}
static void Kept(const quest::QuestState& qs,int reference){assert(qs.ico==reference&&qs.args==-2&&qs.suspend_state==9&&qs.iIndex==23&&qs.bStart&&qs.st==41&&qs._clock_value==17&&qs._counter_value==-18&&qs._title=="title"&&qs._clock_name=="clock"&&qs._counter_name=="counter"&&qs._icon_file=="icon"&&qs.chat_scripts.size()==1&&!qs.chat_scripts[0]);}
int main(){
 desc_test_allow_clock();assert(sizeof(int)==4&&sizeof(lua_Number)==8&&sizeof(packet_script)==6);quest::CQuestManager qm;manager=&qm;quest::PC owner;quest::QuestState state;Init(state);lua_State* A=lua_open();lua_State* B=lua_open();assert(A&&B);Prefix(A);Prefix(B);
 // Nullco does not touch Lua atall, even ownerLNULL, and preserves all other live fields.
 qm.L=0;state.co=0;state.ico=INT_MIN;qm.CloseState(state);assert(!state.co);Kept(state,INT_MIN);
 int refA;qm.L=A;state.co=Thread(A,refA);state.ico=refA;assert(IsThread(A,refA));qm.CloseState(state);assert(!state.co&&!IsThread(A,refA));Top(A);Kept(state,refA);qm.CloseState(state);Top(A);
 int reused;lua_State* held=Thread(A,reused);assert(reused==refA&&held);luaL_unref(A,LUA_REGISTRYINDEX,reused);
 // OwnerCURRENT L is authoritative, independentofco's originatingVM. BothSDKstates remain valid.
 int otherA,otherB;state.co=Thread(A,otherA);Thread(B,otherB);state.ico=otherB;qm.L=B;qm.CloseState(state);assert(!state.co&&IsThread(A,otherA)&&!IsThread(B,otherB));Kept(state,otherB);Top(A);Top(B);luaL_unref(A,LUA_REGISTRYINDEX,otherA);
 // Negative registryreference doesn't become a fakeclose: actualSDK no-op still followed bycoNULL.
 int negativeHeld;state.co=Thread(A,negativeHeld);state.ico=LUA_NOREF;qm.L=A;qm.CloseState(state);assert(!state.co&&IsThread(A,negativeHeld));Kept(state,LUA_NOREF);luaL_unref(A,LUA_REGISTRYINDEX,negativeHeld);Top(A);
 // OpenState is compiled/staticonly: original directretstorage leaves countersindeterminate; no forcedcopies.
 qm.m_strScript=std::string("a\0b",3);qm.m_iCurrentSkin=-7;qm.m_bNoSend=true;qm.m_bError=true;qm.m_pCurrentPC=&owner;owner.m_bShouldSendDone=true;qm.AddScript(qm.m_strScript);assert(qm.m_strScript==std::string("a\0ba\0b",6)&&qm.m_iCurrentSkin==-7&&qm.m_bNoSend&&qm.m_bError&&owner.m_bShouldSendDone);qm.ClearScript();assert(qm.m_strScript.empty()&&qm.m_iCurrentSkin==1&&qm.m_bNoSend&&qm.m_bError&&owner.m_bShouldSendDone);
 {TEMP_BUFFER prime;std::vector<char> large(131072,'p');prime.write(&large[0],static_cast<int>(large.size()));}
 CHARACTER firstCharacter,secondCharacter;alternateCharacter=&secondCharacter;DescOutputProbe firstDesc,secondDesc;LPFDWATCH watch=fdwatch_new(16);outA=buffer_new(0,262144);outB=buffer_new(0,262144);firstDesc.Attach(watch,5,outA);firstDesc.SetPacketState(5);secondDesc.Attach(watch,6,outB);secondDesc.SetPacketState(5);firstCharacter.BindDesc(&firstDesc);secondCharacter.BindDesc(&secondDesc);thecore_memcpy=Copy;
 const std::string scripts[]={"[DONE]","[NEXT]","normal"};const int skins[]={1,0,2,-1,0x123};
 for(unsigned s=0;s<3;++s)for(unsigned p=0;p<2;++p)for(unsigned d=0;d<2;++d)for(unsigned k=0;k<5;++k)for(unsigned e=0;e<2;++e)for(unsigned drop=0;drop<2;++drop){
  const bool control=s<2;const bool sent=!drop&&!(s==0&&p&&!d&&skins[k]==1&&!e);Begin(scripts[s],control?0:skins[k],outA);qm.m_strScript=scripts[s];qm.m_iCurrentSkin=skins[k];qm.m_bNoSend=drop!=0;qm.m_bError=e!=0;qm.m_pCurrentPC=p?&owner:0;qm.m_pCurrentCharacter=&firstCharacter;owner.m_bShouldSendDone=d!=0;active=true;qm.SendScript();End(sent);assert(!qm.m_bNoSend&&qm.m_bError==(e!=0)&&owner.m_bShouldSendDone==((!drop&&control&&p)?false:d!=0)&&!debugLogs);
 }
 // Fullpayloadlength remains separate from WORDheadersizes; every byte is initialized/nonnull payload.
 const unsigned lengths[]={1,65535,65536,65537};for(unsigned n=0;n<4;++n){std::string text(lengths[n],'x');if(text.size()>3)text[2]=0;Begin(text,0x123,outA);qm.m_strScript=text;qm.m_iCurrentSkin=0x123;qm.m_bNoSend=false;qm.m_bError=false;qm.m_pCurrentPC=&owner;qm.m_pCurrentCharacter=&firstCharacter;active=true;qm.SendScript();End(true);}
 // Cachedheader, livepayload, latereceiver, postPacket debug and finalLIVEclear are all distinct.
 Begin("abc",0x123,outB);wantedPayload="longer-body";mode=1;qm.m_strScript="abc";qm.m_iCurrentSkin=0x123;qm.m_bNoSend=false;qm.m_bError=false;qm.m_pCurrentPC=&owner;qm.m_pCurrentCharacter=&firstCharacter;active=true;qm.SendScript();End(true);assert(debugLogs==1&&qm.m_bNoSend&&Read2(&frame[1])==9&&frame.size()==17);
 Begin("",1,outA);qm.m_strScript="";qm.m_bNoSend=true;owner.m_bShouldSendDone=true;active=true;qm.SendScript();End(false);assert(!qm.m_bNoSend&&owner.m_bShouldSendDone); // safeemptydrop, not unsuppressedoperator[](size).
 // GotoEndState never closes/resetsqs: prefix appended, realSendScript output, Lua registry keptalive.
 int ending;state.co=Thread(A,ending);state.ico=ending;qm.L=A;Begin("prefix[DONE]",2,outA);qm.m_strScript="prefix";qm.m_iCurrentSkin=2;qm.m_bNoSend=false;qm.m_bError=false;qm.m_pCurrentPC=&owner;qm.m_pCurrentCharacter=&firstCharacter;owner.m_bShouldSendDone=true;active=true;qm.GotoEndState(state);End(true);assert(state.co&&IsThread(A,ending)&&owner.m_bShouldSendDone);Kept(state,ending);qm.CloseState(state);assert(!state.co);Top(A);
 active=false;thecore_memcpy=std::memcpy;firstCharacter.BindDesc(0);secondCharacter.BindDesc(0);firstDesc.Attach(0,-1,0);secondDesc.Attach(0,-1,0);buffer_delete(outA);buffer_delete(outB);fdwatch_delete(watch);qm.L=0;lua_close(A);lua_close(B);qm.m_pCurrentPC=0;qm.m_pCurrentCharacter=0;test_server=0;std::puts("whole CQM CloseState/ClearScript/AddScript/SendScript/GotoEndState actual SDK/network passed; OpenState originalNRVO/uninitializedreturn staticonly");return 0;
}
