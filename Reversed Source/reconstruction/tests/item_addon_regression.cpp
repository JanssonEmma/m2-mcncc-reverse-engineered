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
extern "C" long item_addon_random() { assert(rngAt<rng.size());return rng[rngAt++]; }
static void Rand(long x) { rng.push_back(x); }
static void RngDone() { assert(rngAt==rng.size());rng.clear();rngAt=0; }
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
extern "C" void item_attr_clamp(int level,int value) {
    assert(activeItem&&socketMode&&level==0&&value==3&&!packets&&!logs);++clamps;
}
extern "C" void item_attr_overflow(const char* function,int line,const char* format) {
    if(!std::strcmp(function,"ApplyAddonTo")) { assert(allowNull&&line==17&&!std::strcmp(format,"ITEM pointer null"));return; }
    assert(activeItem&&!socketMode&&!std::strcmp(function,"AddAttribute")&&line==82);
    assert(!std::strcmp(format,"item attribute overflow!"));++overflows;
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
    virtual int MutexUnlock(pthread_mutex_t* m) { assert(m==logger->m_sql.m_mtxQuery&&depth==1);Saved();--depth;++logs;return 0; }
};
static void Start(CItem& item,bool isOwned,bool skip,bool socket) {
    assert(!activeItem&&logger->m_sql.m_queue_query.empty());
    manager->m_set_pkItemForDelayedSave.clear();buffer_reset(output);
    activeItem=&item;owned=isOwned;dirty=!skip;socketMode=socket;
    packets=logs=clamps=overflows=0;ops.clear();item.SetSkipSave(skip);
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

static void GaussianPair() {
    // Reject radius>=1 and radius==0, then accept v1=.5,v2=0.
    Rand(0);Rand(0);Rand(0x100000);Rand(0x100000);
    Rand(0x580000);Rand(0x500000); // high bits must be masked.
    float a=gauss_random(2.0f,3.0f);
    assert(rngAt==6);
    const double n=std::sqrt(-2.0*std::log(.25));
    assert(std::fabs(a-static_cast<float>(2+3*n))<.00001f);
    assert(gauss_random(-7.0f,123.0f)==-7.0f); // cached0, no RNG.
    RngDone();
    // Exact-zero guard permits this tiny positive radius (reference differs).
    Rand(0x100001);Rand(0x100000);
    a=gauss_random(0,5);assert(a>30 && a<40);
    assert(gauss_random(0,5)==0);RngDone();
}
static void FiniteRemoval() {
    CItem item;item.Initialize();TItemTable p;std::memset(&p,0,sizeof(p));item.SetProto(&p);
    for(int i=0;i<7;++i){item.m_aAttr[i].bType=static_cast<BYTE>(100+i);item.m_aAttr[i].sValue=static_cast<short>(-100-i);}
    TPlayerItemAttribute before[7];std::memcpy(before,item.m_aAttr,sizeof(before));
    assert(!item.RemoveAttributeType(71)); // missing->false
    assert(!item.RemoveAttributeType(102)); // find102=index2 -> find2=missing -> false
    assert(!item.RemoveAttributeType(105)); // rare slot ignored
    assert(!std::memcmp(before,item.m_aAttr,sizeof(before)));
    // Static cycle examples (NOT executed): attr[0].type=0 -> recurse(0)
    // forever; attr[0].type=1 and attr[1].type=0 ->0->1->0 cycle.
}
int main() {
    assert(sizeof(long)==4&&sizeof(DWORD)==4&&sizeof(TPlayerItemAttribute)==3);
    desc_test_allow_clock();test_server=0;
    Runtime runtime;sql_test_runtime=&runtime;
    LogManager log;logger=&log;runtime.db=log.m_sql.GetSQLHandle();assert(log.Connect("host","user","password","database"));g_table_postfix="_attr";
    quest::CQuestManager quests;ITEM_MANAGER items;manager=&items;CHARACTER owner;owner.m_dwPlayerID=777;owner.m_stName="addon-owner";owner.m_vid=VID(7000,55);
    SECTREE sector;owner.SetSectree(&sector);
    output=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe descriptor;descriptor.Attach(watch,5,output);descriptor.SetPacketState(5);owner.BindDesc(&descriptor);
    thecore_memcpy=Copy;
    GaussianPair();FiniteRemoval();
    assert(!CItemAddonManager::instance_ptr());
    {
        CItemAddonManager addons;assert(CItemAddonManager::instance_ptr()==&addons);
        allowNull=true;addons.ApplyAddonTo(INT_MIN,0);allowNull=false;RngDone();
        // Each pair consumes an actual Gaussian then its cached zero value.
        const int raw[]={0x180000,0x080000,0x100000+17367,0x100000+11468,0x100000-14148,0x100000-9249,0x100001,0x0fffff};
        const int skill[]={8,-7,20,21,-20,-21,30,-30};
        const int normal[]={1,31,-23,-41,57,43,-59,61};
        for(int k=0;k<8;++k)for(int preexisting=0;preexisting<2;++preexisting)for(int skip=0;skip<2;++skip) {
            for(int cached=0;cached<2;++cached) {
                CItem item;item.Initialize();TItemTable p;std::memset(&p,0,sizeof(p));item.SetProto(&p);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
                if(preexisting) {item.m_aAttr[0].bType=10;item.m_aAttr[1].bType=11;item.m_aAttr[2].bType=71;item.m_aAttr[2].sValue=123;}
                // Removal71 at index2 terminates through missing type2; leaves123.
                int skillValue=cached?0:skill[k];
                int normalValue=cached?17:normal[k];
                if(!cached){Rand(raw[k]);Rand(0x100000);}
                if(cached || (skillValue>=-20&&skillValue<=20)) {Rand(0);Rand(0);Rand(0);}
                else Rand(0);
                Start(item,true,skip!=0,false);
                int slot=preexisting?3:0;Expect(slot,72,static_cast<short>(normalValue));
                if(!preexisting&&skillValue)Expect(slot+1,71,static_cast<short>(skillValue));
                item.ApplyAddon(k%2?INT_MIN:INT_MAX);
                int count=static_cast<int>(ops.size());Finish(count,count,0,0);
                if(preexisting)assert(item.m_aAttr[2].bType==71&&item.m_aAttr[2].sValue==123);
                RngDone();item.SetCell(0,0);
            }
        }
        // Full5 normal slots: both real AddAttribute calls diagnose overflow,
        // including cached skill0 (overflow precedes zero-value suppression).
        for(int cached=0;cached<2;++cached) {
            CItem item;item.Initialize();TItemTable p;std::memset(&p,0,sizeof(p));item.SetProto(&p);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
            for(int i=0;i<5;++i){item.m_aAttr[i].bType=static_cast<BYTE>(10+i);item.m_aAttr[i].sValue=123;}
            TPlayerItemAttribute before[7];std::memcpy(before,item.m_aAttr,sizeof(before));
            if(!cached){Rand(0x180000);Rand(0x100000);}
            Rand(0);Rand(0);Rand(0);
            Start(item,true,false,false);item.ApplyAddon(0);Finish(0,0,0,2,false);
            assert(!std::memcmp(before,item.m_aAttr,sizeof(before)));RngDone();item.SetCell(0,0);
        }
    }
    assert(!CItemAddonManager::instance_ptr());
    { CItemAddonManager* heap=new CItemAddonManager;assert(CItemAddonManager::instance_ptr()==heap);delete heap; }
    assert(!CItemAddonManager::instance_ptr());
    { struct DerivedAddon : CItemAddonManager {}; DerivedAddon derived;assert(CItemAddonManager::instance_ptr()==&derived); }
    assert(!CItemAddonManager::instance_ptr());
    thecore_memcpy=std::memcpy;owner.BindDesc(0);owner.SetSectree(0);descriptor.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();
    std::puts("addon passed: actual gaussian/cache/RNG, singleton lifecycle, finite original recursion, full packet/save/SQL");
}
