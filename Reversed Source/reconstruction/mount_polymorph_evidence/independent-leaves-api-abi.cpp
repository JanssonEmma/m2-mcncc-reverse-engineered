
#include "../../game/src/char.h"
#include "../../game/src/guild.h"
#include <cstddef>
typedef DWORD(CGuild::*WarSignature)(BYTE);
typedef void(CHARACTER::*LeafSignature)();
WarSignature actual_war_signature=&CGuild::UnderAnyWar;
LeafSignature actual_muyeong_signature=&CHARACTER::StopMuyeongEvent;
LeafSignature actual_empty_skill_signature=&CHARACTER::ComputeSkillPoints;
typedef char DWord32[sizeof(DWORD)==4?1:-1];
typedef char GuildWarTimeAt0[offsetof(TGuildWar,war_start_time)==0?1:-1];
typedef char GuildWarScoreAt4[offsetof(TGuildWar,score)==4?1:-1];
typedef char GuildWarStateAt8[offsetof(TGuildWar,state)==8?1:-1];
typedef char GuildWarTypeAt12[offsetof(TGuildWar,type)==12?1:-1];
typedef char GuildWarMapAt16[offsetof(TGuildWar,map_index)==16?1:-1];
typedef char GuildWarBytes20[sizeof(TGuildWar)==20?1:-1];
// No whole STL class-size assertions, C++11 type_traits, or new data fields.
