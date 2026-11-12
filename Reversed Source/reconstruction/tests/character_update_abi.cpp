#include <cstddef>
#include "../../common/tables.h"
#include "../../game/src/guild.h"
#include "../../game/src/packet.h"
#define SIZE(T,N) typedef char size_##T[(sizeof(T)==N)?1:-1]
#define OFFSET(T,M,N) typedef char off_##T##_##M[(offsetof(T,M)==N)?1:-1]
SIZE(TGuildGrade,10); SIZE(TGuildData,220); SIZE(TGuildMember,20);
SIZE(TGuildWar,20); SIZE(TGuildCreateParameter,20);
OFFSET(TGuildData,grade_array,26); OFFSET(TGuildData,skill_point,176);
OFFSET(TGuildData,abySkill,177); OFFSET(TGuildData,power,192);
OFFSET(TGuildData,gold,216); OFFSET(TGuildMember,name,16);
OFFSET(TGuildWar,map_index,16); OFFSET(TGuildCreateParameter,name,4);
SIZE(TPacketGCCharacterUpdate,35);
OFFSET(TPacketGCCharacterUpdate,header,0); OFFSET(TPacketGCCharacterUpdate,dwVID,1);
OFFSET(TPacketGCCharacterUpdate,awPart,5); OFFSET(TPacketGCCharacterUpdate,bMovingSpeed,13);
OFFSET(TPacketGCCharacterUpdate,bAttackSpeed,14); OFFSET(TPacketGCCharacterUpdate,bStateFlag,15);
OFFSET(TPacketGCCharacterUpdate,dwAffectFlag,16); OFFSET(TPacketGCCharacterUpdate,dwGuildID,24);
OFFSET(TPacketGCCharacterUpdate,sAlignment,28); OFFSET(TPacketGCCharacterUpdate,bPKMode,30);
OFFSET(TPacketGCCharacterUpdate,dwMountVnum,31);

OFFSET(SGuildMember,pid,0);
OFFSET(SGuildMember,grade,4);
OFFSET(SGuildMember,is_general,5);
OFFSET(SGuildMember,job,6);
OFFSET(SGuildMember,level,7);
OFFSET(SGuildMember,offer_exp,8);
OFFSET(SGuildMember,_dummy,12);
OFFSET(SGuildMember,name,16);
OFFSET(SGuildGrade,grade_name,0);
OFFSET(SGuildGrade,auth_flag,9);
OFFSET(SGuildData,guild_id,0);
OFFSET(SGuildData,master_pid,4);
OFFSET(SGuildData,exp,8);
OFFSET(SGuildData,level,12);
OFFSET(SGuildData,name,13);
OFFSET(SGuildData,grade_array,26);
OFFSET(SGuildData,skill_point,176);
OFFSET(SGuildData,abySkill,177);
OFFSET(SGuildData,power,192);
OFFSET(SGuildData,max_power,196);
OFFSET(SGuildData,ladder_point,200);
OFFSET(SGuildData,win,204);
OFFSET(SGuildData,draw,208);
OFFSET(SGuildData,loss,212);
OFFSET(SGuildData,gold,216);
OFFSET(SGuildWar,war_start_time,0);
OFFSET(SGuildWar,score,4);
OFFSET(SGuildWar,state,8);
OFFSET(SGuildWar,type,12);
OFFSET(SGuildWar,map_index,16);
