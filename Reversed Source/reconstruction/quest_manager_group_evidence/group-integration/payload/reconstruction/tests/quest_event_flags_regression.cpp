// Full actual CQM/CHAR::ChatPacket/DESC + CLIENT_DESC DBPacket +buffer/event graph.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#include "../../game/src/char.h"
#include "../../game/src/desc_client.h"
#undef protected
#undef private
#include "../../common/tables.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
class ClientProbe:public CLIENT_DESC{public:void Output(LPBUFFER b){m_lpOutputBuffer=b;}};
static quest::CQuestManager* qm;static CHARACTER* receiver;static DESC* otherDesc;static CLIENT_DESC* otherDB;
static LPBUFFER gcA,gcB,dbA,dbB;static bool active;static int mode,dbLogs;static std::string* borrowedName;
static std::vector<std::string> texts;static std::vector<int> types;static std::vector<int> destinations;
static unsigned Read2(const BYTE* b){return static_cast<unsigned>(b[0])|(static_cast<unsigned>(b[1])<<8);}
static unsigned Read4(const BYTE* b){return static_cast<unsigned>(b[0])|(static_cast<unsigned>(b[1])<<8)|(static_cast<unsigned>(b[2])<<16)|(static_cast<unsigned>(b[3])<<24);}
extern "C" void quest_event_db_log(unsigned header,unsigned long handle,unsigned long size,int buffered){
 if(!active)return;++dbLogs;assert(header==55&&!handle&&size==37&&buffered==0);
 if(mode==3){assert(borrowedName);*borrowedName="changed-after-packet-built";db_clientdesc=otherDB;}
}
static void* Copy(void* output,const void* input,size_t count){
 if(active&&(output==gcA->write_point||output==gcB->write_point)){
  const BYTE* b=static_cast<const BYTE*>(input);assert(count>=10&&b[0]==4&&Read2(b+1)==count&&Read4(b+4)==0&&b[count-1]==0);texts.push_back(std::string(reinterpret_cast<const char*>(b+9)));types.push_back(b[3]);destinations.push_back(output==gcA->write_point?0:1);
  void* result=std::memcpy(output,input,count);
  if(mode==1&&texts.size()==1){qm->m_mapEventFlag["a"]=7;qm->m_mapEventFlag["b"]=-8;qm->m_mapEventFlag["aa"]=3;qm->m_mapEventFlag["0before"]=4;test_server=1;receiver->BindDesc(otherDesc);}
  if(mode==2&&texts.size()==1){qm->m_mapEventFlag["xmas_boom"]=7;qm->m_mapEventFlag["xmas_tree"]=0;receiver->BindDesc(otherDesc);}
  if(mode==2&&texts.size()==3)qm->m_mapEventFlag["newyear_boom"]=INT_MIN;
  return result;
 }
 return std::memcpy(output,input,count);
}
static void Begin(CHARACTER& character,DESC& first){active=false;texts.clear();types.clear();destinations.clear();buffer_reset(gcA);buffer_reset(gcB);buffer_reset(dbA);buffer_reset(dbB);mode=0;dbLogs=0;test_server=0;character.BindDesc(&first);receiver=&character;}
static void Text(unsigned i,int type,const char* text,int dest=0){assert(i<texts.size()&&types[i]==type&&texts[i]==text&&destinations[i]==dest);}
static void Request(quest::CQuestManager& manager,ClientProbe& database,std::string name,int value){
 db_clientdesc=&database;borrowedName=&name;active=true;manager.RequestSetEventFlag(name,value);active=false;
 assert(dbLogs==1&&buffer_size(dbA)==46&&!buffer_size(dbB));const BYTE* b=static_cast<const BYTE*>(buffer_read_peek(dbA));assert(b[0]==55&&Read4(b+1)==0&&Read4(b+5)==37&&Read4(b+42)==static_cast<unsigned>(value));
 const size_t c=std::strlen(mode==3?"original":name.c_str());const std::string expected=mode==3?"original":name;
 for(unsigned i=0;i<32;++i){BYTE want=i<c&&i<expected.size()?static_cast<BYTE>(expected[i]):0;assert(b[9+i]==want);}assert(b[41]==0);borrowedName=0;
}
int main(){
 desc_test_allow_clock();assert(sizeof(long)==4&&sizeof(DWORD)==4&&sizeof(TPacketSetEventFlag)==37);quest::CQuestManager manager;qm=&manager;CHARACTER character;DescOutputProbe first,second;ClientProbe database,alternate;otherDesc=&second;otherDB=&alternate;
 LPFDWATCH watch=fdwatch_new(16);gcA=buffer_new(0,16384);gcB=buffer_new(0,16384);dbA=buffer_new(0,16384);dbB=buffer_new(0,16384);first.Attach(watch,5,gcA);first.SetPacketState(5);second.Attach(watch,6,gcB);second.SetPacketState(5);database.Output(dbA);alternate.Output(dbB);thecore_memcpy=Copy;
 // Missing flags return0/noinsertion; both complete output methods accept null receiver only if no output.
 Begin(character,first);manager.SendEventFlagList(0);manager.BroadcastEventFlagOnLogin(0);assert(manager.m_mapEventFlag.empty()&&texts.empty());
 // Original numeric special cases apply only value1 with test_serverfalse; all sorted rows are emitted.
 const int serverValues[]={0,1,-1};for(unsigned s=0;s<3;++s){Begin(character,first);test_server=serverValues[s];manager.m_mapEventFlag.clear();manager.m_mapEventFlag["newyear_fire"]=1;manager.m_mapEventFlag["newyear_wonso"]=1;manager.m_mapEventFlag["valentine_drop"]=1;manager.m_mapEventFlag["zero"]=0;manager.m_mapEventFlag["negative"]=-3;active=true;manager.SendEventFlagList(&character);active=false;assert(texts.size()==5);Text(0,1,"negative -3");Text(1,1,s?"newyear_fire 1":"newyear_fire 1 prob 1000");Text(2,1,s?"newyear_wonso 1":"newyear_wonso 1 prob 500");Text(3,1,s?"valentine_drop 1":"valentine_drop 1 prob 800");Text(4,1,"zero 0");}
 Begin(character,first);manager.m_mapEventFlag.clear();manager.m_mapEventFlag["newyear_fire"]=INT_MIN;manager.m_mapEventFlag["valentine_drop"]=-1;manager.m_mapEventFlag[std::string("a\0b",3)]=INT_MAX;active=true;manager.SendEventFlagList(&character);active=false;assert(texts.size()==3);Text(0,1,"a 2147483647");Text(1,1,"newyear_fire -2147483648");Text(2,1,"valentine_drop -1");
 // Live traversal after ChatPacket: insertedfuture key included, insertedpast skipped; values read perrow.
 Begin(character,first);manager.m_mapEventFlag.clear();manager.m_mapEventFlag["a"]=1;manager.m_mapEventFlag["b"]=2;manager.m_mapEventFlag["valentine_drop"]=1;mode=1;active=true;manager.SendEventFlagList(&character);active=false;assert(texts.size()==4);Text(0,1,"a 1");Text(1,1,"aa 3",1);Text(2,1,"b -8",1);Text(3,1,"valentine_drop 1",1);assert(manager.m_mapEventFlag["0before"]==4&&manager.m_mapEventFlag["a"]==7);
 // Original login set/order is exactlyfive; modern eclipse alone emitsnothing.
 Begin(character,first);manager.m_mapEventFlag.clear();manager.m_mapEventFlag["eclipse"]=1;active=true;manager.BroadcastEventFlagOnLogin(&character);active=false;assert(texts.empty());
 Begin(character,first);manager.m_mapEventFlag.clear();manager.m_mapEventFlag["xmas_snow"]=-1;manager.m_mapEventFlag["xmas_boom"]=2;manager.m_mapEventFlag["xmas_tree"]=INT_MAX;manager.m_mapEventFlag["day"]=-3;manager.m_mapEventFlag["newyear_boom"]=INT_MIN;active=true;manager.BroadcastEventFlagOnLogin(&character);active=false;assert(texts.size()==5);Text(0,5,"xmas_snow -1");Text(1,5,"xmas_boom 2");Text(2,5,"xmas_tree 2147483647");Text(3,5,"DayMode dark");Text(4,5,"newyear_boom -2147483648");
 Begin(character,first);manager.m_mapEventFlag.clear();manager.m_mapEventFlag["xmas_snow"]=1;manager.m_mapEventFlag["xmas_tree"]=-9;manager.m_mapEventFlag["day"]=1;mode=2;active=true;manager.BroadcastEventFlagOnLogin(&character);active=false;assert(texts.size()==4);Text(0,5,"xmas_snow 1");Text(1,5,"xmas_boom 7",1);Text(2,5,"DayMode dark",1);Text(3,5,"newyear_boom -2147483648",1);
 // Complete37-byte request packet is fullydefined by strncpy32/padding/NUL/value. No map mutation.
 const std::string names[]={"",std::string(1,'a'),std::string(31,'b'),std::string(32,'c'),std::string(33,'d'),std::string(80,'e'),std::string("a\0b",3)};const int values[]={0,INT_MIN,INT_MAX,-1};const std::map<std::string,int> flags=manager.m_mapEventFlag;
 for(unsigned n=0;n<7;++n)for(unsigned v=0;v<4;++v){Begin(character,first);Request(manager,database,names[n],values[v]);assert(manager.m_mapEventFlag==flags);}
 Begin(character,first);mode=3;Request(manager,database,"original",-17);assert(db_clientdesc==&alternate); // DBPacket's this cached before log; originaldescriptor getswhole packet.
 active=false;thecore_memcpy=std::memcpy;character.BindDesc(0);db_clientdesc=0;database.Output(0);alternate.Output(0);first.Attach(0,-1,0);second.Attach(0,-1,0);buffer_delete(gcA);buffer_delete(gcB);buffer_delete(dbA);buffer_delete(dbB);fdwatch_delete(watch);test_server=0;std::puts("whole quest eventflag list/request/login actual network graph passed; broad SetEventFlag gameplayeffects remainpending");return 0;
}
