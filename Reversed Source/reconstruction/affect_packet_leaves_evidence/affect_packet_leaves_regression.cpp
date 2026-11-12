// Actual affect flag/packet leaves with full descriptor, CHAR, DB framing and view graph.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/desc_client.h"
#include "../../game/src/sectree.h"
#include "../../game/src/packet.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
extern void SendAffectRemovePacket(LPDESC,DWORD,DWORD,BYTE);
struct ClientProbe:CLIENT_DESC { void Output(LPBUFFER b){m_lpOutputBuffer=b;} };
struct Expected {
    LPBUFFER output;std::vector<BYTE> bytes;
    Expected(LPBUFFER b,const std::vector<BYTE>& data):output(b),bytes(data){}
};
static std::vector<Expected> expected;
static std::vector<LPBUFFER> outputs;
static unsigned seen;
static bool active,swapDB,changeVID;
static CLIENT_DESC* alternateDB;
static CHARACTER* sourceToChange;
static CHARACTER* victimToChange;
static void* ObserveCopy(void* out,const void* in,size_t bytes) {
    bool packet=false;
    if(active)for(unsigned i=0;i<outputs.size();++i)if(out==outputs[i]->write_point)packet=true;
    if(packet) {
        assert(seen<expected.size());const Expected& e=expected[seen];
        assert(out==e.output->write_point && bytes==e.bytes.size());
        assert(!std::memcmp(in,&e.bytes[0],bytes));++seen;
        if(swapDB){swapDB=false;db_clientdesc=alternateDB;}
        if(changeVID){changeVID=false;sourceToChange->m_vid=VID(7,8);victimToChange->m_vid=VID(9,10);}
    }
    return std::memcpy(out,in,bytes);
}
static void Put4(std::vector<BYTE>& b,unsigned off,DWORD value){for(unsigned i=0;i<4;++i)b[off+i]=static_cast<BYTE>(value>>(i*8));}
static std::vector<BYTE> U4(DWORD v){std::vector<BYTE>b(4);Put4(b,0,v);return b;}
static void Begin(){active=false;expected.clear();seen=0;swapDB=false;changeVID=false;for(unsigned i=0;i<outputs.size();++i)buffer_reset(outputs[i]);}
static void End(){assert(seen==expected.size());active=false;}
static std::vector<BYTE> Affect(DWORD type,BYTE point){std::vector<BYTE>b(6);b[0]=127;Put4(b,1,type);b[5]=point;return b;}
static void ExpectDB(LPBUFFER out,DWORD pid,DWORD type,BYTE point){std::vector<BYTE>h(1,33),b(9);Put4(b,0,pid);Put4(b,4,type);b[8]=point;expected.push_back(Expected(out,h));expected.push_back(Expected(out,U4(0)));expected.push_back(Expected(out,U4(9)));expected.push_back(Expected(out,b));}
static std::vector<BYTE> Fly(BYTE type,DWORD start,DWORD end){std::vector<BYTE>b(10);b[0]=70;b[1]=type;Put4(b,2,start);Put4(b,6,end);return b;}
int main(){
    desc_test_allow_clock();quest::CQuestManager quests;
    CHARACTER source,victim,view,offline;
    DescOutputProbe ownDesc,viewDesc;
    ClientProbe db,otherDB;
    LPFDWATCH watch=fdwatch_new(16);
    LPBUFFER own=buffer_new(0,8192),viewer=buffer_new(0,8192),dbout=buffer_new(0,8192),otherout=buffer_new(0,8192);
    ownDesc.Attach(watch,5,own);ownDesc.SetPacketState(5);
    viewDesc.Attach(watch,6,viewer);viewDesc.SetPacketState(5);
    db.Output(dbout);otherDB.Output(otherout);db_clientdesc=&db;
    outputs.push_back(own);outputs.push_back(viewer);outputs.push_back(dbout);outputs.push_back(otherout);
    thecore_memcpy=ObserveCopy;
    for(unsigned flag=1;flag<=64;++flag){
        source.m_afAffectFlag.bits[0]=source.m_afAffectFlag.bits[1]=0;
        for(unsigned query=1;query<=64;++query)assert(!source.IsAffectFlag(query));
        source.m_afAffectFlag.bits[(flag-1)/32]=static_cast<DWORD>(1u<<((flag-1)%32));
        for(unsigned query=1;query<=64;++query)assert(source.IsAffectFlag(query)==(query==flag));
    }
    source.m_afAffectFlag.bits[0]=source.m_afAffectFlag.bits[1]=0xffffffffUL;
    for(unsigned flag=1;flag<=64;++flag)assert(source.IsAffectFlag(flag));
    const DWORD values[]={0,1,0x80000000UL,0xffffffffUL};
    for(unsigned i=0;i<4;++i)for(unsigned point=0;point<256;++point){
        Begin();db_clientdesc=&db;DWORD pid=values[i],type=values[3-i];
        expected.push_back(Expected(own,Affect(type,static_cast<BYTE>(point))));ExpectDB(dbout,pid,type,static_cast<BYTE>(point));active=true;
        SendAffectRemovePacket(&ownDesc,pid,type,static_cast<BYTE>(point));End();
        assert(buffer_size(own)==6 && buffer_size(dbout)==18 && !buffer_size(otherout));
    }
    // Actual global DB destination is read after the first real descriptor packet callback.
    Begin();db_clientdesc=&db;alternateDB=&otherDB;swapDB=true;
    expected.push_back(Expected(own,Affect(0x87654321UL,255)));ExpectDB(otherout,0xfedcba98UL,0x87654321UL,255);active=true;
    SendAffectRemovePacket(&ownDesc,0xfedcba98UL,0x87654321UL,255);End();assert(!buffer_size(dbout));
    source.BindDesc(&ownDesc);view.BindDesc(&viewDesc);SECTREE sector;
    source.SetSectree(&sector);source.m_map_view[&view]=1;source.m_map_view[&offline]=1;
    for(unsigned type=0;type<256;++type){
        Begin();source.m_vid=VID(0xfedcba98UL,1);victim.m_vid=VID(0x87654321UL,2);
        std::vector<BYTE> bytes=Fly(static_cast<BYTE>(type),0xfedcba98UL,0x87654321UL);
        expected.push_back(Expected(viewer,bytes));expected.push_back(Expected(own,bytes));active=true;
        changeVID=true;sourceToChange=&source;victimToChange=&victim;
        source.CreateFly(static_cast<BYTE>(type),&victim);End();
        assert(buffer_size(viewer)==10 && buffer_size(own)==10);
    }
    Begin();source.m_bIsObserver=true;expected.push_back(Expected(own,Fly(16,7,9)));active=true;source.CreateFly(16,&victim);End();assert(!buffer_size(viewer));
    Begin();source.SetSectree(0);active=true;source.CreateFly(255,&victim);End();assert(!buffer_size(own)&&!buffer_size(viewer));
    source.m_map_view.clear();source.BindDesc(0);view.BindDesc(0);source.SetSectree(0);
    active=false;thecore_memcpy=std::memcpy;db_clientdesc=0;db.Output(0);otherDB.Output(0);
    ownDesc.Attach(0,-1,0);viewDesc.Attach(0,-1,0);outputs.clear();
    buffer_delete(own);buffer_delete(viewer);buffer_delete(dbout);buffer_delete(otherout);fdwatch_delete(watch);
    std::puts("affect flag + actual GC/DB remove + CreateFly view regression passed");return 0;
}
