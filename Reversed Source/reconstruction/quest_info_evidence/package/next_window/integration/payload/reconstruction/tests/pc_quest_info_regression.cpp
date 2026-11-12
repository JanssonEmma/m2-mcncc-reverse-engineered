// Complete actual pool/PC/manager/CHAR/DESC graph; safe initialized fixed-width inputs only.
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
#include "../../game/src/buffer_manager.h"
#undef protected
#undef private
#include "../../game/src/packet.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static std::vector<std::vector<BYTE> > chunks;
static std::vector<BYTE> frame;
static std::vector<int> logKinds;
static unsigned copied,logged,gcSeen;
static bool active;static int mode;
static LPBUFFER pool,outputA,outputB,wantedOutput;
static const BYTE* liveHeader;
static quest::PC* pc;
static quest::QuestState* a;static quest::QuestState* b;
static quest::CQuestManager* qm;
static CHARACTER* charA;static CHARACTER* charB;
static unsigned poolCount;
static WORD Read2(const BYTE* p){return static_cast<WORD>(p[0]|static_cast<unsigned>(p[1])<<8);}
static void Put2(std::vector<BYTE>& v,unsigned i,WORD value){v[i]=static_cast<BYTE>(value);v[i+1]=static_cast<BYTE>(value>>8);}
static void Put4(std::vector<BYTE>& v,unsigned i,DWORD value){for(unsigned j=0;j<4;++j)v[i+j]=static_cast<BYTE>(value>>(8*j));}
static std::vector<BYTE> IntBytes(int value){std::vector<BYTE> v(4);Put4(v,0,static_cast<DWORD>(value));return v;}
static std::vector<BYTE> Text(const std::string& s,unsigned n){assert(s.size()+1>=n);return std::vector<BYTE>(s.c_str(),s.c_str()+n);}
static void Add(const std::vector<BYTE>& c){chunks.push_back(c);frame.insert(frame.end(),c.begin(),c.end());}
static void Begin(LPBUFFER output){active=false;chunks.clear();frame.clear();logKinds.clear();copied=logged=gcSeen=0;liveHeader=0;mode=0;wantedOutput=output;buffer_reset(outputA);buffer_reset(outputB);assert(BUFFER_MANAGER::instance_ptr());poolCount=static_cast<unsigned>(BUFFER_MANAGER::instance().buffer_stack.size());assert(poolCount);pool=BUFFER_MANAGER::instance().buffer_stack.top();assert(!buffer_size(pool));}
static void Header(int mask,int index){std::vector<BYTE> h(6);h[0]=81;Put2(h,1,6);Put2(h,3,static_cast<WORD>(index));h[5]=static_cast<BYTE>(mask);Add(h);}
static void Expected(int mask,const quest::QuestState& s){Header(mask,s.iIndex);if(mask&1){Add(std::vector<BYTE>(1,s.bStart?1:0));logKinds.push_back(1);}if(mask&2){Add(Text(s._title,31));logKinds.push_back(2);}if(mask&4){Add(Text(s._clock_name,17));logKinds.push_back(3);}if(mask&8){Add(IntBytes(s._clock_value));logKinds.push_back(4);}if(mask&16){Add(Text(s._counter_name,17));logKinds.push_back(5);}if(mask&32){Add(IntBytes(s._counter_value));logKinds.push_back(6);}if(mask&64){Add(Text(s._icon_file,25));logKinds.push_back(7);}}
static void* Copy(void* out,const void* in,size_t count){
    if(active && out==pool->write_point){assert(copied<chunks.size());assert(count==chunks[copied].size() && !std::memcmp(in,&chunks[copied][0],count));if(!copied)liveHeader=static_cast<const BYTE*>(in);++copied;if(mode==1 && copied==1){pc->m_RunningQuestState=b;pc->m_iSendToClient=2|8;}else if(mode==1 && copied==2)pc->m_RunningQuestState=a;else if(mode==3 && copied==2){b->bStart=false;pc->m_RunningQuestState=b;}}
    else if(active && (out==outputA->write_point || out==outputB->write_point)){
        assert(out==wantedOutput->write_point && !gcSeen && count==frame.size() && !std::memcmp(in,&frame[0],count));assert(copied==chunks.size() && logged==logKinds.size());
        assert(liveHeader && Read2(liveHeader+1)==frame.size());liveHeader=0; // Observe ONLY while sender is active; no dangling header pointer retained.
        assert(BUFFER_MANAGER::instance().buffer_stack.size()==poolCount-1);++gcSeen;
        if(mode==1)pc->SetSendFlag(0x400);
        if(mode==2){pc->m_RunningQuestState=a;pc->m_stCurQuest="callback";pc->m_iSendToClient=0x800;qm->m_pCurrentCharacter=charB;}
    }
    return std::memcpy(out,in,count);
}
extern "C" void pc_info_log(int kind,const char* text,int value){
    if(!active)return;assert(logged<logKinds.size() && logKinds[logged++]==kind);
    if(mode==3 && kind==1){assert(value==1 && pc->m_RunningQuestState==b && !b->bStart);}
    else if(mode==1 && kind==2){assert(!std::strcmp(text,a->_title.c_str()));pc->m_iSendToClient=32;qm->m_pCurrentCharacter=charB;}
    else if(mode==1 && kind==6){assert(value==a->_counter_value);pc->m_RunningQuestState=b;pc->m_iSendToClient=64;}
}
static void End(int expectedMask,bool sends=true){assert(copied==chunks.size() && logged==logKinds.size() && gcSeen==(sends?1u:0u));assert(pc->m_iSendToClient==expectedMask);assert(BUFFER_MANAGER::instance().buffer_stack.size()==poolCount && !buffer_size(pool));if(sends){assert(buffer_size(wantedOutput)==frame.size());const BYTE* actual=static_cast<const BYTE*>(buffer_read_peek(wantedOutput));assert(Read2(actual+1)==6);}active=false;liveHeader=0;}
static void Init(quest::QuestState& s,char c,int index){s.co=0;s.ico=0;s.args=0;s.suspend_state=0;s.iIndex=index;s.bStart=true;s.st=0;s._title.assign(31,c);s._clock_name.assign(17,c);s._counter_name.assign(17,c);s._icon_file.assign(25,c);s._clock_value=INT_MIN;s._counter_value=INT_MAX;}
int main(){
    desc_test_allow_clock();assert(BUFFER_MANAGER::instance_ptr());{TEMP_BUFFER prime;const char byte='x';prime.write(&byte,1);} // Use existing static manager, never construct another singleton.
    quest::CQuestManager manager;qm=&manager;quest::PC owner;owner.SetID(123);pc=&owner;quest::QuestState first,second;Init(first,'A',0x12345678);Init(second,'B',-1);a=&first;b=&second;
    CHARACTER receiverA,receiverB;receiverA.m_dwPlayerID=456;receiverB.m_dwPlayerID=789;charA=&receiverA;charB=&receiverB;DescOutputProbe dA,dB;LPFDWATCH watch=fdwatch_new(16);outputA=buffer_new(0,8192);outputB=buffer_new(0,8192);dA.Attach(watch,5,outputA);dA.SetPacketState(5);dB.Attach(watch,6,outputB);dB.SetPacketState(5);receiverA.BindDesc(&dA);receiverB.BindDesc(&dB);qm->m_pCurrentCharacter=charA;assert(qm->GetCurrentCharacterPtr()==charA);thecore_memcpy=Copy;
    for(int mask=1;mask<128;++mask){pc->m_RunningQuestState=a;pc->m_iSendToClient=mask;Begin(outputA);Expected(mask,*a);active=true;pc->SendQuestInfoPakcet();End(0);assert(!buffer_size(outputB));}
    const int raw[]={128,256,384,INT_MIN,-1};for(unsigned i=0;i<5;++i){pc->m_RunningQuestState=b;pc->m_iSendToClient=raw[i];Begin(outputA);Expected(raw[i],*b);active=true;pc->SendQuestInfoPakcet();End(0);}
    // Exact N-1 strings include their defined terminator; long/binary strings send raw initialized prefixes.
    first._title.assign(30,'t');first._clock_name.assign(16,'c');first._counter_name.assign(16,'n');first._icon_file.assign(24,'i');pc->m_RunningQuestState=a;pc->m_iSendToClient=127;Begin(outputA);Expected(127,*a);active=true;pc->SendQuestInfoPakcet();End(0);
    first._title.assign(64,'t');first._title[3]=0;first._clock_name.assign(64,'c');first._counter_name.assign(64,'n');first._icon_file.assign(64,'i');pc->m_iSendToClient=127;Begin(outputA);Expected(127,*a);active=true;pc->SendQuestInfoPakcet();End(0);
    Init(first,'A',0x12345678);Init(second,'B',-1);
    // Initial flag/index captured, BODY follows live masks/states, logger sees state AFTER earlier copy callback, recipient read late.
    pc->m_RunningQuestState=a;pc->m_iSendToClient=1;qm->m_pCurrentCharacter=charA;Begin(outputB);Header(1,a->iIndex);Add(Text(b->_title,31));Add(IntBytes(a->_counter_value));Add(Text(b->_icon_file,25));logKinds.push_back(2);logKinds.push_back(6);logKinds.push_back(7);mode=1;active=true;pc->SendQuestInfoPakcet();End(0);assert(pc->m_RunningQuestState==b && qm->m_pCurrentCharacter==charB && !buffer_size(outputA));
    // Begin logger uses cached BYTE written BEFORE callback, not the now-current state.
    pc->m_RunningQuestState=a;a->bStart=true;pc->m_iSendToClient=1;qm->m_pCurrentCharacter=charA;Begin(outputA);Expected(1,*a);mode=3;active=true;pc->SendQuestInfoPakcet();End(0);assert(pc->m_RunningQuestState==b && !b->bStart);
    // Getter is INDEX, not bStart. Current scalar/string setters OR mask then mutate, never send.
    pc->m_RunningQuestState=0;pc->m_iSendToClient=0x200;assert(pc->GetCurrentQuestBeginFlag()==0);pc->SetCurrentQuestDoneFlag();assert(pc->m_iSendToClient==0x200);
    pc->m_RunningQuestState=a;qm->m_hmQuestName["start"]=17;pc->m_stCurQuest="start";a->iIndex=0;a->bStart=false;pc->m_iSendToClient=0;pc->SetCurrentQuestStartFlag();assert(a->iIndex==17 && a->bStart && pc->m_iSendToClient==1);
    a->bStart=false;pc->m_iSendToClient=0;pc->SetCurrentQuestStartFlag();assert(!a->bStart && !pc->m_iSendToClient);pc->SetCurrentQuestDoneFlag();assert(!a->bStart && a->iIndex==17 && pc->m_iSendToClient==1);
    pc->m_iSendToClient=0;pc->SetCurrentQuestDoneFlag();assert(pc->m_iSendToClient==1);
    pc->m_stCurQuest="missing";a->iIndex=0;a->bStart=true;pc->m_iSendToClient=0;pc->SetCurrentQuestStartFlag();assert(a->iIndex==0 && a->bStart && pc->m_iSendToClient==1);pc->m_iSendToClient=0;pc->SetCurrentQuestDoneFlag();assert(a->bStart && !pc->m_iSendToClient);
    const unsigned savedA=buffer_size(outputA),savedB=buffer_size(outputB);
    pc->m_iSendToClient=INT_MIN;pc->SetCurrentQuestTitle(std::string(80,'T'));pc->SetCurrentQuestClockName(std::string(80,'C'));pc->SetCurrentQuestClockValue(INT_MAX);pc->SetCurrentQuestCounterName(std::string(80,'N'));pc->SetCurrentQuestCounterValue(INT_MIN);pc->SetCurrentQuestIconFile(std::string(80,'I'));pc->ClearCurrentQuestBeginFlag();assert(pc->m_iSendToClient==(INT_MIN|127) && a->_title==std::string(80,'T') && a->_clock_name==std::string(80,'C') && a->_counter_name==std::string(80,'N') && a->_icon_file==std::string(80,'I') && a->_clock_value==INT_MAX && a->_counter_value==INT_MIN && !a->bStart);assert(buffer_size(outputA)==savedA && buffer_size(outputB)==savedB);
    // Temporary title context sends through actual sender then restores original PC context/mask, not manager context.
    Init(first,'A',0x12345678);pc->m_QuestInfo.insert(std::make_pair(7u,first));qm->m_hmQuestName["target"]=7;pc->m_stCurQuest="old";pc->m_RunningQuestState=b;pc->m_iSendToClient=0x400;qm->m_pCurrentCharacter=charA;
    quest::QuestState desired=first;desired._title.assign(40,'Z');Begin(outputA);Expected(2,desired);mode=2;active=true;pc->SetQuestTitle("target",desired._title);End(0x400);assert(pc->m_stCurQuest=="old" && pc->m_RunningQuestState==b && pc->m_QuestInfo.find(7)->second.iIndex==0x12345678 && qm->m_pCurrentCharacter==charB);
    unsigned count=pc->m_QuestInfo.size();Begin(outputA);active=true;pc->SetQuestTitle("no-entry",desired._title);End(0x400,false);assert(pc->m_QuestInfo.size()==count && pc->m_stCurQuest=="old");
    active=false;thecore_memcpy=std::memcpy;pc->m_RunningQuestState=0;qm->m_pCurrentCharacter=0;receiverA.BindDesc(0);receiverB.BindDesc(0);dA.Attach(0,-1,0);dB.Attach(0,-1,0);buffer_delete(outputA);buffer_delete(outputB);fdwatch_delete(watch);
    std::puts("PC quest info actual pool/network passed; short/uninitialized tails and VM execution excluded");return 0;
}
