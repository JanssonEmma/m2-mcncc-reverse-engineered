#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/item.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/event.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
struct Op {BYTE header,index,type,pos;Op(BYTE h,BYTE i,BYTE t=0,BYTE p=0):header(h),index(i),type(t),pos(p){}};
static CHARACTER* active;static std::vector<Op> expected;static unsigned used;
static int injectAt=-1;static BYTE injectType,injectPos;
static void* Copy(void* out,const void* in,size_t n){
 const BYTE* b=static_cast<const BYTE*>(in);
 if(active&&((n==2&&b[0]==29)||(n==4&&b[0]==28))){
  assert(used<expected.size());const Op& o=expected[used++];assert(b[0]==o.header&&b[1]==o.index);
  assert(active->m_quickslot[o.index].type==o.type&&active->m_quickslot[o.index].pos==o.pos);
  if(n==4)assert(b[2]==o.type&&b[3]==o.pos);
  if(injectAt>=0){active->m_quickslot[injectAt].type=injectType;active->m_quickslot[injectAt].pos=injectPos;injectAt=-1;}
 }
 return std::memcpy(out,in,n);
}
static void Slot(CHARACTER& ch,int index,BYTE type,BYTE pos){ch.m_quickslot[index].type=type;ch.m_quickslot[index].pos=pos;}
static void Begin(CHARACTER& ch,LPBUFFER out){std::memset(ch.m_quickslot,0,sizeof(ch.m_quickslot));buffer_reset(out);expected.clear();used=0;active=&ch;injectAt=-1;}
static void End(LPBUFFER out){assert(used==expected.size());unsigned count=0;for(unsigned i=0;i<expected.size();++i)count+=expected[i].header==29?2:4;assert(buffer_size(out)==(int)count);active=0;}
int main(){
 desc_test_allow_clock();quest::CQuestManager quests;CHARACTER ch;
 LPFDWATCH watch=fdwatch_new(16);LPBUFFER out=buffer_new(0,8192);DescOutputProbe desc;desc.Attach(watch,5,out);desc.SetPacketState(5);ch.BindDesc(&desc);thecore_memcpy=Copy;
 // Sync scans LIVE all36: later rewrite deletes earlier rewrite as duplicate.
 Begin(ch,out);Slot(ch,2,1,10);Slot(ch,4,1,10);Slot(ch,30,1,20);
 expected.push_back(Op(29,30));expected.push_back(Op(28,2,1,20));expected.push_back(Op(29,2));expected.push_back(Op(28,4,1,20));
 ch.SyncQuickslot(1,10,20);End(out);assert(ch.m_quickslot[4].type==1&&ch.m_quickslot[4].pos==20&&!ch.m_quickslot[2].type&&!ch.m_quickslot[30].type);
 Begin(ch,out);Slot(ch,0,231,17);Slot(ch,35,231,17);expected.push_back(Op(29,0));expected.push_back(Op(29,35));ch.SyncQuickslot(231,17,255);End(out);
 // Unknown type is passed to real SetQuickslot and rejected, not filtered here.
 Begin(ch,out);Slot(ch,1,231,17);ch.SyncQuickslot(231,17,19);End(out);assert(ch.m_quickslot[1].type==231&&ch.m_quickslot[1].pos==17);
 // Invalid new itemcell still deletes preexisting duplicate before returningfalse.
 Begin(ch,out);Slot(ch,1,1,89);Slot(ch,20,1,90);expected.push_back(Op(29,20));ch.SyncQuickslot(1,89,90);End(out);assert(ch.m_quickslot[1].pos==89&&!ch.m_quickslot[20].type);
 Begin(ch,out);for(int i=0;i<36;++i)expected.push_back(Op(29,(BYTE)i));ch.SyncQuickslot(0,0,255);End(out);
 // Reentrant external packet-copy observer adds a later matching slot: scan sees it.
 Begin(ch,out);Slot(ch,1,2,10);injectAt=35;injectType=2;injectPos=10;expected.push_back(Op(29,1));expected.push_back(Op(29,35));ch.SyncQuickslot(2,10,255);End(out);
 TItemTable proto;std::memset(&proto,0,sizeof proto);proto.dwVnum=1234;
 CItem item;item.Initialize();item.SetProto(&proto);item.m_bCell=20;
 // Chain touches only first oldmatch; later oldmatch survives, new duplicates deleted.
 Begin(ch,out);Slot(ch,2,1,10);Slot(ch,4,1,10);Slot(ch,30,1,20);expected.push_back(Op(29,30));expected.push_back(Op(28,2,1,20));ch.ChainQuickslotItem(&item,1,10);End(out);assert(ch.m_quickslot[4].type==1&&ch.m_quickslot[4].pos==10);
 // Unlike Sync, Chain does not skip equal positions; real Set deletes all then adds first.
 Begin(ch,out);item.m_bCell=10;Slot(ch,2,1,10);Slot(ch,4,1,10);expected.push_back(Op(29,2));expected.push_back(Op(29,4));expected.push_back(Op(28,2,1,10));ch.ChainQuickslotItem(&item,1,10);End(out);
 Begin(ch,out);ch.ChainQuickslotItem(0,1,77);End(out); // No match: null item never read.
 Begin(ch,out);item.m_bCell=255;Slot(ch,2,1,10);Slot(ch,30,1,255);expected.push_back(Op(29,30));ch.ChainQuickslotItem(&item,1,10);End(out);assert(ch.m_quickslot[2].pos==10);
 // Actual FindSpecifyItem scans only inventory90 and preserves first-match identity.
 CItem other,empty;other.Initialize();other.SetProto(&proto);empty.Initialize();
 std::memset(ch.m_pointsInstant.pItems,0,sizeof(ch.m_pointsInstant.pItems));ch.m_pointsInstant.pItems[89]=&item;ch.m_pointsInstant.pItems[90]=&other;assert(ch.FindSpecifyItem(1234)==&item);ch.m_pointsInstant.pItems[0]=&other;assert(ch.FindSpecifyItem(1234)==&other);assert(!ch.FindSpecifyItem(9999));ch.m_pointsInstant.pItems[0]=0;ch.m_pointsInstant.pItems[89]=0;assert(!ch.FindSpecifyItem(1234));ch.m_pointsInstant.pItems[5]=&empty;assert(ch.FindSpecifyItem(0)==&empty);std::memset(ch.m_pointsInstant.pItems,0,sizeof(ch.m_pointsInstant.pItems));
 // Descriptorless paths that do not reach old unguarded DelQuickslot are valid.
 ch.BindDesc(0);Begin(ch,out);Slot(ch,1,1,10);ch.SyncQuickslot(1,10,10);End(out);assert(ch.m_quickslot[1].pos==10);ch.ChainQuickslotItem(0,1,77);
 thecore_memcpy=std::memcpy;desc.Attach(0,-1,0);buffer_delete(out);fdwatch_delete(watch);event_destroy();
 std::puts("quickslot chain passed: actual live36 scan, firstmatch, packet order, callback changes and inventory identity");
}
