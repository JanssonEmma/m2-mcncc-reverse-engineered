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
static int packets,logs,clamps,overflows,depth;
static bool owned,dirty,socketMode;
static int indexExpected,typeExpected;
static short valueExpected;
static long socketBefore[3];
struct Op { int i,t;short v;Op(int a,int b,short c):i(a),t(b),v(c){} };
static std::vector<Op> ops;
static std::vector<long> rng;
static unsigned rngAt;
static bool allowNull;
static int reverseCalls,cannotCalls;
static unsigned mutateAt=UINT_MAX;
static int mutateKind,mutateSet;
static bool mutateLocale;

extern "C" long weighted_test_random() {
    assert(rngAt<rng.size());unsigned at=rngAt++;long raw=rng[at];
    if(at==mutateAt) {
        if(mutateKind==1)g_map_itemAttr[7].bMaxLevelBySet[mutateSet]=0;
        if(mutateKind==2){g_map_itemAttr[7].dwProb=0;g_map_itemAttr[8].dwProb=5;}
    }
    return raw;
}
static void Rand(long x) { rng.push_back(x); }
static void RngDone() { assert(rngAt==rng.size());rng.clear();rngAt=0;mutateAt=UINT_MAX;mutateKind=0; }
static void Expect(int i,int t,short v) { ops.push_back(Op(i,t,v)); }

static void* Copy(void* out,const void* in,size_t count) {
    if(activeItem && count==36 && static_cast<const BYTE*>(in)[0]==25) {
        const BYTE* b=static_cast<const BYTE*>(in);
        assert(owned && b[1]==107 && b[2]==44);
        assert(logger->m_sql.m_queue_query.size()==static_cast<unsigned>(logs));
        if(!packets) assert(manager->m_set_pkItemForDelayedSave.empty());
        if(socketMode) {
            assert(activeItem->GetSockets()[packets]==1);
            for(int i=0;i<3;++i) {
                long wire;std::memcpy(&wire,b+3+i*4,4);
                assert(wire==(i<=packets?1:socketBefore[i]));
            }
        } else {
            assert(static_cast<unsigned>(logs)<ops.size());indexExpected=ops[logs].i;typeExpected=ops[logs].t;valueExpected=ops[logs].v;
            TPlayerItemAttribute wire[7];std::memcpy(wire,b+15,sizeof(wire));
            assert(!std::memcmp(wire,activeItem->GetAttributes(),sizeof(wire)));
            assert(wire[indexExpected].bType==typeExpected&&wire[indexExpected].sValue==valueExpected);
        }
        ++packets;
    }
    return std::memcpy(out,in,count);
}
struct Runtime : SqlRuntime {
    MYSQL* db;
    Runtime():db(0){}
    virtual int SemInit(sem_t* s,int shared,unsigned int value) { assert(!shared&&!value);*s=reinterpret_cast<sem_t>(1);return 0; }
    virtual int SemDestroy(sem_t* s) { assert(s&&*s);return 0; }
    virtual int MutexInit(pthread_mutex_t* m,const pthread_mutexattr_t* a) { assert(m&&!a);std::memset(m,0,sizeof(*m));return 0; }
    virtual int Create(pthread_t*,const pthread_attr_t* a,void*(*entry)(void*),void* arg) { assert(!a&&entry==AsyncSQLThread&&arg==&logger->m_sql);return 11; }
    virtual void Close(MYSQL* p) { assert(p==db); }
    virtual int MutexDestroy(pthread_mutex_t* m) { assert(m&&!depth);return 0; }
    void Saved() {
        assert(activeItem);
        assert((manager->m_set_pkItemForDelayedSave.count(activeItem)!=0)==dirty);
        assert(packets==(owned?logs+1:0));
    }
    virtual unsigned long Escape(MYSQL* p,char* to,const char* from,unsigned long count) { assert(p==db&&!count&&!*from);Saved();to[0]=0;return 0; }
    virtual int MutexLock(pthread_mutex_t* m) { assert(m==logger->m_sql.m_mtxQuery&&!depth);Saved();++depth;return 0; }
    virtual int SemPost(sem_t* s) { assert(s&&*s&&depth==1);Saved();assert(logger->m_sql.m_queue_query.size()==static_cast<unsigned>(logs+1));return 0; }
    virtual int MutexUnlock(pthread_mutex_t* m) { assert(m==logger->m_sql.m_mtxQuery&&depth==1);Saved();--depth;++logs;if(mutateLocale&&logs==1)g_iUseLocale=g_iUseLocale?0:1;return 0; }
};
static void Start(CItem& item,bool isOwned,bool skip,bool socket) {
    assert(!activeItem&&logger->m_sql.m_queue_query.empty());
    manager->m_set_pkItemForDelayedSave.clear();buffer_reset(output);
    activeItem=&item;owned=isOwned;dirty=!skip;socketMode=socket;
    packets=logs=clamps=overflows=reverseCalls=cannotCalls=0;ops.clear();item.SetSkipSave(skip);
    for(int i=0;i<3;++i)socketBefore[i]=item.GetSockets()[i];
}
static void Finish(int expectedPackets,int expectedLogs,int expectedClamps,int expectedOverflows,bool mutated=true) {
    assert(packets==expectedPackets&&logs==expectedLogs&&clamps==expectedClamps&&overflows==expectedOverflows);
    assert(buffer_size(output)==static_cast<size_t>(36*packets));
    assert((manager->m_set_pkItemForDelayedSave.count(activeItem)!=0)==(dirty&&mutated));
    // Unowned mutations still save; packet count alone does not imply mutation.
    for(int i=0;i<logs;++i) {
        SQLMsg* m=logger->m_sql.m_queue_query.front();logger->m_sql.m_queue_query.pop();
        char expected[512];DWORD who=ops[i].i;
        DWORD x=ops[i].t;
        DWORD y=static_cast<DWORD>(ops[i].v);
        std::snprintf(expected,sizeof(expected),"INSERT DELAYED INTO log_attr (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), %lu, %lu, %lu, %lu, '%s', '')",who,x,y,activeItem->GetID(),socketMode?"SET_SOCKET":"SET_ATTR");
        assert(m->stQuery==expected&&!m->bReturn&&m->m_pkSQL==logger->m_sql.GetSQLHandle());delete m;
    }
    activeItem=0;manager->m_set_pkItemForDelayedSave.clear();
}


