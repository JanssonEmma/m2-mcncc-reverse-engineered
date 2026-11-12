// Compile-only signature/record evidence after exact declarations are merged.
#include <cstddef>
#include "../../game/src/char.h"
#include "../../game/src/packet.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/guild.h"
#include "../../game/src/log.h"
void (CHARACTER::*point_change_signature)(BYTE,int,bool,bool)=&CHARACTER::PointChange;
void (quest::CQuestManager::*quest_level_signature)(unsigned int)=&quest::CQuestManager::LevelUp;
LPITEM (CHARACTER::*autogive_signature)(DWORD,BYTE,int,bool)=&CHARACTER::AutoGiveItem;
void (CGuild::*guild_level_signature)(DWORD,BYTE)=&CGuild::LevelChange;
void (LogManager::*public_query_signature)(const char*,...)=&LogManager::Query;
void (LogManager::*char_log_signature)(DWORD,DWORD,DWORD,DWORD,const char*,const char*)=&LogManager::CharLog;
typedef char point_change_packet_size[(sizeof(TPacketGCPointChange)==17)?1:-1];
typedef char point_change_packet_type_offset[(offsetof(TPacketGCPointChange,type)==8)?1:-1];
typedef char point_change_packet_amount_offset[(offsetof(TPacketGCPointChange,amount)==9)?1:-1];
typedef char point_change_packet_value_offset[(offsetof(TPacketGCPointChange,value)==13)?1:-1];
