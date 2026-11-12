// Real descriptor, buffer, TEA, profiler and fdwatch; OS and unrecovered lifecycle are fixtures.
#include "desc_fixture.h"
#include "../../game/src/packet.h"
#include "../../game/src/TrafficProfiler.h"
#include "../../libthecore/include/crypt.h"
#include <cassert>
#include <cstring>
#include <string>
#include <sys/event.h>
#include "../../game/src/config.h"
int max_bytes_written, current_bytes_written, total_bytes_written;
extern "C" {
void desc_test_capacity(int);
void desc_test_send_reset(void);
void desc_test_send_step(int, int);
int desc_test_send_calls(void);
const char* desc_test_written(void);
}
static void reset_watch(LPFDWATCH watcher) {
    desc_test_capacity(64);
    assert(fdwatch(watcher,0)==1);
    (void)fdwatch_check_event(watcher,5,0); // Dispatch consumes one-shot registration.
    assert(watcher->nkqevents==0);
}
static void pending(LPFDWATCH watcher, DescOutputProbe& client) {
    assert(watcher->nkqevents==1 && watcher->fd_data[5]==&client);
    assert(watcher->kqevents[0].filter==EVFILT_WRITE && watcher->kqevents[0].flags==17);
}
static void contents(LPBUFFER buffer, const std::string& expected) {
    assert(buffer_size(buffer)==expected.size());
    assert(!std::memcmp(buffer_read_peek(buffer),expected.data(),expected.size()));
}
int main() {
    DescOutputProbe client;
    LPFDWATCH watcher=fdwatch_new(16);
    LPBUFFER output=buffer_new(0,256);
    client.Attach(watcher,5,output);
    fdwatch_add_fd(watcher,5,&client,FDW_READ,0);
    reset_watch(watcher);
    // A closed phase retains buffered bytes and does not register output.
    client.BufferedPacket("ab",2);
    LPBUFFER buffered=client.BufferedOutput();
    assert(buffered && buffered->mem_size>=1024);
    client.BufferedPacket("cd",2);
    assert(client.BufferedOutput()==buffered);
    g_bTrafficProfileOn=true; // Closed phase must not touch the absent profiler.
    client.Packet("e",1);
    g_bTrafficProfileOn=false;
    contents(buffered,"abcd"); contents(output,""); assert(!watcher->nkqevents);
    client.SetPacketState(5);
    client.Packet("ef",2);
    assert(!client.BufferedOutput()); contents(output,"abcdef"); pending(watcher,client);
    // Packet enqueue through the real output transport.
    desc_test_send_reset(); desc_test_send_step(6,0);
    assert(fdwatch(watcher,0)==1);
    assert(fdwatch_check_event(watcher,5,0)==FDW_WRITE);
    assert(client.ProcessOutput()==0 && desc_test_send_calls()==1);
    assert(!std::memcmp(desc_test_written(),"abcdef",6)); contents(output,"");
    // Conservative +8 requirement closes even an aligned payload with only 15 available.
    char small[15]={0}; LPBUFFER tight=buffer_new(small,sizeof small);
    client.Attach(watcher,5,tight); client.SetPacketState(5,true);
    client.BufferedPacket("abc",3); client.Packet("defgh",5);
    assert(client.PacketPhase()==0 && !client.BufferedOutput());
    contents(tight,""); pending(watcher,client); reset_watch(watcher);
    buffer_delete(tight);
    // Ordinary plaintext overflow also deletes pending concatenation and rearms after close.
    char shortmem[4]={0}; tight=buffer_new(shortmem,sizeof shortmem);
    client.Attach(watcher,5,tight); client.SetPacketState(5);
    client.BufferedPacket("abc",3); client.Packet("de",2);
    assert(client.PacketPhase()==0 && !client.BufferedOutput());
    contents(tight,""); pending(watcher,client); reset_watch(watcher); buffer_delete(tight);
    // Plaintext permits exact capacity, unlike the encrypted reserve check.
    // First large-pool allocation gives exact capacity; real buffer_write grows on equality.
    const std::string exact(8193,'E');
    tight=buffer_new(0,static_cast<int>(exact.size()));
    assert(tight->mem_size==static_cast<int>(exact.size()));
    client.Attach(watcher,5,tight); client.SetPacketState(5);
    client.Packet(exact.data(),static_cast<int>(exact.size()));
    contents(tight,exact); assert(client.PacketPhase()==5);
    pending(watcher,client); reset_watch(watcher); buffer_delete(tight);
    // TEA writes in-place, rounds up, and leaves preceding output untouched.
    client.Attach(watcher,5,output); client.SetPacketState(5,true);
    buffer_write(output,"HEAD",4);
    DWORD encrypted[4]={0}, key[4]={1,2,3,4};
    std::memcpy(encrypted,"abcde",5);
    assert(TEA_Encrypt(encrypted,encrypted,key,5)==8);
    client.Packet("abcde",5);
    contents(output,std::string("HEAD",4)+std::string(reinterpret_cast<char*>(encrypted),8));
    pending(watcher,client); reset_watch(watcher); buffer_reset(output);
    // Relay bypasses encryption, profiling, and pending buffered concatenation.
    client.BufferedPacket("held",4);
    client.SetRelay("abcdefghijklmnopqrstuvwxyz");
    g_bTrafficProfileOn=true; // Relay must bypass profiling even when enabled.
    client.Packet("RAW",3);
    g_bTrafficProfileOn=false;
    assert(!client.HasRelay()); contents(client.BufferedOutput(),"held");
    TPacketGGRelay relay; std::memset(&relay,0,sizeof relay);
    relay.bHeader=3; std::memcpy(relay.szName,"abcdefghijklmnopqrstuvwx",24); relay.lSize=3;
    contents(output,std::string(reinterpret_cast<char*>(&relay),sizeof relay)+"RAW");
    pending(watcher,client); reset_watch(watcher); buffer_reset(output);
    // Header-capacity failure keeps the relay name and buffered bytes, with no rearm.
    char relaymem[31]={0}; tight=buffer_new(relaymem,29);
    client.Attach(watcher,5,tight); client.SetPacketState(5);
    client.SetRelay("name"); client.Packet("xyz",3);
    assert(client.PacketPhase()==0 && client.HasRelay() && !watcher->nkqevents);
    contents(tight,""); contents(client.BufferedOutput(),"held"); buffer_delete(tight);
    // Payload failure retains the already-written header but clears the relay name.
    tight=buffer_new(relaymem,31);
    client.Attach(watcher,5,tight); client.SetPacketState(5);
    client.Packet("xyz",3);
    assert(client.PacketPhase()==0 && !client.HasRelay() && !watcher->nkqevents);
    assert(buffer_size(tight)==30); contents(client.BufferedOutput(),"held");
    buffer_delete(tight); client.ReleaseBuffered();
    // LargePacket alone requests growth before taking the ordinary Packet route.
    client.Attach(watcher,5,output); client.SetPacketState(5);
    const std::string large(1024,'L');
    client.LargePacket(large.data(),static_cast<int>(large.size()));
    contents(output,large); pending(watcher,client); reset_watch(watcher);
    buffer_reset(output);
    // LargePacket grows even in a closed phase, before Packet returns early.
    const int prior_capacity=output->mem_size;
    client.SetPacketState(0);
    const std::string larger(prior_capacity+1,'Z');
    client.LargePacket(larger.data(),static_cast<int>(larger.size()));
    assert(output->mem_size>prior_capacity && !watcher->nkqevents);
    contents(output,"");
    // Profiling accounts for attempts before capacity checks, with combined first header.
    {
        TrafficProfiler profiler;
        assert(profiler.Initialize(0xffffffffUL,"desc_packet_profile.log"));
        client.SetPacketState(5);
        g_bTrafficProfileOn=true;
        const char first[]={33,'a','b'}, second[]={33,'z'};
        const char prefix[]={34,'a'}, suffix[]={35,'b'}, rejected[]={36,'z'};
        client.Packet(first,3); reset_watch(watcher);
        client.Packet(second,2); reset_watch(watcher);
        client.BufferedPacket(prefix,2); client.Packet(suffix,2); reset_watch(watcher);
        char no_room[1]={0}; tight=buffer_new(no_room,1);
        client.Attach(watcher,5,tight);
        client.Packet(rejected,2);
        assert(client.PacketPhase()==0);
        pending(watcher,client); reset_watch(watcher);
        buffer_delete(tight);
        g_bTrafficProfileOn=false;
        assert(profiler.Flush());
    }
    FILE* report=std::fopen("desc_packet_profile.log","r"); assert(report);
    char report_text[4096]; const size_t report_size=std::fread(report_text,1,sizeof report_text,report);
    std::fclose(report);
    const std::string profile(report_text,report_size);
    assert(profile.find("Total traffic: 11 bytes")!=std::string::npos);
    assert(profile.find("Total used packet: 3")!=std::string::npos);
    assert(profile.find("33\t2\t5\t\t2\n")!=std::string::npos);
    assert(profile.find("34\t1\t4\t\t4\n")!=std::string::npos);
    assert(profile.find("36\t1\t2\t\t2\n")!=std::string::npos);
    assert(std::remove("desc_packet_profile.log")==0);
    buffer_delete(output); fdwatch_delete(watcher);
    std::puts("descriptor packet integration passed: concatenation, overflow, TEA, relay, growth, profiling and transport");
}
