// Stage in reconstruction/tests after candidates. Not built here.
#include <map>
#include <set>
#include <string>
#define private public
#include "../../game/src/guild.h"
#undef private
#include "../../game/src/char.h"
#include "../../game/src/packet.h"
#include "../../common/tables.h"
#include <cstddef>
#define GUILD_LEVEL_ASSERT(name,expr) typedef char guild_level_assert_##name[(expr)?1:-1]
GUILD_LEVEL_ASSERT(dword4,sizeof(DWORD)==4);
GUILD_LEVEL_ASSERT(byte1,sizeof(BYTE)==1);
GUILD_LEVEL_ASSERT(word2,sizeof(WORD)==2);
GUILD_LEVEL_ASSERT(member20,sizeof(SGuildMember)==20 && sizeof(TGuildMember)==20);
GUILD_LEVEL_ASSERT(member_pid0,offsetof(SGuildMember,pid)==0);
GUILD_LEVEL_ASSERT(member_grade4,offsetof(SGuildMember,grade)==4);
GUILD_LEVEL_ASSERT(member_general5,offsetof(SGuildMember,is_general)==5);
GUILD_LEVEL_ASSERT(member_job6,offsetof(SGuildMember,job)==6);
GUILD_LEVEL_ASSERT(member_level7,offsetof(SGuildMember,level)==7);
GUILD_LEVEL_ASSERT(member_offer8,offsetof(SGuildMember,offer_exp)==8);
GUILD_LEVEL_ASSERT(member_dummy12,offsetof(SGuildMember,_dummy)==12);
GUILD_LEVEL_ASSERT(member_name16,offsetof(SGuildMember,name)==16);
GUILD_LEVEL_ASSERT(grade10,sizeof(TGuildGrade)==10);
GUILD_LEVEL_ASSERT(data220,sizeof(TGuildData)==220);
// Modern STL containers do not reproduce the old whole-object312-byte ABI.
// Verify complete logical members/types and declaration order instead; add no padding.
GUILD_LEVEL_ASSERT(guild_data_first,offsetof(CGuild,m_data)==0);
#define GUILD_LEVEL_ORDER(left,right) GUILD_LEVEL_ASSERT(order_##left##_##right,offsetof(CGuild,right)>=offsetof(CGuild,left)+sizeof(((CGuild*)0)->left))
GUILD_LEVEL_ORDER(m_data,m_general_count);
GUILD_LEVEL_ORDER(m_general_count,m_iMemberCountBonus);
GUILD_LEVEL_ORDER(m_iMemberCountBonus,m_member);
GUILD_LEVEL_ORDER(m_member,m_memberOnline);
GUILD_LEVEL_ORDER(m_memberOnline,m_memberP2POnline);
GUILD_LEVEL_ORDER(m_memberP2POnline,m_EnemyGuild);
GUILD_LEVEL_ORDER(m_EnemyGuild,m_mapGuildWarEndTime);
GUILD_LEVEL_ORDER(m_mapGuildWarEndTime,abSkillUsable);
GUILD_LEVEL_ORDER(abSkillUsable,m_GuildInviteEventMap);
GUILD_LEVEL_ASSERT(db14,sizeof(SPacketGuildChangeMemberData)==14);
GUILD_LEVEL_ASSERT(db_guild0,offsetof(SPacketGuildChangeMemberData,guild_id)==0);
GUILD_LEVEL_ASSERT(db_pid4,offsetof(SPacketGuildChangeMemberData,pid)==4);
GUILD_LEVEL_ASSERT(db_offer8,offsetof(SPacketGuildChangeMemberData,offer)==8);
GUILD_LEVEL_ASSERT(db_level12,offsetof(SPacketGuildChangeMemberData,level)==12);
GUILD_LEVEL_ASSERT(db_grade13,offsetof(SPacketGuildChangeMemberData,grade)==13);
GUILD_LEVEL_ASSERT(client_header4,sizeof(packet_guild)==4);
GUILD_LEVEL_ASSERT(client_header0,offsetof(packet_guild,header)==0);
GUILD_LEVEL_ASSERT(client_size1,offsetof(packet_guild,size)==1);
GUILD_LEVEL_ASSERT(client_sub3,offsetof(packet_guild,subheader)==3);
void guild_level_signature_checks() {
    DWORD (CGuild::*id)()=&CGuild::GetID;
    void (CGuild::*levelChange)(DWORD,BYTE)=&CGuild::LevelChange;
    TGuildMember* (CGuild::*member)(DWORD)=&CGuild::GetMember;
    int (CHARACTER::*level)()=&CHARACTER::GetLevel;
    DWORD (CHARACTER::*pid)()=&CHARACTER::GetPlayerID;
    (void)id;(void)levelChange;(void)member;(void)level;(void)pid;
}

// Exact complete logical data types: six full containers and the full12-bool array.
void guild_level_logical_member_checks() {
    TGuildData CGuild::* data=&CGuild::m_data;
    int CGuild::* general=&CGuild::m_general_count;
    int CGuild::* bonus=&CGuild::m_iMemberCountBonus;
    std::map<DWORD,TGuildMember> CGuild::* members=&CGuild::m_member;
    std::set<CHARACTER*> CGuild::* online=&CGuild::m_memberOnline;
    std::set<DWORD> CGuild::* p2p=&CGuild::m_memberP2POnline;
    std::map<DWORD,TGuildWar> CGuild::* enemies=&CGuild::m_EnemyGuild;
    std::map<DWORD,DWORD> CGuild::* ends=&CGuild::m_mapGuildWarEndTime;
    bool (CGuild::* skills)[12]=&CGuild::abSkillUsable;
    std::map<DWORD,LPEVENT> CGuild::* invites=&CGuild::m_GuildInviteEventMap;
    (void)data;(void)general;(void)bonus;(void)members;(void)online;
    (void)p2p;(void)enemies;(void)ends;(void)skills;(void)invites;
}
