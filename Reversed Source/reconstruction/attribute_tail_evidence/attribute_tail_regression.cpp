#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/item.h"
#include "../../game/src/item_manager.h"
#include "../../game/src/char.h"
#include "../../game/src/sectree.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/log.h"
#undef protected
#undef private
#include "../../game/src/config.h"
#include "../../game/src/locale.hpp"
#include "../../game/src/packet.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include "item_socket_support.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <climits>
#include <cmath>
#include "../../game/src/item_addon.h"
#include "../../game/src/utils.h"
#include "../../game/src/constants.h"
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
void ShutdownOnFatalError() { std::abort(); }

static CItem* activeItem;
static ITEM_MANAGER* manager;
static LogManager* logger;
static LPBUFFER output;
struct Packet {int i,t;short v;Packet(int a,int b,short c):i(a),t(b),v(c){}};
struct Audit {int i,t;short v;const char* how;int packets;bool saved;Audit(int a,int b,short c,const char* h,int n,bool s):i(a),t(b),v(c),how(h),packets(n),saved(s){}};
static std::vector<Packet> expectedPackets;
static std::vector<Audit> expectedLogs;
static std::vector<long> rng;
static unsigned rngAt,packetAt,logAt;
static int depth,copy21,rareFull;
static bool skip,mutation;
static int randomMutation;
extern "C" long tail_test_random() {
 assert(rngAt<rng.size());long n=rng[rngAt++];
 if(randomMutation==1){g_map_itemRare[7].dwApplyIndex=263;g_map_itemRare[7].bMaxLevelBySet[0]=2;randomMutation=0;}
 if(randomMutation==2){g_map_itemRare[7].dwApplyIndex=0;g_map_itemRare[7].bMaxLevelBySet[0]=1;randomMutation=0;}
 return n;
}
extern "C" void tail_test_rare_full(){++rareFull;}
static void Rand(long n){rng.push_back(n);}
static void PacketOnly(int i,int t,short v){expectedPackets.push_back(Packet(i,t,v));}
static void LogOnly(int i,int t,short v,const char* how,bool saved){expectedLogs.push_back(Audit(i,t,v,how,static_cast<int>(expectedPackets.size()),saved));}
static void SetExpected(int i,int t,short v,const char* how="SET_ATTR") {PacketOnly(i,t,v);if(t||!std::strcmp(how,"SET_RARE"))LogOnly(i,t,v,how,true);}
static void* Copy(void* out,const void* in,size_t n){
 if(activeItem&&n==36&&static_cast<const BYTE*>(in)[0]==25){
  assert(packetAt<expectedPackets.size());const Packet& e=expectedPackets[packetAt];
  const BYTE* b=static_cast<const BYTE*>(in);assert(b[1]==107&&b[2]==44);
  TPlayerItemAttribute wire[7];std::memcpy(wire,b+15,sizeof(wire));assert(!std::memcmp(wire,activeItem->GetAttributes(),sizeof(wire)));
  assert(wire[e.i].bType==e.t&&wire[e.i].sValue==e.v);
  if(!packetAt&&(!expectedLogs.empty()&&expectedLogs.front().saved))assert(manager->m_set_pkItemForDelayedSave.empty());
  ++packetAt;
 }
 if(activeItem&&n==21){assert(out==activeItem->m_aAttr);assert(manager->m_set_pkItemForDelayedSave.empty());++copy21;}
 return out==in?out:std::memcpy(out,in,n);
}
struct Runtime:SqlRuntime{
 MYSQL* db;Runtime():db(0){}
 virtual int SemInit(sem_t* s,int shared,unsigned value){assert(!shared&&!value);*s=reinterpret_cast<sem_t>(1);return 0;}
 virtual int SemDestroy(sem_t* s){assert(s&&*s);return 0;}
 virtual int MutexInit(pthread_mutex_t* m,const pthread_mutexattr_t* a){assert(m&&!a);std::memset(m,0,sizeof(*m));return 0;}
 virtual int Create(pthread_t*,const pthread_attr_t* a,void*(*entry)(void*),void* arg){assert(!a&&entry==AsyncSQLThread&&arg==&logger->m_sql);return 11;}
 virtual void Close(MYSQL* p){assert(p==db);}
 virtual int MutexDestroy(pthread_mutex_t* m){assert(m&&!depth);return 0;}
 void Saved(){assert(activeItem&&logAt<expectedLogs.size());const Audit&e=expectedLogs[logAt];assert(packetAt==static_cast<unsigned>(e.packets));assert((manager->m_set_pkItemForDelayedSave.count(activeItem)!=0)==(e.saved&&!skip));}
 virtual unsigned long Escape(MYSQL* p,char* to,const char* from,unsigned long n){assert(p==db&&!n&&!*from);Saved();to[0]=0;return 0;}
 virtual int MutexLock(pthread_mutex_t* m){assert(m==logger->m_sql.m_mtxQuery&&!depth);Saved();++depth;return 0;}
 virtual int SemPost(sem_t* s){assert(s&&*s&&depth==1);Saved();assert(logger->m_sql.m_queue_query.size()==logAt+1);return 0;}
 virtual int MutexUnlock(pthread_mutex_t* m){assert(m==logger->m_sql.m_mtxQuery&&depth==1);Saved();--depth;++logAt;if(mutation&&logAt==1){activeItem->m_aAttr[2].bType=99;activeItem->m_aAttr[2].sValue=-99;}return 0;}
};
static void Begin(CItem& item,bool noSave=false){assert(!activeItem&&logger->m_sql.m_queue_query.empty());activeItem=&item;manager->m_set_pkItemForDelayedSave.clear();buffer_reset(output);expectedPackets.clear();expectedLogs.clear();rng.clear();rngAt=packetAt=logAt=0;copy21=rareFull=0;mutation=false;randomMutation=0;skip=noSave;item.SetSkipSave(skip);}
static void Done(bool saved){
 assert(packetAt==expectedPackets.size()&&logAt==expectedLogs.size()&&rngAt==rng.size()&&!depth);
 assert(buffer_size(output)==36*packetAt);assert((manager->m_set_pkItemForDelayedSave.count(activeItem)!=0)==(saved&&!skip));
 for(unsigned i=0;i<expectedLogs.size();++i){const Audit&e=expectedLogs[i];SQLMsg*m=logger->m_sql.m_queue_query.front();logger->m_sql.m_queue_query.pop();char q[512];
 std::snprintf(q,sizeof(q),"INSERT DELAYED INTO log_tail (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), %lu, %lu, %lu, %lu, '%s', '')",static_cast<DWORD>(e.i),static_cast<DWORD>(e.t),static_cast<DWORD>(e.v),activeItem->GetID(),e.how);
 assert(m->stQuery==q&&!m->bReturn&&m->m_pkSQL==logger->m_sql.GetSQLHandle());delete m;}
 activeItem=0;manager->m_set_pkItemForDelayedSave.clear();
}
static void Attach(CItem& item,TItemTable& proto,CHARACTER& owner,int type=1,int subtype=0){item.Initialize();std::memset(&proto,0,sizeof(proto));proto.bType=static_cast<BYTE>(type);proto.bSubType=static_cast<BYTE>(subtype);item.SetProto(&proto);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;}
static void Row(TItemAttrMap& map,int key,int apply,int set,int cap){TItemAttrTable r;std::memset(&r,0,sizeof(r));std::snprintf(r.szApply,sizeof(r.szApply),"synthetic-%d",key);r.dwApplyIndex=apply;r.dwProb=1000-key;for(int i=0;i<5;++i)r.lValues[i]=key*100+i+1;r.bMaxLevelBySet[set]=static_cast<BYTE>(cap);map[key]=r;}
static void Maps(){g_map_itemAttr.clear();g_map_itemRare.clear();}
static void PlainCases(CHARACTER& owner){
 CItem item;TItemTable proto;Attach(item,proto,owner);
 for(int i=0;i<7;++i){item.m_aAttr[i].bType=static_cast<BYTE>(10+i);item.m_aAttr[i].sValue=static_cast<short>(-10-i);}
 Begin(item);item.ClearAttribute();Done(false);for(int i=0;i<5;++i)assert(!item.m_aAttr[i].bType&&!item.m_aAttr[i].sValue);assert(item.m_aAttr[5].bType==15&&item.m_aAttr[6].bType==16);
 for(int i=0;i<5;++i){item.m_aAttr[i].bType=static_cast<BYTE>(10+i);item.m_aAttr[i].sValue=static_cast<short>(-10-i);}
 Begin(item);mutation=true;SetExpected(0,11,-11);SetExpected(1,99,-99);SetExpected(2,13,-13);SetExpected(3,14,-14);SetExpected(4,0,0);assert(item.RemoveAttributeAt(0));Done(true);
 Begin(item);assert(!item.RemoveAttributeAt(4)&&!item.RemoveAttributeAt(INT_MAX));Done(false);
 item.m_aAttr[0].bType=0;item.m_aAttr[1].bType=77;Begin(item);assert(!item.RemoveAttributeAt(1));Done(false);assert(item.m_aAttr[1].bType==77);
 TPlayerItemAttribute attrs[7];for(int i=0;i<7;++i){attrs[i].bType=static_cast<BYTE>(200+i);attrs[i].sValue=static_cast<short>(INT_MIN+i*7000);}
 for(int n=0;n<2;++n){Begin(item,n!=0);item.SetAttributes(attrs);assert(copy21==1&&!std::memcmp(item.GetAttributes(),attrs,sizeof(attrs)));Done(true);}
 CItem dest;TItemTable p2;Attach(dest,p2,owner);Begin(dest);item.CopyAttributeTo(&dest);assert(copy21==1&&!std::memcmp(dest.GetAttributes(),attrs,sizeof(attrs)));Done(true);dest.SetCell(0,0);
 // Self copy uses the same original indirect hook; test hook uses memmove here only for exact alias.
 Begin(item);item.CopyAttributeTo(&item);assert(copy21==1);Done(true);
 item.SetCell(0,0);
}
static void NormalCases(CHARACTER& owner){
 for(int pct=1;pct<=100;++pct){Maps();Row(g_map_itemAttr,7,777,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);Begin(item);int level=pct<=40?1:pct<=90?2:3;SetExpected(0,7,static_cast<short>(700+level));Rand(pct-1);Rand(0);item.AddAttribute();Done(true);item.SetCell(0,0);}
 {CItem item;TItemTable proto;Attach(item,proto,owner);for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);Begin(item);item.AddAttribute();Done(false);item.SetCell(0,0);}
 for(int custom=0;custom<2;++custom)for(int pct=1;pct<=100;++pct){Maps();Row(g_map_itemAttr,7,7,0,5);Row(g_map_itemAttr,8,8,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);item.m_aAttr[0].bType=20;item.m_aAttr[1].bType=21;item.m_aAttr[5].bType=80;item.m_aAttr[5].sValue=-80;
 int probs[5]={100,0,0,0,0};int level=custom?1:pct<=10?2:pct<=50?3:pct<=85?4:5;Begin(item);SetExpected(0,7,static_cast<short>(700+level));SetExpected(1,8,static_cast<short>(800+level));Rand(pct-1);Rand(0);Rand(pct-1);Rand(0);item.ChangeAttribute(custom?probs:0);Done(true);assert(item.m_aAttr[5].bType==80&&item.m_aAttr[5].sValue==-80&&probs[0]==100);item.SetCell(0,0);}
 {CItem noProto;noProto.Initialize();noProto.m_aAttr[1].bType=4;Begin(noProto);noProto.ChangeAttribute();Done(false);assert(!noProto.m_aAttr[1].bType);}
 {CItemAddonManager addons;for(int cached=0;cached<2;++cached){Maps();Row(g_map_itemAttr,7,7,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);proto.sAddonType=1;item.m_aAttr[0].bType=20;if(!cached){item.m_aAttr[1].bType=21;item.m_aAttr[2].bType=22;}
 Begin(item);if(!cached){Rand(0x180000);Rand(0x100000);}Rand(0);Rand(0);Rand(0);SetExpected(0,72,cached?17:1);if(!cached){SetExpected(1,71,8);SetExpected(2,7,705);Rand(99);Rand(0);}item.ChangeAttribute();Done(true);assert(item.GetAttributeCount()==(cached?1:3));item.SetCell(0,0);}}
}
static void RareCases(CHARACTER& owner){
 {CItem item;item.Initialize();for(int bits=0;bits<4;++bits){item.m_aAttr[5].bType=(bits&1)?7:0;item.m_aAttr[6].bType=(bits&2)?8:0;assert(item.GetRareAttrCount()==((bits&1)!=0)+((bits&2)!=0));assert(item.HasRareAttr(7)==((bits&1)!=0));assert(item.HasRareAttr(8)==((bits&2)!=0));assert(item.HasRareAttr(0)==(bits!=3));assert(!item.HasRareAttr(255));}}
 const int sub[]={0,0,3,4,5,1,2,6};
 for(int set=0;set<8;++set)for(int choose=0;choose<3;++choose){Maps();Row(g_map_itemRare,7,263,set,2);Row(g_map_itemRare,8,8,set,4);Row(g_map_itemRare,9,9,set,255);Row(g_map_itemRare,10,0,set,5);Row(g_map_itemRare,11,11,set,0);Row(g_map_itemRare,90,90,set,5);
 CItem item;TItemTable proto;Attach(item,proto,owner,set?2:1,sub[set]);item.m_aAttr[0].bType=7;Begin(item);int key=7+choose,level=choose==0?2:choose==1?4:5;SetExpected(5,choose==0?7:key,static_cast<short>(key*100+level),"SET_RARE");Rand(choose);assert(item.AddRareAttribute());Done(true);assert(g_map_itemRare.size()==83);item.SetCell(0,0);}
 for(int hole=0;hole<2;++hole){Maps();Row(g_map_itemRare,8,8,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);item.m_aAttr[hole?6:5].bType=7;Begin(item);SetExpected(6,8,805,"SET_RARE");Rand(0);assert(item.AddRareAttribute());Done(true);assert(item.m_aAttr[5].bType==(hole?0:7));item.SetCell(0,0);}
 {Maps();Row(g_map_itemRare,8,7,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);item.m_aAttr[5].bType=7;Begin(item);SetExpected(6,7,805,"SET_RARE");Rand(0);assert(item.AddRareAttribute());Done(true);item.SetCell(0,0);}
 for(int mode=1;mode<=2;++mode){Maps();Row(g_map_itemRare,7,7,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);Begin(item);randomMutation=mode;SetExpected(5,mode==1?7:0,mode==1?702:701,"SET_RARE");Rand(0);assert(item.AddRareAttribute());Done(true);item.SetCell(0,0);}
 {Maps();CItem item;TItemTable proto;Attach(item,proto,owner);item.m_aAttr[5].bType=7;item.m_aAttr[6].bType=8;Begin(item);assert(!item.AddRareAttribute());assert(rareFull==1);Done(false);assert(g_map_itemRare.empty());item.SetCell(0,0);}
 {CItem item;item.Initialize();Begin(item);assert(!item.ChangeRareAttribute());Done(false);}
 for(int n=1;n<=2;++n){Maps();Row(g_map_itemRare,7,7,0,5);Row(g_map_itemRare,8,8,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);item.m_aAttr[6].bType=9;if(n==2)item.m_aAttr[5].bType=10;Begin(item);LogOnly(0,0,0,"SET_RARE_CHANGE",false);SetExpected(n==1?6:5,7,705,"SET_RARE");Rand(0);if(n==2){SetExpected(6,8,805,"SET_RARE");Rand(0);}assert(item.ChangeRareAttribute());Done(true);assert(item.m_aAttr[5].bType==(n==1?0:7));item.SetCell(0,0);}
}
int main(){assert(sizeof(DWORD)==4&&sizeof(long)==4&&sizeof(TItemAttrTable)==69);desc_test_allow_clock();test_server=0;Runtime runtime;sql_test_runtime=&runtime;LogManager log;logger=&log;runtime.db=log.m_sql.GetSQLHandle();assert(log.Connect("host","user","password","database"));g_table_postfix="_tail";quest::CQuestManager quests;ITEM_MANAGER items;manager=&items;CHARACTER owner;owner.m_dwPlayerID=777;owner.m_stName="tail-owner";owner.m_vid=VID(7000,55);SECTREE sector;owner.SetSectree(&sector);output=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe descriptor;descriptor.Attach(watch,5,output);descriptor.SetPacketState(5);owner.BindDesc(&descriptor);thecore_memcpy=Copy;
 PlainCases(owner);NormalCases(owner);RareCases(owner);Maps();thecore_memcpy=std::memcpy;owner.BindDesc(0);owner.SetSectree(0);descriptor.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();std::puts("attribute tail passed:10 complete original methods, copy/save semantics, addon change and rare quirks with actual packet/SQL/events");}
