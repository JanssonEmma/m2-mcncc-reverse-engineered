from pathlib import Path
p=Path('work/guild_level/guild_level_abi.cpp');s=p.read_text()
s=s.replace('#include "../../game/src/guild.h"','#include <map>\n#include <set>\n#include <string>\n#define private public\n#include "../../game/src/guild.h"\n#undef private')
s=s.replace('GUILD_LEVEL_ASSERT(guild312,sizeof(CGuild)==312);','''// Modern STL containers do not reproduce the old whole-object312-byte ABI.
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
GUILD_LEVEL_ORDER(abSkillUsable,m_GuildInviteEventMap);''')
s += '''
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
'''
p.write_text(s)
print(str(p.resolve()))
