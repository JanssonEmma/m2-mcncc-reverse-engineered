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
    assert(activeItem&&!socketMode&&!std::strcmp(function,"AddAttribute")&&line==82);
    assert(!std::strcmp(format,"item attribute overflow!")&&!packets&&!logs);++overflows;
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
    packets=logs=clamps=overflows=0;item.SetSkipSave(skip);
    for(int i=0;i<3;++i)socketBefore[i]=item.GetSockets()[i];
}
static void Finish(int expectedPackets,int expectedLogs,int expectedClamps,int expectedOverflows,bool mutated=true) {
    assert(packets==expectedPackets&&logs==expectedLogs&&clamps==expectedClamps&&overflows==expectedOverflows);
    assert(buffer_size(output)==static_cast<size_t>(36*packets));
    assert((manager->m_set_pkItemForDelayedSave.count(activeItem)!=0)==(dirty&&mutated));
    // Unowned mutations still save; packet count alone does not imply mutation.
    for(int i=0;i<logs;++i) {
        SQLMsg* m=logger->m_sql.m_queue_query.front();logger->m_sql.m_queue_query.pop();
        char expected[512];DWORD who=socketMode?i:indexExpected;
        DWORD x=socketMode?1:typeExpected;
        DWORD y=socketMode?0:static_cast<DWORD>(valueExpected);
        std::snprintf(expected,sizeof(expected),"INSERT DELAYED INTO log_attr (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), %lu, %lu, %lu, %lu, '%s', '')",who,x,y,activeItem->GetID(),socketMode?"SET_SOCKET":"SET_ATTR");
        assert(m->stQuery==expected&&!m->bReturn&&m->m_pkSQL==logger->m_sql.GetSQLHandle());delete m;
    }
    activeItem=0;manager->m_set_pkItemForDelayedSave.clear();
}
static void PureQueries(CItem& item,TItemTable& p) {
    assert(item.GetAttributeSetIndex()==-1); // No prototype is supported here.
    item.SetProto(&p);
    for(int type=0;type<256;++type)for(int subtype=0;subtype<256;++subtype) {
        p.bType=static_cast<BYTE>(type);p.bSubType=static_cast<BYTE>(subtype);
        const int armor[7]={1,5,6,2,3,4,7};
        int expected=type==1?(subtype==6?-1:0):(type==2&&subtype<7?armor[subtype]:-1);
        assert(item.GetAttributeSetIndex()==expected);
    }
    std::memset(item.m_aAttr,0,sizeof(item.m_aAttr));
    p.aApplies[2].bType=71;
    item.m_aAttr[4].bType=72;item.m_aAttr[5].bType=73;item.m_aAttr[6].bType=74;
    assert(item.HasAttr(71)&&item.HasAttr(72)&&!item.HasAttr(73)&&!item.HasAttr(74));
    assert(item.FindAttribute(71)==-1&&item.FindAttribute(72)==4&&item.FindAttribute(73)==-1);
    assert(item.FindAttribute(0)==0&&item.GetAttributeCount()==0);
    item.m_aAttr[0].bType=72;
    assert(item.GetAttributeCount()==1&&item.FindAttribute(72)==0);
    for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
    assert(item.GetAttributeCount()==5);
}
int main() {
    assert(sizeof(long)==4&&sizeof(DWORD)==4&&sizeof(TPlayerItemAttribute)==3);
    desc_test_allow_clock();test_server=0;
    Runtime runtime;sql_test_runtime=&runtime;
    LogManager log;logger=&log;runtime.db=log.m_sql.GetSQLHandle();assert(log.Connect("host","user","password","database"));g_table_postfix="_attr";
    quest::CQuestManager quests;ITEM_MANAGER items;manager=&items;CHARACTER owner;owner.m_dwPlayerID=777;owner.m_stName="attr-owner";owner.m_vid=VID(7000,55);
    SECTREE sector;owner.SetSectree(&sector);
    output=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe descriptor;descriptor.Attach(watch,5,output);descriptor.SetPacketState(5);owner.BindDesc(&descriptor);
    thecore_memcpy=Copy;
    { CItem item;item.Initialize();TItemTable p;std::memset(&p,0,sizeof(p));PureQueries(item,p); }
    // Complete mutation paths, repeat writes, signed boundaries, bType zero,
    // unowned packet suppression and skip-save suppression.
    const short values[]={SHRT_MIN,-1,0,SHRT_MAX};
    for(int isOwned=0;isOwned<2;++isOwned)for(int skip=0;skip<2;++skip)for(int type=0;type<2;++type)for(int i=0;i<5;++i)for(unsigned v=0;v<4;++v) {
        CItem item;item.Initialize();TItemTable p;std::memset(&p,0,sizeof(p));item.SetProto(&p);item.SetID(1234);item.m_dwCount=300;if(isOwned)item.SetCell(&owner,107);
        indexExpected=i;typeExpected=type?255:0;valueExpected=values[v];
        for(int repeat=0;repeat<2;++repeat) {
            Start(item,isOwned!=0,skip!=0,false);item.SetAttribute(i,static_cast<BYTE>(typeExpected),values[v]);



            Finish(isOwned,type?1:0,0,0);
        }
        item.SetCell(0,0);
    }
    const int requests[]={INT_MIN,-1,0,1,2,3,4,INT_MAX};
    for(unsigned n=0;n<8;++n) {
        CItem item;item.Initialize();TItemTable p;std::memset(&p,0,sizeof(p));std::strcpy(p.szLocaleName,"socketed");item.SetProto(&p);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
        item.m_alSockets[0]=1;item.m_alSockets[1]=22;item.m_alSockets[2]=33;
        int count=requests[n]>2?3:(requests[n]>0?requests[n]:0);
        Start(item,true,false,true);item.AlterToSocketItem(requests[n]);Finish(count,count,requests[n]>2,0,count!=0);
        item.SetCell(0,0);
    }
    {
        CItem item;item.Initialize();TItemTable p;std::memset(&p,0,sizeof(p));item.SetProto(&p);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
        // Add at first hole while preserving following entries and rare slots.
        item.m_aAttr[0].bType=1;item.m_aAttr[2].bType=2;item.m_aAttr[5].bType=71;p.aApplies[2].bType=72;
        indexExpected=1;typeExpected=71;valueExpected=-32768;
        Start(item,true,false,false);item.AddAttribute(71,-32768);Finish(1,1,0,0);
        assert(item.m_aAttr[2].bType==2&&item.m_aAttr[5].bType==71);
        Start(item,true,false,false);item.AddAttribute(71,5);item.AddAttribute(72,5);item.AddAttribute(73,0);Finish(0,0,0,0,false);
        for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
        Start(item,true,false,false);item.AddAttribute(80,0);Finish(0,0,0,1,false); // overflow is checked before zero value.
        item.SetCell(0,0);
    }
    thecore_memcpy=std::memcpy;owner.BindDesc(0);owner.SetSectree(0);descriptor.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();
    std::puts("item attributes/socket conversion passed: real packet/save/SQL graph and all query domains");
}
