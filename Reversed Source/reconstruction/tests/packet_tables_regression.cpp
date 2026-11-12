#include "../../game/src/packet_info.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
static DWORD tick;
extern "C" DWORD get_dword_time() { return tick; }
template<class T> class Probe : public T {
public:
    size_t Entries() const { return this->m_pPacketMap.size(); }
};
static void Check(CPacketInfo& registry,int header,int expected,const char* label,bool sequence) {
    int size=-1; const char* name=0;
    assert(registry.Get(header,&size,&name));
    assert(size==expected && !std::strcmp(name,label));
    assert(registry.IsSequencePacket(header)==sequence);
}
static std::string Read(const char* path) {
    FILE* file=std::fopen(path,"r"); assert(file);
    char bytes[1024]; size_t n; std::string text;
    while((n=std::fread(bytes,1,sizeof bytes,file)))text.append(bytes,n);
    std::fclose(file); return text;
}
int main() {
    // All tables coexist independently despite overlapping wire headers.
    {
        Probe<CPacketInfoCG> client;
        Probe<CPacketInfoGG> peer;
        Probe<CPacketInfoUDP> udp;
        assert(client.Entries()==73 && peer.Entries()==24 && udp.Entries()==1);
        Check(client,1,50,"Login",true);
        Check(peer,1,33,"Login",false);
        Check(udp,1,9,"ServerStateRequest",false);
        Check(client,255,13,"Handshake",false);
        Check(client,252,14,"TimeSync",true);
        Check(client,64,1,"Text",false);
        Check(client,254,2,"Pong",true);
        Check(client,2,9,"Attack",true);
        Check(client,3,4,"Chat",true); // Minimum variable-length packet prefix plus sequence.
        Check(client,253,68,"Version",true);
        Check(client,241,68,"Version",true);
        Check(client,102,773,"MarkUpload",false);
        Check(client,202,11,"PasspodAnswer",true);
        Check(peer,3,30,"Relay",false);
        Check(peer,4,5,"Notice",false);
        Check(peer,25,1,"Siege",false);
        int size=111; const char* name="unchanged";
        assert(!client.Get(0,&size,&name) && size==111);
        assert(!peer.Get(24,&size,&name));
        assert(!udp.Get(2,&size,&name));
        // Measure one client packet; destructor must log it before base cleanup.
        Check(client,2,9,"Attack",true);
        tick=100; client.Start(); tick=105; client.End();
        // Existing registrations ignore attempted replacement.
        client.Set(2,999,"Replacement",false);
        Check(client,2,9,"Attack",true);
    }
    const std::string cg=Read("packet_info.txt");
    const std::string gg=Read("p2p_packet_info.txt");
    const std::string udp=Read("udp_packet_info.txt");
    char expected[128]; std::sprintf(expected,"%-16s %-10d %-10ld %.2f\n","Attack",1,5L,5.0);
    assert(cg.find(expected)!=std::string::npos);
    const size_t version=cg.find("Version");
    assert(version!=std::string::npos && cg.find("Version",version+7)!=std::string::npos);
    assert(gg.find("Relay")!=std::string::npos && udp.find("ServerStateRequest")!=std::string::npos);
    // Virtual deletion invokes the derived report writer, including for UDP.
    CPacketInfo* base=new CPacketInfoUDP;
    Check(*base,1,9,"ServerStateRequest",false);
    tick=10; base->Start(); tick=13; base->End();
    delete base;
    const std::string virtual_report=Read("udp_packet_info.txt");
    std::sprintf(expected,"%-16s %-10d %-10ld %.2f\n","ServerStateRequest",1,3L,3.0);
    assert(virtual_report.find(expected)!=std::string::npos);
    assert(std::remove("packet_info.txt")==0);
    assert(std::remove("p2p_packet_info.txt")==0);
    assert(std::remove("udp_packet_info.txt")==0);
    std::puts("derived packet tables passed: domain-specific headers, legacy sizes, sequences and virtual report cleanup");
}
