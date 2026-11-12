
#include "guild.h"
//080add20. Independent actual read-only map leaf; ctor uncertainty is separate.
DWORD CGuild::UnderAnyWar(BYTE type) {
    for(std::map<DWORD,TGuildWar>::iterator it=m_EnemyGuild.begin();it!=m_EnemyGuild.end();++it)
        if((type>2 || it->second.type==type) && it->second.state==6)
            return it->first;
    return 0;
}
