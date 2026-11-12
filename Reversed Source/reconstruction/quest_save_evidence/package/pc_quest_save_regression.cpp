// Actual complete PC + CLIENT_DESC header/body + buffer/event graph. NativeOS observer only.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questpc.h"
#include "../../game/src/desc_client.h"
#undef protected
#undef private
#include "../../common/tables.h"
#include "../../libthecore/include/memcpy.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cstddef>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
class ClientProbe:public CLIENT_DESC{public:void Output(LPBUFFER b){m_lpOutputBuffer=b;}};
static quest::PC* current;static CLIENT_DESC* second;
static LPBUFFER outA,outB;static bool active;static int mode;
static unsigned headers,bodies,validLogs,finalLogs;static std::vector<int> errors;
static std::vector<BYTE> captured;
static unsigned Read4(const BYTE* b){return static_cast<unsigned>(b[0])|(static_cast<unsigned>(b[1])<<8)|(static_cast<unsigned>(b[2])<<16)|(static_cast<unsigned>(b[3])<<24);}
extern "C" void pc_save_error(int line,const char* text){assert(active);assert(text);errors.push_back(line);}
extern "C" void pc_save_log(int kind,const char* name,const char* state,long value,int index){
 if(!active)return;
 if(kind==1){assert(name&&state);++validLogs;if(mode==1){assert(index==0&&value==-17);current->m_FlagSaveMap["a.q"]=1234;current->SetID(0xaabbccddUL);current->m_FlagSaveMap["z.q"]=99;mode=2;}}
 else {assert(kind==2&&index>0);++finalLogs;if(mode==3)current->m_FlagSaveMap["late.log"]=7;}
}
static void* Copy(void* target,const void* source,size_t n){
 if(active&&(target==outA->write_point||target==outB->write_point)){
  if(headers<3){assert(target==outA->write_point);assert(n==(headers?4u:1u));++headers;}
  else {assert(!bodies&&n%106==0);++bodies;captured.assign(static_cast<const BYTE*>(source),static_cast<const BYTE*>(source)+n);assert(current->m_FlagSaveMap.size());}
  void* answer=std::memcpy(target,source,n);
  if(headers==3&&!bodies&&mode==3){db_clientdesc=second;current->m_FlagSaveMap["late.header"]=8;}
  if(bodies&&mode==3)current->m_FlagSaveMap["late.body"]=9;
  return answer;
 }
 return std::memcpy(target,source,n);
}
static void Begin(){active=false;buffer_reset(outA);buffer_reset(outB);headers=bodies=validLogs=finalLogs=0;errors.clear();captured.clear();mode=0;}
static void Save(quest::PC& pc,ClientProbe& database){current=&pc;db_clientdesc=&database;active=true;pc.Save();active=false;assert(pc.m_FlagSaveMap.empty());}
static void Check(unsigned count,bool split=false){
 assert(headers==(count?3u:0u)&&bodies==(count?1u:0u)&&finalLogs==(count?1u:0u));
 if(!count){assert(!buffer_size(outA)&&!buffer_size(outB));return;}
 const BYTE* h=static_cast<const BYTE*>(buffer_read_peek(outA));assert(h[0]==11&&Read4(h+1)==0&&Read4(h+5)==106*count);assert(captured.size()==106*count);
 if(split){assert(buffer_size(outA)==9&&buffer_size(outB)==106*count);assert(!std::memcmp(buffer_read_peek(outB),&captured[0],captured.size()));}
 else {assert(buffer_size(outA)==9+106*count&&!buffer_size(outB));assert(!std::memcmp(h+9,&captured[0],captured.size()));}
}
static TQuestTable Row(unsigned i){assert(captured.size()>=(i+1)*106);TQuestTable result;std::memcpy(&result,&captured[i*106],106);return result;}
static void Fields(unsigned i,DWORD pid,const std::string& name,const std::string& state,long value){const TQuestTable& r=Row(i);assert(r.dwPID==pid&&!std::strcmp(r.szName,name.c_str())&&!std::strcmp(r.szState,state.c_str())&&r.lValue==value);}
int main(){
 assert(sizeof(long)==4&&sizeof(DWORD)==4&&sizeof(TQuestTable)==106);assert(offsetof(TQuestTable,szName)==4&&offsetof(TQuestTable,szState)==37&&offsetof(TQuestTable,lValue)==102);
 desc_test_allow_clock();quest::PC pc,other;pc.SetID(0x11223344UL);other.SetID(0x55667788UL);ClientProbe database,alternate;second=&alternate;outA=buffer_new(0,16384);outB=buffer_new(0,16384);database.Output(outA);alternate.Output(outB);thecore_memcpy=Copy;
 Begin();Save(pc,database);Check(0);assert(!validLogs&&errors.empty());
 // First allocation zero-initializes every new record; fields leave two reserved tail bytes untouched.
 const std::string longName(31,'n'),longState(63,'s');Begin();pc.m_FlagSaveMap[longName+"."+longState]=INT_MIN;Save(pc,database);Check(1);Fields(0,0x11223344UL,longName,longState,INT_MIN);assert(Row(0).szName[32]==0&&Row(0).szState[64]==0);std::vector<BYTE> prior=captured;
 Begin();other.m_FlagSaveMap["a.b"]=INT_MAX;Save(other,database);Check(1);Fields(0,0x55667788UL,"a","b",INT_MAX);assert(!std::memcmp(&captured[6],&prior[6],31));assert(!std::memcmp(&captured[39],&prior[39],63));
 // Invalid rows log the original lines, compact accepted rows, and clear all pending flags.
 Begin();pc.m_FlagSaveMap["no-dot"]=1;pc.m_FlagSaveMap[".bad"]=2;pc.m_FlagSaveMap["bad."]=3;pc.m_FlagSaveMap[std::string(32,'x')+".y"]=4;pc.m_FlagSaveMap["x."+std::string(64,'y')]=5;pc.m_FlagSaveMap["good.multi.dot"]=0;Save(pc,database);Check(1);Fields(0,0x11223344UL,"good","multi.dot",0);assert(errors.size()==5&&validLogs==3);bool seen[5]={false,false,false,false,false};for(unsigned i=0;i<errors.size();++i){if(errors[i]==537)seen[0]=true;else if(errors[i]==549)seen[1]=true;else if(errors[i]==557)seen[2]=true;else if(errors[i]==563)seen[3]=true;else assert(false);}assert(seen[0]&&seen[1]&&seen[2]&&seen[3]);
 Begin();pc.m_FlagSaveMap["invalid"]=1;Save(pc,database);Check(0);assert(errors.size()==1&&errors[0]==537);
 // EmbeddedNUL find/length uses complete string, strcpy writes only each C prefix.
 Begin();std::string key("a\0x.b\0y",7);pc.m_FlagSaveMap[key]=-1;Save(pc,database);Check(1);Fields(0,0x11223344UL,"a","b",-1);
 // Current value is cached before log; PID is reread after. Future iterator row remains live.
 Begin();pc.m_FlagSaveMap["a.q"]=-17;pc.m_FlagSaveMap["z.q"]=18;mode=1;Save(pc,database);Check(2);Fields(0,0xaabbccddUL,"a","q",-17);Fields(1,0xaabbccddUL,"z","q",99);
 // Three actual header writes use original DB. Header callback swaps global receiver; body rereads it.
 Begin();pc.m_FlagSaveMap["x.q"]=5;mode=3;Save(pc,database);Check(1,true);Fields(0,0xaabbccddUL,"x","q",5);
 // Grow after shrink: newly appended rows are fully zero-initialized, retained row tail remains old.
 Begin();pc.m_FlagSaveMap["a.b"]=1;pc.m_FlagSaveMap["c.d"]=2;pc.m_FlagSaveMap["e.f"]=3;Save(pc,database);Check(3);Fields(2,0xaabbccddUL,"e","f",3);for(unsigned i=2;i<33;++i)assert(Row(2).szName[i]==0);for(unsigned i=2;i<65;++i)assert(Row(2).szState[i]==0);
 active=false;thecore_memcpy=std::memcpy;db_clientdesc=0;database.Output(0);alternate.Output(0);buffer_delete(outA);buffer_delete(outB);std::puts("complete PC::Save actual DB graph passed; original unsafe alias/reentrant overflow domains excluded");return 0;
}

