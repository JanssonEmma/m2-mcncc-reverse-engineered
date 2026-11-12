#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include "../../libthecore/include/main.h"
#include <cassert>
#include <climits>
#include <cstring>
#include <cstdio>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" void desc_test_allow_clock(void);
static int errors,assertions;
static bool allowAssert,insertDuringError;
static unsigned int expectedID;
static quest::CQuestManager* active;
extern "C" void quest_indices_error(int line,const char* format,unsigned int id) {
    assert(line==1077&&!std::strcmp(format,"cannot find quest name by index %u")&&id==expectedID);
    ++errors;std::fprintf(stderr,"INDEX-MISSING %u\n",id);
    if(insertDuringError) {active->m_mapQuestNameByIndex.insert(std::make_pair(id,std::string("inserted-by-diagnostic")));insertDuringError=false;}
}
extern "C" int quest_indices_assert(const char* expression,const char* file,int line) {
    if(allowAssert&&!std::strcmp(expression,"!\"cannot find quest name by index\"")&&std::strstr(file,"questmanager.cpp")) {
        assert(line>0&&errors==assertions+1);++assertions;return 1;
    }
    return 0; // All unexpected assertions retain fail-fast current DESC boundary.
}
static void HashVectors();
static void Lookup(quest::CQuestManager& manager) {
    // TEST inputs only, never a fake production RegisterQuest/LoadStartQuest.
    typedef __gnu_cxx::hash_map<std::string,int,quest::CQuestManager::stringhash,std::equal_to<std::string>,std::allocator<int> > NameMap;
    assert(manager.m_hmQuestName.empty()&&manager.m_mapQuestNameByIndex.empty());
    std::string binary("A\0B",3),leading("\0A",2);
    manager.m_hmQuestName.insert(std::make_pair(std::string("A"),17));
    manager.m_hmQuestName.insert(std::make_pair(leading,-1)); // Same hash as A; different full key.
    manager.m_hmQuestName.insert(std::make_pair(binary,INT_MIN));
    manager.m_hmQuestName.insert(std::make_pair(std::string(""),0));
    assert(manager.GetQuestIndexByName("missing")==0&&manager.m_hmQuestName.size()==4);
    assert(manager.GetQuestIndexByName("A")==17&&manager.GetQuestIndexByName(leading)==UINT_MAX);
    assert(manager.GetQuestIndexByName(binary)==0x80000000U&&manager.GetQuestIndexByName("")==0);
    assert(manager.GetQuestIndexByName("a")==0&&manager.m_hmQuestName.size()==4);
    const int* stable=&manager.m_hmQuestName.find("A")->second;
    for(unsigned i=0;i<700;++i) {char key[32];std::sprintf(key,"typed-input-%u",i);manager.m_hmQuestName.insert(std::make_pair(std::string(key),static_cast<int>(i+1)));}
    assert(manager.GetQuestIndexByName("A")==17&& &manager.m_hmQuestName.find("A")->second==stable);
    for(unsigned i=0;i<700;++i) {char key[32];std::sprintf(key,"typed-input-%u",i);assert(manager.GetQuestIndexByName(key)==i+1);}
    manager.m_hmQuestName.find("A")->second=31;assert(manager.GetQuestIndexByName("A")==31);
    manager.m_hmQuestName.erase(leading);assert(manager.GetQuestIndexByName(leading)==0&&manager.GetQuestIndexByName("A")==31);
    manager.m_mapQuestNameByIndex[0]="zero";manager.m_mapQuestNameByIndex[UINT_MAX]=binary;
    const std::string& zero=manager.GetQuestNameByIndex(0);const std::string* address=&zero;
    assert(address==&manager.m_mapQuestNameByIndex.find(0)->second&&zero=="zero");
    manager.m_mapQuestNameByIndex[0]="updated";assert(zero=="updated"&& &manager.GetQuestNameByIndex(0)==address);
    assert(manager.GetQuestNameByIndex(UINT_MAX)==binary);
    manager.m_mapNPCNameID[""]=0;manager.m_mapNPCNameID["NPC"]=UINT_MAX;manager.m_mapNPCNameID[binary]=0x80000000U;
    assert(manager.FindNPCIDByName("NPC")==UINT_MAX&&manager.FindNPCIDByName(binary)==0x80000000U);
    assert(manager.FindNPCIDByName("npc")==0&&manager.FindNPCIDByName("")==0&&manager.m_mapNPCNameID.size()==3);
    quest::PC pc;manager.m_pCurrentPC=&pc;pc.m_stCurQuest=binary;
    assert(&manager.GetCurrentQuestName()==&pc.m_stCurQuest&&manager.GetCurrentQuestName()==binary);
    pc.m_stCurQuest="live";assert(manager.GetCurrentQuestName()=="live");manager.m_pCurrentPC=0;
    // Preserve log/assert/fallback order and no retry after external diagnostic.
    active=&manager;allowAssert=true;errors=assertions=0;expectedID=12345;
    const std::string* fallback=&manager.GetQuestNameByIndex(expectedID);
    assert(fallback->empty()&&errors==1&&assertions==1&&!manager.m_mapQuestNameByIndex.count(expectedID));
    expectedID=12346;insertDuringError=true;
    assert(&manager.GetQuestNameByIndex(expectedID)==fallback&&fallback->empty());
    assert(errors==2&&assertions==2&&manager.GetQuestNameByIndex(expectedID)=="inserted-by-diagnostic");
    expectedID=12347;assert(&manager.GetQuestNameByIndex(expectedID)==fallback&&errors==3&&assertions==3);
    allowAssert=false;active=0;
}
int main(int argc,char** argv) {
    desc_test_allow_clock();assert(sizeof(unsigned int)==4&&sizeof(int)==4);
    quest::CQuestManager manager;
    if(argc>1&&!std::strcmp(argv[1],"--fatal-missing")) {expectedID=777;manager.GetQuestNameByIndex(777);return 9;}
    HashVectors();Lookup(manager);assert(!event_count());event_destroy();
    std::puts("quest manager indices passed: original 32-bit hash, full typed maps, reference identity and diagnostic/assertion order; no registration/VM claim");
}

static void HashVectors() {
    quest::CQuestManager::stringhash hash;
    assert(hash(std::string())==0U);
    const unsigned char v1[]={0x41};
    assert(hash(std::string(reinterpret_cast<const char*>(v1),sizeof(v1)))==0x00000041U);
    const unsigned char v2[]={0x41,0x42};
    assert(hash(std::string(reinterpret_cast<const char*>(v2),sizeof(v2)))==0x41006611U);
    const unsigned char v3[]={0x68,0x65,0x6c,0x6c,0x6f};
    assert(hash(std::string(reinterpret_cast<const char*>(v3),sizeof(v3)))==0xec6d6be8U);
    const unsigned char v4[]={0x00,0x41};
    assert(hash(std::string(reinterpret_cast<const char*>(v4),sizeof(v4)))==0x00000041U);
    const unsigned char v5[]={0x41,0x00,0x42};
    assert(hash(std::string(reinterpret_cast<const char*>(v5),sizeof(v5)))==0xa6a114ebU);
    const unsigned char v6[]={0x80,0xff,0x7f,0x00};
    assert(hash(std::string(reinterpret_cast<const char*>(v6),sizeof(v6)))==0xf2569bd6U);
    const unsigned char v7[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};
    assert(hash(std::string(reinterpret_cast<const char*>(v7),sizeof(v7)))==0xf56c4800U);
    assert(hash(std::string(4096,'x'))==0xd1428000U);
    assert(hash(std::string("A"))==hash(std::string("\0A",2)));
}
