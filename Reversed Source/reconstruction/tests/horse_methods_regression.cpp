// Run against actual closed-horse-methods.cpp/closed-move-packet.cpp and full classes.
// Existing CHARACTER/quest/guild lifecycle boundaries remain explicitly TEST ONLY.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/char_manager.h"
#include "../../game/src/guild.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/sectree.h"
#undef protected
#undef private
#include "../../game/src/packet.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/utils.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstddef>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
extern "C" void desc_test_set_millis(unsigned long);
void EncodeMovePacket(TPacketGCMove&,DWORD,BYTE,BYTE,DWORD,DWORD,DWORD,DWORD,BYTE);
static void Empty(LPBUFFER buffer) { buffer_read_proceed(buffer,buffer_size(buffer)); }
static void Command(LPBUFFER output,const char* text) {
    unsigned size=static_cast<unsigned>(std::strlen(text)+10);
    assert(buffer_size(output)==size);
    const unsigned char* raw=static_cast<const unsigned char*>(buffer_read_peek(output));
    assert(raw[0]==4 && raw[3]==5);
    WORD packedSize=0;std::memcpy(&packedSize,raw+1,2);assert(packedSize==size);
    assert(!std::strcmp(reinterpret_cast<const char*>(raw+9),text));
    Empty(output);
}
int main() {
    desc_test_allow_clock();test_server=0;
    quest::CQuestManager quests;CHARACTER_MANAGER manager;
    CHARACTER owner,horse,newRider,victim;
    CGuild guild(0x12345678UL); // Existing TEST ONLY full-record lifecycle.
    // Actual SetRotation preserves raw representation, including -0 and NaN.
    const unsigned rotations[]={0,0x80000000U,0x43fa0000U,0x7fc12345U,0x7f800000U};
    for(unsigned i=0;i<sizeof rotations/sizeof *rotations;++i) {
        float value;std::memcpy(&value,&rotations[i],4);owner.SetRotation(value);
        unsigned stored=0;std::memcpy(&stored,&owner.m_pointsInstant.fRot,4);
        assert(stored==rotations[i]);
    }
    owner.SetEmpire(255);assert(owner.m_bEmpire==255);
    owner.m_dwPlayerID=0x80000000UL;guild.m_data.master_pid=0x80000000UL;
    assert(guild.GetMasterPID()==0x80000000UL);
    for(unsigned level=0;level<=30;++level) {
        owner.m_Horse.bLevel=static_cast<BYTE>(level);owner.m_pGuild=0;
        DWORD base=static_cast<DWORD>(c_aHorseStat[level].iNPCRace);
        assert(owner.GetMyHorseVnum()==base);
        owner.m_pGuild=&guild;guild.m_data.master_pid=17;
        assert(owner.GetMyHorseVnum()==base+1);
        guild.m_data.master_pid=owner.m_dwPlayerID;
        assert(owner.GetMyHorseVnum()==base+2);
    }
    owner.m_pGuild=0;
    victim.m_vid=VID(77,1234);manager.m_map_pkChrByVID[77]=&victim;
    owner.m_kVIDVictim=VID(77,1235);assert(!owner.GetVictim());
    owner.m_kVIDVictim=VID(77,1234);assert(owner.GetVictim()==&victim);
    assert(manager.m_map_pkChrByVID.size()==1);
    manager.m_map_pkChrByVID.clear();assert(!owner.GetVictim());

    owner.m_Horse.bLevel=30;owner.m_Horse.bRiding=0;owner.m_chHorse=&horse;
    owner.m_Horse.sHealth=50;owner.m_Horse.sStamina=200;owner.SendHorseInfo();
    assert(owner.m_bSendHorseLevel==30 && owner.m_bSendHorseHealthGrade==3 && owner.m_bSendHorseStaminaGrade==3);
    DescOutputProbe first,second;
    LPBUFFER firstOut=buffer_new(0,8192),secondOut=buffer_new(0,8192);
    LPFDWATCH watch=fdwatch_new(16);
    first.Attach(watch,5,firstOut);first.SetPacketState(5);
    second.Attach(watch,6,secondOut);second.SetPacketState(5);
    owner.BindDesc(&first);newRider.BindDesc(&second);
    owner.m_Horse.bLevel=30;owner.m_Horse.bRiding=0;owner.m_chHorse=&horse;
    struct Case { short health,stamina;int hg,sg; } cases[]={
        {0,0,0,0},{15,20,1,0},{16,21,2,1},{35,60,2,1},
        {36,61,3,2},{50,140,3,2},{50,141,3,3},{-1,-1,1,0}
    };
    for(unsigned i=0;i<sizeof cases/sizeof *cases;++i) {
        owner.m_Horse.sHealth=cases[i].health;owner.m_Horse.sStamina=cases[i].stamina;
        owner.m_bSendHorseLevel=owner.m_bSendHorseHealthGrade=owner.m_bSendHorseStaminaGrade=255;
        owner.SendHorseInfo();
        char expected[64];std::sprintf(expected,"horse_state 30 %d %d",cases[i].hg,cases[i].sg);
        Command(firstOut,expected);
        assert(owner.m_bSendHorseLevel==30 && owner.m_bSendHorseHealthGrade==cases[i].hg &&
               owner.m_bSendHorseStaminaGrade==cases[i].sg);
        owner.SendHorseInfo();assert(buffer_size(firstOut)==0); // No repeat when grades unchanged.
    }
    owner.m_Horse.bRiding=0;owner.ClearHorseInfo();
    Command(firstOut,"hide_horse_state");
    assert(!owner.m_chHorse && !owner.m_bSendHorseLevel && !owner.m_bSendHorseHealthGrade &&
           !owner.m_bSendHorseStaminaGrade);
    owner.m_Horse.bRiding=1;owner.m_chHorse=&horse;
    owner.m_bSendHorseLevel=17;owner.m_bSendHorseHealthGrade=2;owner.m_bSendHorseStaminaGrade=1;
    owner.ClearHorseInfo();
    assert(!owner.m_chHorse && buffer_size(firstOut)==0 && owner.m_bSendHorseLevel==17 &&
           owner.m_bSendHorseHealthGrade==2 && owner.m_bSendHorseStaminaGrade==1);
    owner.m_Horse.bRiding=0;owner.SendHorseInfo();assert(buffer_size(firstOut)==0);
    // SetRider's actual virtual old-clear/new-send path, without overriding either.
    owner.m_chHorse=&horse;horse.m_chRider=&owner;
    horse.SetRider(&owner); // Same-pointer replacement must still clear old rider.
    Command(firstOut,"hide_horse_state");
    assert(horse.m_chRider==&owner && !owner.m_chHorse);
    newRider.m_chHorse=&horse;newRider.m_Horse.bLevel=30;
    newRider.m_Horse.sHealth=50;newRider.m_Horse.sStamina=200;
    newRider.m_bSendHorseLevel=0;
    horse.SetRider(&newRider);
    Command(firstOut,"hide_horse_state");Command(secondOut,"horse_state 30 3 3");
    assert(!owner.m_chHorse && horse.m_chRider==&newRider);
    horse.SetRider(0);Command(secondOut,"hide_horse_state");
    assert(!horse.m_chRider && !newRider.m_chHorse);

    // Exact24-byte original packet and explicit/default time behavior.
    TPacketGCMove packet;
    EncodeMovePacket(packet,0x12345678UL,2,7,0xffffffffUL,0x80000000UL,
                     0x11223344UL,0xaabbccddUL,255);
    const BYTE golden[]={3,2,7,255,0x78,0x56,0x34,0x12,
        255,255,255,255,0,0,0,128,0xdd,0xcc,0xbb,0xaa,0x44,0x33,0x22,0x11};
    assert(sizeof packet==24 && !std::memcmp(&packet,golden,24));
    desc_test_set_millis(0x0badcafeUL);
    EncodeMovePacket(packet,1,1,0,10,20,30,0,0);
    assert(packet.dwTime==0x0badcafeUL);
    // Actual PacketView sends to view recipient and excludes owner descriptor.
    SECTREE sector;owner.SetSectree(&sector);owner.m_map_view[&newRider]=1;
    owner.m_vid=VID(0x12345678UL,9);owner.m_posDest.x=-1;owner.m_posDest.y=0x12345678L;
    owner.m_dwMoveDuration=0x11223344UL;owner.SetRotation(-9.75f);
    owner.SendMovePacket(0,4,101,202,303,0xaabbccddUL,-1);
    const BYTE movement[]={3,0,4,255,0x78,0x56,0x34,0x12,
        255,255,255,255,0x78,0x56,0x34,0x12,0xdd,0xcc,0xbb,0xaa,0x44,0x33,0x22,0x11};
    assert(buffer_size(firstOut)==0 && buffer_size(secondOut)==24 &&
           !std::memcmp(buffer_read_peek(secondOut),movement,24));Empty(secondOut);
    owner.m_bIsObserver=true;owner.SendMovePacket(1,0,1,2,3,4,5);
    assert(buffer_size(firstOut)==0 && buffer_size(secondOut)==0);
    owner.m_bIsObserver=false;
    owner.m_map_view.clear();owner.SetSectree(0);owner.BindDesc(0);newRider.BindDesc(0);
    first.ReleaseBuffered();second.ReleaseBuffered();first.Attach(0,-1,0);second.Attach(0,-1,0);
    buffer_delete(firstOut);buffer_delete(secondOut);fdwatch_delete(watch);
    std::puts("horse methods passed: guild race delta, CRC victim, real command thresholds/cache/rider virtuals, packed move24 and actual view dispatch");
}