extern "C" void weighted_test_error(const char* function,int line,const char* format,int a,int b,const char* file,int sourceLine) {
    assert(activeItem);
    if(!std::strcmp(function,"number_ex")) {
        assert(line==339&&a==1&&b==0&&!std::strcmp(file,"item_attribute.cpp")&&sourceLine==139);
        assert(!std::strcmp(format,"number(): first argument is bigger than second argument %d -> %d, %s %d"));
        ++reverseCalls;
    } else if(!std::strcmp(function,"AddAttr")) {
        assert(line==101&&!std::strcmp(format,"item attribute overflow!"));++overflows;
    } else {
        assert(!std::strcmp(function,"PutAttributeWithLevel")&&line==157);
        assert(!std::strcmp(format,"Cannot put item attribute %d %d")&&a>=0&&a<8&&b>=0&&b<=5);++cannotCalls;
    }
}
static TItemAttrTable rareSentinel;
static void Maps() {
    g_map_itemAttr.clear();g_map_itemRare.clear();
    std::memset(&rareSentinel,0,sizeof(rareSentinel));std::strcpy(rareSentinel.szApply,"rare-only");
    rareSentinel.dwApplyIndex=999;rareSentinel.dwProb=0xffffffffUL;
    for(int i=0;i<5;++i)rareSentinel.lValues[i]=(-2147483647L-1)+i;
    for(int i=0;i<8;++i)rareSentinel.bMaxLevelBySet[i]=static_cast<BYTE>(i+1);
    g_map_itemRare[999]=rareSentinel;
}
static void RareUnchanged() {
    assert(g_map_itemRare.size()==1&&g_map_itemRare.find(999)!=g_map_itemRare.end());
    assert(!std::memcmp(&g_map_itemRare.find(999)->second,&rareSentinel,sizeof(rareSentinel)));
}
static void Row(int key,DWORD weight,int set,int maximum) {
    TItemAttrTable r;std::memset(&r,0,sizeof(r));std::snprintf(r.szApply,sizeof(r.szApply),"row-%d",key);
    r.dwApplyIndex=777; // Selection must use KEY, not record's independently stored index.
    r.dwProb=weight;for(int i=0;i<5;++i)r.lValues[i]=key*100+i+1;
    r.bMaxLevelBySet[set]=static_cast<BYTE>(maximum);g_map_itemAttr[key]=r;
}
static void Attach(CItem& item,TItemTable& proto,CHARACTER& owner,int type=1,int subtype=0) {
    item.Initialize();std::memset(&proto,0,sizeof(proto));proto.bType=static_cast<BYTE>(type);proto.bSubType=static_cast<BYTE>(subtype);item.SetProto(&proto);
    item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
}
static void Begin(CItem& item,bool skip=false) { Start(item,true,skip,false);mutateLocale=false; }
static void Done(int overflow=0,int reverse=0,int cannot=0) {
    int n=static_cast<int>(ops.size());Finish(n,n,0,overflow,n!=0);
    assert(reverseCalls==reverse&&cannotCalls==cannot);RngDone();RareUnchanged();
}
static void QueryGuards(CHARACTER& owner) {
    const int types[]={0,1,2,99};const int subtypes[]={0,6,7,0};
    for(int i=0;i<4;++i) {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner,types[i],subtypes[i]);
        Begin(item);item.PutAttributeWithLevel(1);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    Maps();CItem noProto;noProto.Initialize();Begin(noProto);noProto.PutAttributeWithLevel(1);Done();assert(g_map_itemAttr.empty());
    Maps();CItem item;TItemTable proto;Attach(item,proto,owner);Begin(item);item.PutAttributeWithLevel(6);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
}
static void AddAttrCases(CHARACTER& owner) {
    const int levels[]={0,1,5,6,255};
    for(unsigned k=0;k<5;++k) {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner,99);Row(7,0,0,0);
        g_map_itemAttr[7].lValues[0]=0;g_map_itemAttr[7].lValues[4]=65536;
        Begin(item);if(levels[k]>1)Expect(0,7,0);item.AddAttr(7,static_cast<BYTE>(levels[k]));Done();
        assert(g_map_itemAttr.size()==1);item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);item.AddAttr(255,1);Done();assert(g_map_itemAttr.size()==1);
        TItemAttrTable zero;std::memset(&zero,0,sizeof(zero));assert(!std::memcmp(&g_map_itemAttr[255],&zero,sizeof(zero)));item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);proto.aApplies[2].bType=7;
        Begin(item);item.AddAttr(7,255);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
        Begin(item);item.AddAttr(7,1);Done(1);assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);Row(7,0,0,0);g_map_itemAttr[7].lValues[4]=2147483647;
        item.m_aAttr[5].bType=7;item.m_aAttr[5].sValue=55;
        Begin(item,true);Expect(0,7,-1);item.AddAttr(7,255);Done();assert(item.m_aAttr[5].sValue==55);item.SetCell(0,0);
    }
}
static void WeightedCases(CHARACTER& owner) {
    const int types[8]={1,2,2,2,2,2,2,2},subtypes[8]={0,0,3,4,5,1,2,6};
    for(int set=0;set<8;++set)for(int q=1;q<=10;++q) {
        Maps();Row(7,2,set,2);Row(8,3,set,4);Row(9,5,set,5);Row(90,0xffffffffUL,set,5);
        CItem item;TItemTable proto;Attach(item,proto,owner,types[set],subtypes[set]);
        Begin(item);int key=q<=2?7:q<=5?8:9;int cap=key==7?2:key==8?4:5;
        Expect(0,key,static_cast<short>(key*100+cap));Rand(q-1);item.PutAttributeWithLevel(5);Done();
        assert(g_map_itemAttr.size()==83&&g_map_itemAttr[90].dwProb==0xffffffffUL);item.SetCell(0,0);
    }
    // Zero level still scans/inserts maps and draws before AddAttr refuses level0.
    {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Rand(0);item.PutAttributeWithLevel(0);Done();assert(g_map_itemAttr.size()==82);item.SetCell(0,0);
    }
    // Empty distribution still number_ex(1,0): original reversal diagnostic + draw.
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Rand(0);item.PutAttributeWithLevel(1);Done(0,1,1);assert(g_map_itemAttr.size()==82);
        TItemAttrTable zero;std::memset(&zero,0,sizeof(zero));for(int i=0;i<82;++i)assert(!std::memcmp(&g_map_itemAttr[i],&zero,sizeof(zero)));item.SetCell(0,0);
    }
    // An eligible zero-weight row can be chosen by original swapped-range result0.
    for(int raw=0;raw<2;++raw) {
        Maps();Row(7,0,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);if(!raw)Expect(0,7,701);Rand(raw);item.PutAttributeWithLevel(1);Done(0,1,raw);item.SetCell(0,0);
    }
    // Unsigned weights sum with signed32 wrap; no saturation/rejection.
    {
        Maps();Row(7,0xffffffffUL,0,5);Row(8,2,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Expect(0,7,701);Rand(0);item.PutAttributeWithLevel(1);Done();item.SetCell(0,0);
    }
    // APPLY_NONE can be eligible when every proto/normal type is nonzero,
    // but selected key0 remains the original failure sentinel.
    {
        Maps();Row(0,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        for(int i=0;i<3;++i)proto.aApplies[i].bType=static_cast<BYTE>(90+i);
        for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
        Begin(item);Rand(0);item.PutAttributeWithLevel(1);Done(0,0,1);item.SetCell(0,0);
    }
    // External RNG callback mutates current record data: final cap is reread.
    {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);mutateAt=0;mutateKind=1;mutateSet=0;Rand(0);item.PutAttributeWithLevel(5);Done();item.SetCell(0,0);
    }
    // Availability/total already captured; selection weights remain live.
    {
        Maps();Row(7,2,0,5);Row(8,3,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Expect(0,8,801);mutateAt=0;mutateKind=2;Rand(0);item.PutAttributeWithLevel(1);Done();item.SetCell(0,0);
    }
}
static int Level(const int* weights,int percent) {
    int cumulative=0;for(int i=0;i<5;++i){cumulative+=weights[i];if(percent<=cumulative)return i+1;}return 6;
}
static void LevelCases(CHARACTER& owner) {
    const int expectedHigh[5]={30,40,20,8,2},expectedLow[5]={50,40,10,0,0};
    assert(!std::memcmp(aiItemMagicAttributePercentHigh,expectedHigh,sizeof(expectedHigh))&&!std::memcmp(aiItemMagicAttributePercentLow,expectedLow,sizeof(expectedLow)));
    for(int low=0;low<2;++low)for(int percent=1;percent<=100;++percent) {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        int weights[5];std::memcpy(weights,low?expectedLow:expectedHigh,sizeof(weights));
        Begin(item);Expect(0,7,static_cast<short>(700+Level(weights,percent)));Rand(percent-1);Rand(0);item.PutAttribute(weights);Done();
        assert(!std::memcmp(weights,low?expectedLow:expectedHigh,sizeof(weights)));item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);int weights[5]={0,0,0,0,0};
        Begin(item);Rand(0);item.PutAttribute(weights);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);int weights[5]={INT_MIN,0,0,0,0};
        Begin(item);Expect(0,7,702);Rand(0);Rand(0);item.PutAttribute(weights);Done();item.SetCell(0,0);
    }
}
static void PlanPut(int percentile) { Rand(percentile-1);Rand(0); }
static void MagicCases(CHARACTER& owner) {
    const int locales[]={0,1,-1,123},types[]={1,2,2},subtypes[]={0,0,1},sets[]={0,1,5};
    for(unsigned l=0;l<4;++l)for(int kind=0;kind<3;++kind)for(int passes=0;passes<4;++passes)for(int change=0;change<2;++change) {
        Maps();Row(7,1,sets[kind],5);Row(8,1,sets[kind],5);Row(9,1,sets[kind],5);
        CItem item;TItemTable proto;Attach(item,proto,owner,types[kind],subtypes[kind]);
        int second,third;
        if(locales[l]==0){second=kind==0?30:kind==1?20:10;third=kind==0?15:kind==1?10:5;}
        else {second=kind==0?20:10;third=kind==0?5:kind==1?2:1;}
        bool doSecond=(passes&1)!=0,doThird=(passes&2)!=0;
        Begin(item);g_iUseLocale=locales[l];mutateLocale=change!=0;Expect(0,7,705);PlanPut(100);
        Rand((doSecond?second:second+1)-1);if(doSecond){Expect(1,8,803);PlanPut(100);}
        Rand((doThird?third:third+1)-1);if(doThird){int key=doSecond?9:8;Expect(doSecond?2:1,key,static_cast<short>(key*100+1));PlanPut(1);}
        item.AlterToMagicItem();Done();item.SetCell(0,0);
    }
    // Existing attributes are retained; full capacity does not bypass any draws.
    {
        Maps();Row(7,1,0,5);Row(8,1,0,5);Row(9,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
        TPlayerItemAttribute before[7];std::memcpy(before,item.m_aAttr,sizeof(before));
        Begin(item);g_iUseLocale=0;PlanPut(100);Rand(0);PlanPut(100);Rand(0);PlanPut(1);
        item.AlterToMagicItem();Done(3);assert(!std::memcmp(before,item.m_aAttr,sizeof(before)));item.SetCell(0,0);
    }
    {
        Maps();Row(7,1,0,5);Row(8,1,0,5);Row(9,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        item.m_aAttr[0].bType=7;item.m_aAttr[0].sValue=42;
        Begin(item);g_iUseLocale=0;Expect(1,8,805);PlanPut(100);Rand(99);Rand(99);
        item.AlterToMagicItem();Done();assert(item.m_aAttr[0].sValue==42);item.SetCell(0,0);
    }
    for(int t=0;t<3;++t) {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner,t==0?1:t==1?2:99,t==0?6:t==1?7:0);
        Begin(item);item.AlterToMagicItem();Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    g_iUseLocale=0;
}
int main() {
    assert(sizeof(DWORD)==4&&sizeof(long)==4&&sizeof(TItemAttrTable)==69);
    assert(g_map_itemAttr.empty()&&g_map_itemRare.empty());
    desc_test_allow_clock();test_server=0;
    Runtime runtime;sql_test_runtime=&runtime;
    LogManager log;logger=&log;runtime.db=log.m_sql.GetSQLHandle();assert(log.Connect("host","user","password","database"));g_table_postfix="_attr";
    quest::CQuestManager quests;ITEM_MANAGER items;manager=&items;CHARACTER owner;owner.m_dwPlayerID=777;owner.m_stName="weighted-owner";owner.m_vid=VID(7000,55);
    SECTREE sector;owner.SetSectree(&sector);
    output=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe descriptor;descriptor.Attach(watch,5,output);descriptor.SetPacketState(5);owner.BindDesc(&descriptor);
    thecore_memcpy=Copy;
    QueryGuards(owner);AddAttrCases(owner);WeightedCases(owner);LevelCases(owner);MagicCases(owner);
    g_map_itemAttr.clear();g_map_itemRare.clear();
    thecore_memcpy=std::memcpy;owner.BindDesc(0);owner.SetSectree(0);descriptor.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();
    std::puts("weighted attributes passed: complete maps/record, all selection/locale branches, raw32 quirks and actual packet/save/SQL");
}
