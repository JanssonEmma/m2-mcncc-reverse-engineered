#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/guild.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/sectree.h"
#include "../../game/src/packet.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
// Explicit test-only guild DB lifecycle boundary. All old data fields are present.
static TPacketGCCharacterUpdate Read(LPBUFFER buffer) {
    assert(buffer_size(buffer)==35); TPacketGCCharacterUpdate p;
    std::memcpy(&p,buffer_read_peek(buffer),35); return p;
}
int main() {
    desc_test_allow_clock(); quest::CQuestManager quests;
    CHARACTER character,viewer; SECTREE sector; CGuild guild(0xfedcba98UL);
    LPBUFFER own=buffer_new(0,256),other=buffer_new(0,256);
    LPFDWATCH watch=fdwatch_new(16); DescOutputProbe self,recipient;
    self.Attach(watch,5,own); self.SetPacketState(5);
    recipient.Attach(watch,6,other); recipient.SetPacketState(5);
    character.BindDesc(&self); viewer.BindDesc(&recipient);
    character.m_vid=VID(0x11223344,0xdeadbeef);
    for(BYTE i=0;i<4;++i) character.SetPart(i,static_cast<WORD>(60000+i));
    character.m_pointsInstant.points[19]=999;
    character.m_pointsInstant.points[17]=999;
    character.m_bAddChrState=0xa5;
    character.m_afAffectFlag.bits[0]=0x89abcdef;
    character.m_afAffectFlag.bits[1]=0x76543210;
    character.m_iAlignment=-199; character.m_iRealAlignment=-327689;
    character.m_bPKMode=0xfe; character.m_dwMountVnum=0x12345678;
    character.UpdatePacket(); assert(!buffer_size(own) && !buffer_size(other));
    character.SetSectree(&sector); character.m_map_view[&viewer]=1;
    character.UpdatePacket();
    assert(buffer_size(other)==35 && !std::memcmp(buffer_read_peek(own),buffer_read_peek(other),35));
    TPacketGCCharacterUpdate p=Read(own);
    assert(p.header==19 && p.dwVID==0x11223344);
    for(int i=0;i<4;++i) assert(p.awPart[i]==60000+i);
    assert(p.bMovingSpeed==200 && p.bAttackSpeed==170 && p.bStateFlag==0xa5);
    assert(p.dwAffectFlag[0]==0x89abcdef && p.dwAffectFlag[1]==0x76543210);
    assert(!p.dwGuildID && p.sAlignment==-19 && p.bPKMode==0xfe && p.dwMountVnum==0x12345678);
    character.m_pGuild=&guild;
    buffer_reset(own); buffer_reset(other); character.ShowAlignment(true);
    p=Read(own); assert(p.dwGuildID==0xfedcba98UL && p.sAlignment==-32768);
    assert(character.m_iAlignment==character.m_iRealAlignment && buffer_size(other)==35);
    buffer_reset(own); buffer_reset(other); character.ShowAlignment(true);
    assert(!buffer_size(own) && !buffer_size(other));
    character.ShowAlignment(false); assert(!Read(own).sAlignment && character.m_iAlignment==0);
    buffer_reset(own); buffer_reset(other); character.ShowAlignment(false);
    assert(!buffer_size(own) && !buffer_size(other));
    int alignments[]={-9,9,327680,INT_MIN,INT_MAX};
    for(unsigned i=0;i<sizeof(alignments)/sizeof(*alignments);++i) {
        character.m_iRealAlignment=alignments[i]; character.ShowAlignment(true);
        assert(Read(own).sAlignment==static_cast<short>(alignments[i]/10));
        buffer_reset(own); buffer_reset(other);
    }
    character.m_bIsObserver=true; character.UpdatePacket();
    assert(buffer_size(own)==35 && !buffer_size(other));
    buffer_reset(own); character.BindDesc(0); character.m_bIsObserver=false;
    character.m_pointsInstant.points[19]=999; character.m_pointsInstant.points[17]=-1;
    character.UpdatePacket(); p=Read(other); assert(p.bMovingSpeed==250 && !p.bAttackSpeed && !buffer_size(own));
    character.m_map_view.clear(); character.SetSectree(0); character.m_pGuild=0;
    viewer.BindDesc(0); self.Attach(0,-1,0); recipient.Attach(0,-1,0);
    buffer_delete(own); buffer_delete(other); fdwatch_delete(watch);
    std::puts("character update passed: packed fields, real fanout, speed caps, guild, alignment toggles and observer behavior");
}
