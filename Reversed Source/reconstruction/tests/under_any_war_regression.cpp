#include <map>
#include <set>
#include <string>
#define private public
#include "../../game/src/guild.h"
#undef private
#include <cassert>
#include <cstdio>
static void Add(CGuild& guild,DWORD key,BYTE type,DWORD state) {
    TGuildWar war(type); // Explicit TEST ONLY ctor fixture, all fields set below.
    war.war_start_time=0x12340000UL+key;
    war.score=0x70000000UL+key;
    war.state=state;war.type=type;war.map_index=0x10000000UL+key;
    guild.m_EnemyGuild.insert(std::make_pair(key,war));
}
static void Same(const std::map<DWORD,TGuildWar>& a,const std::map<DWORD,TGuildWar>& b) {
    assert(a.size()==b.size());
    std::map<DWORD,TGuildWar>::const_iterator x=a.begin(),y=b.begin();
    for(;x!=a.end();++x,++y) {
        assert(x->first==y->first);
        assert(x->second.war_start_time==y->second.war_start_time);
        assert(x->second.score==y->second.score && x->second.state==y->second.state);
        assert(x->second.type==y->second.type && x->second.map_index==y->second.map_index);
    }
}
void CheckGuildWarLookup() {
    // Reuse existing TEST ONLY CGuild lifecycle fixture, full real data/API class.
    // No replacement for actual UnderAnyWar or STL map is permitted.
    CGuild guild(123);
    assert(guild.m_EnemyGuild.empty());
    for(unsigned type=0;type<256;++type) assert(guild.UnderAnyWar(static_cast<BYTE>(type))==0);
    Add(guild,0xffffffffUL,2,6);Add(guild,17,0,6);Add(guild,7,2,5);Add(guild,29,1,6);
    const std::map<DWORD,TGuildWar> before=guild.m_EnemyGuild;
    assert(guild.UnderAnyWar(0)==17);
    assert(guild.UnderAnyWar(1)==29);
    assert(guild.UnderAnyWar(2)==0xffffffffUL);
    for(unsigned type=3;type<256;++type) assert(guild.UnderAnyWar(static_cast<BYTE>(type))==17);
    Same(guild.m_EnemyGuild,before);
    // State must equal6; values on either side never count.
    guild.m_EnemyGuild.clear();
    for(unsigned stored=0;stored<256;++stored) {
        for(unsigned state=0;state<10;++state) {
            guild.m_EnemyGuild.clear();Add(guild,0x80000000UL,static_cast<BYTE>(stored),state);
            for(unsigned requested=0;requested<4;++requested) {
                DWORD expected=(state==6 && (requested==3 || requested==stored))?0x80000000UL:0;
                assert(guild.UnderAnyWar(static_cast<BYTE>(requested))==expected);
            }
            assert(guild.m_EnemyGuild.size()==1 && guild.m_EnemyGuild.begin()->second.state==state);
        }
    }
    // Key0 is a matching result, returned immediately rather than skipped.
    guild.m_EnemyGuild.clear();Add(guild,0,0,6);Add(guild,4,0,6);
    assert(guild.UnderAnyWar(0)==0 && guild.UnderAnyWar(255)==0);
    assert(guild.m_EnemyGuild.size()==2);
    std::puts("UnderAnyWar passed: original full map, exact state6/type policy, unsigned ordered key selection and read-only behavior");
}
