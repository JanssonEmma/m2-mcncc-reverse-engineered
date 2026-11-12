#include "desc_fixture.h"
#include "../../game/src/main.cpp"
#include "../../game/src/input.h"
#include "../../game/src/packet.h"
#include "../../game/src/config.h"
#include "../../game/src/sequence.h"
#include "../../game/src/TrafficProfiler.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "../../libthecore/include/memcpy.h"
int max_bytes_written,current_bytes_written,total_bytes_written;

extern bool g_bTrafficProfileOn;
static DWORD tick;
static int random_value, random_calls, exit_calls;
static DESC* pong_desc;
struct ExitSignal { int status; explicit ExitSignal(int value):status(value) {} };
extern "C" int pong_test_rand() { assert(pong_desc->IsPong()); ++random_calls; return random_value; }
extern "C" void pong_test_exit(int status) { assert(pong_desc->IsPong()); ++exit_calls; throw ExitSignal(status); }

static int error_line,errors,dumps,dumped_size;
static bool handshake_output;
static int handshake_rearms;
static const unsigned char* dumped;
extern "C" {
socket_t socket_connect(const char*,WORD) { std::abort(); }
void socket_close(socket_t) { std::abort(); }
void fdwatch_del_fd(LPFDWATCH,socket_t) { std::abort(); }
int gettimeofday(struct timeval*,struct timezone*) { std::abort(); }
int usleep(unsigned int) { std::abort(); }
void socket_block(socket_t) { std::abort(); }
int fdwatch(LPFDWATCH,struct timeval*) { std::abort(); }
void* fdwatch_get_client_data(LPFDWATCH,unsigned int) { std::abort(); }
int fdwatch_check_event(LPFDWATCH,socket_t,unsigned int) { std::abort(); }
int socket_read(socket_t,char*,size_t) { std::abort(); }
DWORD get_dword_time() { return ++tick; }
void sys_log(int,const char*,...) {}
void _sys_err(const char* function,int line,const char*,...) {
    assert(!std::strcmp(function,"Process") || !std::strcmp(function,"Handshake")); error_line=line; ++errors;
}
void printdata(const unsigned char* data,int length) { ++dumps; dumped=data; dumped_size=length; }
int MIN(int a,int b) { return a<b?a:b; }
void* (*thecore_memcpy)(void*,const void*,size_t)=std::memcpy;
int fdwatch_get_buffer_size(LPFDWATCH,socket_t) { std::abort(); }
void fdwatch_add_fd(LPFDWATCH,socket_t,void*,int rw,int once) { assert(handshake_output && rw==2 && once==1); ++handshake_rearms; }
int socket_write(socket_t,const char*,size_t) { std::abort(); }
}
class InputDesc : public DescOutputProbe {
public:
    int closed;
    InputDesc():closed(0) {}
    void Select(CInputProcessor* processor) { m_pInputProcessor=processor; }
    void Sequence(int index) { m_iCurrentSequence=index; }
    void HandshakeKey(DWORD key) { m_dwHandshake=key; }
    virtual void SetPhase(int phase) { m_iPhase=phase; if(!phase) ++closed; }
};
class Processor : public CInputProcessor {
public:
    BYTE type;
    int extra;
    CInputProcessor* next;
    std::vector<BYTE> calls;
    Processor(BYTE value=1):type(value),extra(0),next(0) {}
    virtual BYTE GetType() { return type; }
    CPacketInfo* Info() { return m_pPacketInfo; }
    int Left() { return m_iBufferLeft; }
protected:
    virtual int Analyze(DESC* d,BYTE header,const char*) {
        calls.push_back(header);
        if(next) static_cast<InputDesc*>(d)->Select(next);
        return extra;
    }
};
static std::string Read(const char* path) {
    FILE* f=std::fopen(path,"r"); assert(f);
    std::string text; char data[512]; size_t n;
    while((n=std::fread(data,1,sizeof data,f)))text.append(data,n);
    std::fclose(f); return text;
}
static void ExitReport() {
    // Registered before first processor: shared registry destructor runs before this.
    const std::string report=Read("packet_info.txt");
    assert(report.find("Handshake")!=std::string::npos);
    assert(std::remove("packet_info.txt")==0);
}
int main() {
    std::atexit(ExitReport);
    Processor p,other(2),same(1);
    assert(p.Info()==other.Info() && p.Left()==0); // Shared, no embedded registry.
    CPacketInfo table; table.Set(32,2,"Fixed"); table.Set(33,2,"Sequenced",true);
    p.BindPacketInfo(&table); same.BindPacketInfo(&table); other.BindPacketInfo(&table);
    InputDesc d; d.Select(&p);
    pong_desc=&d;
    d.SetPong(false); p.Pong(&d); assert(d.IsPong() && !random_calls && !exit_calls);
    g_isInvalidServer=true;
    const int values[]={0,49999,50000,50001,100000,2147483647};
    for(size_t i=0;i<sizeof values/sizeof values[0];++i) {
        random_value=values[i]; d.SetPong(false); bool exited=false;
        try { p.Pong(&d); } catch(const ExitSignal& signal) { assert(signal.status==0); exited=true; }
        assert(d.IsPong() && exited==(values[i]%50000==0));
    }
    assert(random_calls==6 && exit_calls==3); g_isInvalidServer=false;
    TPacketCGHandshake hello; hello.bHeader=255; hello.dwHandshake=77; hello.dwTime=100; hello.lDelta=0;
    d.HandshakeKey(77); d.SetPacketState(1); tick=100; g_bAuthServer=false;
    p.Handshake(&d,reinterpret_cast<const char*>(&hello)); assert(d.IsPhase(2));
    d.SetPacketState(1); tick=100; g_bAuthServer=true;
    p.Handshake(&d,reinterpret_cast<const char*>(&hello)); assert(d.IsPhase(10));
    g_bAuthServer=false;
    hello.dwHandshake=78; p.Handshake(&d,reinterpret_cast<const char*>(&hello));
    assert(d.IsPhase(0) && error_line==302);
    hello.dwHandshake=77; LPBUFFER handshake_buffer=buffer_new(0,64);
    d.Attach(0,17,handshake_buffer); d.SetPacketState(5); tick=100; handshake_output=true;
    p.Handshake(&d,reinterpret_cast<const char*>(&hello));
    assert(d.IsPhase(5) && buffer_size(handshake_buffer)==1 && handshake_rearms==1);
    assert(*static_cast<const BYTE*>(buffer_read_peek(handshake_buffer))==252);
    handshake_output=false; buffer_delete(handshake_buffer);
    int used=7; const BYTE frames[]={0,32,42,32,43};
    assert(p.Process(&d,frames,sizeof frames,used));
    assert(used==12 && p.calls.size()==2 && p.Left()==0);
    assert(table.GetElement(32)->iCalled==2 && table.GetElement(32)->dwLoad==2);
    // Partial frame is left untouched, while padding preceding it is consumed.
    p.calls.clear(); used=0; const BYTE partial[]={0,32};
    assert(p.Process(&d,partial,sizeof partial,used) && used==1 && p.calls.empty() && p.Left()==1);
    // Handler can request additional input by returning negative; no End/consumption occurs.
    p.extra=-1; used=0;
    assert(p.Process(&d,frames+1,2,used) && used==0);
    assert(table.GetElement(32)->iCalled==2);
    // A variable tail adds to the registered minimum.
    p.extra=2; used=0; const BYTE variable[]={32,1,2,3};
    assert(p.Process(&d,variable,4,used) && used==4);
    p.extra=0;
    // Dispatch notices processor type changes after consuming this packet.
    p.next=&other; used=0; d.Select(&p);
    assert(!p.Process(&d,frames+1,4,used) && used==2 && p.Left()==2);
    // Replacing an object with one of the same type does not stop the old processor.
    p.next=&same; d.Select(&p); used=0;
    assert(p.Process(&d,frames+1,4,used) && used==4);
    p.next=0; d.Select(&p);
    // Valid and invalid sequence bytes, including table wrap at65536.
    d.Sequence(65535); const BYTE last=d.GetSequence();
    BYTE sequence[]={33,0,last}; used=0;
    assert(p.Process(&d,sequence,3,used) && used==3 && d.GetSequence()==gc_abSequence[0]);
    d.Sequence(32768); assert(d.GetSequence()==gc_abSequence[32768]);
    sequence[2]=static_cast<BYTE>(d.GetSequence()+1); used=0; const int closed=d.closed;
    assert(p.Process(&d,sequence,3,used) && used==0 && d.closed==closed+1 && error_line==241);
    // Unknown frame diagnostics use the original pointer with remaining-byte length.
    const BYTE unknown[]={0,32,42,99}; used=0;
    assert(p.Process(&d,unknown,4,used) && used==3 && error_line==204);
    assert(dumps==1 && dumped==unknown && dumped_size==1);
    p.BindPacketInfo(0); used=9;
    assert(p.Process(&d,frames,5,used) && used==9 && error_line==189);
    p.BindPacketInfo(&table);
    // Negative/empty input skips dispatch without resetting caller's accumulated count.
    used=11; assert(p.Process(&d,0,-1,used) && used==11 && p.Left()==-1);
    // Retain the closed processor's peculiar extra-size return (left+registered size).
    CInputClose close; close.BindPacketInfo(&table); d.Select(&close); used=0;
    const BYTE close_data[8]={32,0};
    assert(close.Process(&d,close_data,2,used) && used==4 && close.GetType()==0);
    // Input traffic accounts zero padding and rejected sequence attempts before validation.
    {
        TrafficProfiler traffic; assert(traffic.Initialize(0xffffffffUL,"input_profile.txt"));
        g_bTrafficProfileOn=true; d.Select(&p); d.Sequence(0); used=0;
        const BYTE pad[]={0}; assert(p.Process(&d,pad,1,used) && used==1);
        sequence[2]=static_cast<BYTE>(d.GetSequence()+1); used=0;
        assert(p.Process(&d,sequence,3,used) && used==0);
        g_bTrafficProfileOn=false; assert(traffic.Flush());
    }
    const std::string report=Read("input_profile.txt");
    assert(report.find("Total traffic: 4 bytes")!=std::string::npos);
    assert(report.find("0\t1\t1\t\t1")!=std::string::npos);
    assert(report.find("33\t1\t3\t\t3")!=std::string::npos);
    assert(std::remove("input_profile.txt")==0);
    assert(errors>=4);
    std::puts("input dispatch passed: fragmentation, tails, type switches, sequences, diagnostics and profiling");
}
