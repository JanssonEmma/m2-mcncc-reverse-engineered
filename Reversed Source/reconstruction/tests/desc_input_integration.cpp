#include "desc_fixture.h"
#include "../../game/src/input.h"
#include "../../libthecore/include/crypt.h"
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
int max_bytes_written,current_bytes_written,total_bytes_written;

extern "C" {
void desc_test_recv(const char*,int,int);
int desc_test_recv_calls(void);
int desc_test_input_errors(void);
int desc_test_proceed_warnings(void);
void desc_test_allow_clock(void);
}
class InputDesc : public DescOutputProbe {
public:
    void SetupInput(LPBUFFER input,CInputProcessor* processor,bool crypt=false) {
        m_sock=5; m_lpInputBuffer=input; m_pInputProcessor=processor;
        m_iMinInputBufferLen=16; m_bEncrypted=crypt;
        for(int i=0;i<4;++i) m_adwDecryptionKey[i]=i+1;
    }
    void Select(CInputProcessor* p) { m_pInputProcessor=p; }
};
class Frames : public CInputProcessor {
public:
    BYTE type;
    CInputProcessor* next;
    std::vector<BYTE> payloads;
    Frames(BYTE t=1):type(t),next(0) {}
    virtual BYTE GetType() { return type; }
protected:
    virtual int Analyze(DESC* d,BYTE header,const char* bytes) {
        assert(header==32); payloads.push_back(static_cast<BYTE>(bytes[1]));
        if(next) static_cast<InputDesc*>(d)->Select(next);
        return 0;
    }
};
class Script : public CInputProcessor {
public:
    bool done;
    int consume,calls,seen;
    Script():done(true),consume(0),calls(0),seen(0) {}
    virtual BYTE GetType() { return 8; }
    virtual bool Process(DESC*,const void*,int size,int& bytes) {
        ++calls; seen=size; bytes=consume; return done;
    }
protected:
    virtual int Analyze(DESC*,BYTE,const char*) { std::abort(); }
};
static void Cleanup() { assert(std::remove("packet_info.txt")==0); }
int main() {
    std::atexit(Cleanup); desc_test_allow_clock();
    InputDesc d;
    assert(d.ProcessInput()==-1 && desc_test_input_errors()==1);
    LPBUFFER input=buffer_new(0,64);
    d.SetupInput(input,0);
    desc_test_recv(0,-1,EAGAIN);
    assert(d.ProcessInput()==0 && desc_test_recv_calls()==1 && !buffer_size(input));
    desc_test_recv(0,0,0); assert(d.ProcessInput()==-1 && !buffer_size(input));
    desc_test_recv(0,-1,EINTR); assert(d.ProcessInput()==0);
    // Positive input is preserved even when no processor has been attached.
    const char frame[]={32,42}; desc_test_recv(frame,2,0);
    assert(d.ProcessInput()==2 && buffer_size(input)==2 && desc_test_input_errors()==2);
    Frames first,second(2); CPacketInfo table; table.Set(32,2,"Frame");
    first.BindPacketInfo(&table); second.BindPacketInfo(&table);
    d.SetupInput(input,&first);
    const char padding=0; desc_test_recv(&padding,1,0);
    assert(d.ProcessInput()==1 && !buffer_size(input) && first.payloads.size()==1);
    assert(first.payloads[0]==42);
    // The read return value is newly received bytes, while dispatch sees accumulated bytes.
    desc_test_recv(frame,1,0); assert(d.ProcessInput()==1 && buffer_size(input)==1);
    desc_test_recv(frame+1,1,0); assert(d.ProcessInput()==1 && !buffer_size(input));
    assert(first.payloads.size()==2);
    // A type change forces a second virtual dispatch on the newly selected processor.
    first.next=&second; const char two[]={32,1,32,2}; desc_test_recv(two,4,0);
    assert(d.ProcessInput()==4 && !buffer_size(input));
    assert(first.payloads.back()==1 && second.payloads.back()==2);
    first.next=0;
    // Real TEA, temporary buffer manager, frame parser and socket read path.
    d.SetupInput(input,&first,true);
    DWORD encrypted[4]={0},key[4]={1,2,3,4}; const char clear[]={32,3,32,4,32,5,32,6};
    std::memcpy(encrypted,clear,8); assert(TEA_Encrypt(encrypted,encrypted,key,8)==8);
    desc_test_recv(reinterpret_cast<const char*>(encrypted),8,0);
    const size_t prior=first.payloads.size();
    assert(d.ProcessInput()==8 && !buffer_size(input) && first.payloads.size()==prior+4);
    for(int i=0;i<4;++i) assert(first.payloads[prior+i]==i+3);
    // False-return overconsumption clamps against raw bytes without a buffer warning.
    Script script; script.done=false; script.consume=12; d.SetupInput(input,&script,true);
    desc_test_recv(reinterpret_cast<const char*>(encrypted),8,0);
    int warnings=desc_test_proceed_warnings();
    assert(d.ProcessInput()==8 && !buffer_size(input) && script.calls==1);
    assert(desc_test_proceed_warnings()==warnings);
    // True-return overconsumption reaches the real buffer layer's own clamp/log.
    script.done=true; script.calls=0;
    desc_test_recv(reinterpret_cast<const char*>(encrypted),8,0);
    assert(d.ProcessInput()==8 && !buffer_size(input) && script.calls==1);
    assert(desc_test_proceed_warnings()==warnings+1);
    // Partial encrypted input is still rounded up for decrypt/dispatch, as in the target.
    script.consume=2; script.calls=0;
    desc_test_recv(reinterpret_cast<const char*>(encrypted),5,0);
    assert(d.ProcessInput()==5 && buffer_size(input)==3 && script.seen==8);
    // No-data reads do not re-dispatch already buffered bytes.
    desc_test_recv(0,-1,EAGAIN); const int calls=script.calls;
    assert(d.ProcessInput()==0 && script.calls==calls && buffer_size(input)==3);
    buffer_delete(input);
    std::puts("descriptor input integration passed: reads, fragmentation, phase dispatch, real TEA and legacy clamps");
}
