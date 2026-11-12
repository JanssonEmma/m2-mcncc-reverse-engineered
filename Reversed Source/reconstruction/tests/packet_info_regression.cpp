#include "../../game/src/packet_info.h"
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstring>
#include <string>
static DWORD clock_value;
extern "C" DWORD get_dword_time() { return clock_value; }
class Probe : public CPacketInfo {
public:
    TPacketElement* Current() { return m_pCurrentPacket; }
    size_t Count() { return m_pPacketMap.size(); }
};
static std::string Read(const char* path) {
    FILE* file=std::fopen(path,"r"); assert(file);
    std::string result; char data[512]; size_t n;
    while((n=std::fread(data,1,sizeof data,file))!=0) result.append(data,n);
    std::fclose(file); return result;
}
int main() {
    Probe registry;
    assert(!registry.Current() && !registry.Count());
    int size=777; const char* name="sentinel";
    assert(!registry.Get(1,&size,&name) && size==777 && !std::strcmp(name,"sentinel"));
    assert(!registry.Get(1,0,0));
    assert(!registry.IsSequencePacket(1));
    char mutable_name[]="Later";
    registry.Set(300,8,mutable_name,true); mutable_name[0]='X';
    registry.Set(-5,4,"First"); registry.Set(3,2,"Middle");
    assert(registry.Count()==3);
    assert(registry.Get(300,&size,&name) && size==9 && !std::strcmp(name,"Later"));
    TPacketElement* original=registry.Current();
    registry.Set(300,-1,0,false); // Duplicate exits before reading even the name.
    assert(registry.Get(300,&size,&name) && size==9 && registry.Current()==original);
    assert(registry.IsSequencePacket(300) && !registry.IsSequencePacket(-5));
    assert(registry.GetElement(300)==original && !registry.GetElement(999));
    assert(registry.Current()==original); // Sequence lookup does not select the measured packet.
    assert(!registry.Get(999,&size,&name) && registry.Current()==original);
    clock_value=0xfffffff0UL; registry.Start(); clock_value=0x10; registry.End();
    assert(original->iCalled==1 && original->dwLoad==32);
    clock_value=100; registry.Start(); clock_value=105; registry.End();
    assert(original->iCalled==2 && original->dwLoad==37);
    registry.Log(""); assert(original->iCalled==2 && original->dwLoad==37);
    registry.Log("packet_info_regression.log");
    const std::string text=Read("packet_info_regression.log");
    assert(text.find("Name             Called     Load       Ratio\n")==0);
    const size_t first=text.find("First"), middle=text.find("Middle"), later=text.find("Later");
    assert(first<middle && middle<later); // Numeric map order, including non-byte headers.
    char expected[128]; std::sprintf(expected,"%-16s %-10d %-10ld %.2f\n","Later",2,37L,18.5);
    assert(text.find(expected)!=std::string::npos);
    assert(std::remove("packet_info_regression.log")==0);
    // Original logging prints signed long load, but ratio converts unsigned DWORD to double.
    original->dwLoad=0x80000001UL; original->iCalled=1;
    registry.Log("packet_info_regression.log");
    const std::string high=Read("packet_info_regression.log");
    assert(high.find("-2147483647 2147483649.00")!=std::string::npos);
    assert(std::remove("packet_info_regression.log")==0);
    original->iCalled=INT_MAX; original->dwLoad=0xfffffffeUL;
    clock_value=10; registry.Start(); clock_value=15; registry.End();
    assert(original->iCalled==INT_MIN && original->dwLoad==3);
    registry.Set(800,INT_MAX,"Wrapped",true);
    assert(registry.Get(800,&size,&name) && size==INT_MIN);
    std::puts("packet info regression passed: lookup, duplicate/sequence semantics, timing wrap and log output");
}
