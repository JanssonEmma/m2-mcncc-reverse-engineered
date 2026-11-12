#include <map>
#include <set>
#include <vector>
#include <string>
#define private public
#define protected public
#include "../../game/src/item_manager.h"
#undef protected
#undef private
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
static ITEM_MANAGER* manager;
static int expectKind,seen,oldShutdown;
static DWORD expectedCurrent,id,lower,upper;
static bool mutate;
extern "C" void item_id_observe(int error,const char* function,int line,const char* format,const char* text) {
    assert(expectKind&&manager&&seen==0);++seen;
    assert(shutdowned==oldShutdown&&manager->m_dwCurrentID==expectedCurrent);
    char expected[512];
    if(expectKind==1){assert(error&&line==203&&!std::strcmp(function,"GetNewID"));assert(!std::strcmp(format,"FATAL ERROR! ITEM ID Overflow, contact your administrator!"));assert(!std::strcmp(text,format));}
    else if(expectKind==2){assert(error&&line==218&&!std::strcmp(function,"SetMaxItemID"));assert(!std::strcmp(format,"FATAL ERROR! item id out of range! id %lu, range %lu ~ %lu"));std::sprintf(expected,format,id,lower,upper);assert(!std::strcmp(expected,text));}
    else {assert(!error&&expectKind==3&&!std::strcmp(function,"SetMaxItemID"));assert(!std::strcmp(format,"ITEM: ID start at %lu"));std::sprintf(expected,format,id);assert(!std::strcmp(expected,text));}
    if(mutate){manager->m_dwCurrentID=77;g_dwItemIDRange[0]=88;g_dwItemIDRange[1]=99;}
}
static void Prepare(int kind,DWORD current,int shutdown,bool callback=false){expectKind=kind;seen=0;expectedCurrent=current;oldShutdown=shutdown;shutdowned=shutdown;mutate=callback;lower=g_dwItemIDRange[0];upper=g_dwItemIDRange[1];}
static void Get(ITEM_MANAGER& m,DWORD current,DWORD max,DWORD expected,DWORD after,int shutdown,bool fail,bool callback=false){
    m.m_dwCurrentID=current;g_dwItemIDRange[1]=max;Prepare(fail?1:0,current,shutdown,callback);
    assert(m.GetNewID()==expected);assert(m.m_dwCurrentID==after&&shutdowned==(fail?1:shutdown)&&seen==(fail?1:0));expectKind=0;
}
static void Set(ITEM_MANAGER& m,DWORD value,bool expected,int shutdown,bool callback=false){
    const DWORD old=m.m_dwCurrentID;id=value;Prepare(value?(expected?3:2):0,expected?value:old,shutdown,callback);
    assert(m.SetMaxItemID(value)==expected);assert(seen==(value?1:0));assert(shutdowned==((value&&!expected)?1:shutdown));assert(m.m_dwCurrentID==(callback?77:(expected?value:old)));expectKind=0;
}
int main(int argc,char** argv){
    assert(sizeof(DWORD)==4&&sizeof(g_dwItemIDRange)==8);
    assert(g_dwItemIDRange[0]==0&&g_dwItemIDRange[1]==0&&shutdowned==0);
    ITEM_MANAGER m;manager=&m;assert(m.m_dwCurrentID==0);
    // Optional separate death invocation; do not run this path in parent process.
    if(argc==2&&!std::strcmp(argv[1],"--assert-zero")){g_dwItemIDRange[1]=10;(void)m.GetNewID();return 99;}
    g_dwItemIDRange[0]=10;
    Get(m,17,20,17,18,0,false);Get(m,18,20,18,19,0,false);Get(m,19,20,19,20,0,false);
    Get(m,20,20,0,20,0,true);Get(m,21,20,0,21,7,true);Get(m,20,20,0,20,1,true);
    Get(m,1,20,1,2,7,false); // Lower bound and existing shutdown do not suppress allocation.
    g_dwItemIDRange[0]=0x80000000UL;
    Get(m,0xfffffffeUL,0xffffffffUL,0xfffffffeUL,0xffffffffUL,0,false);
    Get(m,0xffffffffUL,0xffffffffUL,0,0xffffffffUL,0,true);Get(m,1,0,0,1,0,true);
    Get(m,20,20,0,77,9,true,true);assert(g_dwItemIDRange[1]==99); // Error callback cannot prevent actual shutdown.
    g_dwItemIDRange[0]=10;g_dwItemIDRange[1]=20;m.m_dwCurrentID=15;
    Set(m,0,false,7);Set(m,10,false,7);Set(m,20,false,0);Set(m,0xffffffffUL,false,0);Set(m,11,true,0);Set(m,19,true,7);
    Set(m,12,true,7,true);assert(m.m_dwCurrentID==77&&shutdowned==7); // No post-log rewrite.
    g_dwItemIDRange[0]=20;g_dwItemIDRange[1]=10;Set(m,15,false,0);
    g_dwItemIDRange[0]=10;g_dwItemIDRange[1]=10;Set(m,10,false,0);
    g_dwItemIDRange[0]=0x80000000UL;g_dwItemIDRange[1]=0xffffffffUL;Set(m,0x80000001UL,true,0);Set(m,0x7fffffffUL,false,0);
    Set(m,0x80000000UL,false,7,true);assert(shutdowned==1&&m.m_dwCurrentID==77);
    manager=0;g_dwItemIDRange[0]=g_dwItemIDRange[1]=0;shutdowned=0;
    std::puts("item IDs passed: full manager, real shutdown state, unsigned boundaries and diagnostic callback ordering");
}
