#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/db.h"
#include "../../game/src/char.h"
#undef protected
#undef private
#include "../../game/src/questmanager.h"
#include "../../game/src/desc_client.h"
#include "../../game/src/event.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include "sql_connection_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cstdio>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
struct Runtime : SqlRuntime {
    int init,destroy,close;std::vector<MYSQL*> expected;
    Runtime():init(0),destroy(0),close(0){}
    virtual int SemInit(sem_t* s,int shared,unsigned int value){assert(s&&!shared&&!value);*s=reinterpret_cast<sem_t>(1);++init;return 0;}
    virtual int SemDestroy(sem_t* s){assert(s&&*s);++destroy;return 0;}
    virtual void Close(MYSQL* h){assert((unsigned)close<expected.size()&&h==expected[close]);++close;}
};
class ClientProbe:public CLIENT_DESC{public:void Output(LPBUFFER b){m_lpOutputBuffer=b;}};
static DWORD Read4(const BYTE* p){return (DWORD)p[0]|(DWORD)p[1]<<8|(DWORD)p[2]<<16|(DWORD)p[3]<<24;}
struct Operation {BYTE header,pos,type,itempos;Operation(BYTE h,BYTE p,BYTE t=0,BYTE i=0):header(h),pos(p),type(t),itempos(i){}};
static CHARACTER* active;static std::vector<Operation> ops;static unsigned used;
static void* Copy(void* out,const void* in,size_t n){
    const BYTE* p=static_cast<const BYTE*>(in);
    if(active&&((n==2&&p[0]==29)||(n==4&&p[0]==28))){
        assert(used<ops.size());const Operation& o=ops[used++];assert(p[0]==o.header&&p[1]==o.pos);
        assert(active->m_quickslot[o.pos].type==o.type&&active->m_quickslot[o.pos].pos==o.itempos);
        if(n==4)assert(p[2]==o.type&&p[3]==o.itempos);
    }
    return std::memcpy(out,in,n);
}
static void Begin(CHARACTER& ch,LPBUFFER b){buffer_reset(b);std::memset(ch.m_quickslot,0,sizeof(ch.m_quickslot));ops.clear();used=0;active=&ch;}
static void End(LPBUFFER b){assert(used==ops.size());unsigned bytes=0;for(unsigned i=0;i<ops.size();++i)bytes+=ops[i].header==29?2:4;assert(buffer_size(b)==(int)bytes);active=0;}
static void Grid(CHARACTER& ch){
    BYTE* g=ch.m_pointsInstant.bItemGrid;
    std::memset(g,0,sizeof(ch.m_pointsInstant.bItemGrid));
    for(int c=0;c<90;++c){
        assert(ch.IsEmptyItemGrid((BYTE)c,1,-1));
        assert(ch.IsEmptyItemGrid((BYTE)c,0,-1)==(c+5<90&&(c+5)/45==c/45));
        for(int s=2;s<=10;++s){const int last=c+5*(s-1);const bool fits=last<90&&last/45==c/45;assert(ch.IsEmptyItemGrid((BYTE)c,(BYTE)s,-1)==fits);}
    }
    assert(!ch.IsEmptyItemGrid(90,1,-1)&&!ch.IsEmptyItemGrid(255,1,-1));
    assert(ch.GetEmptyInventory(1)==0&&ch.GetEmptyInventory(0)==0);
    g[5]=7;assert(!ch.IsEmptyItemGrid(0,0,-1)&&!ch.IsEmptyItemGrid(0,2,-1));assert(ch.IsEmptyItemGrid(0,1,-1));
    assert(ch.IsEmptyItemGrid(0,2,6));assert(!ch.IsEmptyItemGrid(0,2,INT_MAX)&&!ch.IsEmptyItemGrid(0,2,INT_MIN));
    g[0]=7;assert(ch.IsEmptyItemGrid(0,2,6));assert(!ch.IsEmptyItemGrid(0,1,-1));
    g[5]=0;assert(ch.IsEmptyItemGrid(0,0,6));
    std::memset(g,1,90);assert(ch.GetEmptyInventory(1)==-1);g[44]=0;assert(ch.GetEmptyInventory(1)==44&&ch.GetEmptyInventory(0)==-1);
    g[45]=0;g[50]=0;assert(ch.GetEmptyInventory(0)==45&&ch.GetEmptyInventory(2)==45);
    assert(!ch.IsEmptyItemGrid(0,255,0)); // same-page failure before BYTE wrap.
    std::memset(g,0,sizeof(ch.m_pointsInstant.bItemGrid));
}
int main(){
    assert(sizeof(DWORD)==4&&sizeof(long)==4&&sizeof(TUseTime)==56&&sizeof(TPacketMoneyLog)==9);
    desc_test_allow_clock();Runtime runtime;sql_test_runtime=&runtime;
    {
        DBManager db;assert(DBManager::instance_ptr()==&db&&!db.m_bIsConnect&&runtime.init==2);
        assert(db.m_map_dbstring.empty()&&db.m_vec_GreetMessage.empty()&&db.m_map_pkLoginData.empty()&&db.mapLDBilling.empty()&&db.m_vec_kUseTime.empty());
        assert(!db.m_sql.m_mtxQuery&&!db.m_sql_direct.m_mtxQuery&&!db.m_sql.m_hThread&&!db.m_sql_direct.m_hThread);
        runtime.expected.push_back(&db.m_sql_direct.m_hDB);runtime.expected.push_back(&db.m_sql.m_hDB);
        // Exercise actual STL teardown as well as both real SQL destructors.
        db.m_map_dbstring["x"]="y";db.m_vec_GreetMessage.push_back("greet");TUseTime use;std::memset(&use,0,sizeof(use));db.m_vec_kUseTime.push_back(use);
        db_clientdesc=0;db.SendMoneyLog(255,0xffffffffUL,0);
        ClientProbe client;LPBUFFER output=buffer_new(0,8192);client.Output(output);db_clientdesc=&client;
        const int values[]={1,-1,INT_MIN,INT_MAX};
        for(unsigned i=0;i<4;++i){buffer_reset(output);BYTE type=i&1?255:0;DWORD vnum=i&1?0xffffffffUL:0;db.SendMoneyLog(type,vnum,values[i]);assert(buffer_size(output)==18);const BYTE* p=static_cast<const BYTE*>(buffer_read_peek(output));assert(p[0]==50&&Read4(p+1)==0&&Read4(p+5)==9&&p[9]==type&&Read4(p+10)==vnum&&Read4(p+14)==static_cast<DWORD>(values[i]));}
        db_clientdesc=0;client.Output(0);buffer_delete(output);
    }
    assert(!DBManager::instance_ptr()&&runtime.close==2&&runtime.destroy==2);
    quest::CQuestManager quests;CHARACTER ch;Grid(ch);LPFDWATCH watch=fdwatch_new(16);LPBUFFER output=buffer_new(0,8192);DescOutputProbe d;d.Attach(watch,5,output);d.SetPacketState(5);ch.BindDesc(&d);thecore_memcpy=Copy;
    for(int i=0;i<36;++i){TQuickslot* p=0;assert(ch.GetQuickslot((BYTE)i,&p)&&p==&ch.m_quickslot[i]);}
    TQuickslot* sentinel=&ch.m_quickslot[0];assert(!ch.GetQuickslot(36,&sentinel)&&sentinel==&ch.m_quickslot[0]);assert(!ch.GetQuickslot(255,&sentinel));
    const BYTE types[]={0,1,1,2,2,3,3,4,255};const BYTE positions[]={0,89,90,254,255,0,255,0,0};
    for(unsigned i=0;i<9;++i){Begin(ch,output);TQuickslot s;s.type=types[i];s.pos=positions[i];const bool ok=(i==1||i==3||i==5||i==6);if(ok)ops.push_back(Operation(28,35,s.type,s.pos));assert(ch.SetQuickslot(35,s)==ok);End(output);for(int j=0;j<36;++j){assert(ch.m_quickslot[j].type==((ok&&j==35)?s.type:0));assert(ch.m_quickslot[j].pos==((ok&&j==35)?s.pos:0));}}
    Begin(ch,output);TQuickslot s;s.type=1;s.pos=90;ch.m_quickslot[2]=s;ch.m_quickslot[34]=s;ops.push_back(Operation(29,2));ops.push_back(Operation(29,34));assert(!ch.SetQuickslot(0,s));End(output);
    Begin(ch,output);s.type=2;s.pos=255;ch.m_quickslot[1]=s;ops.push_back(Operation(29,1));assert(!ch.SetQuickslot(0,s));End(output);
    Begin(ch,output);s.type=1;s.pos=89;ch.m_quickslot[2]=s;ch.m_quickslot[34]=s;ops.push_back(Operation(29,2));ops.push_back(Operation(29,34));ops.push_back(Operation(28,0,1,89));assert(ch.SetQuickslot(0,s));End(output);
    Begin(ch,output);ch.m_quickslot[7].type=3;ch.m_quickslot[7].pos=255;ops.push_back(Operation(29,7));assert(!ch.SetQuickslot(0,ch.m_quickslot[7]));End(output);assert(ch.m_quickslot[7].type==0&&ch.m_quickslot[7].pos==0);
    Begin(ch,output);s.type=3;s.pos=99;ch.m_quickslot[1]=s;assert(!ch.SetQuickslot(36,s)&&!ch.SetQuickslot(255,s));assert(!ch.DelQuickslot(36)&&!ch.DelQuickslot(255));assert(ch.m_quickslot[1].type==3&&ch.m_quickslot[1].pos==99);End(output);
    Begin(ch,output);ch.m_quickslot[35].type=255;ch.m_quickslot[35].pos=17;ops.push_back(Operation(29,35));assert(ch.DelQuickslot(35));End(output);
    ch.BindDesc(0);buffer_reset(output);s.type=3;s.pos=99;assert(ch.SetQuickslot(5,s)&&ch.m_quickslot[5].pos==99&&!buffer_size(output));assert(!ch.DelQuickslot(36));
    thecore_memcpy=std::memcpy;d.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();sql_test_runtime=0;
    std::puts("autogive closed dependencies passed");
}


