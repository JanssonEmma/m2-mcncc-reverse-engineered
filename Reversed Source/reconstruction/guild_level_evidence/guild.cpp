// Complete bounded old guild member/level methods; original guild.cpp placement.
// Other CGuild lifecycle/DB/war behavior remains pending; no substitutes are added.
#include "guild.h"
#include "char.h"
#include "packet.h"
#include "desc.h"
#include "desc_client.h"

//08089f80/08089ff0. _dummy is intentionally NOT initialized by either old ctor.
SGuildMember::SGuildMember(CHARACTER* ch,BYTE grade,DWORD offer_exp)
    : pid(ch->GetPlayerID()),grade(grade),is_general(0),job(ch->GetJob()),
      level(static_cast<BYTE>(ch->GetLevel())),offer_exp(offer_exp),name(ch->GetName()) {}
//0808a060/0808a0b0. Original name parameter is char*, not const char*.
SGuildMember::SGuildMember(DWORD pid,BYTE grade,BYTE is_general,BYTE job,
        BYTE level,DWORD offer_exp,char* name)
    : pid(pid),grade(grade),is_general(is_general),job(job),level(level),
      offer_exp(offer_exp),name(name) {}

//0808cf20. Lookup does not insert; return the actual mapped object's address.
TGuildMember* CGuild::GetMember(DWORD pid) {
    std::map<DWORD,TGuildMember>::iterator it=m_member.find(pid);
    return it==m_member.end()?0:&it->second;
}
//0808f5d0. Every existing member call persists/broadcasts, even unchanged levels.
void CGuild::LevelChange(DWORD pid,BYTE level) {
    std::map<DWORD,TGuildMember>::iterator member=m_member.find(pid);
    if(member==m_member.end()) return;
    member->second.level=level;
    TPacketGuildChangeMemberData db;
    db.guild_id=GetID();db.pid=pid;db.offer=member->second.offer_exp;
    db.grade=member->second.grade;db.level=level;
    // Original HEADER_GD_GUILD_CHANGE_MEMBER_DATA24, handle0, complete14-byte record.
    db_clientdesc->DBPacket(24,0,&db,sizeof(db));
    TPacketGCGuild packet;
    packet.header=75; // Original HEADER_GC_GUILD
    member->second._dummy=0; // AFTER real DBPacket, before client-prefix reads.
    for(std::set<CHARACTER*>::iterator online=m_memberOnline.begin();
            online!=m_memberOnline.end();++online) {
        DESC* descriptor=(*online)->GetDesc();
        if(descriptor) {
            packet.subheader=2; // Original GUILD_SUBHEADER_GC_LIST
            packet.size=17;
            descriptor->BufferedPacket(&packet,sizeof(packet));
            // Full original TGuildMember stays intact. Only its original13-byte prefix is sent.
            descriptor->Packet(&member->second,13);
        }
    }
}
